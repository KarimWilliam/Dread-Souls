// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/AbilityIcon.h"
#include "Components/ProgressBar.h"


void UAbilityIcon::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAbilityIcon::SetBarPercent(float Percentage)
{
	if (PB_AbilityIcon)
	{
		PB_AbilityIcon->SetPercent(Percentage);
	}
}
