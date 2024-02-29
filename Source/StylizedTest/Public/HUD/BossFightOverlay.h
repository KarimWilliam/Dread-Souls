// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossFightOverlay.generated.h"

class UProgressBar;
class UTextBlock;
UCLASS()
class STYLIZEDTEST_API UBossFightOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetHealthBarPercent(float Percent);

	void SetBossName(FText Name);

	UPROPERTY(BlueprintReadWrite,Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeIn;

private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* BossName;


	
};
