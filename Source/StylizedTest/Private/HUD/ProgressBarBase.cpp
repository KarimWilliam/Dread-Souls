// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ProgressBarBase.h"
#include "Components/ProgressBar.h"

void UProgressBarBase::SetBarPercent(float Percentage)
{
	if (BaseProgressBar)
	{
		BaseProgressBar->SetPercent(Percentage);
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "FollowAlong", Percentage);

		GetWorld()->GetTimerManager().SetTimer(FollowAlongHandle, Delegate, InRate, true, VisualDelay);
	} 
}

void UProgressBarBase::SetVisualBarPercent(float Percentage)
{
	if (BaseProgressBar_Visual)
	{
		BaseProgressBar_Visual->SetPercent(Percentage);
	}
}

void UProgressBarBase::FollowAlong(float Percentage)
{
	if (BaseProgressBar_Visual->GetPercent() > BaseProgressBar->GetPercent())
	{
		BaseProgressBar_Visual->SetPercent(BaseProgressBar_Visual->GetPercent() - 0.01 );
	}
	else if (BaseProgressBar_Visual->GetPercent() < BaseProgressBar->GetPercent() &&BaseProgressBar_Visual->GetPercent()+0.01 <BaseProgressBar->GetPercent())
	{
		BaseProgressBar_Visual->SetPercent(BaseProgressBar_Visual->GetPercent() + 0.01);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FollowAlongHandle);
	}
}