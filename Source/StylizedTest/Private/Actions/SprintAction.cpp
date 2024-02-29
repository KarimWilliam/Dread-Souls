// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SprintAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ActionComponent.h"
#include "Characters/MainCharacter.h"
#include "Components/AttributesComponent.h"

USprintAction::USprintAction()
{
	ActionName = "SprintAction";
	bBypassBusy=true;
	bQueueableAction=false;
	StaminaConsumption=1.0f;
}

void USprintAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Sprint initiated"));
	SprintInit=false;
	Super::StartAction_Implementation(Instigator);
	Character = Cast<AMainCharacter>(Instigator);
	if (Character)
	{
		MovementComponent = Cast<UCharacterMovementComponent>(Character->GetMovementComponent());
	}

	FTimerDelegate SpeedDelegate;
	SpeedDelegate.BindUFunction(this, "SprintAcceleration");
	GetWorld()->GetTimerManager().SetTimer(Character->SprintTimerHandle, SpeedDelegate, SprintDelay, false);
}

void USprintAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Sprint Stopped"));
	GetWorld()->GetTimerManager().ClearTimer(Character->SprintTimerHandle);
	Character->SetCurrentMaxSpeed(Character->GetRunSpeed());
	static FGameplayTag BusyTag = FGameplayTag::RequestGameplayTag("Busy"); 
	if(!GetOwningComponent()->ActiveGamePlayTags.HasTag(BusyTag))
	{
		if (ensure(MovementComponent))MovementComponent->MaxWalkSpeed = Character->GetRunSpeed();
	}
	if(SprintInit)
	{
		Character->GetAttributesComp()->ImmediateStaminaRegain();
	}


	Super::StopAction_Implementation(Instigator);
}

void USprintAction::InterruptAction(AActor* Instigator)
{
	if(!bIsRunning)return;
	if(SprintInit)
	{
		Character->GetAttributesComp()->ImmediateStaminaRegain();
	}
	Super::InterruptAction(Instigator);
}

void USprintAction::SprintAcceleration()
{
	SprintInit=true;
	Character->SetCurrentMaxSpeed(Character->GetSprintSpeed());
	Character->UnlockTarget();
	UActionComponent* Comp = GetOwningComponent();
	static FGameplayTag BusyTag = FGameplayTag::RequestGameplayTag("Busy");
	if(!GrantsTags.HasTag(BusyTag) && !Comp->ActiveGamePlayTags.HasTag(BusyTag))
	{
		if (ensure(MovementComponent)) MovementComponent->MaxWalkSpeed = Character->GetSprintSpeed();
	}
}
