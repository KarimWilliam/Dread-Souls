// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "HUD/AbilityOverlay.h"
#include "HUD/MainMenu.h"
#include "HUD/SettingsOverlay.h"
#include "Kismet/GameplayStatics.h"

void AMainGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void AMainGameMode::StartPlay()
{
	UWorld* World = GetWorld();
	MainMenu = CreateWidget<UMainMenu>(World->GetFirstPlayerController(), MainMenuClass);
	Super::StartPlay();
}

void AMainGameMode::TogglePause_Implementation(APlayerController* PlayerController)
{
	// Adding and removing the pause menu blueprint overlay is done in blueprints
	if (IsPaused())
	{
		PlayerController->SetPause(false);
		PlayerController->SetShowMouseCursor(false);
		if (MainMenu)
		{
			MainMenu->UnbindDelegates();
			MainMenu->RemoveFromParent();
			MainMenu->SettingsOverlay->RemoveFromParent();
			MainMenu->AbilityOverlay->RemoveFromParent();
			MainMenu->SettingsOpen=false;
			MainMenu->AbilitiesOpen=false;
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
		}
	}
	else
	{
		PlayerController->SetPause(true);
		PlayerController->SetShowMouseCursor(true);
		if (MainMenu)
		{
			MainMenu->AddToViewport();
			MainMenu->PauseMenuAlteration();
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController,MainMenu);
		}
	}
}
