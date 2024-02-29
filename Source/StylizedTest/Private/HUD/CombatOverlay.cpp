// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CombatOverlay.h"

#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "HUD/ProgressBarBase.h"
#include "Components/TextBlock.h"
#include "HUD/AbilityIcon.h"


void UCombatOverlay::NativeConstruct()
{
	Super::NativeConstruct();
	WBP_PlayerStaminaBar->BaseProgressBar_Visual->SetVisibility(ESlateVisibility::Collapsed);
	WBP_PlayerHealthBar->BaseProgressBar_Visual->SetPercent(WBP_PlayerHealthBar->BaseProgressBar->GetPercent());
	WBP_BossHealthBar->BaseProgressBar_Visual->SetPercent(WBP_PlayerHealthBar->BaseProgressBar->GetPercent());
	EnableAbility1(false);
	EnableAbility2(false);
}


void UCombatOverlay::SetBossHealthBarPercent(float Percent)
{
	if (WBP_BossHealthBar)
	{
		WBP_BossHealthBar->SetBarPercent(Percent);
	}
}

void UCombatOverlay::SetPlayerHealthBarPercent(float Percent)
{
	if (WBP_PlayerHealthBar)
	{
		WBP_PlayerHealthBar->SetBarPercent(Percent);
	}
}

void UCombatOverlay::SetStaminaBarPercent(float Percent)
{
	if (WBP_PlayerStaminaBar)
	{
		WBP_PlayerStaminaBar->SetBarPercent(Percent);
	}
}

void UCombatOverlay::SetBossName(FText Name)
{
	if (BossNameText)
	{
		BossNameText->SetText(Name);
	}
}

void UCombatOverlay::SetHealthChargeAmount(int Amount)
{
	if (HealChargeText)
	{
		FString x = "x";
		x.AppendInt(Amount);
		HealChargeText->SetText((FText::FromString(x)));
	}
}

void UCombatOverlay::SetHealProgressBar(float Percent)
{
	if (WBP_HealAbilityIcon)
	{
		WBP_HealAbilityIcon->SetBarPercent(Percent);
	}
}

void UCombatOverlay::SetAbility1ProgressBar(float Percent)
{
	if (WBP_AbilityIcon_1)
	{
		WBP_AbilityIcon_1->SetBarPercent(Percent);
	}
}

void UCombatOverlay::SetAbility2ProgressBar(float Percent)
{
	if (WBP_AbilityIcon_2)
	{
		WBP_AbilityIcon_2->SetBarPercent(Percent);
	}
}

void UCombatOverlay::SetLockOnVisibility(bool MakeVisible)
{
	if (LockOnPanel)
	{
		if (MakeVisible)
		{
			LockOnVisible = true;
		}
		else
		{
			LockOnVisible = false;
		}
	}
}

bool UCombatOverlay::GetLockOnVisibility()
{
	return LockOnVisible;
}

void UCombatOverlay::EnableAbility1(bool Enable)
{
	WBP_AbilityIcon_1->SetIsEnabled(Enable);
}

void UCombatOverlay::EnableAbility2(bool Enable)
{
	WBP_AbilityIcon_2->SetIsEnabled(Enable);
}

void UCombatOverlay::EnableAbility3(bool Enable)
{
	WBP_AbilityIcon_3->SetIsEnabled(Enable);
}

void UCombatOverlay::EnableHealAbility(bool Enable)
{
	WBP_HealAbilityIcon->SetIsEnabled(Enable);
}
