// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_OverHeadAttack.generated.h"

/**
 * 
 */
UCLASS()
class STYLIZEDTEST_API UBTTask_OverHeadAttack : public UBTTaskNode
{
	GENERATED_BODY()
		
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	/* Max Random Bullet Spread (in Degrees) in positive and negative angle (shared between Yaw and Pitch) */
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* OverHeadAttackMontage;



public:
	
	UBTTask_OverHeadAttack();
};