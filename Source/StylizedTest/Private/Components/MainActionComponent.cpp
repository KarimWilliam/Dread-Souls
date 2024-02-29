// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MainActionComponent.h"
#include "Characters/BaseCharacter.h"
#include "Characters/MainCharacter.h"
#include "Components/Action.h"
#include "Components/AttributesComponent.h"
#include "GameFramework/PawnMovementComponent.h"

bool UMainActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	ActionName = CheckIfSprintAttacking(Instigator, ActionName);
	AMainCharacter* MC = Cast<AMainCharacter>(Instigator);

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

			//Interrupt AUX Actions if an higher priority action comes along
			if (Action->GetGrantsTags().HasTag(FGameplayTag::RequestGameplayTag("Busy"))) InterruptAUXActions(Instigator);

			//reset idle timer if we are about to start an action
			MC->bIsIdle = false;
			GetWorld()->GetTimerManager().SetTimer(MC->IdleTimerHandle, MC->IdleDelegate, MC->LongIdleDelay, true, MC->LongIdleDelay);

			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}


FName UMainActionComponent::CheckIfSprintAttacking(AActor* Instigator, FName ActionName)
{
	AMainCharacter* MC = Cast<AMainCharacter>(Instigator);
	if ((ActionName == "LightAttackAction" || ActionName == "SwordAbilityAction") && ActiveGamePlayTags.
		HasTag(FGameplayTag::RequestGameplayTag("Sprinting")) && MC->GetMovementComponent()->Velocity.Size() > MC->GetRunSpeed() + 20)
	{
		if (CurrentRunningAction && CurrentRunningAction->ActionName == "DodgeRollAction")
		{
			return ActionName;
		}
		return "SprintAttackAction";
	}
	return ActionName;
}
