// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/LightAttackAction.h"

#include "TimerManager.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributesComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/BaseWeapon.h"

ULightAttackAction::ULightAttackAction()
{
	ActionName = "LightAttackAction";
	//important to set this so the lock tick knows to stop looking at a certain thing and let the attack finish in its running direction before changing the camera view
	bActionCanLockDirection = true;
}

void ULightAttackAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (DoOnce)
	{
		MC = Cast<AMainCharacter>(Instigator);
		MC->OnMove.AddDynamic(this, &ULightAttackAction::OnMove);
		EquippedWeapon = MC->GetEquippedWeapon();
		DoOnce = false;
	}
	MC->DisableMovement();

	switch (MC->GetComboCounter())
	{
	case 0: MC->PlayMontageSection(EquippedWeapon->GetAnimMontage(), "Attack0");
		break;
	case 1: MC->PlayMontageSection(EquippedWeapon->GetAnimMontage(), "Attack1");
		break;
	case 2: MC->PlayMontageSection(EquippedWeapon->GetAnimMontage(), "Attack2");
		break;
	case 3: MC->PlayMontageSection(EquippedWeapon->GetAnimMontage(), "Attack3");
		break;
	default: MC->PlayMontageSection(EquippedWeapon->GetAnimMontage(), "Attack0");
		break;
	}


	//increment the character's combo counter if the weapon has more combo attacks after the current one.
	EquippedWeapon->GetMaxComboAmount() > MC->GetComboCounter() ? MC->SetComboCounter(MC->GetComboCounter() + 1) : MC->SetComboCounter(0);
	FTimerDelegate Delegate;
	IntendedDirection = MC->GetCapsuleComponent()->GetForwardVector();
	Delegate.BindUFunction(this, "AttackInDirection", MC, IntendedDirection);
	float InRate = 0.005;
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, Delegate, InRate, true, LockDirectionTime);
}

void ULightAttackAction::StopAction_Implementation(AActor* Instigator)
{
	bUpdateDirection = true;
	AMainCharacter* Character = Cast<AMainCharacter>(Instigator);
	if (Character)
	{
		//Enabling movement in handeled in animation blueprint
		GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
		static FGameplayTag Recovering = FGameplayTag::RequestGameplayTag("Recovering");
		Character->GetActionComp()->ActiveGamePlayTags.AddTag(Recovering);
		if (Character->bIsLockedOn)
		{
			Character->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		}
	}
	Super::StopAction_Implementation(Instigator);
}

void ULightAttackAction::InterruptAction(AActor* Instigator)
{
	bUpdateDirection = true;
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	Super::InterruptAction(Instigator);
	if (MC)
	{
		MC->ResetCombo();
		MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	}

	if (MC->bIsLockedOn)
	{
		MC->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
}

void ULightAttackAction::AttackInDirection(AMainCharacter* MainCharacter, FVector Direction)

{
	if (bUpdateDirection)
	{
		bUpdateDirection = false;
		if (!MainCharacter->bIsLockedOn) { IntendedDirection = MainCharacter->GetDirectionalIntent(); }
	}
	MainCharacter->AddMovementInput(IntendedDirection * 0.001);
}

void ULightAttackAction::OnMove(AActor* InstigatorActor)
{
	if (MC)
	{
		MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
		MC->StopMontage(EquippedWeapon->GetAnimMontage());
		MC->ResetCombo();
	}
}
