// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "SideStepAction.generated.h"

class AMainCharacter;
UCLASS()
class STYLIZEDTEST_API USideStepAction : public UAction
{
	GENERATED_BODY()

public:
	USideStepAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> SideStepMontage;
	AMainCharacter* MC;

private:
	UFUNCTION()
	void StepInDirection(AMainCharacter* MainCharacter, FVector Direction);
	FTimerHandle StepTimerHandle;
	bool DoOnce = true;
	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="Time to pass before grabbing the attack direction", ClampMin=0))
	float LockDirectionTime = 0.02f;
	bool bUpdateDirection = true;
	FVector IntendedDirection;
	float OriginalRotationRate;

	UFUNCTION()
	void OnMove(AActor* InstigatorActor);
};
