// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActionComponent.h"
#include "EnemyActionComponent.generated.h"


UCLASS()
class STYLIZEDTEST_API UEnemyActionComponent : public UActionComponent
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStopAction, AActor*, InstigatorActor, UEnemyActionComponent*,OwningComp,FName, ActionName);


	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnStopAction OnStopAction;
	virtual bool StopActionByName(AActor* Instigator, FName ActionName) override;
	virtual void InterruptRunningActions(AActor* Instigator) override;
};
