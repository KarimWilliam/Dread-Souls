// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_OverHeadAttack.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/AttributesComponent.h"
#include "Characters/EnemyCharacter.h"

UBTTask_OverHeadAttack::UBTTask_OverHeadAttack()
{
}


EBTNodeResult::Type UBTTask_OverHeadAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		ACharacter* MyPawn = Cast<AEnemyCharacter>(MyController->GetPawn());
		if (MyPawn == nullptr) return EBTNodeResult::Failed;

		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
		if (TargetActor == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		if (!UAttributesComponent::IsActorAlive(TargetActor))
		{
			return EBTNodeResult::Failed;
		}

		//TODO DARK Activate Attack montage
		Cast<ABaseCharacter>(MyController->GetPawn())->PlayAnimMontage(OverHeadAttackMontage);
		return Super::ExecuteTask(OwnerComp, NodeMemory);
	}

	return EBTNodeResult::Failed;
}
