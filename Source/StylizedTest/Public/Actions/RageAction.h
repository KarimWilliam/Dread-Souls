// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "RageAction.generated.h"

class AMainCharacter;
UCLASS()
class STYLIZEDTEST_API URageAction : public UAction
{
	GENERATED_BODY()
public:
	URageAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;
	virtual bool CanStart_Implementation(AActor* Instigator) override;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> RageMontage;


protected:
private:
	FTimerHandle RageTimerHandle;
	FTimerHandle StartTimerHandle;
	FTimerHandle UpdateUI;

	float ReduceAmountPerTick=0;

	UPROPERTY(EditDefaultsOnly, Category="Combat", meta=(ToolTip="time to wait before rage kicks in"))
	float RageDelay = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category="Combat", meta=(ToolTip="Animation speed during rage"))
	float RageSpeed = 1.2f;

	UPROPERTY(EditDefaultsOnly, Category="Combat", meta=(ToolTip="Movement speed during rage"))
	float RageMoveSpeed = 1.2f;

	UPROPERTY(EditDefaultsOnly, Category="Combat", meta=(ToolTip="Duration of the rage effect"))
	float RageDuration = 12.f;

	UFUNCTION()
	void EndRage(AMainCharacter* MainCharacter);

	UFUNCTION()
	void StartRage(AMainCharacter* MainCharacter);

	UFUNCTION()
void UpdateRageUI(AMainCharacter* MainCharacter);

	UFUNCTION()
	void OnMove(AActor* InstigatorActor);

	bool DoOnce = true;
};
