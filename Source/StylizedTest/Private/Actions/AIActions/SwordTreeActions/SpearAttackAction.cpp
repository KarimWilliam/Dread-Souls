// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/AIActions/SwordTreeActions/SpearAttackAction.h"
#include "Characters/EnemyCharacter.h"
#include "GenericStructs.h"
#include "AIController.h"

USpearAttackAction::USpearAttackAction()
{
	ActionName="SpearAttackAction";
}

void USpearAttackAction::StartAction_Implementation(AActor* Instigator)
{

	Super::StartAction_Implementation(Instigator);
	EnemyCharacter= Cast<AEnemyCharacter>(Instigator);
	AAIController* MyController = Cast<AAIController>(Instigator->GetInstigatorController());
	if (ensure(MyController))
	{

		if(EnemyCharacter->GetCharacterLegPosition()==ECharacterLegPosition::ECL_Right)
		{
			Cast<ABaseCharacter>(MyController->GetPawn())->PlayMontageSection(SpearAttackMontage,"AttackL");
			EnemyCharacter->SetCharacterLegPosition(ECharacterLegPosition::ECL_Left);
			UE_LOG(LogTemp, Warning, TEXT("Left forward attack"));
		} else if (EnemyCharacter->GetCharacterLegPosition()==ECharacterLegPosition::ECL_Left)
		{
			Cast<ABaseCharacter>(MyController->GetPawn())->PlayMontageSection(SpearAttackMontage,"AttackR");
			EnemyCharacter->SetCharacterLegPosition(ECharacterLegPosition::ECL_Right);
			UE_LOG(LogTemp, Warning, TEXT("Right forward attack"));
		}else
		{
			Cast<ABaseCharacter>(MyController->GetPawn())->PlayMontageSection(SpearAttackMontage,"AttackN");
			EnemyCharacter->SetCharacterLegPosition(ECharacterLegPosition::ECL_Right);
			UE_LOG(LogTemp, Warning, TEXT("Neutral attack"));

		}

	}
}

void USpearAttackAction::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}

void USpearAttackAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
}
