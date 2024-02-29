// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SideStepAction.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

USideStepAction::USideStepAction()
{
	ActionName = "SideStepAction";
	bActionCanLockDirection = true;
}

void USideStepAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("SideStep Started"));
	Super::StartAction_Implementation(Instigator);
	if (DoOnce)
	{
		MC = Cast<AMainCharacter>(Instigator);
		MC->OnMove.AddDynamic(this, &USideStepAction::OnMove);
		DoOnce = false;
	}

	if (MC)
	{
		OriginalRotationRate = MC->GetCharacterMovement()->RotationRate.Yaw;
		MC->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		MC->ResetCombo();
		MC->PlayAnimMontage(SideStepMontage);
		MC->DisableMovement();

		FVector Direction = MC->GetDirectionalIntent();
		MC->GetCapsuleComponent()->SetWorldRotation(Direction.Rotation());

		FTimerDelegate Delegates;
		IntendedDirection = MC->GetCapsuleComponent()->GetForwardVector();
		Delegates.BindUFunction(this, "StepInDirection", MC, IntendedDirection);
		float InRate = 0.005;
		GetWorld()->GetTimerManager().SetTimer(StepTimerHandle, Delegates, InRate, true, LockDirectionTime);
	}
}

void USideStepAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("SideStep stopped"));
	//MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	MC->StopMontage(SideStepMontage);
	MC->ResetCombo();
	bUpdateDirection = true;

	GetWorld()->GetTimerManager().ClearTimer(StepTimerHandle);
	if (MC)
	{
		MC->GetCharacterMovement()->RotationRate.Yaw = OriginalRotationRate;
		MC->EnableMovement();
		if (MC->bIsLockedOn)
		{
			MC->GetCharacterMovement()->bUseControllerDesiredRotation = true;
			MC->GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}
	Super::StopAction_Implementation(Instigator);
}

void USideStepAction::InterruptAction(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Step interrupted"));
	bUpdateDirection = true;
	GetWorld()->GetTimerManager().ClearTimer(StepTimerHandle);
	if (!bIsRunning)return;
	MC->GetCharacterMovement()->RotationRate.Yaw = OriginalRotationRate;
	if (MC->bIsLockedOn)
	{
		MC->GetCharacterMovement()->bOrientRotationToMovement = false;
		MC->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
	Super::InterruptAction(Instigator);
}

void USideStepAction::StepInDirection(AMainCharacter* MainCharacter, FVector Direction)
{
	if (bUpdateDirection)
	{
		bUpdateDirection = false;
		MainCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		MainCharacter->GetCharacterMovement()->RotationRate.Yaw = -1;
		IntendedDirection = MainCharacter->GetDirectionalIntent();
	}
	MainCharacter->AddMovementInput(IntendedDirection);
}

void USideStepAction::OnMove(AActor* InstigatorActor)
{
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	MC->StopMontage(SideStepMontage);
	GetWorld()->GetTimerManager().ClearTimer(StepTimerHandle);
	MC->ResetCombo();
}
