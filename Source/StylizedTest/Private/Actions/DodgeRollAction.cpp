// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/DodgeRollAction.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "Components/AttributesComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UDodgeRollAction::UDodgeRollAction()
{
	ActionName = "DodgeRollAction";
	bActionCanLockDirection = true;
}

void UDodgeRollAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge Started"));
	Super::StartAction_Implementation(Instigator);
	MC = Cast<AMainCharacter>(Instigator);
	if (MC)
	{
		OriginalRotationRate = MC->GetCharacterMovement()->RotationRate.Yaw;
		MC->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		MC->ResetCombo();
		MC->PlayAnimMontage(DodgeMontage);
		MC->DisableMovement();

		FVector Direction = MC->GetDirectionalIntent();
		MC->GetCapsuleComponent()->SetWorldRotation(Direction.Rotation());

		FTimerDelegate Delegate;
		IntendedDirection = MC->GetCapsuleComponent()->GetForwardVector();
		Delegate.BindUFunction(this, "RollInDirection", MC, IntendedDirection);
		float InRate = 0.005;
		GetWorld()->GetTimerManager().SetTimer(RollTimerHandle, Delegate, InRate, true, LockDirectionTime);
	}
}

void UDodgeRollAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge stopped"));
	bUpdateDirection = true;

	GetWorld()->GetTimerManager().ClearTimer(RollTimerHandle);
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

void UDodgeRollAction::InterruptAction(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge interrupted"));
	bUpdateDirection = true;
	GetWorld()->GetTimerManager().ClearTimer(RollTimerHandle);
	if (!bIsRunning)return;
	MC->GetCharacterMovement()->RotationRate.Yaw = OriginalRotationRate;
	if (MC->bIsLockedOn)
	{
		MC->GetCharacterMovement()->bOrientRotationToMovement = false;
		MC->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
	Super::InterruptAction(Instigator);
}

void UDodgeRollAction::RollInDirection(AMainCharacter* MainCharacter, FVector Direction)
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

void UDodgeRollAction::OnMove(AActor* InstigatorActor)
{
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Recovering"));
	MC->StopMontage(DodgeMontage);
	MC->ResetCombo();

}
