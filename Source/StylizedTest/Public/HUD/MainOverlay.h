// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainOverlay.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class STYLIZEDTEST_API UMainOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetText(FText Name);
	void SetHealChargeBar(float Percent);

	UPROPERTY(BlueprintReadWrite,Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* HealFlash;

private:
	UPROPERTY(meta = (BindWidget))
	 UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	 UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealChargeBar;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealText;





	
};
