// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/AIActions/EnemyAction.h"

#include "Characters/EnemyCharacter.h"
#include "Components/AttributesComponent.h"
#include "Components/EnemyActionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

bool UEnemyAction::CanStart_Implementation(AActor* Instigator)
{
	//allow the AI To start any action for now without checking //TODO DARK
	return true;
}

void UEnemyAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running %s"), *GetNameSafe(this))
	if (StaminaCost > 0) ConsumeStamina();
	AEnemyCharacter* EC = Cast<AEnemyCharacter>(Instigator);
	EC->SetLatestDamageMulti(ActionDamageMultiplier);
	EC->GetCharacterMovement()->RotationRate.Yaw = RotationSpeed;

	UEnemyActionComponent* Comp = Cast<UEnemyActionComponent>(GetOwningComponent());
	Comp->ActiveGamePlayTags.AppendTags(GrantsTags);

	bIsRunning = true;

	TimeStarted = GetWorld()->TimeSeconds;
	
	GetOwningComponent()->OnActionStarted.Broadcast(GetOwningComponent(), this);
}

void UEnemyAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running %s"), *GetNameSafe(this))
	if (IsRunning())
	{
		UEnemyActionComponent* Comp = Cast<UEnemyActionComponent>(GetOwningComponent());
		Comp->ActiveGamePlayTags.RemoveTags(GrantsTags);
		bIsRunning = false;
		AEnemyCharacter* EC = Cast<AEnemyCharacter>(Instigator);
		if (EC) EC->GetCharacterMovement()->RotationRate.Yaw = EC->MoveRotationSpeed;
		GetOwningComponent()->OnActionStopped.Broadcast(GetOwningComponent(), this);
	}
}

void UEnemyAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
}
