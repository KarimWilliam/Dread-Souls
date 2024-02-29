// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/HealAction.h"
#include "GameplayFunctionLibrary.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "Components/AttributesComponent.h"
#include "HUD/MainOverlay.h"
#include "GameFramework/CharacterMovementComponent.h"

UHealAction::UHealAction()
{
	ActionName = "HealAction";
}

void UHealAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	AMainCharacter* MC = Cast<AMainCharacter>(Instigator);
	if (MC)
	{
		float HealFreq = HealDuration / Period;
		HealAmountPerTick = HealAmount / HealFreq;
		if (DoOnce)
		{
			MC->OnMove.AddDynamic(this, &UHealAction::OnMove);
			DoOnce = false;
		}

		Cast<UCharacterMovementComponent>(MC->GetMovementComponent())->MaxWalkSpeed = MC->GetHealMoveSpeed();
		MC->EnableMovement();
		MC->ResetCombo();
		MC->PlayAnimMontage(HealMontage);
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "HealSelf", MC);
		GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, Delegate, HealDelay, false, HealDelay);
	}
}

void UHealAction::StopAction_Implementation(AActor* Instigator)
{
	AMainCharacter* Character = Cast<AMainCharacter>(Instigator);
	if (Character)
	{
		Character->EnableMovement();
		GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);
		GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
		static FGameplayTag HealRecover = FGameplayTag::RequestGameplayTag("HealRecover");
		Character->GetActionComp()->ActiveGamePlayTags.AddTag(HealRecover);
	}
	Super::StopAction_Implementation(Instigator);
}

void UHealAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
	if (!bIsRunning) return;
	GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);
	GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
	AMainCharacter* MC = Cast<AMainCharacter>(Instigator);
}

bool UHealAction::CanStart_Implementation(AActor* Instigator)
{
	AMainCharacter* MC = Cast<AMainCharacter>(Instigator);
	//if we have no potions return.
	if (MC->GetAttributesComp()->GetHeals() <= 0)
	{
		MC->UpdateMainOverlayHealText(false);
		return false;
	}
	return Super::CanStart_Implementation(Instigator);
}


void UHealAction::HealSelf(AMainCharacter* MainCharacter)
{
	//UGameplayFunctionLibrary::ApplyHealing(MainCharacter, MainCharacter, HealAmount);

	MainCharacter->GetAttributesComp()->SetHeals(MainCharacter->GetAttributesComp()->GetHeals() - 1);
	MainCharacter->UpdateMainOverlayHealText(true);
	if (Period > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ExecutePeriodicEffect", MainCharacter);

		GetWorld()->GetTimerManager().SetTimer(PeriodHandle, Delegate, Period, true);
	}
}

void UHealAction::OnMove(AActor* InstigatorActor)
{
	AMainCharacter* MC = Cast<AMainCharacter>(InstigatorActor);
	static FGameplayTag HealingRecover = FGameplayTag::RequestGameplayTag("HealRecover");
	static FGameplayTag Healing = FGameplayTag::RequestGameplayTag("Healing");
	if (MC->GetActionComp()->ActiveGamePlayTags.HasTag(HealingRecover) && !MC->GetActionComp()->ActiveGamePlayTags.HasTag(Healing))
	{
		MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(HealingRecover);
		MC->StopMontage(HealMontage);
		MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("HealRecover"));
	}
}

void UHealAction::ExecutePeriodicEffect_Implementation(AMainCharacter* MainCharacter)
{
	UGameplayFunctionLibrary::ApplyHealing(MainCharacter, MainCharacter, HealAmountPerTick);
}
