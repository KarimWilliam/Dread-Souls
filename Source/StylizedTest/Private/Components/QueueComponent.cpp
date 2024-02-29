// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QueueComponent.h"
#include "Characters/MainCharacter.h"
#include "Components/Action.h"
#include "Components/ActionComponent.h"

UQueueComponent::UQueueComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UQueueComponent::BeginPlay()
{
	Super::BeginPlay();
	MC = Cast<AMainCharacter>(GetOwner());
}

void UQueueComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


bool UQueueComponent::UpdateQueuedAction(UAction* Action)
{
	if (IsActionLocked) return false;
	if (PreviousAction)
	{
		if (PreviousAction->ActionName == Action->ActionName && IsPreviousActionLocked)
		{
			UE_LOG(LogTemp, Warning, TEXT("same action detected"));
			return false;
		}
	}

	IsActionQueued = true;
	QueuedAction = Action;
	return true;
}


void UQueueComponent::ActivateAction()
{
	LockPreviousQueuedAction();
	LockQueuedAction();
	if (IsActionQueued && MC)
	{
		UActionComponent* ActionComp = MC->GetActionComp();
		//If the action i am queueing is an attack and my currently running action is dodge, then the attack transforms to a roll attack
		if (MC &&  ActionComp->GetLastPerformedAction() && (GetQueuedActionName() == "LightAttackAction" || GetQueuedActionName() == "SwordAbilityAction") && ActionComp->GetLastPerformedAction()->ActionName == "DodgeRollAction")
		{
			ActionComp->StartActionByName(MC, "DodgeRollAttackAction");
		}
		else
		{
			ActionComp->StartActionByName(MC, GetQueuedActionName());
		}
		ClearQueuedAction();
	}
}


//HELPER FUNCTIONS

bool UQueueComponent::ClearQueuedAction()
{
	IsActionQueued = false;
	PreviousAction = QueuedAction;
	QueuedAction = nullptr;
	return true;
}

bool UQueueComponent::GetIsActionQueued()
{
	return IsActionQueued;
}

FName UQueueComponent::GetQueuedActionName()
{
	return QueuedAction->ActionName;
}

void UQueueComponent::LockQueuedAction()
{
	IsActionLocked = true;
}

void UQueueComponent::UnlockQueuedAction()
{
	IsActionLocked = false;
}


void UQueueComponent::LockPreviousQueuedAction()
{
	IsPreviousActionLocked = true;
}

void UQueueComponent::UnlockPreviousQueuedAction()
{
	IsPreviousActionLocked = false;
}


bool UQueueComponent::GetIsPreviousActionLocked()
{
	return IsPreviousActionLocked;
}

void UQueueComponent::LockAllQueuedAction()
{
	IsSlowActionLocked = true;
	IsRegularActionLocked = true;
	IsFastActionLocked = true;
}

bool UQueueComponent::GetIsActionLocked()
{
	return IsActionLocked;
}
