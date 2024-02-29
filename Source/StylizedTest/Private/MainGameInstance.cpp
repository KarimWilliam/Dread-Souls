// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"
#include "MainSaveGame.h"
#include "Kismet/GameplayStatics.h"


UMainGameInstance::UMainGameInstance()
{
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	// USomeUObjectClass::LoadGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData
	LoadedDelegate.BindUObject(this, &UMainGameInstance::LoadSaveData);
	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("SaveSlot1"), 0, LoadedDelegate);
}


void UMainGameInstance::ChangeMasterVolume_Implementation(float Value)
{
	MasterVolume = Value;
	//SYNC Saving
	if (MainSaveGame)
	{
		// Set data on the savegame object.
		MainSaveGame->MasterVolume = Value;

		// Save the data immediately.
		if (UGameplayStatics::SaveGameToSlot(MainSaveGame, MainSaveGame->SaveSlotName, MainSaveGame->UserIndex))
		{
			// Save succeeded.
			//UE_LOG(LogTemp, Warning, TEXT("save SUCCESS"));
		}
	}
}


void UMainGameInstance::ChangeDialogueVolume_Implementation(float Value)
{
	DialogueVolume = Value;
	//ASYNC SAVING
	if (MainSaveGame)
	{
		// Set up the (optional) delegate.
		FAsyncSaveGameToSlotDelegate SavedDelegate;
		// USomeUObjectClass::SaveGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, bool bSuccess
		SavedDelegate.BindUObject(this, &UMainGameInstance::ASyncSaveData);

		// Set data on the savegame object.
		MainSaveGame->DialogueVolume = Value;

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(MainSaveGame, TEXT("SaveSlot1"), 0, SavedDelegate);
	}
}

void UMainGameInstance::ChangeMusicVolume_Implementation(float Value)
{
	MusicVolume = Value;
	//ASYNC NO DELEGATE
	MainSaveGame->MusicVolume = Value;
	UGameplayStatics::AsyncSaveGameToSlot(MainSaveGame, TEXT("SaveSlot1"), 0);
}

void UMainGameInstance::ChangeEffectsVolume_Implementation(float Value)
{
	EffectsVolume = Value;
	MainSaveGame->EffectsVolume = Value;
	UGameplayStatics::AsyncSaveGameToSlot(MainSaveGame, TEXT("SaveSlot1"), 0);
}

void UMainGameInstance::ChangeMouseSensitivityHorizontal_Implementation(float Value)
{
	
	MouseSensitivityHorizontal = Value;
	MainSaveGame->MouseSensitivityHorizontal = Value;
	UGameplayStatics::AsyncSaveGameToSlot(MainSaveGame, TEXT("SaveSlot1"), 0);
}

void UMainGameInstance::ChangeMouseSensitivityVertical_Implementation(float Value)
{
	MouseSensitivityVertical = Value;
	MainSaveGame->MouseSensitivityVertical = Value;
	UGameplayStatics::AsyncSaveGameToSlot(MainSaveGame, TEXT("SaveSlot1"), 0);
}

void UMainGameInstance::ChangeMouseSensitivity_Implementation(float Value)
{
	MouseSensitivity = Value;
	MainSaveGame->MouseSensitivity = Value;
	UGameplayStatics::AsyncSaveGameToSlot(MainSaveGame, TEXT("SaveSlot1"), 0);
}

//Getters and setters
float UMainGameInstance::GetMasterVolume() const
{
	return MasterVolume;
}

float UMainGameInstance::GetDialogueVolume() const
{
	return DialogueVolume;
}

float UMainGameInstance::GetMusicVolume() const
{
	return MusicVolume;
}

float UMainGameInstance::GetEffectsVolume() const
{
	return EffectsVolume;
}

float UMainGameInstance::GetMouseSensitivityVertical() const
{
	return MouseSensitivityVertical;
}

float UMainGameInstance::GetMouseSensitivityHorizontal() const
{
	return MouseSensitivityHorizontal;
}

float UMainGameInstance::GetMouseSensitivity() const
{
	return MouseSensitivity;
}

//LOAD AND SAVE
void UMainGameInstance::LoadSaveData(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	if (LoadedGameData)
	{
		MainSaveGame = Cast<UMainSaveGame>(LoadedGameData);

		if (MainSaveGame)
		{
			if (MainSaveGame->MasterVolume) MasterVolume = MainSaveGame->MasterVolume;
			if (MainSaveGame->DialogueVolume) DialogueVolume = MainSaveGame->DialogueVolume;
			if (MainSaveGame->MusicVolume) MusicVolume = MainSaveGame->MusicVolume;
			if (MainSaveGame->EffectsVolume) EffectsVolume = MainSaveGame->EffectsVolume;
			if (MainSaveGame->MouseSensitivity) MouseSensitivity = MainSaveGame->MouseSensitivity;
			if (MainSaveGame->MouseSensitivityHorizontal) MouseSensitivityHorizontal = MainSaveGame->MouseSensitivityHorizontal;
			if (MainSaveGame->MouseSensitivityVertical) MouseSensitivityVertical = MainSaveGame->MouseSensitivityVertical;
		}
	}
	else
	{
		MainSaveGame = Cast<UMainSaveGame>(UGameplayStatics::CreateSaveGameObject(UMainSaveGame::StaticClass()));
	}
}

void UMainGameInstance::ASyncSaveData(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
}
