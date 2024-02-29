// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/StaggerAction.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"
#include "Components/AttributesComponent.h"

UStaggerAction::UStaggerAction()
{
	ActionName = "StaggerAction";
	bBypassBusy = true;
}

void UStaggerAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);


	if (DoOnce)
	{
		MC = Cast<AMainCharacter>(Instigator);
		MC->OnMove.AddDynamic(this, &UStaggerAction::OnMove);
		DoOnce = false;
	}
	UActionComponent* Comp = GetOwningComponent();
	Comp->InterruptRunningActions(Instigator);
	Comp->ActiveGamePlayTags.RemoveTags(RemoveTags);
	if (MC)
	{
		MC->ResetCombo();

		MC->DisableMovement();
		FVector ImpactPoint = MC->GetLatestImpactPoint();

		const FVector Forward = MC->GetActorForwardVector();
		// Lower Impact Point to the Enemy's Actor Location Z
		const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, MC->GetActorLocation().Z);
		const FVector ToHit = (ImpactLowered - MC->GetActorLocation()).GetSafeNormal();

		// Forward * ToHit = |Forward||ToHit| * cos(theta)
		// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
		const double CosTheta = FVector::DotProduct(Forward, ToHit);
		// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
		double Theta = FMath::Acos(CosTheta);
		// convert from radians to degrees
		Theta = FMath::RadiansToDegrees(Theta);

		// if CrossProduct points down, Theta should be negative
		const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
		if (CrossProduct.Z < 0)
		{
			Theta *= -1.f;
		}

		FName Section("FromBack");

		if (Theta >= -45.f && Theta < 45.f)
		{
			Section = FName("FromFront");
		}
		else if (Theta >= -135.f && Theta < -45.f)
		{
			Section = FName("FromLeft");
		}
		else if (Theta >= 45.f && Theta < 135.f)
		{
			Section = FName("FromRight");
		}

		MC->PlayMontageSection(StaggerMontage, Section);
	}
}

void UStaggerAction::StopAction_Implementation(AActor* Instigator)
{
	MC->EnableMovement();
	static FGameplayTag StaggerRecover = FGameplayTag::RequestGameplayTag("StaggerRecover");
	MC->GetActionComp()->ActiveGamePlayTags.AddTag(StaggerRecover);
	UActionComponent* Comp = GetOwningComponent();
	MC->GetAttributesComp()->BeginStaminaRegain();
	Super::StopAction_Implementation(Instigator);
}

void UStaggerAction::OnMove(AActor* InstigatorActor)
{
	MC->GetActionComp()->ActiveGamePlayTags.RemoveTag(FGameplayTag::RequestGameplayTag("StaggerRecover"));
	MC->StopMontage(StaggerMontage);
}
