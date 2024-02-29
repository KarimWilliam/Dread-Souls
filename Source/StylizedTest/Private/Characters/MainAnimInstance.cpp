// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainAnimInstance.h"

#include "Camera/CameraComponent.h"
#include "Characters/MainCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	MainCharacter = Cast<AMainCharacter>(TryGetPawnOwner());
	if (MainCharacter)
	{
		MainCharacterMovement = MainCharacter->GetCharacterMovement();
		MainCharacterActionComponent = MainCharacter->GetActionComp();
		MainCharacterAttributesComponent = MainCharacter->GetAttributesComp();
		MainCharacterQueueComponent = MainCharacter->GetQueueComp();
		EquippedWeapon = MainCharacter->GetEquippedWeapon();
		EquippedWeapon2 = MainCharacter->GetEquippedWeapon2();
		bMontageIsPlaying = false;
		CameraPitch=MainCharacter->GetControlRotation().Pitch;
		bIsLongIdle=false;
	}
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (MainCharacter && MainCharacterMovement)
	{
		EquippedWeapon = MainCharacter->GetEquippedWeapon();
		EquippedWeapon2 = MainCharacter->GetEquippedWeapon2();
		IsFalling = MainCharacterMovement->IsFalling();
		IsTurning = MainCharacter->b180Turn;
		IsLockedOn = MainCharacter->bIsLockedOn;
		bIsLongIdle=MainCharacter->bIsIdle;
		bMontageIsPlaying = Montage_IsPlaying(NULL);
		ZWorldRotation = MainCharacter->GetCapsuleComponent()->GetComponentRotation().Yaw;
		GroundSpeed = UKismetMathLibrary::VSizeXY(MainCharacterMovement->Velocity);
		if(MainCharacter->GetControlRotation().Pitch>30)
		{
			CameraPitch=(MainCharacter->GetControlRotation().Pitch-360) ;
		}else
		{
			CameraPitch=MainCharacter->GetControlRotation().Pitch ;
		}
		CameraYaw=CalculateViewYaw();
			
		//UE_LOG(LogTemp, Warning, TEXT("Camera Yaw:  %f"),CameraYaw);
	}
}

float UMainAnimInstance::CalculateViewYaw()
{
	FRotator CharacterRotation=MainCharacter->GetActorRotation();
	FRotator ControlRotation = MainCharacter->GetControlRotation();
	// Extract Yaw components
	float CharacterYaw = CharacterRotation.Yaw;
	float ControlYaw = ControlRotation.Yaw;
	// Calculate the angle between Yaw components
	float Angle = FMath::FindDeltaAngleDegrees(CharacterYaw, ControlYaw);

	// Adjust angle range if needed
	Angle = FMath::Fmod(Angle, 360.0f);
	if (Angle > 180.0f) {
		Angle = 360.0f - Angle;
	}
return Angle;
}
