// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

class UPauseMenuHUD;
class AMainHUD;
class UMainMenu;

UCLASS()
class STYLIZEDTEST_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;
	UFUNCTION(BlueprintNativeEvent)
	void TogglePause(APlayerController* PlayerController);
	UPROPERTY(EditDefaultsOnly, Category = Overlay)
	TSubclassOf<UMainMenu> MainMenuClass;
	UPROPERTY()
	UMainMenu* MainMenu;
	
	
};
