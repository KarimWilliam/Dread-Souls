// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/AIActions/SwordTreeActions/BashAttackGunAction.h"
#include "Characters/EnemyCharacter.h"
#include "GenericStructs.h"
#include "AIController.h"

UBashAttackGunAction::UBashAttackGunAction()
{
	ActionName="BashAttackAction";
}

void UBashAttackGunAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	EnemyCharacter= Cast<AEnemyCharacter>(Instigator);
	AAIController* MyController = Cast<AAIController>(Instigator->GetInstigatorController());
	if (ensure(MyController))
	{
		Cast<ABaseCharacter>(MyController->GetPawn())->PlayAnimMontage(BashAttackMontage);
		EnemyCharacter->SetCharacterLegPosition(ECharacterLegPosition::ECL_Neutral);
	}
}

void UBashAttackGunAction::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}

void UBashAttackGunAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
}
