// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "HealAction.generated.h"


class UMainOverlay;
class AMainCharacter;
UCLASS()
class STYLIZEDTEST_API UHealAction : public UAction
{
	GENERATED_BODY()


public:
	UHealAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;
	virtual bool CanStart_Implementation(AActor* Instigator) override;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> HealMontage;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
	void ExecutePeriodicEffect(AMainCharacter* MainCharacter);

private:
	UFUNCTION()
	void HealSelf( AMainCharacter* MainCharacter);
	FTimerHandle HealTimerHandle;
	FTimerHandle PeriodHandle;

	UPROPERTY(EditDefaultsOnly, Category="Combat",meta=(ToolTip=" Time before healing starts ticking"))
	float HealDelay = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Combat", meta=(ToolTip=" Total Heal amount over the duration"))
	float HealAmount = 25.f;

	UPROPERTY(EditDefaultsOnly, Category="Combat", meta=(ToolTip="How long the healing goes on for"))
	float HealDuration = 1.5f;
	
	UPROPERTY(VisibleAnywhere, Category = "Effect", meta=(ToolTip="Time between 'ticks' to apply effect"))
	float Period=0.01;
	
	float HealAmountPerTick;

	UFUNCTION()
	void OnMove(AActor* InstigatorActor);

	bool DoOnce = true;




};
