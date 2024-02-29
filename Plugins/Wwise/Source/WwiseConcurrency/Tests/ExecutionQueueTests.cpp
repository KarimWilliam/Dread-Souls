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

#include "Wwise/WwiseUnitTests.h"

#if WWISE_UNIT_TESTS
#include "Wwise/WwiseExecutionQueue.h"
#include <atomic>

TEST_CASE("Audio::Wwise::Concurrency::ExecutionQueue_Smoke", "[ApplicationContextMask][SmokeFilter]")
{
	SECTION("Static")
	{
		static_assert(std::is_constructible<FWwiseExecutionQueue>::value);
		static_assert(std::is_constructible<FWwiseExecutionQueue, ENamedThreads::Type>::value);
		static_assert(std::is_constructible<FWwiseExecutionQueue, FQueuedThreadPool*>::value);
		static_assert(std::is_constructible<FWwiseExecutionQueue, const TCHAR*>::value);
		static_assert(std::is_constructible<FWwiseExecutionQueue, const TCHAR*, EThreadPriority>::value);
		static_assert(std::is_constructible<FWwiseExecutionQueue, const TCHAR*, EThreadPriority, int32>::value);
		static_assert(!std::is_copy_constructible<FWwiseExecutionQueue>::value);
		static_assert(!std::is_move_constructible<FWwiseExecutionQueue>::value);
		static_assert(!std::is_copy_assignable<FWwiseExecutionQueue>::value);
		static_assert(!std::is_move_assignable<FWwiseExecutionQueue>::value);
	}

	SECTION("Instantiation")
	{
		FWwiseExecutionQueue NoParam;
		FWwiseExecutionQueue NamedThread(ENamedThreads::AnyThread);
		FWwiseExecutionQueue Pool(GThreadPool);
		FWwiseExecutionQueue NewPool(TEXT("New Execution Queue"));

		CHECK(NoParam.ThreadPool);
		CHECK(NoParam.NamedThread == ENamedThreads::UnusedAnchor);
		CHECK(NoParam.ThreadPool == FWwiseExecutionQueue::GetDefaultThreadPool());
		
		CHECK_FALSE(NamedThread.ThreadPool);
		CHECK_FALSE(NamedThread.NamedThread == ENamedThreads::UnusedAnchor);

		CHECK(Pool.NamedThread == ENamedThreads::UnusedAnchor);
		CHECK_FALSE(Pool.bOwnedPool);
		CHECK(Pool.ThreadPool);

		CHECK(NewPool.NamedThread == ENamedThreads::UnusedAnchor);
		CHECK(NewPool.bOwnedPool);
		CHECK(NewPool.ThreadPool);
		CHECK(NewPool.ThreadPool && NewPool.ThreadPool->GetNumThreads() == 1);
	}

	SECTION("Async At Destructor")
	{
		constexpr const int LoopCount = 10;
		std::atomic<int> Value{ 0 };
		{
			FWwiseExecutionQueue ExecutionQueue;
			for (int i = 0; i < LoopCount; ++i)
			{
				ExecutionQueue.Async([&Value]
				{
					++Value;
				});
			}
		}
		CHECK(Value.load() == LoopCount);
	}

	SECTION("AsyncWait")
	{
		constexpr const int LoopCount = 10;
		std::atomic<int> Value{ 0 };
		{
			const auto CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
			FWwiseExecutionQueue ExecutionQueue;
			for (int i = 0; i < LoopCount; ++i)
			{
				ExecutionQueue.AsyncWait([&Value, CurrentThreadId]
				{
					CHECK_FALSE(CurrentThreadId == FPlatformTLS::GetCurrentThreadId());
					++Value;
				});
			}
			CHECK(Value.load() == LoopCount);
		}
	}

	SECTION("Async in order")
	{
		constexpr const int LoopCount = 10;
		std::atomic<int> Value{ 0 };
		{
			FWwiseExecutionQueue ExecutionQueue;
			for (int i = 0; i < LoopCount; ++i)
			{
				ExecutionQueue.Async([&Value, ShouldBe = i]
				{
					CHECK(Value++ == ShouldBe);
				});
			}
		}
		CHECK(Value.load() == LoopCount);
	}

	SECTION("Async at exit")
	{
		constexpr const int LoopCount = 10;
		std::atomic<int> Value{ 0 };
		{
			FWwiseExecutionQueue::Test::bMockEngineDeletion = true;
			FWwiseExecutionQueue ExecutionQueue;
			for (int i = 0; i < LoopCount; ++i)
			{
				ExecutionQueue.Async([&Value, ShouldBe = i]
				{
					CHECK(Value++ == ShouldBe);
				});
			}
		}
		FWwiseExecutionQueue::Test::bMockEngineDeletion = false;
		CHECK(Value.load() == LoopCount);
	}

	SECTION("Async after exit")
	{
		constexpr const int LoopCount = 10;
		std::atomic<int> Value{ 0 };
		{
			const auto CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
			FWwiseExecutionQueue::Test::bMockEngineDeleted = true;
			FWwiseExecutionQueue ExecutionQueue;
			for (int i = 0; i < LoopCount; ++i)
			{
				ExecutionQueue.Async([&Value, ShouldBe = i, CurrentThreadId]
				{
					CHECK(CurrentThreadId == FPlatformTLS::GetCurrentThreadId());
					CHECK(Value++ == ShouldBe);
				});
			}
		}
		FWwiseExecutionQueue::Test::bMockEngineDeleted = false;
		CHECK(Value.load() == LoopCount);
	}
}

TEST_CASE("Audio::Wwise::Concurrency::ExecutionQueue_Perf", "[ApplicationContextMask][PerfFilter]")
{
	SECTION("AsyncAddingOpPerf")
	{
		constexpr const int LoopCount = 1000000;
		constexpr const int ExpectedUS = 600000;
		std::atomic<int> Value{ 0 };

		{
			FWwiseExecutionQueue ExecutionQueue;

			FDateTime StartTime = FDateTime::UtcNow();
			for (int i = 0; i < LoopCount; ++i)
			{
				ExecutionQueue.Async([&Value]
				{
					++Value;
				});
			}
			FTimespan Duration = FDateTime::UtcNow() - StartTime;
			WWISETEST_LOG("AsyncAddingOpPerf %dus < %dus", (int)Duration.GetTotalMicroseconds(), ExpectedUS);
			CHECK(Duration.GetTotalMicroseconds() < ExpectedUS);
		}
		CHECK(Value.load() == LoopCount);
	}

	SECTION("AsyncExecutionPerf")
	{
		constexpr const int LoopCount = 1000000;
		constexpr const int ExpectedUS = 200000;
		std::atomic<int> Value{ 0 };

		FDateTime StartTime;
		{
			FWwiseExecutionQueue ExecutionQueue;

			ExecutionQueue.AsyncWait([&ExecutionQueue, LoopCount, &Value]
			{
				for (int i = 0; i < LoopCount; ++i)
				{
					ExecutionQueue.Async([&Value]
					{
						++Value;
					});
				}
			});
			StartTime = FDateTime::UtcNow();
		}
		FTimespan Duration = FDateTime::UtcNow() - StartTime;
		WWISETEST_LOG("AsyncExecutionPerf %dus < %dus", (int)Duration.GetTotalMicroseconds(), ExpectedUS);
		CHECK(Value.load() == LoopCount);
		CHECK(Duration.GetTotalMicroseconds() < ExpectedUS);
	}
}

TEST_CASE("Audio::Wwise::Concurrency::ExecutionQueue_Stress", "[ApplicationContextMask][StressFilter]")
{
	SECTION("AsyncStress")
	{
		constexpr const int LoopCount = 2000000;
		constexpr const int MainLoopCount = 100;
		constexpr const int SubLoopCount = 100;
		constexpr const int FinalLoopCount = LoopCount / MainLoopCount / SubLoopCount;

		constexpr const int ExpectedUS = 1500000;

		FDateTime StartTime = FDateTime::UtcNow();
		{
			FWwiseExecutionQueue MainExecutionQueue;
			FWwiseExecutionQueue SubExecutionQueue;
			FWwiseExecutionQueue DeletionExecutionQueue;

			for (int i = 0; i < MainLoopCount; ++i)
			{
				MainExecutionQueue.Async([&SubExecutionQueue, &DeletionExecutionQueue, SubLoopCount, FinalLoopCount]
				{
					for (int i = 0; i < SubLoopCount; ++i)
					{
						SubExecutionQueue.Async([&DeletionExecutionQueue, FinalLoopCount]
						{
							auto ExecutionQueue = new FWwiseExecutionQueue;
							for (int i = 0; i < FinalLoopCount; ++i)
							{
								ExecutionQueue->Async([]
								{
								});
							}
							DeletionExecutionQueue.Async([ExecutionQueue]
							{
								delete ExecutionQueue;
							});
						});
					}
				});
			}
		}
		FTimespan Duration = FDateTime::UtcNow() - StartTime;
		WWISETEST_LOG("AsyncAddingOpPerf %dus < %dus", (int)Duration.GetTotalMicroseconds(), ExpectedUS);
		CHECK(Duration.GetTotalMicroseconds() < ExpectedUS);
	}

	SECTION("AsyncStress at Exit")
	{
		constexpr const int LoopCount = 100000;
		constexpr const int MainLoopCount = 100;
		constexpr const int SubLoopCount = 100;
		constexpr const int FinalLoopCount = LoopCount / MainLoopCount / SubLoopCount;

		constexpr const int ExpectedUS = 3000000;

		FDateTime StartTime = FDateTime::UtcNow();
		{
			FWwiseExecutionQueue MainExecutionQueue;
			FWwiseExecutionQueue SubExecutionQueue;
			FWwiseExecutionQueue DeletionExecutionQueue;

			for (int i = 0; i < MainLoopCount; ++i)
			{
				MainExecutionQueue.Async([&SubExecutionQueue, &DeletionExecutionQueue, SubLoopCount, FinalLoopCount]
				{
					for (int i = 0; i < SubLoopCount; ++i)
					{
						SubExecutionQueue.Async([&DeletionExecutionQueue, FinalLoopCount]
						{
							auto ExecutionQueue = new FWwiseExecutionQueue;
							for (int i = 0; i < FinalLoopCount; ++i)
							{
								ExecutionQueue->Async([]
								{
								});
							}
							DeletionExecutionQueue.Async([ExecutionQueue]
							{
								delete ExecutionQueue;
							});
						});
					}
				});

				if (i == MainLoopCount / 3)
				{
					MainExecutionQueue.Async([]
					{
						FWwiseExecutionQueue::Test::bMockEngineDeletion = true;
					});
				}
			}
			MainExecutionQueue.Async([]
			{
				FWwiseExecutionQueue::Test::bMockEngineDeleted = true;
			});
		}
		FWwiseExecutionQueue::Test::bMockEngineDeletion = false;
		FWwiseExecutionQueue::Test::bMockEngineDeleted = false;

		FTimespan Duration = FDateTime::UtcNow() - StartTime;
		WWISETEST_LOG("AsyncAddingOpPerf %dus < %dus", (int)Duration.GetTotalMicroseconds(), ExpectedUS);
		CHECK(Duration.GetTotalMicroseconds() < ExpectedUS);
	}
}

#endif // WWISE_UNIT_TESTS