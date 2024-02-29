// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

class ABaseWeapon;

UCLASS()
class STYLIZEDTEST_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	class AMainCharacter* MainCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* MainCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Action)
	class UActionComponent* MainCharacterActionComponent;

	UPROPERTY(BlueprintReadOnly, Category = Camera)
	float CameraPitch;

	UPROPERTY(BlueprintReadOnly, Category = Camera)
	float CameraYaw;

	UPROPERTY(BlueprintReadOnly, Category = Action)
	class UAttributesComponent* MainCharacterAttributesComponent;

	UPROPERTY(BlueprintReadOnly, Category = Action)
	class UQueueComponent* MainCharacterQueueComponent;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	ABaseWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	ABaseWeapon* EquippedWeapon2;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsTurning;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsLockedOn;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float ZWorldRotation;

	UPROPERTY(BlueprintReadOnly, Category = Action)
	bool bMontageIsPlaying;

	UPROPERTY(BlueprintReadOnly, Category = Action)
	bool bIsLongIdle;

protected:
	float CalculateViewYaw(); 
};
