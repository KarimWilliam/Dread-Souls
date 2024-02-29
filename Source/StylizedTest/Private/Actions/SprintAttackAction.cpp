// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SprintAttackAction.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/BaseWeapon.h"

USprintAttackAction::USprintAttackAction()
{
	ActionName="SprintAttackAction";
	//important to set this so the lock tick knows to stop looking at a certain thing and let the attack finish in its running direction before changing the camera view
	bActionCanLockDirection = true;
}

void USprintAttackAction::StartAction_Implementation(AActor* Instigator)
{

	Super::StartAction_Implementation(Instigator);
	MC = Cast<AMainCharacter>(Instigator);
	if (MC)
	{
		if (DoOnce)
		{
			MC->OnMove.AddDynamic(this, &USprintAttackAction::OnMove);
			EquippedWeapon = MC->GetEquippedWeapon();
			DoOnce = false;
		}
		MC->DisableMovement();
		MC->ResetCombo();
		MC->PlayAnimMontage(EquippedWeapon->GetSprintAttackAnimMontage());
	}
}

void USprintAttackAction::StopAction_Implementation(AActor* Instigator)
{
	bUpdateDirection = true;
	if (MC)
	{
		static FGameplayTag Recovering = FGameplayTag::RequestGameplayTag("Recovering");
		MC->GetActionComp()->ActiveGamePlayTags.AddTag(Recovering);
		if (MC->bIsLockedOn)
		{
			MC->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		}
	}
	Super::StopAction_Implementation(Instigator);
}

void USprintAttackAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
	bUpdateDirection = true;
	MC->ResetCombo();
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	if (MC->bIsLockedOn)
	{
		MC->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
}

void USprintAttackAction::OnMove(AActor* InstigatorActor)
{
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	MC->StopMontage(EquippedWeapon->GetSprintAttackAnimMontage());
	MC->ResetCombo();
}
