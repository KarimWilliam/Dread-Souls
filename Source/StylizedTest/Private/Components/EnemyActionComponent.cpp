// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EnemyActionComponent.h"

bool UEnemyActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{

	OnStopAction.Broadcast(Instigator,this, ActionName);
	return Super::StopActionByName(Instigator, ActionName);
}

void UEnemyActionComponent::InterruptRunningActions(AActor* Instigator)
{
	OnStopAction.Broadcast(Instigator,this, "all");
	Super::InterruptRunningActions(Instigator);
}
