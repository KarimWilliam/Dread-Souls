// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayFunctionLibrary.h"
#include "Components/AttributesComponent.h"

bool UGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, bool Blockable, bool Parryable, bool Dodgeable,bool CauseStagger)
{
	UAttributesComponent* AttributeComp = UAttributesComponent::GetAttributes(TargetActor);
	if (AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount,Blockable,Parryable,Dodgeable,CauseStagger);
	}
	return false;
}


bool UGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult, bool Blockable, bool Parryable, bool Dodgeable)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			// Direction = Target - Origin
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();

			HitComp->AddImpulseAtLocation(Direction * 300000.f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	return false;
}

bool UGameplayFunctionLibrary::ApplyHealing(AActor* HealCauser, AActor* TargetActor, float HealAmount)
{
	UAttributesComponent* AttributeComp = UAttributesComponent::GetAttributes(TargetActor);
	if (AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(HealCauser, HealAmount,false,false,false,false);
	}
	return false;
}
