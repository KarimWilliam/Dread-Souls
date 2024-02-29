// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "SprintAttackAction.generated.h"

class AMainCharacter;
class ABaseWeapon;
UCLASS()
class STYLIZEDTEST_API USprintAttackAction : public UAction
{
	GENERATED_BODY()


	
public:
	USprintAttackAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;

protected:
	UPROPERTY()
	ABaseWeapon* EquippedWeapon;
	AMainCharacter* MC;

private:
	UFUNCTION()
	void OnMove(AActor* InstigatorActor);
	bool DoOnce=true;
	bool bUpdateDirection=true;
};
