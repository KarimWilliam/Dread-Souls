// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

class ABaseWeapon;
class AEnemyCharacter;
class UEnemyActionComponent;
UCLASS()
class STYLIZEDTEST_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	class AEnemyCharacter* EnemyCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* EnemyCharacterMovement;
	
	UPROPERTY(BlueprintReadOnly, Category = Action)
	class UAttributesComponent* EnemyCharacterAttributesComponent;
	
	UPROPERTY(BlueprintReadOnly, Category = Combat)
	ABaseWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = Action)
	UEnemyActionComponent* EnemyCharacterActionComponent;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float ZWorldRotation;

	UPROPERTY(BlueprintReadOnly, Category = AI)
	bool IsDead;

	UPROPERTY(BlueprintReadOnly, Category = AI)
	bool AIActive;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float AngleToTarget;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsFocused;

	UPROPERTY()
	class AAIController* AIController;
	

private:
	float CalculateTargetAngle();
};
