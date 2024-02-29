// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/AIActions/EnemyAction.h"
#include "StandUpGunAction.generated.h"

/**
 * 
 */
UCLASS()
class STYLIZEDTEST_API UStandUpGunAction : public UEnemyAction
{
	GENERATED_BODY()
	UStandUpGunAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> StandUpMontage;

	UPROPERTY()
	class UBlackboardComponent* BlackBoardComp;
};
