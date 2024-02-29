// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/AIActions/SwordTreeActions/ReturnGunAction.h"
#include "AIController.h"
#include "Characters/EnemyCharacter.h"

UReturnGunAction::UReturnGunAction()
{
	ActionName = "ReturnAction";
}

void UReturnGunAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	EnemyCharacter= Cast<AEnemyCharacter>(Instigator);
	AAIController* MyController = Cast<AAIController>(Instigator->GetInstigatorController());
	if (ensure(MyController && EnemyCharacter))
	{
		if (EnemyCharacter->GetCharacterLegPosition() == ECharacterLegPosition::ECL_Right)
		{
			Cast<ABaseCharacter>(MyController->GetPawn())->PlayMontageSection(ReturnMontage, "ReturnR");
			EnemyCharacter->SetCharacterLegPosition(ECharacterLegPosition::ECL_Neutral);
		}
	
		else
		{
			Cast<ABaseCharacter>(MyController->GetPawn())->PlayMontageSection(ReturnMontage, "ReturnL");
			EnemyCharacter->SetCharacterLegPosition(ECharacterLegPosition::ECL_Neutral);

		}
	}
}

void UReturnGunAction::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}

void UReturnGunAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
}
