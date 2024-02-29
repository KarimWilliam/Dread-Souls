// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/TurnAction.h"
#include "Characters/MainCharacter.h"
#include "Components/ActionComponent.h"


UTurnAction::UTurnAction()
{
	ActionName = "TurnAction";
	bQueueableAction = false;
	bAUXAction=true;
}

void UTurnAction::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	AMainCharacter* MC = Cast<AMainCharacter>(Instigator);
	if (MC)
	{
		Character = MC;
		MC->StartRotation = MC->GetActorForwardVector();
		MC->EndRotation = MC->GetDirectionalIntent();
		MC->DisableMovement();
		if(MC->EndRotation.Y>MC->StartRotation.Y)
		{
			MC->PlayMontageSection(TurnMontage, "TurnRight");
		} else
		{
			MC->PlayMontageSection(TurnMontage, "TurnLeft");
		}


	}
}

void UTurnAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Turn action stopped"));
	//Enable movement and stopping the timeline is dealt with in blueprints
	Character->EnableMovement();
	Super::StopAction_Implementation(Instigator);
}

void UTurnAction::InterruptAction(AActor* Instigator)
{
	UE_LOG(LogTemp, Warning, TEXT("Turn action interrupted"));
	Super::InterruptAction(Instigator);
	AMainCharacter* MC= Cast<AMainCharacter>(Instigator);
	MC->GetActionComp()->StopActionByName(MC,ActionName);
	MC->EnableMovement();
}

