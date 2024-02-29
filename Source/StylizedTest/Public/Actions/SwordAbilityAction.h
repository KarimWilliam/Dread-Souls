// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "SwordAbilityAction.generated.h"


UCLASS()
class STYLIZEDTEST_API USwordAbilityAction : public UAction
{
	GENERATED_BODY()


public:
	USwordAbilityAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;

	UPROPERTY()
	class ABaseWeapon* EquippedWeapon;

	FTimerHandle AttackTimerHandle;
	UFUNCTION()
	void AttackInDirection(class AMainCharacter* MainCharacter, FVector Direction);
	UFUNCTION()
	void OnMove(AActor* InstigatorActor);
	bool DoOnce = true;

private:
	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="Time to pass before grabbing the attack direction", ClampMin=0))
	float LockDirectionTime = 0.2f;
	bool bUpdateDirection = true;
	FVector IntendedDirection;
};
