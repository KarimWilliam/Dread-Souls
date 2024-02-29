// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "LightAttackAction.generated.h"

class ABaseWeapon;
class AMainCharacter;

UCLASS()
class STYLIZEDTEST_API ULightAttackAction : public UAction
{
	GENERATED_BODY()

public:
	ULightAttackAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;


protected:
	UPROPERTY()
	ABaseWeapon* EquippedWeapon;
	FTimerHandle AttackTimerHandle;
	UFUNCTION()
	void AttackInDirection(AMainCharacter* MainCharacter, FVector Direction);
private:
	UFUNCTION()
	void OnMove(AActor* InstigatorActor);
	bool DoOnce=true;
	UPROPERTY(EditDefaultsOnly,meta=(ToolTip="Time to pass before grabbing the attack direction",ClampMin=0))
	float LockDirectionTime=0.2f;
	bool bUpdateDirection=true;
	AMainCharacter* MC;
	FVector IntendedDirection;
};
