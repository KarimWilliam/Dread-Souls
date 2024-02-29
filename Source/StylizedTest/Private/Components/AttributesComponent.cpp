// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributesComponent.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PawnMovementComponent.h"

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxHealth = 100.0f;
	MaxStamina = 100.f;
	MinStamina = -50.f;
	Stamina = MaxStamina;
	Health = MaxHealth;
	StaminaRegenRate = 2.f;
	StaminaRegainDelay = 3.f;
	StaminaSprintDecay = 30.f;
	SlowStaminaRegainRate = 1.f;
}

void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

	StaminaDelegate.BindUFunction(this, "SetRegainActive");
	GetWorld()->GetTimerManager().SetTimer(StaminaRegainTimerHandle, StaminaDelegate, StaminaRegainDelay, true);

	MC = Cast<AMainCharacter>(GetOwner());
	if (MC) ActionComp = MC->GetActionComp();
}

void UAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//UE_LOG(LogTemp, Warning, TEXT("Regainactive? :  %s"),bRegainActive? "True":"False");
	if (bRegainActive && Stamina < MaxStamina)
	{
		RegainStamina(DeltaTime);
	}

	if (ActionComp && ActionComp->ActiveGamePlayTags.HasTag(FGameplayTag::RequestGameplayTag("Sprinting")) && MC->GetMovementComponent()->Velocity.Size() > MC->GetRunSpeed() + 20 && !MC->
		GetActionComp()->ActiveGamePlayTags.HasTag(FGameplayTag::RequestGameplayTag("Busy")) && !IsRaging)
	{
		DecreaseStamina(DeltaTime, StaminaSprintDecay);
	}
}

bool UAttributesComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta, bool Blockable, bool Parryable, bool Dodgeable, bool CauseStagger)
{
	if (MC && ActionComp)
	{
		if (Dodgeable && ActionComp->ActiveGamePlayTags.HasTag(FGameplayTag::RequestGameplayTag("Rolling"))) return false;
	}
	const float OldHealth = Health;
	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);
	const float ActualDelta = Health - OldHealth;

	//call the custom event we made.
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	//Died 
	if (ActualDelta < 0.0f && Health <= 0.0f)
	{
		if (MC)
		{
			//MC->DisableInput(GetWorld()->GetFirstPlayerController());
			MC->DisableMovement();
			MC->GetActionComp()->InterruptRunningActions(MC);
			MC->PlayAnimMontage(DeathMontage);
		}
	}
	return ActualDelta != 0;
}

void UAttributesComponent::ApplyStaminaChange(float Delta, bool StaminaRegain)
{

	LatestDelta = Delta;
	if (bIsInstantStaminaRegainPossible) bIsInstantStaminaRegainPossible = StaminaRegain;
	if (IsRaging && Delta < 0) return;
	if (Delta < 0)
	{
		bRegainActive = false;
		GetWorld()->GetTimerManager().SetTimer(StaminaRegainTimerHandle, StaminaDelegate, StaminaRegainDelay, true, 100);
		if (Stamina <= 0) StaminaHitZero();
	}

	Stamina = FMath::Clamp(Stamina + Delta, MinStamina, MaxHealth);


	OnStaminaChanged.Broadcast(this, Stamina, Delta);
}

void UAttributesComponent::BeginStaminaRegain()
{
	GetWorld()->GetTimerManager().SetTimer(StaminaRegainTimerHandle, StaminaDelegate, StaminaRegainDelay, true);
}

void UAttributesComponent::ImmediateStaminaRegain()
{
	if (bIsInstantStaminaRegainPossible) SetRegainActive();
}

void UAttributesComponent::RegainStamina(float DeltaTime)
{
	static FGameplayTag Healing = FGameplayTag::RequestGameplayTag("Healing");
	bIsInstantStaminaRegainPossible = true;
	if (MC->GetActionComp()->ActiveGamePlayTags.HasTag(Healing))
	{
		ApplyStaminaChange(SlowStaminaRegainRate * DeltaTime, true);
	}
	else
	{

		ApplyStaminaChange(StaminaRegenRate * DeltaTime, true);
	}

	//Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, MinStamina, MaxStamina);
}

void UAttributesComponent::DecreaseStamina(float DeltaTime, float Rate)
{
	bIsInstantStaminaRegainPossible = true;
	//if (IsRaging)return;
	ApplyStaminaChange(-Rate * DeltaTime, true);
	//Stamina = FMath::Clamp(Stamina - Rate * DeltaTime, MinStamina, MaxStamina);
}

void UAttributesComponent::StaminaHitZero()
{
	OnStaminaDeplete.Broadcast();
}

void UAttributesComponent::ModifyHealChargeProgress(float Delta)
{
	if (HealCharges == MaxHealCharges) return;

	if (Delta < 0)
	{
		if (HealChargeProgress + Delta <= 0)
		{
			HealChargeProgress = 0;
		}
		else
		{
			HealChargeProgress = HealChargeProgress + Delta;
		}
		MC->UpdateMainOverlayHealCharge(HealChargeProgress);
		return;
	}
	if (HealChargeProgress + Delta < HealChargeProgressMax)
	{
		HealChargeProgress = HealChargeProgress + Delta;
		MC->UpdateMainOverlayHealCharge(HealChargeProgress);
		return;
	}

	if (HealChargeProgress + Delta >= HealChargeProgressMax)
	{
		HealCharges++;
		//maxed out and keep the charge bar full

		Delta = Delta - HealChargeProgressMax - HealChargeProgress;
		float newDelta = FMath::Max(0, Delta);
		if (newDelta < HealChargeProgressMax)
		{
			HealChargeProgress = newDelta;
		}
		else
		{
			HealChargeProgress = 0;
			ModifyHealChargeProgress(newDelta);
		}
		MC->UpdateMainOverlayHealText(true);
		MC->UpdateMainOverlayHealCharge(HealChargeProgress);
		return;
	}
}

void UAttributesComponent::ModifyAbility1ChargeProgress(float Delta)
{
	Ability1ChargeProgress = FMath::Clamp(Ability1ChargeProgress + Delta, 0, 1);
	MC->UpdateMainOverlayAbility1Charge(Ability1ChargeProgress, true);
}

void UAttributesComponent::ModifyAbility2ChargeProgress(float Delta)
{
	Ability2ChargeProgress = FMath::Clamp(Ability2ChargeProgress + Delta, 0, 1);
	MC->UpdateMainOverlayAbility2Charge(Ability2ChargeProgress, true);
}


bool UAttributesComponent::IsActorAlive(AActor* Actor)
{
	UAttributesComponent* AttributesComp = GetAttributes(Actor);
	if (AttributesComp)
	{
		return AttributesComp->IsAlive();
	}
	return false;
}

//HELPER FUNCTIONS

bool UAttributesComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetMaxHealth(), false, false, false, false);
}

void UAttributesComponent::SetRegainActive()
{
	bRegainActive = true;
}

void UAttributesComponent::SetMaxHealth(float MaxH)
{
	this->MaxHealth = MaxH;
}

float UAttributesComponent::GetMaxHealth()
{
	return MaxHealth;
}

float UAttributesComponent::GetHealth()
{
	return Health;
}

float UAttributesComponent::GetStamina()
{
	return Stamina;
}

float UAttributesComponent::GetMaxStamina()
{
	return MaxStamina;
}

float UAttributesComponent::GetMinStamina()
{
	return MinStamina;
}

void UAttributesComponent::SetMaxStamina(float MaxS)
{
	MaxStamina = MaxS;
}

bool UAttributesComponent::HasStamina()
{
	return Stamina > 0;
}

int UAttributesComponent::GetMaxHeals()
{
	return MaxHealCharges;
}

int UAttributesComponent::GetHeals()
{
	return HealCharges;
}


void UAttributesComponent::SetHeals(int HealsLeft)
{
	HealCharges = HealsLeft;
	MC->UpdateMainOverlayHealCharge(HealChargeProgress);
	MC->UpdateMainOverlayHealCharge(HealChargeProgress);
}

float UAttributesComponent::GetHealChargeProgress()
{
	return HealChargeProgress;
}

float UAttributesComponent::GetAbility1ChargeProgress()
{
	return Ability1ChargeProgress;
}

float UAttributesComponent::GetAbility2ChargeProgress()
{
	return Ability2ChargeProgress;
}

void UAttributesComponent::SetAbility1ChargeProgress(float Percentage)
{
	Ability1ChargeProgress = Percentage;
}

void UAttributesComponent::SetAbility2ChargeProgress(float Percentage)
{
	Ability2ChargeProgress = Percentage;
}


float UAttributesComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributesComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;
}


bool UAttributesComponent::IsAlive() const
{
	return Health > 0.f;
}

UAttributesComponent* UAttributesComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<UAttributesComponent>(FromActor->GetComponentByClass(UAttributesComponent::StaticClass()));
	}
	return nullptr;
}
