// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "SwordSweepAction.generated.h"

class ABaseWeapon;
class AMainCharacter;
UCLASS()
class STYLIZEDTEST_API USwordSweepAction : public UAction
{
	GENERATED_BODY()

public:
	USwordSweepAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;
	virtual bool CanStart_Implementation(AActor* Instigator) override;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> SwordSweepMontage;

protected:
	UPROPERTY()
	ABaseWeapon* EquippedWeapon;
	UPROPERTY()
	ABaseWeapon* EquippedWeapon2;
	float OGBaseBoxExtentY;
	float OGBoxLocationY;
	float OGBoxTraceEndY;
	UPROPERTY(EditDefaultsOnly)
	float SkillBaseBoxExtentY = 315.f;
	UPROPERTY(EditDefaultsOnly)
	float SkillBoxLocationY = 315.f;
	UPROPERTY(EditDefaultsOnly)
	float SkillBoxTraceEndY = 630.0;

	FTimerHandle AttackTimerHandle;
	AMainCharacter* MC;
	UFUNCTION()
	void AttackInDirection(AMainCharacter* MainCharacter, FVector Direction);

	class UNiagaraComponent* EffectInstance;

private:
	UFUNCTION()
	void OnMove(AActor* InstigatorActor);
	bool DoOnce = true;
	UPROPERTY(EditDefaultsOnly, meta=(ToolTip="Time to pass before grabbing the attack direction", ClampMin=0))
	float LockDirectionTime = 0.2f;
	bool bUpdateDirection = true;
	FVector IntendedDirection;
};
