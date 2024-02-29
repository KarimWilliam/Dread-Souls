// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STYLIZEDTEST_API UGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, bool Blockable = true, bool Parryable = true, bool Dodgeable = true, bool CauseStagger=true);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult, bool Blockable = true, bool Parryable = true, bool Dodgeable = true);


	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyHealing(AActor* HealCauser, AActor* TargetActor, float HealAmount);
};
