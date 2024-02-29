// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/ConfirmInterface.h"
#include "MainMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClickDelegate);

class UConfirmationOverlay;
class UButtonBase;
class UAbilityOverlay;
class USettingsOverlay;
class UCredits;
UCLASS()
class STYLIZEDTEST_API UMainMenu : public UUserWidget , public IConfirmInterface
{
	GENERATED_BODY()


public:
	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void QuitGame();

	UFUNCTION()
	void ContinueGame();

	UFUNCTION()
	void ResumeGame();

	UFUNCTION()
	void OpenSettings();

	UFUNCTION()
	void OpenCredits();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void OpenCharacterSettings();


	UFUNCTION()
	void UnbindDelegates();


	UPROPERTY()
	 USettingsOverlay* SettingsOverlay;

	UPROPERTY()
	 UCredits* CreditsOverlay;
	
	UPROPERTY()
	UConfirmationOverlay* ConfirmationOverlay;

	UPROPERTY()
	UAbilityOverlay* AbilityOverlay;
	
	UPROPERTY(EditDefaultsOnly, Category = Overlay)
	TSubclassOf<USettingsOverlay> SettingsOverlayClass;

	UPROPERTY(EditDefaultsOnly, Category = Overlay)
	TSubclassOf<UCredits> CreditsOverlayClass;

	UPROPERTY(EditDefaultsOnly, Category = Overlay)
	TSubclassOf<UConfirmationOverlay> ConfirmationOverlayClass;

	UPROPERTY(EditDefaultsOnly, Category = Overlay)
	TSubclassOf<UAbilityOverlay> AbilityOverlayClass;


	bool SettingsOpen = false;
	bool CreditsOpen = false;
	bool AbilitiesOpen = false;
	bool QuitGamePressed=false;

	UFUNCTION()
	void PauseMenuAlteration();

	UFUNCTION()
	void MainMenuAlteration();

	virtual void GetConfirmation_Implementation(bool IsConfirm) override;

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_NewGame;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_Continue;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_Settings;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_Credits;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_QuitGame;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_Resume;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_Character;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Button_MainMenu;

};
