// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsOverlay.generated.h"

class USaveGame;
class UButtonBase;
class UVerticalBox;
class UImage;
class USliderBase;
class UMainGameInstance;
UCLASS()
class STYLIZEDTEST_API USettingsOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	//Main
	UFUNCTION()
	void OpenDisplay();

	UFUNCTION()
	void OpenGraphics();

	UFUNCTION()
	void OpenAudio();

	UFUNCTION()
	void OpenControls();

	UFUNCTION()
	void OpenBindings();

	UFUNCTION()
	void UnbindButtons();
	
	//Audio
	UFUNCTION()
	void ChangeMasterVolume(float Value);
	UFUNCTION()
	void ChangeEffectsVolume(float Value);
	UFUNCTION()
	void ChangeMusicVolume(float Value);
	UFUNCTION()
	void ChangeDialogueVolume(float Value);

	//Controls
	UFUNCTION()
	void ChangeMouseHorizontal(float Value);
	UFUNCTION()
	void ChangeMouseVertical(float Value);
	UFUNCTION()
	void ChangeMouseSensitivity(float Value);



protected:
	virtual void NativeConstruct() override;
	void SetVBVisibility(UVerticalBox* VB, UImage* Image);

private:
	UPROPERTY()
	UMainGameInstance* GI;

	//MAIN
	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_Display;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_Graphics;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_Audio;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_Controls;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_Bindings;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_Audio;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_Graphics;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_Display;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_Bindings;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_Controls;

	void InitializeSliders();

	//AUDIO 
	UPROPERTY(meta = (BindWidget))
	USliderBase* WBP_SliderBase_Master;
	UPROPERTY(meta = (BindWidget))
	USliderBase* WBP_SliderBase_SoundEffects;
	UPROPERTY(meta = (BindWidget))
	USliderBase* WBP_SliderBase_Voice;
	UPROPERTY(meta = (BindWidget))
	USliderBase* WBP_SliderBase_Music;

	//CONTROLS

	UPROPERTY(meta = (BindWidget))
	USliderBase* WBP_SliderBase_Vertical_Mouse;
	UPROPERTY(meta = (BindWidget))
	USliderBase* WBP_SliderBase_Horizontal_Mouse;
	UPROPERTY(meta = (BindWidget))
	USliderBase* WBP_SliderBase_MouseSensitivity;
};
