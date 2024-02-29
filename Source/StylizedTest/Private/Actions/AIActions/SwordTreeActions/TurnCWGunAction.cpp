// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/AIActions/SwordTreeActions/TurnCWGunAction.h"
#include "AIController.h"
#include "Characters/EnemyCharacter.h"

UTurnCWGunAction::UTurnCWGunAction()
{
	ActionName="TurnCWAction";
}

void UTurnCWGunAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	AAIController* MyController = Cast<AAIController>(Instigator->GetInstigatorController());
	if (ensure(MyController))
	{
		Cast<ABaseCharacter>(MyController->GetPawn())->PlayAnimMontage(TurnCWMontage);
	}
}

void UTurnCWGunAction::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}

void UTurnCWGunAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
}
