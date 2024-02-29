// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "SpawnAction.generated.h"


UCLASS()
class STYLIZEDTEST_API USpawnAction : public UAction
{
	GENERATED_BODY()

public:
	USpawnAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	virtual void InterruptAction(AActor* Instigator) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> SpawnMontage;
	class AMainCharacter* MC;
};
