// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/AIActions/SwordTreeActions/JumpAttackGunAction.h"
#include "AIController.h"
#include "AI/EnemyAIController.h"
#include "Characters/EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"


UJumpAttackGunAction::UJumpAttackGunAction()
{
	ActionName = "JumpAttackAction";
}

void UJumpAttackGunAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	EnemyCharacter=Cast<AEnemyCharacter>(Instigator);
	AAIController* MyController = Cast<AAIController>(Instigator->GetInstigatorController());
	if (ensure(MyController))
	{
		Cast<ABaseCharacter>(MyController->GetPawn())->PlayAnimMontage(JumpAttackMontage);
	}

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "JumpAbility");

	GetWorld()->GetTimerManager().SetTimer(JumpTimerHandle, Delegate, InRate, true, JumpDelay);
}

void UJumpAttackGunAction::StopAction_Implementation(AActor* Instigator)
{
	JumpDurationTracker=JumpDuration;
	GetWorld()->GetTimerManager().ClearTimer(JumpTimerHandle);
	Super::StopAction_Implementation(Instigator);
}

void UJumpAttackGunAction::InterruptAction(AActor* Instigator)
{
	JumpDurationTracker=JumpDuration;
	GetWorld()->GetTimerManager().ClearTimer(JumpTimerHandle);
	Super::InterruptAction(Instigator);
}

void UJumpAttackGunAction::JumpAbility()
{
	//UE_LOG(LogTemp, Warning, TEXT("Jump Duration Tracker: %f"),TargetDistance);
	JumpDurationTracker=JumpDurationTracker-InRate;
	if(JumpDurationTracker<=0)
	{
		GetWorld()->GetTimerManager().ClearTimer(JumpTimerHandle);
	}

	if (EnemyCharacter)
	{
		if (DoOnce)
		{
			BlackBoardComp = Cast<AEnemyAIController>(EnemyCharacter->GetController())->GetBlackboardComponent();
		}
		float  TargetDistance = BlackBoardComp->GetValueAsFloat("DistanceToTarget");
		//How much distance in cm i want to cover per second 
		DesiredSpeed=FMath::Clamp(TargetDistance/JumpDurationTracker,0,MaxSpeed);
		EnemyCharacter->SetActorLocation(EnemyCharacter->GetActorLocation()+EnemyCharacter->GetActorForwardVector()*DesiredSpeed*InRate,true);
		//EnemyCharacter->AddMovementInput(EnemyCharacter->GetActorForwardVector());
	}

	DoOnce = false;
	
}
