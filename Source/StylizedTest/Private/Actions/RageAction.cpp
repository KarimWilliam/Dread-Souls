// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/RageAction.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "Components/AttributesComponent.h"
#include "HUD/CombatOverlay.h"

URageAction::URageAction()
{
	ActionName = "RageAction";
	//ability1
}

void URageAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	AMainCharacter* MC = Cast<AMainCharacter>(Instigator);
	if (MC)
	{
		if (DoOnce)
		{
			MC->OnMove.AddDynamic(this, &URageAction::OnMove);
			DoOnce = false;
		}

		MC->DisableMovement();
		MC->ResetCombo();
		MC->PlayAnimMontage(RageMontage);
		FTimerDelegate Delegate2;
		Delegate2.BindUFunction(this, "StartRage", MC);
		GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, Delegate2, RageDelay, false, RageDelay);
	}
	
}

void URageAction::StopAction_Implementation(AActor* Instigator)
{
	AMainCharacter* Character = Cast<AMainCharacter>(Instigator);
	if (Character)
	{
		Character->EnableMovement();
		GetWorld()->GetTimerManager().ClearTimer(StartTimerHandle);
		static FGameplayTag Recovering = FGameplayTag::RequestGameplayTag("Recovering");
		Character->GetActionComp()->ActiveGamePlayTags.AddTag(Recovering);
		Character->GetAttributesComp()->BeginStaminaRegain();
	}

	Super::StopAction_Implementation(Instigator);
}

void URageAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
	AMainCharacter* MC = Cast<AMainCharacter>(Instigator);
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	GetWorld()->GetTimerManager().ClearTimer(StartTimerHandle);
}

bool URageAction::CanStart_Implementation(AActor* Instigator)
{
	AMainCharacter* MC = Cast<AMainCharacter>(Instigator);
	if (MC->GetAttributesComp()->GetAbility1ChargeProgress() != 1)
	{
		MC->UpdateMainOverlayAbility1Charge(0,false);
		return false;
	}
	return Super::CanStart_Implementation(Instigator);
}

void URageAction::EndRage(AMainCharacter* MC)
{
	MC->bIsRaging = false;
	MC->GetAttributesComp()->IsRaging=false;
	MC->GetCombatOverlay()->EnableAbility1(false);
	GetWorld()->GetTimerManager().ClearTimer(UpdateUI);
	MC->GetMesh()->GlobalAnimRateScale = 1;
	MC->ResolveSpeed();
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Raging"));

}

void URageAction::StartRage(AMainCharacter* MC)
{
	{
		MC->GetAttributesComp()->IsRaging=true;
		FTimerDelegate Delegate3;
		MC->GetActionComp()->ActiveGamePlayTags.AddTag(FGameplayTag::RequestGameplayTag("Raging"));
		Delegate3.BindUFunction(this, "EndRage", MC);
		GetWorld()->GetTimerManager().SetTimer(RageTimerHandle, Delegate3, RageDelay, false, RageDuration);
		MC->bIsRaging = true;
		MC->RageSpeedMultiplier = RageMoveSpeed;
		MC->GetMesh()->GlobalAnimRateScale = RageSpeed;
		float UpdateFreq = RageDuration / 0.01;
		ReduceAmountPerTick = 1 / UpdateFreq;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "UpdateRageUI", MC);
		GetWorld()->GetTimerManager().SetTimer(UpdateUI, Delegate, 0.01, true);
	}
}

void URageAction::UpdateRageUI(AMainCharacter* MainCharacter)
{
	MainCharacter->GetAttributesComp()->ModifyAbility1ChargeProgress(-ReduceAmountPerTick);
}

void URageAction::OnMove(AActor* InstigatorActor)
{
	AMainCharacter* MC = Cast<AMainCharacter>(InstigatorActor);
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	MC->StopMontage(RageMontage);
}
