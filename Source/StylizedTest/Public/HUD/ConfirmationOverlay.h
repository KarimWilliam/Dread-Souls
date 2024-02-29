// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmationOverlay.generated.h"

class UButtonBase;
class IConfirmInterface;
class UButton;
UCLASS()
class STYLIZEDTEST_API UConfirmationOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void PressYes();

	UFUNCTION()
	void PressNo();

	void SetBoxTitle(FText Title);

	UFUNCTION()
	void UnbindButtons();

	UPROPERTY(EditDefaultsOnly, Category = Overlay)
	UObject* Owner;
	

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Yes;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_No;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UTextBlock* T_Title;



	
};
