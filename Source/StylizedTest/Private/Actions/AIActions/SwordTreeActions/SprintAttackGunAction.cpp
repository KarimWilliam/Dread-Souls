// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/AIActions/SwordTreeActions/SprintAttackGunAction.h"
#include "Characters/EnemyCharacter.h"
#include "GenericStructs.h"
#include "AIController.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

USprintAttackGunAction::USprintAttackGunAction()
{
	ActionName="SprintAttackAction";
}

void USprintAttackGunAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	EnemyCharacter= Cast<AEnemyCharacter>(Instigator);
	AAIController* MyController = Cast<AAIController>(Instigator->GetInstigatorController());
	if (ensure(MyController))
	{
		Cast<ABaseCharacter>(MyController->GetPawn())->PlayAnimMontage(SprintAttackMontage);
		EnemyCharacter->SetCharacterLegPosition(ECharacterLegPosition::ECL_Neutral);
	}
	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "DashAbility");

	GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, Delegate, InRate, true, DashDelay);
}

void USprintAttackGunAction::StopAction_Implementation(AActor* Instigator)
{
	DashDurationTracker=DashDuration;
	GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
	Super::StopAction_Implementation(Instigator);
}

void USprintAttackGunAction::InterruptAction(AActor* Instigator)
{
	DashDurationTracker=DashDuration;
	GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
	Super::InterruptAction(Instigator);
}

void USprintAttackGunAction::DashAbility()
{
	//UE_LOG(LogTemp, Warning, TEXT("Jump Duration Tracker: %f"),TargetDistance);
	DashDurationTracker=DashDurationTracker-InRate;
	if(DashDurationTracker<=0)
	{
		GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
	}

	if (EnemyCharacter)
	{
		if (DoOnce)
		{
			BlackBoardComp = Cast<AEnemyAIController>(EnemyCharacter->GetController())->GetBlackboardComponent();
		}
		float  TargetDistance = BlackBoardComp->GetValueAsFloat("DistanceToTarget");
		//How much distance in cm i want to cover per second 
		DesiredSpeed=FMath::Clamp(TargetDistance/DashDurationTracker,0,MaxSpeed);
		EnemyCharacter->SetActorLocation(EnemyCharacter->GetActorLocation()+EnemyCharacter->GetActorForwardVector()*DesiredSpeed*InRate,true);
	}

	DoOnce = false;
}
