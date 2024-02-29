// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "DodgeRollAction.generated.h"

class AMainCharacter;
UCLASS()
class STYLIZEDTEST_API UDodgeRollAction : public UAction
{
	GENERATED_BODY()

public:
	UDodgeRollAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> DodgeMontage;
	AMainCharacter* MC;

private:
	UFUNCTION()
	void RollInDirection(AMainCharacter* MainCharacter, FVector Direction);
	FTimerHandle RollTimerHandle;
	bool DoOnce = true;
	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="Time to pass before grabbing the attack direction", ClampMin=0))
	float LockDirectionTime = 0.15f;
	bool bUpdateDirection = true;
	FVector IntendedDirection;
	float OriginalRotationRate;

	UFUNCTION()
void OnMove(AActor* InstigatorActor);
};
