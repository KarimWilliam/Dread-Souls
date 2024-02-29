// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Action.h"

#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "Components/AttributesComponent.h"
#include "Components/QueueComponent.h"


bool UAction::CanStart_Implementation(AActor* Instigator)
{
	UActionComponent* Comp = GetOwningComponent();
	QueueComponent = Cast<AMainCharacter>(Comp->GetOwner())->GetQueueComp();

	//Actions that bypass being busy go here.
	if (bBypassBusy)
	{
		if (Comp->ActiveGamePlayTags.HasAny(BlockedTags) || !HaveEnoughStamina())
		{
			return false;
		}
		return true;
	}

	//If character is Busy move this action to the queue system.
	static FGameplayTag BusyTag = FGameplayTag::RequestGameplayTag("Busy");
	if (Comp->ActiveGamePlayTags.HasTag(BusyTag))
	{
		//If Action cant be queued simply do nothing
		if (!bQueueableAction) return false;

		QueueComponent->UpdateQueuedAction(this);
		return false;
	}
	if (Comp->ActiveGamePlayTags.HasAny(BlockedTags) || !HaveEnoughStamina())
	{
		return false;
	}

	QueueComponent->SetPreviousAction(this);
	return true;
}

void UAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running %s"), *GetNameSafe(this))

	AMainCharacter* MC= Cast<AMainCharacter>(Instigator);
	if (StaminaCost > 0) ConsumeStamina();


	UActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGamePlayTags.AppendTags(GrantsTags);

	bIsRunning = true;

	TimeStarted = GetWorld()->TimeSeconds;

	MC->SetAnimPauseDuration(ActionPauseDuration);
	MC->SetLatestDamageMulti(ActionDamageMultiplier);

	GetOwningComponent()->OnActionStarted.Broadcast(GetOwningComponent(), this);
}

void UAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running %s"), *GetNameSafe(this))
	if (IsRunning())
	{
		AMainCharacter* Character = Cast<AMainCharacter>(Instigator);
		if (Character && ActionName != "SprintAction")
		{
			Character->ResolveSpeed();
		}
		UActionComponent* Comp = GetOwningComponent();
		Comp->ActiveGamePlayTags.RemoveTags(GrantsTags);
		bIsRunning = false;
		//If Queue has a queued action, play it once the currently running action is finished.
		static FGameplayTag BusyTag = FGameplayTag::RequestGameplayTag("Busy");
		if (GrantsTags.HasTag(BusyTag))
		{
			QueueComponent->ActivateAction();
		}
		GetOwningComponent()->OnActionStopped.Broadcast(GetOwningComponent(), this);
	}
}


UActionComponent* UAction::GetOwningComponent() const
{
	return Cast<UActionComponent>(GetOuter());
}

bool UAction::HaveEnoughStamina()
{
	UActionComponent* Comp = GetOwningComponent();
	UAttributesComponent* AttributeComp = Cast<AMainCharacter>(Comp->GetOwner())->GetAttributesComp();

	if (AttributeComp->HasStamina() || (StaminaCost <= 0 && StaminaConsumption<=0))
	{
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Out of stamina!"));
	return false;
}

void UAction::ConsumeStamina()
{
	UActionComponent* Comp = GetOwningComponent();
	UAttributesComponent* AttributeComp = Cast<AMainCharacter>(Comp->GetOwner())->GetAttributesComp();

	if(ActionName!="SprintAction" || ActionName!="RageAction")
	{
		AttributeComp->ApplyStaminaChange(-StaminaCost,false);
	} else
	{
		AttributeComp->ApplyStaminaChange(-StaminaCost,true);
	}

}

FName UAction::GetActionName()
{
	return ActionName;
}

bool UAction::GetIsActionQueueable()
{
	return bQueueableAction;
}

void UAction::InterruptAction(AActor* Instigator)
{
}


bool UAction::IsRunning() const
{
	return bIsRunning;
}

UWorld* UAction::GetWorld() const
{
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if (Comp)
	{
		return Comp->GetWorld();
	}
	return nullptr;
}
