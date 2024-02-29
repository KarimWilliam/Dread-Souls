// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SliderBase.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"

void USliderBase::SetBarPercent(float Percentage)
{
	if (BaseSpinBox && BaseSlider)
	{
		BaseSpinBox->SetValue(Percentage);
		BaseSlider->SetValue(Percentage);
		
	}
}
