// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/ActionComponent.h"

#include "Characters/BaseCharacter.h"
#include "Components/Action.h"
#include "Components/AttributesComponent.h"


// Sets default values for this component's properties
UActionComponent::UActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UActionComponent::BeginPlay()
{
	Super::BeginPlay();

	//Add all actions in the default actions list to the component on begin play.
	for (TSubclassOf<UAction> ActionClass : DefaultActions)
	{
		AddAction(GetOwner(), ActionClass);
	}

	Cast<ABaseCharacter>(GetOwner())->GetAttributesComp()->OnStaminaDeplete.AddDynamic(this, &UActionComponent::OnStaminaDepleted);
}

void UActionComponent::OnStaminaDepleted()
{
	StopActionByName(GetOwner(), "SprintAction");
}


void UActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//stop all running actions
	TArray<UAction*> ActionCopy = Actions;
	for (UAction* Action : ActionCopy)
	{
		if (Action && Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}
	}
	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UActionComponent::AddAction(AActor* Instigator, TSubclassOf<UAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}
	UAction* NewAction = NewObject<UAction>(this, ActionClass);
	if (ensure(NewAction))
	{
		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
}

void UActionComponent::RemoveAction(UAction* ActionToRemove)
{
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning()))
	{
		return;
	}
	Actions.Remove(ActionToRemove);
}

 bool UActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	//SEPERATE FOR Main character CHILD CLASS. Currently we use this for the AI one but might have to move it to enemy action class at some point.

	for (UAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{

			if (!Action->CanStart(Instigator))
			{
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
				continue;
			}
			if (!Cast<ABaseCharacter>(Instigator)->GetAttributesComp()->IsAlive()) continue;
			//I only care about actions that block other actions. sprinting is kinda its own thing
			if (Action->GetIsActionQueueable()) CurrentRunningAction = Action;
			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}

inline void UActionComponent::InterruptAUXActions(AActor* Instigator)
{
	for (UAction* Action : Actions)
	{
		if (Action->IsRunning() && Action->GetAUXAction())
		{
			Action->InterruptAction(Instigator);
		}
	}
}



bool UActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (UAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning())
			{
				if(Action->GetIsActionQueueable())
				{
					CurrentRunningAction = nullptr;
					LastPerformedAction = Action;
				}
				Action->StopAction(Instigator);
				return true;
			}
		}
	}
	return false;
}

void UActionComponent::InterruptRunningActions(AActor* Instigator)
{
	for (UAction* Action : Actions)
	{
		if (Action->IsRunning())
		{
			Action->InterruptAction(Instigator);
		}
	}
}

//HELPER FUNCS

UAction* UActionComponent::GetLastPerformedAction()
{
	return LastPerformedAction;
}

UAction* UActionComponent::GetRunningAction()
{
	return CurrentRunningAction;
}
