// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VictoryDeathOverlay.generated.h"

class UTextBlock;

UCLASS()
class STYLIZEDTEST_API UVictoryDeathOverlay : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite,Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOut;

	void SetText(FText Name);
private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text;
};
