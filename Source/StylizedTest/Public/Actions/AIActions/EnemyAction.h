// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "EnemyAction.generated.h"

/**
 * 
 */
UCLASS()
class STYLIZEDTEST_API UEnemyAction : public UAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	float RotationSpeed=200.f;

	
virtual bool CanStart_Implementation(AActor* Instigator) override;
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;
};
