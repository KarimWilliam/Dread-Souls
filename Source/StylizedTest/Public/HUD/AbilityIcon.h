// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityIcon.generated.h"

class UProgressBar;
UCLASS()
class STYLIZEDTEST_API UAbilityIcon : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UProgressBar* PB_AbilityIcon;

	void SetBarPercent(float Percentage);

protected:
	virtual void NativeConstruct() override;

private:
};
