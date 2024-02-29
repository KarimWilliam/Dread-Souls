// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/EnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	EnemyCharacter = Cast<AEnemyCharacter>(TryGetPawnOwner());
	if (EnemyCharacter)
	{
		EnemyCharacterMovement = EnemyCharacter->GetCharacterMovement();
		EnemyCharacterAttributesComponent = EnemyCharacter->GetAttributesComp();
		IsDead = EnemyCharacter->GetIsDead();
		EnemyCharacterActionComponent = EnemyCharacter->GetActionComp();
		AIActive = EnemyCharacter->GetAIActive();
		AIController = Cast<AAIController>(EnemyCharacter->GetController());
		bIsFocused = false;
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (EnemyCharacter && EnemyCharacterMovement)
	{
		AIController = Cast<AAIController>(EnemyCharacter->GetController());
		EquippedWeapon = EnemyCharacter->GetEquippedWeapon();
		IsFalling = EnemyCharacterMovement->IsFalling();
		GroundSpeed = UKismetMathLibrary::VSizeXY(EnemyCharacterMovement->Velocity);
		ZWorldRotation = EnemyCharacter->GetCapsuleComponent()->GetComponentRotation().Yaw;
		IsDead = EnemyCharacter->GetIsDead();
		AIActive = EnemyCharacter->GetAIActive();

		if (AIController) AIController->GetFocusActor() ? AngleToTarget = CalculateTargetAngle() : AngleToTarget = 0;

	}
}

float UEnemyAnimInstance::CalculateTargetAngle()
{
	//Drop the Z axis cause we dont care about it in the calculation. we want everything to be in the XY axis
	if (EnemyCharacter->GetTargetActor())
	{
		FVector DropZ = FVector(1, 1, 0);
		FVector ForwardVector = (EnemyCharacter->GetActorForwardVector()).GetSafeNormal();
		FVector TargetLocation = EnemyCharacter->GetTargetActor()->GetActorLocation() * DropZ;
		FVector ActorLocation = EnemyCharacter->GetActorLocation() * DropZ;
		//The vector going from me to my target
		FVector DirectionVec = (TargetLocation - ActorLocation).GetSafeNormal();
		//Angle between Forward vector and vector going to my target in radians and unsigned.
		float Angle = FMath::Acos(FVector::DotProduct(ForwardVector, DirectionVec));
		float AngleInDegree = FMath::RadiansToDegrees(Angle);
		//Cross product helps us distinguish between negative and positive angle. i am not sure how exactly.
		FVector CrossVector = FVector::CrossProduct(ForwardVector, DirectionVec);
		if (CrossVector.Z < 0) AngleInDegree = AngleInDegree * -1;

		return AngleInDegree;
	}
	return 0;
}
