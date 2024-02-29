// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Action.h"
#include "StaggerAction.generated.h"

class AMainCharacter;

UCLASS()
class STYLIZEDTEST_API UStaggerAction : public UAction
{
	GENERATED_BODY()

public:
	UStaggerAction();
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;


protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> StaggerMontage;
	

	AMainCharacter* MC;

	UPROPERTY(EditDefaultsOnly, Category="Tags")
	FGameplayTagContainer RemoveTags;
	UFUNCTION()
	void OnMove(AActor* InstigatorActor);

	bool DoOnce=true;
};
