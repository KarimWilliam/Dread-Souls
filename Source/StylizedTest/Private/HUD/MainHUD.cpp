// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainHUD.h"

#include "HUD/BossFightOverlay.h"
#include "HUD/CombatOverlay.h"
#include "HUD/MainOverlay.h"
#include "HUD/Credits.h"
#include "HUD/SettingsOverlay.h"
#include "HUD/PauseOverlay.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && MainOverlayClass && BossFightOverlayClass)
		{
			CombatOverlay = CreateWidget<UCombatOverlay>(Controller, CombatOverlayClass);
			MainOverlay = CreateWidget<UMainOverlay>(Controller, MainOverlayClass);
			BossOverlay = CreateWidget<UBossFightOverlay>(Controller, BossFightOverlayClass);
			SettingsOverlay = CreateWidget<USettingsOverlay>(Controller, SettingsOverlayClass);
			VictoryDeathOverlay = CreateWidget<UVictoryDeathOverlay>(Controller, VictoryDeathOverlayClass);
			//MainOverlay->AddToViewport();
			//CombatOverlay->AddToViewport();
		}
	}
}
