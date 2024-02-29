// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UMainOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UMainOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UMainOverlay::SetText(FText words)
{
	if(HealText)
	{
		HealText->SetText(words);
	}
}

void UMainOverlay::SetHealChargeBar(float Percent)
{
	if(HealChargeBar)
	{
		HealChargeBar->SetPercent(Percent);
	}
}


