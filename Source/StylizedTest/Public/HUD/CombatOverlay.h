// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatOverlay.generated.h"


class UProgressBarBase;
class UTextBlock;
class UAbilityIcon;
class UCanvasPanel;
UCLASS()
class STYLIZEDTEST_API UCombatOverlay : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintReadWrite,Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOut;

	UPROPERTY(BlueprintReadWrite,Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* LockOnFadeIn;

	UPROPERTY(BlueprintReadWrite,Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* HealBlink;
	
	UPROPERTY(BlueprintReadWrite,Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Ability1Blink;
	
	UPROPERTY(BlueprintReadWrite,Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Ability2Blink;


	void SetBossHealthBarPercent(float Percent);
	void SetPlayerHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetBossName(FText Name);
	void SetHealthChargeAmount(int Amount);
	void SetHealProgressBar(float Percent);
	void SetAbility1ProgressBar(float Percent);
	void SetAbility2ProgressBar(float Percent);
	void SetLockOnVisibility (bool MakeVisible);
	bool GetLockOnVisibility();
	void EnableAbility1(bool Enable);
	void EnableAbility2(bool Enable);
	void EnableAbility3(bool Enable);
	void EnableHealAbility(bool Enable);
	
protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	UProgressBarBase* WBP_PlayerHealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBarBase* WBP_PlayerStaminaBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBarBase* WBP_BossHealthBar;

	UPROPERTY(meta = (BindWidget))
	 UTextBlock* BossNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealChargeText;

	UPROPERTY(meta = (BindWidget))
	UAbilityIcon* WBP_AbilityIcon_1;

	UPROPERTY(meta = (BindWidget))
	UAbilityIcon* WBP_AbilityIcon_2;

	UPROPERTY(meta = (BindWidget))
	UAbilityIcon* WBP_AbilityIcon_3;

	UPROPERTY(meta = (BindWidget))
	UAbilityIcon* WBP_HealAbilityIcon;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* LockOnPanel;

	bool LockOnVisible=false;
	
	
};
