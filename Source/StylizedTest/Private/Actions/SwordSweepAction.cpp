// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SwordSweepAction.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributesComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/BaseWeapon.h"

USwordSweepAction::USwordSweepAction()
{
	ActionName = "SwordSweepAction";
	//important to set this so the lock tick knows to stop looking at a certain thing and let the attack finish in its running direction before changing the camera view
	bActionCanLockDirection = true;
}

void USwordSweepAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("SwordSweep Started"));
	Super::StartAction_Implementation(Instigator);
	MC = Cast<AMainCharacter>(Instigator);
	if (MC)
	{
		if (DoOnce)
		{
			MC->OnMove.AddDynamic(this, &USwordSweepAction::OnMove);
			EquippedWeapon = MC->GetEquippedWeapon();
			EquippedWeapon2=MC->GetEquippedWeapon2();
			DoOnce = false;
			// Save the original weapon hitbox
			OGBaseBoxExtentY = EquippedWeapon->GetWeaponBox()->GetScaledBoxExtent().Y;
			OGBoxLocationY = EquippedWeapon->GetWeaponBox()->GetRelativeLocation().Y;
			OGBoxTraceEndY = EquippedWeapon->GetBoxTraceEnd()->GetRelativeLocation().Y;
		}
		MC->DisableMovement();
		MC->ResetCombo();
		MC->PlayAnimMontage(SwordSweepMontage);

		//Set and Activate the VFX effect
		EquippedWeapon->WeaponSkillEffectComponent->SetAsset(EquippedWeapon->GetWeaponSkillVfx(),false);
		EquippedWeapon->WeaponSkillEffectComponent->SetRelativeLocation(EquippedWeapon->GetSkillVfxLocation());
		EquippedWeapon->WeaponSkillEffectComponent->SetRelativeRotation(EquippedWeapon->GetSkillVfxRotation());
		EquippedWeapon->WeaponSkillEffectComponent->Activate(true);

		EquippedWeapon2->WeaponSkillEffectComponent->SetAsset(EquippedWeapon->GetWeaponSkillVfx(),false);
		EquippedWeapon2->WeaponSkillEffectComponent->SetRelativeLocation(EquippedWeapon->GetSkillVfxLocation());
		EquippedWeapon2->WeaponSkillEffectComponent->SetRelativeRotation(EquippedWeapon->GetSkillVfxRotation());
		EquippedWeapon2->WeaponSkillEffectComponent->Activate(true);
	}
	FTimerDelegate Delegate;
	IntendedDirection = MC->GetCapsuleComponent()->GetForwardVector();
	Delegate.BindUFunction(this, "AttackInDirection", MC, IntendedDirection);
	float InRate = 0.005;
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, Delegate, InRate, true, LockDirectionTime);
	MC->GetAttributesComp()->ModifyAbility2ChargeProgress(-1);

	//make the weapon hit box larger
	EquippedWeapon->SetWeaponBoxExtentY(SkillBaseBoxExtentY);
	EquippedWeapon->SetBoxLocationY(SkillBoxLocationY);
	EquippedWeapon->SetBoxTraceEndY(SkillBoxTraceEndY);

}

void USwordSweepAction::StopAction_Implementation(AActor* Instigator)
{
	bUpdateDirection = true;
	if (MC)
	{
		EquippedWeapon->WeaponSkillEffectComponent->DestroyInstance();
		EquippedWeapon2->WeaponSkillEffectComponent->DestroyInstance();
		EquippedWeapon->SetWeaponBoxExtentY(OGBaseBoxExtentY);
		EquippedWeapon->SetBoxLocationY(OGBoxLocationY);
		EquippedWeapon->SetBoxTraceEndY(OGBoxTraceEndY);
		GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
		static FGameplayTag Recovering = FGameplayTag::RequestGameplayTag("Recovering");
		MC->GetActionComp()->ActiveGamePlayTags.AddTag(Recovering);
		if (MC->bIsLockedOn)
		{
			MC->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		}
	}
	Super::StopAction_Implementation(Instigator);
}

void USwordSweepAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
	EquippedWeapon->WeaponSkillEffectComponent->DestroyInstance();
	EquippedWeapon2->WeaponSkillEffectComponent->DestroyInstance();
	EquippedWeapon->SetWeaponBoxExtentY(OGBaseBoxExtentY);
	EquippedWeapon->SetBoxLocationY(OGBoxLocationY);
	EquippedWeapon->SetBoxTraceEndY(OGBoxTraceEndY);
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	bUpdateDirection = true;
	MC->ResetCombo();
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	if (MC->bIsLockedOn)
	{
		MC->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
}

bool USwordSweepAction::CanStart_Implementation(AActor* Instigator)
{
	AMainCharacter* Mc = Cast<AMainCharacter>(Instigator);
	if (Mc->GetAttributesComp()->GetAbility2ChargeProgress() != 1)
	{
		Mc->UpdateMainOverlayAbility2Charge(0, false);
		return false;
	}
	return Super::CanStart_Implementation(Instigator);
}

void USwordSweepAction::AttackInDirection(AMainCharacter* MainCharacter, FVector Direction)
{
	if (bUpdateDirection)
	{
		bUpdateDirection = false;
		if (!MainCharacter->bIsLockedOn) IntendedDirection = MainCharacter->GetDirectionalIntent();
	}
	MainCharacter->AddMovementInput(IntendedDirection * 0.001);
}

void USwordSweepAction::OnMove(AActor* InstigatorActor)
{
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	EquippedWeapon->SetWeaponBoxExtentY(OGBaseBoxExtentY);
	EquippedWeapon->SetBoxLocationY(OGBoxLocationY);
	EquippedWeapon->SetBoxTraceEndY(OGBoxTraceEndY);
	EquippedWeapon->WeaponSkillEffectComponent->DestroyInstance();
	EquippedWeapon2->WeaponSkillEffectComponent->DestroyInstance();
	MC->StopMontage(SwordSweepMontage);
	MC->ResetCombo();
}
