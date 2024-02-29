// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SliderBase.generated.h"

class USpinBox;
class USlider;
UCLASS()
class STYLIZEDTEST_API USliderBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	USpinBox* BaseSpinBox;

	UPROPERTY(meta = (BindWidget))
	USlider* BaseSlider;
	

	void SetBarPercent(float Percentage);
};
