// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "Components/TimelineComponent.h"
#include "TurnAction.generated.h"

class AMainCharacter;
class UTimelineComponent;
UCLASS()
class STYLIZEDTEST_API UTurnAction : public UAction
{
	GENERATED_BODY()

public:
	UTurnAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;
	
	UPROPERTY()
	AMainCharacter* Character;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> TurnMontage;

	
};
