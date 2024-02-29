// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"


class USaveGame;
class UMainSaveGame;
UCLASS()
class STYLIZEDTEST_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	UMainGameInstance();

	void LoadSaveData(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
	void ASyncSaveData( const FString& SlotName, const int32 UserIndex, bool bSuccess);

	UFUNCTION(BlueprintNativeEvent)
	void ChangeMasterVolume(float Value);
	UFUNCTION(BlueprintNativeEvent)
void ChangeDialogueVolume(float Value);
	UFUNCTION(BlueprintNativeEvent)
void ChangeMusicVolume(float Value);
	UFUNCTION(BlueprintNativeEvent)
void ChangeEffectsVolume(float Value);

	UFUNCTION(BlueprintNativeEvent)
void ChangeMouseSensitivityHorizontal(float Value);
	UFUNCTION(BlueprintNativeEvent)
void ChangeMouseSensitivityVertical(float Value);
	UFUNCTION(BlueprintNativeEvent)
void ChangeMouseSensitivity(float Value);

	


private:
	float MasterVolume =0.5;
	float DialogueVolume =1;
	float MusicVolume=1;
	float EffectsVolume=1;
	float MouseSensitivityVertical=0.5;
	float MouseSensitivityHorizontal=0.5;
	float MouseSensitivity=0.5;
	UPROPERTY()
	UMainSaveGame* MainSaveGame ;



public:
	//Getters and setters
	float GetMasterVolume() const;
	float GetDialogueVolume() const;
	float GetMusicVolume() const;
	float GetEffectsVolume() const;
	float GetMouseSensitivityVertical() const;
	float GetMouseSensitivityHorizontal() const;
	float GetMouseSensitivity() const;

};

