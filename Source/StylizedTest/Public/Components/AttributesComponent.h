// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributesComponent.generated.h"

class UAttributesComponent;
class AMainCharacter;
class UActionComponent;
//Create a custom event that notifies subscribers when something happens (avoids checking with ticks) 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, UAttributesComponent*, OwningComp, float, NewHealth, float, Delta);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStaminaChanged, UAttributesComponent*, OwningComp, float, NewStamina, float, Delta);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STYLIZEDTEST_API UAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributesComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	static UAttributesComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category="Attributes", meta=(AdvancedDisplay="IsAlive"))
	static bool IsActorAlive(AActor* Actor);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaDeplete);

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnStaminaDeplete OnStaminaDeplete;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Stamina;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes", meta=(ToolTip="Passive Stamina regian rate per second"))
	float StaminaRegenRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes", meta=(ToolTip="Passive Stamina regian rate per second when focusing on an action like healing"))
	float SlowStaminaRegainRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes", meta=(ToolTip="Stamina decay when sprinting per second"))
	float StaminaSprintDecay;

	float GetHealthPercent();
	float GetStaminaPercent();

	UFUNCTION(BlueprintCallable)
	void BeginStaminaRegain();
	UFUNCTION()
	void ImmediateStaminaRegain();


	UFUNCTION(BlueprintCallable)
	bool IsAlive() const;

	void ModifyHealChargeProgress(float Delta);
	void ModifyAbility1ChargeProgress(float Delta);
	void ModifyAbility2ChargeProgress(float Delta);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta, bool Blockable, bool Parryable, bool Dodgeable, bool CauseStagger);

	UFUNCTION(BlueprintCallable, Category="Attributes")
	void ApplyStaminaChange(float Delta, bool StaminaRegain);

	UFUNCTION(BlueprintCallable)
	bool Kill(AActor* InstigatorActor);

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnHealthChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnStaminaChanged OnStaminaChanged;

protected:
	virtual void BeginPlay() override;

	void RegainStamina(float DeltaTime);
	void DecreaseStamina(float DeltaTime, float Rate);
	void StaminaHitZero();


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float MinStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float StaminaRegainDelay;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* DeathMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	int8 MaxHealCharges = 3;
	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	int8 HealCharges = 1;
	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	float HealChargeProgressMax = 1;
	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	float HealChargeProgress = 0;
	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	float Ability1ChargeProgress = 0;
	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	float Ability2ChargeProgress = 0;
	
	bool bIsInstantStaminaRegainPossible = true;
	float LatestDelta = 0;

	UFUNCTION()
	void SetRegainActive();
	FTimerHandle StaminaRegainTimerHandle;
	FTimerDelegate StaminaDelegate;

	UPROPERTY()
	AMainCharacter* MC;
	UPROPERTY()
	UActionComponent* ActionComp;

private:
public:
	//HELPERS

	void SetMaxHealth(float MaxH);
	float GetMaxHealth();
	float GetHealth();
	float GetStamina();
	float GetMaxStamina();
	float GetMinStamina();
	void SetMaxStamina(float MaxS);
	bool HasStamina();
	int GetMaxHeals();
	int GetHeals();
	void SetHeals(int HealsLeft);
	float GetHealChargeProgress();
	float GetAbility1ChargeProgress();
	float GetAbility2ChargeProgress();
	void SetAbility1ChargeProgress(float Percentage);
	void SetAbility2ChargeProgress(float Percentage);

	bool bRegainActive = true;
	bool IsRaging = false;
};
