// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/DodgeRollAttackAction.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/BaseWeapon.h"

UDodgeRollAttackAction::UDodgeRollAttackAction()
{
	ActionName = "DodgeRollAttackAction";
	//important to set this so the lock tick knows to stop looking at a certain thing and let the attack finish in its running direction before changing the camera view
	bActionCanLockDirection = true;
}

void UDodgeRollAttackAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	MC = Cast<AMainCharacter>(Instigator);
	if (MC)
	{
		if (DoOnce)
		{
			MC->OnMove.AddDynamic(this, &UDodgeRollAttackAction::OnMove);
			EquippedWeapon = MC->GetEquippedWeapon();
			DoOnce = false;
		}
		MC->DisableMovement();
		MC->ResetCombo();
		MC->PlayAnimMontage(EquippedWeapon->GetRollAttackAnimMontage());

		FTimerDelegate Delegate;
		IntendedDirection = MC->GetCapsuleComponent()->GetForwardVector();
		Delegate.BindUFunction(this, "AttackInDirection", MC, IntendedDirection);
		float InRate = 0.005;
		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, Delegate, InRate, true, LockDirectionTime);
	}
}


void UDodgeRollAttackAction::StopAction_Implementation(AActor* Instigator)
{
	bUpdateDirection = true;
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
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

void UDodgeRollAttackAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
	bUpdateDirection = true;
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	MC->ResetCombo();
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	if (MC->bIsLockedOn)
	{
		MC->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
}

void UDodgeRollAttackAction::OnMove(AActor* InstigatorActor)
{
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	MC->StopMontage(EquippedWeapon->GetRollAttackAnimMontage());
	MC->ResetCombo();
}

void UDodgeRollAttackAction::AttackInDirection(AMainCharacter* MainCharacter, FVector Direction)
{
	if (bUpdateDirection)
	{
		bUpdateDirection = false;
		if (!MainCharacter->bIsLockedOn) IntendedDirection = MainCharacter->GetDirectionalIntent();
	}
	MainCharacter->AddMovementInput(IntendedDirection*0.001);
}
