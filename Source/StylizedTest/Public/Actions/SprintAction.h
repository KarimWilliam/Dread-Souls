// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "SprintAction.generated.h"

/*
 * 
 */
UCLASS()
class STYLIZEDTEST_API USprintAction : public UAction
{
	GENERATED_BODY()
public:
	USprintAction();
	UFUNCTION()
	void SprintAcceleration();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;
	
private:
	class AMainCharacter* Character;
	class UCharacterMovementComponent* MovementComponent;
	UPROPERTY(EditAnywhere,meta=(ToolTip="Amount of time before sprinting kicks in measured in seconds",ClampMin="0.05"))
	float SprintDelay=0.1f;
	bool SprintInit=false;

	
	
};
