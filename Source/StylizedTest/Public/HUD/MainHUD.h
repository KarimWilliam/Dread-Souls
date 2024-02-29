// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SettingsOverlay.h"
#include "VictoryDeathOverlay.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class UMainOverlay;
class UPauseOverlay;
class UBossFightOverlay;
class UCombatOverlay;
class UCredits;
UCLASS()
class STYLIZEDTEST_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	FORCEINLINE UMainOverlay* GetMainOverlay() const { return MainOverlay; }
	FORCEINLINE UBossFightOverlay* GetBossFightOverlay() const { return BossOverlay; }
	FORCEINLINE UVictoryDeathOverlay* GetVictoryDeathOverlay() const { return VictoryDeathOverlay; }
	FORCEINLINE USettingsOverlay* GetSettingsOverlay() const { return SettingsOverlay; }
	FORCEINLINE UCombatOverlay* GetCombatOverlay() const { return CombatOverlay; }
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = Overlay)
	TSubclassOf<UMainOverlay> MainOverlayClass;

	UPROPERTY(EditDefaultsOnly, Category = Overlay)
	TSubclassOf<UBossFightOverlay> BossFightOverlayClass;

	UPROPERTY(EditDefaultsOnly, Category = Overlay)
	TSubclassOf<UVictoryDeathOverlay> VictoryDeathOverlayClass;

	UPROPERTY(EditDefaultsOnly, Category = Overlay)
	TSubclassOf<USettingsOverlay> SettingsOverlayClass;

	
	UPROPERTY(EditDefaultsOnly, Category = Overlay)
	TSubclassOf<UCombatOverlay> CombatOverlayClass;
	

	UPROPERTY()
	 USettingsOverlay* SettingsOverlay;
	
	UPROPERTY()
	UMainOverlay* MainOverlay;

	UPROPERTY()
	UCombatOverlay* CombatOverlay;

	UPROPERTY()
	UPauseOverlay* PauseOverlay;

	UPROPERTY()
	UBossFightOverlay* BossOverlay;
	
	UPROPERTY()
	UVictoryDeathOverlay* VictoryDeathOverlay;

};
