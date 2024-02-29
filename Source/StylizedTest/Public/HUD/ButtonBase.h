// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ButtonBase.generated.h"

class UTextBlock;
class UImage;
class UButton;
/**
 * 
 */
UCLASS()
class STYLIZEDTEST_API UButtonBase : public UUserWidget
{
	GENERATED_BODY()



public:
	UPROPERTY(meta = (BindWidget))
	UButton* BaseButton;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_Pressed;

	UPROPERTY(meta = (BindWidget),BlueprintReadWrite)
	UTextBlock* BaseButtonText;
};
