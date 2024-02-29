// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/AIActions/EnemyAction.h"
#include "BashAttackGunAction.generated.h"

/**
 * 
 */
UCLASS()
class STYLIZEDTEST_API UBashAttackGunAction : public UEnemyAction
{
	GENERATED_BODY()
	UBashAttackGunAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> BashAttackMontage;
	UPROPERTY()
	class AEnemyCharacter* EnemyCharacter; 
};
