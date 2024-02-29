// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SpawnAction.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"

USpawnAction::USpawnAction()
{
	ActionName="SpawnAction";
	bActionCanLockDirection = true;
}

void USpawnAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	MC = Cast<AMainCharacter>(Instigator);
	if (MC)
	{
		MC->PlayAnimMontage(SpawnMontage);
		MC->DisableMovement();
	}
}

void USpawnAction::StopAction_Implementation(AActor* Instigator)
{

	MC->EnableMovement();
	Super::StopAction_Implementation(Instigator);
}

void USpawnAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
}
