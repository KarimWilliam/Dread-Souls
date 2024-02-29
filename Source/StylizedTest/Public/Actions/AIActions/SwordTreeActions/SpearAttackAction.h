// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/AIActions/EnemyAction.h"
#include "SpearAttackAction.generated.h"

/**
 * 
 */
UCLASS()
class STYLIZEDTEST_API USpearAttackAction : public UEnemyAction
{
	GENERATED_BODY()
	USpearAttackAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> SpearAttackMontage;
	UPROPERTY()
	class AEnemyCharacter* EnemyCharacter; 
};
