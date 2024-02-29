// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ConfirmationOverlay.h"
#include "HUD/ButtonBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Interfaces/ConfirmInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UConfirmationOverlay::NativeConstruct()
{
	if (Button_Yes) Button_Yes->OnClicked.AddDynamic(this, &UConfirmationOverlay::PressYes);
	if (Button_No) Button_No->OnClicked.AddDynamic(this, &UConfirmationOverlay::PressNo);
	Super::NativeConstruct();
}

void UConfirmationOverlay::PressYes()
{
	if (Owner && Owner->Implements<UConfirmInterface>())
	{
		IConfirmInterface::Execute_GetConfirmation(Owner,true);
	}
}

void UConfirmationOverlay::PressNo()
{
	if (Owner)
	{
		IConfirmInterface::Execute_GetConfirmation(Owner,false);
	}
}

void UConfirmationOverlay::SetBoxTitle(FText Title)
{
	if (T_Title)
	{
		T_Title->SetText(Title);
	}
}

void UConfirmationOverlay::UnbindButtons()
{
	if (Button_Yes) Button_Yes->OnClicked.RemoveDynamic(this, &UConfirmationOverlay::PressYes);
	if (Button_No) Button_No->OnClicked.RemoveDynamic(this, &UConfirmationOverlay::PressNo);
}
