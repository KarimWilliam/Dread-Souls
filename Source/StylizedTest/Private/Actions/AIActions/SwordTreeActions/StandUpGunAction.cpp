// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/AIActions/SwordTreeActions/StandUpGunAction.h"
#include "AIController.h"
#include "Characters/EnemyCharacter.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UStandUpGunAction::UStandUpGunAction()
{
	ActionName = "StandUpAction";
}

void UStandUpGunAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	AAIController* MyController = Cast<AAIController>(Instigator->GetInstigatorController());
	if (ensure(MyController))
	{
		Cast<ABaseCharacter>(MyController->GetPawn())->PlayAnimMontage(StandUpMontage);
	}
	AEnemyCharacter* EC = Cast<AEnemyCharacter>(Instigator);
	BlackBoardComp = Cast<AEnemyAIController>(EC->GetController())->GetBlackboardComponent();
	EC->InitializeBossOverlay();

	BlackBoardComp->SetValueAsBool("PerformStandupOnce",true);
}

void UStandUpGunAction::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}

void UStandUpGunAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
}
