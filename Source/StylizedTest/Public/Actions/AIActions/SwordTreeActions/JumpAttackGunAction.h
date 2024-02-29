// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/AIActions/EnemyAction.h"
#include "JumpAttackGunAction.generated.h"

/**
 * 
 */
UCLASS()
class STYLIZEDTEST_API UJumpAttackGunAction : public UEnemyAction
{
	GENERATED_BODY()
	UJumpAttackGunAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> JumpAttackMontage;
	UPROPERTY()
	class AEnemyCharacter* EnemyCharacter;

	FTimerHandle JumpTimerHandle;
	bool DoOnce = true;
	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="Delay Before Movement is applied", ClampMin=0))
	float JumpDelay = 2.5f;
	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="How long to move for", ClampMin=0))
	float JumpDuration = 0.9f;
	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="Max speed cm/s the ability can cover", ClampMin=0))
	float MaxSpeed = 1000.f;
	UPROPERTY()
	float JumpDurationTracker=JumpDuration;
	UPROPERTY()
	float DesiredSpeed=0;
	UPROPERTY()
	class UBlackboardComponent* BlackBoardComp;
	UFUNCTION()
	void JumpAbility();
	FVector DropZ = FVector(1, 1, 0);
	UPROPERTY()
	float InRate = 0.005;
};

