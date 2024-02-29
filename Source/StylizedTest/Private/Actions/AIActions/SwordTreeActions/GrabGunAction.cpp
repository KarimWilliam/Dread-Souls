// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/AIActions/SwordTreeActions/GrabGunAction.h"
#include "AIController.h"
#include "Characters/EnemyCharacter.h"

UGrabGunAction::UGrabGunAction()
{
	ActionName="GrabAttackAction";
}

void UGrabGunAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	AAIController* MyController = Cast<AAIController>(Instigator->GetInstigatorController());
	if (ensure(MyController))
	{
		Cast<ABaseCharacter>(MyController->GetPawn())->PlayAnimMontage(GrabAttackMontage,1,"Grab");
	}

}

void UGrabGunAction::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);
}

void UGrabGunAction::InterruptAction(AActor* Instigator)
{
	Super::InterruptAction(Instigator);
}
