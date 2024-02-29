/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unreal(R) Engine End User
License Agreement at https://www.unrealengine.com/en-US/eula/unreal
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

#include "Wwise/WwiseExecutionQueue.h"
#include "Async/TaskGraphInterfaces.h"
#include "HAL/Event.h"
#include "Misc/IQueuedWork.h"
#include "Wwise/WwiseConcurrencyModule.h"
#include "Wwise/Stats/AsyncStats.h"
#include "Wwise/Stats/Concurrency.h"

#include <inttypes.h>

static constexpr const auto DEBUG_WWISEEXECUTIONQUEUE{ false };

#if defined(WITH_LOW_LEVEL_TESTS) && WITH_LOW_LEVEL_TESTS || defined(WITH_AUTOMATION_TESTS) || (WITH_DEV_AUTOMATION_TESTS || WITH_PERF_AUTOMATION_TESTS)
bool FWwiseExecutionQueue::Test::bMockEngineDeletion{ false };
bool FWwiseExecutionQueue::Test::bMockEngineDeleted{ false };
#endif

class FWwiseExecutionQueue::ExecutionQueuePoolTask
	: public IQueuedWork
{
public:
	ExecutionQueuePoolTask(TUniqueFunction<void()>&& InFunction)
		: Function(MoveTemp(InFunction))
	{ }

public:
	virtual void DoThreadedWork() override
	{
		Function();
		delete this;
	}

	virtual void Abandon() override
	{
	}

private:
	TUniqueFunction<void()> Function;
};


FWwiseExecutionQueue::FWwiseExecutionQueue(ENamedThreads::Type InNamedThread) :
	NamedThread(InNamedThread),
	ThreadPool(nullptr),
	bOwnedPool(false)
{
	ASYNC_INC_DWORD_STAT(STAT_WwiseExecutionQueues);
	UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue (%p): Creating in named thread %d"), this, (int)InNamedThread);
}

FWwiseExecutionQueue::FWwiseExecutionQueue(FQueuedThreadPool* InThreadPool) :
	NamedThread(ENamedThreads::UnusedAnchor),
	ThreadPool(InThreadPool ? InThreadPool : GetDefaultThreadPool()),
	bOwnedPool(false)
{
	ASYNC_INC_DWORD_STAT(STAT_WwiseExecutionQueues);
	UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue (%p): Creating in thread pool %p"), this, InThreadPool);
}

FWwiseExecutionQueue::FWwiseExecutionQueue(const TCHAR* InThreadName, EThreadPriority InThreadPriority, int32 InStackSize) :
	NamedThread(ENamedThreads::UnusedAnchor),
	ThreadPool(FQueuedThreadPool::Allocate()),
	bOwnedPool(true)
{
	ASYNC_INC_DWORD_STAT(STAT_WwiseExecutionQueues);
	UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue (%p): Creating new thread pool %p named %s"), this, ThreadPool, InThreadName);
	verify(ThreadPool->Create(1, InStackSize, InThreadPriority, InThreadName));
}

FWwiseExecutionQueue::~FWwiseExecutionQueue()
{
	UE_CLOG(UNLIKELY(bDeleteOnceClosed && WorkerState.Load(EMemoryOrder::SequentiallyConsistent) != EWorkerState::Closed), LogWwiseConcurrency, Fatal, TEXT("Deleting FWwiseExectionQueue twice!"));

	Close();
	if (bOwnedPool)
	{
		delete ThreadPool;
	}
	ASYNC_DEC_DWORD_STAT(STAT_WwiseExecutionQueues);
	UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue (%p): Deleted Execution Queue"), this);
}

void FWwiseExecutionQueue::Async(FBasicFunction&& InFunction)
{
	TrySetRunningWorkerToAddOp();
	if (UNLIKELY(!OpQueue.Enqueue(MoveTemp(InFunction))))
	{
		ASYNC_INC_DWORD_STAT(STAT_WwiseExecutionQueueAsyncCalls);
		InFunction();
		return;
	}
	StartWorkerIfNeeded();
}

void FWwiseExecutionQueue::AsyncAlways(FBasicFunction&& InFunction)
{
	Async([CallerThreadId = FPlatformTLS::GetCurrentThreadId(), InFunction = MoveTemp(InFunction)]() mutable
	{
		if (CallerThreadId == FPlatformTLS::GetCurrentThreadId())
		{
			FFunctionGraphTask::CreateAndDispatchWhenReady(MoveTemp(InFunction));
		}
		else
		{
			InFunction();
		}
	});
}

void FWwiseExecutionQueue::AsyncWait(FBasicFunction&& InFunction)
{
	SCOPED_WWISECONCURRENCY_EVENT_4(TEXT("FWwiseExecutionQueue::AsyncWait"));
	TrySetRunningWorkerToAddOp();
	FEventRef Event(EEventMode::ManualReset);
	if (UNLIKELY(!OpQueue.Enqueue([&Event, &InFunction] {
		ASYNC_INC_DWORD_STAT(STAT_WwiseExecutionQueueAsyncWaitCalls);
		InFunction();
		Event->Trigger();
	})))
	{
		InFunction();
		return;
	}
	StartWorkerIfNeeded();
	Event->Wait();
}

void FWwiseExecutionQueue::Close()
{
	UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::Close (%p): Closing"), this);
	auto State = WorkerState.Load(EMemoryOrder::Relaxed);
	if (State != EWorkerState::Closing && State != EWorkerState::Closed)
	{
		AsyncWait([this]
		{
			TrySetRunningWorkerToClosing();
		});
		State = WorkerState.Load(EMemoryOrder::Relaxed);
	}
	if (State != EWorkerState::Closed)
	{
		SCOPED_WWISECONCURRENCY_EVENT_4(TEXT("FWwiseExecutionQueue::Close Waiting"));
		UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::Close (%p): Waiting for Closed"), this);
		while (State != EWorkerState::Closed)
		{
			FPlatformProcess::Sleep(0.01);
			State = WorkerState.Load(EMemoryOrder::Relaxed);
		}
	}
	UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::Close (%p): Done Closing"), this);
}

void FWwiseExecutionQueue::CloseAndDelete()
{
	UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::Close (%p): Closing and Request Deleting"), this);
	bDeleteOnceClosed = true;
	auto State = WorkerState.Load(EMemoryOrder::Relaxed);
	Async([this]
	{
		TrySetRunningWorkerToClosing();
	});
}

bool FWwiseExecutionQueue::IsBeingClosed() const
{
	const auto State = WorkerState.Load(EMemoryOrder::Relaxed);
	return UNLIKELY(State == EWorkerState::Closed || State == EWorkerState::Closing);
}

bool FWwiseExecutionQueue::IsClosed() const
{
	const auto State = WorkerState.Load(EMemoryOrder::Relaxed);
	return State == EWorkerState::Closed;
}

FQueuedThreadPool* FWwiseExecutionQueue::GetDefaultThreadPool()
{
	auto* ConcurrencyModule = IWwiseConcurrencyModule::GetModule();
	if (UNLIKELY(!ConcurrencyModule))
	{
		return nullptr;
	}
	return ConcurrencyModule->GetExecutionQueueThreadPool();
}

void FWwiseExecutionQueue::StartWorkerIfNeeded()
{
	if (TrySetStoppedWorkerToRunning())
	{
		if (UNLIKELY(!IWwiseConcurrencyModule::GetModule() || Test::bMockEngineDeletion || Test::bMockEngineDeleted))
		{
			if (UNLIKELY(!FTaskGraphInterface::IsRunning() || Test::bMockEngineDeleted))
			{
				UE_LOG(LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::StartWorkerIfNeeded (%p): No Task Graph. Do tasks now"), this);
				Work();
			}
			else
			{
				UE_LOG(LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::StartWorkerIfNeeded (%p): Concurrency is not available. Starting new worker thread on Task Graph"), this);
				FFunctionGraphTask::CreateAndDispatchWhenReady([this]
				{
					Work();
				});
			}
		}
		else if (ThreadPool)
		{
			UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::StartWorkerIfNeeded (%p): Starting new worker thread on thread pool %p"), this, ThreadPool);
			ThreadPool->AddQueuedWork(new ExecutionQueuePoolTask([this]
			{
				Work();
			}));
		}
		else
		{
			UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::StartWorkerIfNeeded (%p): Starting new worker thread on named thread %d"), this, (int)NamedThread);
			FFunctionGraphTask::CreateAndDispatchWhenReady([this]
			{
				Work();
			}, TStatId{}, nullptr, NamedThread);
		}
	}
}

void FWwiseExecutionQueue::Work()
{
	SCOPED_WWISECONCURRENCY_EVENT_4(TEXT("FWwiseExecutionQueue::Work"));
	UE_LOG(LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::Work (%p): Started work on %" PRIi32), this, FPlatformTLS::GetCurrentThreadId());

	do
	{
		ProcessWork();
	}
	while (!StopWorkerIfDone());
}

bool FWwiseExecutionQueue::StopWorkerIfDone()
{
	if (!OpQueue.IsEmpty())
	{
		TrySetAddOpWorkerToRunning();
		return false;
	}

	const auto CurrentThreadId = FPlatformTLS::GetCurrentThreadId();

	if (LIKELY(TrySetAddOpWorkerToRunning()))
	{
		// We have a new operation in the queue.
		if (LIKELY(!OpQueue.IsEmpty()))
		{
			// Keep on chugging...
			return false;
		}

		if (UNLIKELY(!IWwiseConcurrencyModule::GetModule()))
		{
			UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::StartWorkerIfNeeded (%p): Concurrency is not available. Starting new worker thread on Task Graph to replace %" PRIi32), this, CurrentThreadId);
			FFunctionGraphTask::CreateAndDispatchWhenReady([this]
			{
				Work();
			});
		}
		else if (ThreadPool)
		{
			UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::StartWorkerIfNeeded (%p): Starting new worker thread on thread pool %p to replace %" PRIi32), this, ThreadPool, CurrentThreadId);
			ThreadPool->AddQueuedWork(new ExecutionQueuePoolTask([this]
			{
				Work();
			}));
		}
		else
		{
			UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::StartWorkerIfNeeded (%p): Starting new task on named thread %d to replace %" PRIi32), this, (int)NamedThread, CurrentThreadId);
			FFunctionGraphTask::CreateAndDispatchWhenReady([this]
			{
				Work();
			}, TStatId{}, nullptr, NamedThread);
		}
		return true;		// This precise worker thread is done, we tagged teamed it to a new one.
	}

	const auto bDeleteOnceClosedCopy = this->bDeleteOnceClosed;

	if (LIKELY(TrySetRunningWorkerToStopped()))
	{
		UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::StopWorkerIfDone (%p): Stopped worker on thread %" PRIi32 ), this, CurrentThreadId);
		// We don't have any more operations queued. Done.
		// Don't execute operations here, as the Execution Queue might be deleted here.
		return true;
	}
	else if (LIKELY(TrySetClosingWorkerToClosed()))
	{
		// We were exiting and we don't have operations anymore. Immediately return, as our worker is not valid at this point.
		// Don't do any operations here!

		if (bDeleteOnceClosedCopy)		// We use a copy since the deletion might've already occurred
		{
			FFunctionGraphTask::CreateAndDispatchWhenReady([this]
			{
				SCOPED_WWISECONCURRENCY_EVENT_4(TEXT("FWwiseExecutionQueue::StopWorkerIfDone DeleteOnceClosed"));
				UE_LOG(LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue::StopWorkerIfDone (%p): Auto deleting on Any Thread"), this);
				delete this;
			}, TStatId{}, nullptr, ENamedThreads::GameThread);
		}
		return true;
	}
	else if (LIKELY(WorkerState.Load() == EWorkerState::Closed))
	{
		// We were already closed, and we had some extra operations to do somehow.
		return true;
	}
	else if (LIKELY(WorkerState.Load() == EWorkerState::Stopped))
	{
		UE_LOG(LogWwiseConcurrency, Error, TEXT("FWwiseExecutionQueue::StopWorkerIfDone (%p): Worker on thread %" PRIi32 " is stopped, but we haven't stopped it ourselves."), this, CurrentThreadId);
		return true;
	}
	else
	{
		// Reiterate because we got changed midway
		return false;
	}
}

void FWwiseExecutionQueue::ProcessWork()
{
	FBasicFunction Op;
	if (OpQueue.Dequeue(Op))
	{
		UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue (%p): Executing async operation"), this);
		Op();
	}
}

bool FWwiseExecutionQueue::TrySetStoppedWorkerToRunning()
{
	return TryStateUpdate(EWorkerState::Stopped, EWorkerState::Running);

}

bool FWwiseExecutionQueue::TrySetRunningWorkerToStopped()
{
	return TryStateUpdate(EWorkerState::Running, EWorkerState::Stopped);
}

bool FWwiseExecutionQueue::TrySetRunningWorkerToAddOp()
{
	return TryStateUpdate(EWorkerState::Running, EWorkerState::AddOp);
}

bool FWwiseExecutionQueue::TrySetAddOpWorkerToRunning()
{
	return TryStateUpdate(EWorkerState::AddOp, EWorkerState::Running);
}

bool FWwiseExecutionQueue::TrySetRunningWorkerToClosing()
{
	return TryStateUpdate(EWorkerState::Running, EWorkerState::Closing)
		|| TryStateUpdate(EWorkerState::AddOp, EWorkerState::Closing)
		|| TryStateUpdate(EWorkerState::Stopped, EWorkerState::Closing);
	// Warning: Try not to do operations past this method returning "true". There's a slight chance "this" might be deleted!
}

bool FWwiseExecutionQueue::TrySetClosingWorkerToClosed()
{
	return TryStateUpdate(EWorkerState::Closing, EWorkerState::Closed);
	// Warning: NEVER do operations past this method returning "true". "this" is probably deleted!
}

const TCHAR* FWwiseExecutionQueue::StateName(EWorkerState State)
{
	switch (State)
	{
	case EWorkerState::Stopped: return TEXT("Stopped");
	case EWorkerState::Running: return TEXT("Running");
	case EWorkerState::AddOp: return TEXT("AddOp");
	case EWorkerState::Closing: return TEXT("Closing");
	case EWorkerState::Closed: return TEXT("Closed");
	default: return TEXT("UNKNOWN");
	}
}

bool FWwiseExecutionQueue::TryStateUpdate(EWorkerState NeededState, EWorkerState WantedState)
{
	EWorkerState PreviousState = NeededState;
	bool bResult = WorkerState.CompareExchange(PreviousState, WantedState);
	bResult = bResult && PreviousState == NeededState;

	UE_CLOG(DEBUG_WWISEEXECUTIONQUEUE, LogWwiseConcurrency, VeryVerbose, TEXT("FWwiseExecutionQueue (%p): %s %s [%s -> %s] %s %s [%" PRIi32 "]"), this,
		StateName(PreviousState),
		bResult ? TEXT("==>") : TEXT("XX>"),
		StateName(NeededState), StateName(WantedState),
		bResult ? TEXT("==>") : TEXT("XX>"),
		bResult ? StateName(WantedState) : StateName(PreviousState),
		FPlatformTLS::GetCurrentThreadId());
	return bResult;
}
