// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SwordAbilityAction.h"
#include "Weapons/BaseWeapon.h"
#include "Components/ActionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Characters/MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

USwordAbilityAction::USwordAbilityAction()
{
	ActionName = "SwordAbilityAction";
	bActionCanLockDirection = true;
}

void USwordAbilityAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	if (AMainCharacter* MC = Cast<AMainCharacter>(Instigator))
	{
		if (DoOnce)
		{
			MC->OnMove.AddDynamic(this, &USwordAbilityAction::OnMove);
			DoOnce = false;
		}
		EquippedWeapon = MC->GetEquippedWeapon2();
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


		EquippedWeapon->GetMaxComboAmount() > MC->GetComboCounter() ? MC->SetComboCounter(MC->GetComboCounter() + 1) : MC->SetComboCounter(0);
		FTimerDelegate Delegate;
		IntendedDirection = MC->GetCapsuleComponent()->GetForwardVector();
		Delegate.BindUFunction(this, "AttackInDirection", MC, IntendedDirection);
		float InRate = 0.005;
		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, Delegate, InRate, true, LockDirectionTime);
	}
}

void USwordAbilityAction::StopAction_Implementation(AActor* Instigator)
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

void USwordAbilityAction::InterruptAction(AActor* Instigator)
{
	bUpdateDirection = true;
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	if (!IsRunning())return;
	AMainCharacter* MC = Cast<AMainCharacter>(Instigator);
	Super::InterruptAction(Instigator);
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	MC->ResetCombo();
	if (MC->bIsLockedOn)
	{
		MC->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
}

void USwordAbilityAction::AttackInDirection(AMainCharacter* MainCharacter, FVector Direction)

{
	if (bUpdateDirection)
	{
		bUpdateDirection = false;
		if (!MainCharacter->bIsLockedOn) IntendedDirection = MainCharacter->GetDirectionalIntent();
	}
	MainCharacter->AddMovementInput(IntendedDirection*0.001);
}

void USwordAbilityAction::OnMove(AActor* InstigatorActor)
{
	AMainCharacter* MC = Cast<AMainCharacter>(InstigatorActor);
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	MC->StopMontage(EquippedWeapon->GetAnimMontage());
	MC->ResetCombo();
}
