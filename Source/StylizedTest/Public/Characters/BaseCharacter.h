// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "FMODBlueprintStatics.h"
#include "BaseCharacter.generated.h"

class ABaseWeapon;
class UAttributesComponent;

UCLASS()
class STYLIZEDTEST_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	void StopMontage(UAnimMontage* Montage, float BlendTime = 0.2);

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	ABaseWeapon* EquippedWeapon;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	ABaseWeapon* EquippedWeapon2;

	FTimerHandle SprintTimerHandle;

	UPROPERTY(EditDefaultsOnly,Category="Animation")
	float MontageToLocomotionBlendTime =0.25;
	


protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<ABaseWeapon> RightHandWeapon;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<ABaseWeapon> LeftHandWeapon;

	bool IsDead=false;


	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly, Category="Movement", meta=(ToolTip="The base run speed of the character"))
	float RunSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, Category="Movement", meta=(ToolTip="The base sprinting speed of the character"))
	float SprintSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly, Category="Weapons", meta=(ToolTip="Data Table of all the weapons in the game", RowType="WeaponsData"))
	class UDataTable* WeaponsTable;

	UPROPERTY(VisibleAnywhere, Category="Weapons")
	uint8 ComboCounter = 0;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Movement", meta=(ToolTip="The base movement speed of the character when he is using the healing ability"))
	float HealMoveSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, Category="Movement", meta=(ToolTip="The Current Max Running speed of the character"))
	float CurrentMaxSpeed = RunSpeed;

	UPROPERTY(EditDefaultsOnly, Category="Movement", meta=(ToolTip="The Speed at which the character will play the turning animation when taking a 180 degree turn"))
	float TurningSpeed = RunSpeed / 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UAttributesComponent* AttributesComp;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter,bool CauseStagger) override;

	FVector LatestImpactPoint;

	UFUNCTION(BlueprintCallable)
	void AssignWeapon(FName Weapon, FName Hand, uint8 SlotNumber);

	UPROPERTY()
	float LatestDamageMulti=1;
	
	//GETTERS AND SETTERS
	public:
	UAttributesComponent* GetAttributesComp()
	{
		return AttributesComp;
	}

	UFUNCTION(BlueprintCallable)
	uint8 GetComboCounter() const
	{
		return ComboCounter;
	}

	UFUNCTION(BlueprintCallable)
	void SetComboCounter(const uint8 CounterValue)
	{
		this->ComboCounter = CounterValue;
	}

	ABaseWeapon* GetEquippedWeapon() const
	{
		return EquippedWeapon;
	}

	ABaseWeapon* GetEquippedWeapon2() const
	{
		return EquippedWeapon2;
	}

	float GetRunSpeed() const
	{
		return RunSpeed;
	}

	float GetSprintSpeed()
	{
		return SprintSpeed;
	}

	void SetSprintSpeed(float Speed)
	{
		SprintSpeed = Speed;
	}

	float GetHealMoveSpeed()
	{
		return HealMoveSpeed;
	}

	void SetHealMoveSpeed(float Speed)
	{
		HealMoveSpeed = Speed;
	}

	float GetCurrentMaxSpeed() const
	{
		return CurrentMaxSpeed;
	}

	void SetCurrentMaxSpeed(float CurrentSpeed)
	{
		this->CurrentMaxSpeed = CurrentSpeed;
	}

	float GetTurningSpeed() const
	{
		return TurningSpeed;
	}

	void SetTurningSpeed(float TurnSpeed)
	{
		this->TurningSpeed = TurnSpeed;
	}

	FVector GetLatestImpactPoint() const
	{
		return LatestImpactPoint;
	}

	void SetLatestImpactPoint(const FVector& LatestImpact)
	{
		this->LatestImpactPoint = LatestImpact;
	}

	bool GetIsDead()
	{
		return IsDead;
	}

	void SetIsDead(bool Dead)
	{
		IsDead=Dead;
	}

	void SetLatestDamageMulti(float DamageMulti)
	{
		LatestDamageMulti= DamageMulti;
	}

	float GetLatestDamageMulti()
	{
		return LatestDamageMulti;
	}


	//Helper Functions
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled();
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionDisabled();

	UFUNCTION(BlueprintCallable)
	void SetWeapon2CollisionEnabled();
	UFUNCTION(BlueprintCallable)
	void SetWeapon2CollisionDisabled();

	UFUNCTION(BlueprintCallable)
	void ResetCombo();


	UPROPERTY(BlueprintReadOnly)
	FVector StartRotation;
	UPROPERTY(BlueprintReadOnly)
	FVector EndRotation;
};
