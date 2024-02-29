// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/AIActions/SwordTreeActions/OverHeadAttackAction.h"
#include "AIController.h"
#include "Characters/EnemyCharacter.h"

UOverHeadAttackAction::UOverHeadAttackAction()
{
	ActionName="OverHeadAttackAction";
}

void UOverHeadAttackAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	AAIController* MyController = Cast<AAIController>(Instigator->GetInstigatorController());
	if (ensure(MyController))
	{
		Cast<ABaseCharacter>(MyController->GetPawn())->PlayAnimMontage(OverHeadAttackMontage);
	}

}

void UOverHeadAttackAction::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}

void UOverHeadAttackAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
}
