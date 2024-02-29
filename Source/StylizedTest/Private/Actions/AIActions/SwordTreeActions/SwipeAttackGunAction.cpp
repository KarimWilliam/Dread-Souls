// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/AIActions/SwordTreeActions/SwipeAttackGunAction.h"
#include "Characters/EnemyCharacter.h"
#include "GenericStructs.h"
#include "AIController.h"


USwipeAttackGunAction::USwipeAttackGunAction()
{
	ActionName="SwipeAttackAction";
}

void USwipeAttackGunAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	EnemyCharacter= Cast<AEnemyCharacter>(Instigator);
	AAIController* MyController = Cast<AAIController>(Instigator->GetInstigatorController());
	if (ensure(MyController))
	{

		if(EnemyCharacter->GetCharacterLegPosition()==ECharacterLegPosition::ECL_Right)
		{
			Cast<ABaseCharacter>(MyController->GetPawn())->PlayMontageSection(SwipeAttackMontage,"AttackL");
			EnemyCharacter->SetCharacterLegPosition(ECharacterLegPosition::ECL_Left);
			UE_LOG(LogTemp, Warning, TEXT("Left forward attack"));
		} else if (EnemyCharacter->GetCharacterLegPosition()==ECharacterLegPosition::ECL_Left)
		{
			Cast<ABaseCharacter>(MyController->GetPawn())->PlayMontageSection(SwipeAttackMontage,"AttackR");
			EnemyCharacter->SetCharacterLegPosition(ECharacterLegPosition::ECL_Right);
			UE_LOG(LogTemp, Warning, TEXT("Right forward attack"));
		}else
		{
			Cast<ABaseCharacter>(MyController->GetPawn())->PlayMontageSection(SwipeAttackMontage,"AttackN");
			EnemyCharacter->SetCharacterLegPosition(ECharacterLegPosition::ECL_Right);
			UE_LOG(LogTemp, Warning, TEXT("Neutral attack"));

		}

	}
}

void USwipeAttackGunAction::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}

void USwipeAttackGunAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
}
