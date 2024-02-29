// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/VictoryDeathOverlay.h"

#include "Components/TextBlock.h"

void UVictoryDeathOverlay::SetText(FText words)
{
	if(Text)
	{
		Text->SetText(words);
	}
}
