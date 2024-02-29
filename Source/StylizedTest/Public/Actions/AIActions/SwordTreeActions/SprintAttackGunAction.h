// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/AIActions/EnemyAction.h"
#include "SprintAttackGunAction.generated.h"

/**
 * 
 */
UCLASS()
class STYLIZEDTEST_API USprintAttackGunAction : public UEnemyAction
{
	GENERATED_BODY()
	USprintAttackGunAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> SprintAttackMontage;
	UPROPERTY()
	class AEnemyCharacter* EnemyCharacter;

	FTimerHandle DashTimerHandle;
	bool DoOnce = true;
	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="Delay Before Movement is applied", ClampMin=0))
	float DashDelay = 1.f;
	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="How long to move for", ClampMin=0))
	float DashDuration = 0.5f;
	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="Max speed cm/s the ability can cover", ClampMin=0))
	float MaxSpeed = 1500.f;
	UPROPERTY()
	float DashDurationTracker=DashDuration;
	UPROPERTY()
	float DesiredSpeed=0;
	UPROPERTY()
	class UBlackboardComponent* BlackBoardComp;
	UFUNCTION()
	void DashAbility();
	FVector DropZ = FVector(1, 1, 0);
	UPROPERTY()
	float InRate = 0.005;
};
