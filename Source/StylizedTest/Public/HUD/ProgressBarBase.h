// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProgressBarBase.generated.h"

class UProgressBar;

UCLASS()
class STYLIZEDTEST_API UProgressBarBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UProgressBar* BaseProgressBar;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UProgressBar* BaseProgressBar_Visual;

	void SetBarPercent(float Percentage);
	void SetVisualBarPercent(float Percentage);
	UFUNCTION()
	void FollowAlong(float Percentage);

private:
	FTimerHandle FollowAlongHandle;
	UPROPERTY(EditDefaultsOnly,Category="Timer",meta=(ToolTip="Rate at which the visual healing triggers change. Lower numbers is faster."))
	float InRate = 0.05;
	UPROPERTY(EditDefaultsOnly,Category="Timer",meta=(ToolTip="Delay before visual starts moving"))
	float VisualDelay=0.2;
};
