// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainMenu.h"

#include "MainGameMode.h"
#include "Characters/MainCharacter.h"
#include "HUD/ButtonBase.h"
#include "HUD/Credits.h"
#include "HUD/AbilityOverlay.h"
#include "Components/Button.h"
#include "HUD/MainHUD.h"
#include "HUD/ConfirmationOverlay.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"


void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();


	if (Button_NewGame) Button_NewGame->BaseButton->OnClicked.AddDynamic(this, &UMainMenu::StartGame);
	if (Button_Continue) Button_Continue->BaseButton->OnClicked.AddDynamic(this, &UMainMenu::ContinueGame);
	if (Button_Settings) Button_Settings->BaseButton->OnClicked.AddDynamic(this, &UMainMenu::OpenSettings);
	if (Button_Credits) Button_Credits->BaseButton->OnClicked.AddDynamic(this, &UMainMenu::OpenCredits);
	if (Button_QuitGame) Button_QuitGame->BaseButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);
	if (Button_MainMenu) Button_MainMenu->BaseButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
	if (Button_Resume) Button_Resume->BaseButton->OnClicked.AddDynamic(this, &UMainMenu::ResumeGame);
	if (Button_Character) Button_Character->BaseButton->OnClicked.AddDynamic(this, &UMainMenu::OpenCharacterSettings);
	SettingsOverlay = CreateWidget<USettingsOverlay>(this, SettingsOverlayClass);
	CreditsOverlay = CreateWidget<UCredits>(this, CreditsOverlayClass);
	AbilityOverlay = CreateWidget<UAbilityOverlay>(this, AbilityOverlayClass);

	Button_Settings->Image_Pressed->SetVisibility(ESlateVisibility::Collapsed);
	Button_Character->Image_Pressed->SetVisibility(ESlateVisibility::Collapsed);
	MainMenuAlteration();
}

void UMainMenu::PauseMenuAlteration()
{
	Button_NewGame->SetVisibility(ESlateVisibility::Collapsed);
	Button_Credits->SetVisibility(ESlateVisibility::Collapsed);
	Button_Continue->SetVisibility(ESlateVisibility::Collapsed);
	Button_Resume->SetVisibility(ESlateVisibility::Visible);
	Button_MainMenu->SetVisibility(ESlateVisibility::Visible);
	Button_Character->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenu::MainMenuAlteration()
{
	Button_NewGame->SetVisibility(ESlateVisibility::Visible);
	Button_Credits->SetVisibility(ESlateVisibility::Visible);
	Button_Continue->SetVisibility(ESlateVisibility::Visible);
	Button_Resume->SetVisibility(ESlateVisibility::Collapsed);
	Button_MainMenu->SetVisibility(ESlateVisibility::Collapsed);
	Button_Character->SetVisibility(ESlateVisibility::Collapsed);
}

void UMainMenu::GetConfirmation_Implementation(bool IsConfirm)
{
	IConfirmInterface::GetConfirmation_Implementation(IsConfirm);
	if (IsConfirm)
	{
		if (QuitGamePressed)
		{
			FGenericPlatformMisc::RequestExit(false);
		}
		else
		{
			MainMenuAlteration();
			UGameplayStatics::OpenLevel(this, "MainMenu");
			SettingsOpen = false;
			CreditsOpen = false;
			AbilitiesOpen = false;
		}
	}
	else
	{
		ConfirmationOverlay->UnbindButtons();
		ConfirmationOverlay->RemoveFromParent();
	}
}

void UMainMenu::StartGame()
{
	UnbindDelegates();
	UGameplayStatics::OpenLevel(this, "Iudex");
	SettingsOpen = false;
	CreditsOpen = false;
	AbilitiesOpen = false;
}

void UMainMenu::QuitGame()
{
	//FGenericPlatformMisc::RequestExit(false);
	ConfirmationOverlay = CreateWidget<UConfirmationOverlay>(this, ConfirmationOverlayClass);
	ConfirmationOverlay->SetBoxTitle(FText::FromName("Quit Game?"));
	ConfirmationOverlay->Owner = this;
	QuitGamePressed = true;
	ConfirmationOverlay->AddToViewport();
}

void UMainMenu::ContinueGame()
{
	UnbindDelegates();
	SettingsOpen = false;
	CreditsOpen = false;
	AbilitiesOpen = false;
	//TODO DARK Continue game
}

void UMainMenu::ResumeGame()
{
	if (AbilityOverlay)AbilityOverlay->RemoveFromParent();
	if (SettingsOverlay)SettingsOverlay->RemoveFromParent();
	AMainGameMode* GameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (GameMode) GameMode->TogglePause(Cast<APlayerController>(PlayerController));
}

void UMainMenu::OpenMainMenu()
{
	ConfirmationOverlay = CreateWidget<UConfirmationOverlay>(this, ConfirmationOverlayClass);
	ConfirmationOverlay->SetBoxTitle(FText::FromName("Return to Main Menu?"));
	ConfirmationOverlay->Owner = this;
	ConfirmationOverlay->AddToViewport();
	QuitGamePressed = false;
}


void UMainMenu::OpenSettings()
{
	if (!SettingsOpen)
	{
		if (CreditsOverlay) CreditsOverlay->RemoveFromParent();
		if (AbilityOverlay)AbilityOverlay->RemoveFromParent();
		if (SettingsOverlay) SettingsOverlay->AddToViewport();
		CreditsOpen = false;
		SettingsOpen = true;
		AbilitiesOpen = false;

		Button_Settings->Image_Pressed->SetVisibility(ESlateVisibility::Visible);
		Button_Credits->Image_Pressed->SetVisibility(ESlateVisibility::Hidden);
		Button_Character->Image_Pressed->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainMenu::OpenCredits()
{
	if (!CreditsOpen)
	{
		if (SettingsOverlay)SettingsOverlay->RemoveFromParent();
		if (AbilityOverlay)AbilityOverlay->RemoveFromParent();
		if (CreditsOverlay) CreditsOverlay->AddToViewport();
		SettingsOpen = false;
		CreditsOpen = true;
		AbilitiesOpen = false;
		SettingsOverlay->UnbindButtons();
		Button_Settings->Image_Pressed->SetVisibility(ESlateVisibility::Hidden);
		Button_Credits->Image_Pressed->SetVisibility(ESlateVisibility::Visible);
		Button_Character->Image_Pressed->SetVisibility(ESlateVisibility::Hidden);
	}
	//TODO DARK Credits
}


void UMainMenu::OpenCharacterSettings()
{
	if (!AbilitiesOpen)
	{
		SettingsOverlay->UnbindButtons();
		if (SettingsOverlay)SettingsOverlay->RemoveFromParent();
		if (CreditsOverlay)CreditsOverlay->RemoveFromParent();
		if (AbilityOverlay) AbilityOverlay->AddToViewport();
		SettingsOpen = false;
		CreditsOpen = false;
		AbilitiesOpen = true;
		Button_Character->Image_Pressed->SetVisibility(ESlateVisibility::Visible);
		Button_Settings->Image_Pressed->SetVisibility(ESlateVisibility::Hidden);
		Button_Credits->Image_Pressed->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainMenu::UnbindDelegates()
{
	if (Button_NewGame) Button_NewGame->BaseButton->OnClicked.RemoveDynamic(this, &UMainMenu::StartGame);
	if (Button_Continue) Button_Continue->BaseButton->OnClicked.RemoveDynamic(this, &UMainMenu::ContinueGame);
	if (Button_Settings) Button_Settings->BaseButton->OnClicked.RemoveDynamic(this, &UMainMenu::OpenSettings);
	if (Button_Credits) Button_Credits->BaseButton->OnClicked.RemoveDynamic(this, &UMainMenu::OpenCredits);
	if (Button_QuitGame) Button_QuitGame->BaseButton->OnClicked.RemoveDynamic(this, &UMainMenu::QuitGame);
	if (Button_Resume) Button_Resume->BaseButton->OnClicked.RemoveDynamic(this, &UMainMenu::ResumeGame);
	if (Button_Character) Button_Character->BaseButton->OnClicked.RemoveDynamic(this, &UMainMenu::OpenCharacterSettings);
	if (Button_MainMenu) Button_MainMenu->BaseButton->OnClicked.RemoveDynamic(this, &UMainMenu::OpenMainMenu);
}
