// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SettingsOverlay.h"

#include "MainGameInstance.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "Components/Slider.h"
#include "HUD/SliderBase.h"
#include "HUD/ButtonBase.h"


void USettingsOverlay::NativeConstruct()
{
	if (Button_Audio) Button_Audio->BaseButton->OnClicked.AddDynamic(this, &USettingsOverlay::OpenAudio);
	if (Button_Bindings) Button_Bindings->BaseButton->OnClicked.AddDynamic(this, &USettingsOverlay::OpenBindings);
	if (Button_Controls) Button_Controls->BaseButton->OnClicked.AddDynamic(this, &USettingsOverlay::OpenControls);
	if (Button_Display) Button_Display->BaseButton->OnClicked.AddDynamic(this, &USettingsOverlay::OpenDisplay);
	if (Button_Graphics) Button_Graphics->BaseButton->OnClicked.AddDynamic(this, &USettingsOverlay::OpenGraphics);
	if (WBP_SliderBase_Master) WBP_SliderBase_Master->BaseSlider->OnValueChanged.AddDynamic(this, &USettingsOverlay::ChangeMasterVolume);
	if (WBP_SliderBase_Music) WBP_SliderBase_Music->BaseSlider->OnValueChanged.AddDynamic(this, &USettingsOverlay::ChangeMusicVolume);
	if (WBP_SliderBase_Voice) WBP_SliderBase_Voice->BaseSlider->OnValueChanged.AddDynamic(this, &USettingsOverlay::ChangeDialogueVolume);
	if (WBP_SliderBase_SoundEffects) WBP_SliderBase_SoundEffects->BaseSlider->OnValueChanged.AddDynamic(this, &USettingsOverlay::ChangeEffectsVolume);
	if (WBP_SliderBase_Vertical_Mouse) WBP_SliderBase_Vertical_Mouse->BaseSlider->OnValueChanged.AddDynamic(this, &USettingsOverlay::ChangeMouseVertical);
	if (WBP_SliderBase_Horizontal_Mouse) WBP_SliderBase_Horizontal_Mouse->BaseSlider->OnValueChanged.AddDynamic(this, &USettingsOverlay::ChangeMouseHorizontal);
	if (WBP_SliderBase_MouseSensitivity) WBP_SliderBase_MouseSensitivity->BaseSlider->OnValueChanged.AddDynamic(this, &USettingsOverlay::ChangeMouseSensitivity);
	GI = Cast<UMainGameInstance>(GetGameInstance());
	InitializeSliders();
	OpenDisplay();
	Super::NativeConstruct();
}

void USettingsOverlay::SetVBVisibility(UVerticalBox* VB, UImage* Image)
{
	VB_Audio->SetVisibility(ESlateVisibility::Collapsed);
	VB_Bindings->SetVisibility(ESlateVisibility::Collapsed);
	VB_Controls->SetVisibility(ESlateVisibility::Collapsed);
	VB_Display->SetVisibility(ESlateVisibility::Collapsed);
	VB_Graphics->SetVisibility(ESlateVisibility::Collapsed);

	Button_Display->Image_Pressed->SetVisibility(ESlateVisibility::Collapsed);
	Button_Audio->Image_Pressed->SetVisibility(ESlateVisibility::Collapsed);
	Button_Bindings->Image_Pressed->SetVisibility(ESlateVisibility::Collapsed);
	Button_Controls->Image_Pressed->SetVisibility(ESlateVisibility::Collapsed);
	Button_Graphics->Image_Pressed->SetVisibility(ESlateVisibility::Collapsed);

	VB->SetVisibility(ESlateVisibility::Visible);
	Image->SetVisibility(ESlateVisibility::Visible);
}

void USettingsOverlay::InitializeSliders()
{
	//Audio
	WBP_SliderBase_Master->SetBarPercent(GI->GetMasterVolume());
	WBP_SliderBase_Music->SetBarPercent(GI->GetMusicVolume());
	WBP_SliderBase_Voice->SetBarPercent(GI->GetDialogueVolume());
	WBP_SliderBase_SoundEffects->SetBarPercent(GI->GetEffectsVolume());
	//Controls
	WBP_SliderBase_Horizontal_Mouse->SetBarPercent(GI->GetMouseSensitivityHorizontal());
	WBP_SliderBase_Vertical_Mouse->SetBarPercent(GI->GetMouseSensitivityVertical());
	WBP_SliderBase_MouseSensitivity->SetBarPercent(GI->GetMouseSensitivity());
}

//MAIN 
void USettingsOverlay::OpenDisplay()
{
	SetVBVisibility(VB_Display, Button_Display->Image_Pressed);
}

void USettingsOverlay::OpenGraphics()
{
	SetVBVisibility(VB_Graphics, Button_Graphics->Image_Pressed);
}

void USettingsOverlay::OpenAudio()
{
	SetVBVisibility(VB_Audio, Button_Audio->Image_Pressed);
}

void USettingsOverlay::OpenControls()
{
	SetVBVisibility(VB_Controls, Button_Controls->Image_Pressed);
}

void USettingsOverlay::OpenBindings()
{
	SetVBVisibility(VB_Bindings, Button_Bindings->Image_Pressed);
}

void USettingsOverlay::UnbindButtons()
{
	if (Button_Audio) Button_Audio->BaseButton->OnClicked.RemoveDynamic(this, &USettingsOverlay::OpenAudio);
	if (Button_Bindings) Button_Bindings->BaseButton->OnClicked.RemoveDynamic(this, &USettingsOverlay::OpenBindings);
	if (Button_Controls) Button_Controls->BaseButton->OnClicked.RemoveDynamic(this, &USettingsOverlay::OpenControls);
	if (Button_Display) Button_Display->BaseButton->OnClicked.RemoveDynamic(this, &USettingsOverlay::OpenDisplay);
	if (Button_Graphics) Button_Graphics->BaseButton->OnClicked.RemoveDynamic(this, &USettingsOverlay::OpenGraphics);
	if (WBP_SliderBase_Master) WBP_SliderBase_Master->BaseSlider->OnValueChanged.RemoveDynamic(this, &USettingsOverlay::ChangeMasterVolume);
	if (WBP_SliderBase_Music) WBP_SliderBase_Music->BaseSlider->OnValueChanged.RemoveDynamic(this, &USettingsOverlay::ChangeMusicVolume);
	if (WBP_SliderBase_Voice) WBP_SliderBase_Voice->BaseSlider->OnValueChanged.RemoveDynamic(this, &USettingsOverlay::ChangeDialogueVolume);
	if (WBP_SliderBase_SoundEffects) WBP_SliderBase_SoundEffects->BaseSlider->OnValueChanged.RemoveDynamic(this, &USettingsOverlay::ChangeEffectsVolume);
	if (WBP_SliderBase_Vertical_Mouse) WBP_SliderBase_Vertical_Mouse->BaseSlider->OnValueChanged.RemoveDynamic(this, &USettingsOverlay::ChangeMouseVertical);
	if (WBP_SliderBase_Horizontal_Mouse) WBP_SliderBase_Horizontal_Mouse->BaseSlider->OnValueChanged.RemoveDynamic(this, &USettingsOverlay::ChangeMouseHorizontal);
	if (WBP_SliderBase_MouseSensitivity) WBP_SliderBase_MouseSensitivity->BaseSlider->OnValueChanged.RemoveDynamic(this, &USettingsOverlay::ChangeMouseSensitivity);
}

//Audio
void USettingsOverlay::ChangeMasterVolume(float Value)
{
	GI->ChangeMasterVolume(Value);
	WBP_SliderBase_Master->SetBarPercent(Value);
}

void USettingsOverlay::ChangeEffectsVolume(float Value)
{
	GI->ChangeEffectsVolume(Value);
	WBP_SliderBase_SoundEffects->SetBarPercent(Value);
}

void USettingsOverlay::ChangeMusicVolume(float Value)
{
	GI->ChangeMusicVolume(Value);
	WBP_SliderBase_Music->SetBarPercent(Value);
}

void USettingsOverlay::ChangeDialogueVolume(float Value)
{
	GI->ChangeDialogueVolume(Value);
	WBP_SliderBase_Voice->SetBarPercent(Value);
}

//Controls
void USettingsOverlay::ChangeMouseHorizontal(float Value)
{
	GI->ChangeMouseSensitivityHorizontal(Value);
	WBP_SliderBase_Horizontal_Mouse->SetBarPercent(Value);
}

void USettingsOverlay::ChangeMouseVertical(float Value)
{
	GI->ChangeMouseSensitivityVertical(Value);
	WBP_SliderBase_Vertical_Mouse->SetBarPercent(Value);
}


void USettingsOverlay::ChangeMouseSensitivity(float Value)
{
	GI->ChangeMouseSensitivity(Value);
	WBP_SliderBase_MouseSensitivity->SetBarPercent(Value);
}

