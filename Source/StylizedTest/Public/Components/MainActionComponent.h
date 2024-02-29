// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActionComponent.h"
#include "MainActionComponent.generated.h"

UCLASS()
class STYLIZEDTEST_API UMainActionComponent : public UActionComponent
{
	GENERATED_BODY()


public:
	virtual bool StartActionByName(AActor* Instigator, FName ActionName) override;

	
	FName CheckIfSprintAttacking(AActor* Instigator, FName ActionName);

protected:

		
};
