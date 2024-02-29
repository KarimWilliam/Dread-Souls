// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MainSaveGame.generated.h"


UCLASS()
class STYLIZEDTEST_API UMainSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;
	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = Sound)
	float MasterVolume;
	UPROPERTY(VisibleAnywhere, Category = Sound)
	float DialogueVolume;
	UPROPERTY(VisibleAnywhere, Category = Sound)
	float MusicVolume;
	UPROPERTY(VisibleAnywhere, Category = Sound)
	float EffectsVolume;
	UPROPERTY(VisibleAnywhere, Category = Controls)
	float MouseSensitivityVertical;
	UPROPERTY(VisibleAnywhere, Category = Controls)
	float MouseSensitivityHorizontal;
	UPROPERTY(VisibleAnywhere, Category = Controls)
	float MouseSensitivity;


	UMainSaveGame();
};
