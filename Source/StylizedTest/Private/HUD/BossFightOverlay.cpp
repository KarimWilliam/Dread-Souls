// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BossFightOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UBossFightOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UBossFightOverlay::SetBossName(FText Name)
{
	if(BossName)
	{
		BossName->SetText(Name);
	}
}
