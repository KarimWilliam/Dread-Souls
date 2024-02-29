// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include  "Weapons/BaseWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GenericStructs.h"
#include "Components/BoxComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}



// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed=RunSpeed;
	CurrentMaxSpeed =RunSpeed;

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter,bool CauseStagger)
{
	IHitInterface::GetHit_Implementation(ImpactPoint, Hitter,CauseStagger);
	//TODO DARK Play hit sounds and spawn special effects. could also do it in subclasses maybe
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void ABaseCharacter::StopMontage(UAnimMontage* Montage,float BlendTime)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(MontageToLocomotionBlendTime,Montage);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::AssignWeapon(FName Weapon, FName HandSlot, uint8 SlotNumber)
{
	//TODO DARK Combine left and right weapons into one function with more variables and figuire out what actually calls this function
	FWeaponsStruct* MyDataRow = WeaponsTable->FindRow<FWeaponsStruct>(FName(Weapon), TEXT(""));
	if (ensureMsgf(RightHandWeapon,TEXT("Make sure RightHandWeapon Is assigned in the character Blueprint")))
	{
		if (SlotNumber == 1)
		{
			EquippedWeapon = GetWorld()->SpawnActor<ABaseWeapon>(RightHandWeapon);
			if (ensure(WeaponsTable) && ensure(MyDataRow) && ensure(EquippedWeapon))
			{
				EquippedWeapon->GetWeaponBox()->SetRelativeRotation(MyDataRow->HitBoxRotation);
				EquippedWeapon->GetWeaponBox()->SetBoxExtent(MyDataRow->HitBoxExtent);
				EquippedWeapon->ItemMesh->SetStaticMesh(MyDataRow->WeaponMesh);
				EquippedWeapon->SetStepSpeed(MyDataRow->StepSpeed);
				EquippedWeapon->SetDamage(MyDataRow->BaseDamage);
				EquippedWeapon->GetBoxTraceStart()->SetRelativeLocation(MyDataRow->TraceStart);
				EquippedWeapon->GetBoxTraceEnd()->SetRelativeLocation(MyDataRow->TraceEnd);
				EquippedWeapon->SetAnimMontage(MyDataRow->AttackMontage);
				EquippedWeapon->SetRollAttackAnimMontage(MyDataRow->RollAttackMontage);
				EquippedWeapon->SetSprintAttackAnimMontage(MyDataRow->SprintAttackMontage);
				EquippedWeapon->SetMaxComboAmount(MyDataRow->ComboAmount);
				EquippedWeapon->SetName(MyDataRow->WeaponName);
				EquippedWeapon->SetSwingSFX(MyDataRow->SwingSFX);
				EquippedWeapon->SetImpactSFX(MyDataRow->ImpactSFX);
				EquippedWeapon->SetSkillVfxLocation(MyDataRow->SkillVFXLocation);
				EquippedWeapon->SetSkillVfxRotation(MyDataRow->SkillVFXRotation);
				EquippedWeapon->SetWeaponSkillVfx(MyDataRow->WeaponSkillVFX);
				Cast<ABaseWeapon>(EquippedWeapon)->AttachMeshToSocket(GetMesh(), HandSlot);
				EquippedWeapon->SetOwner(this);
			}
		}
		else
		{
			EquippedWeapon2 = GetWorld()->SpawnActor<ABaseWeapon>(RightHandWeapon);
			if (ensure(WeaponsTable) && ensure(MyDataRow) && ensure(EquippedWeapon2))
			{
				EquippedWeapon2->GetWeaponBox()->SetRelativeRotation(MyDataRow->HitBoxRotation);
				EquippedWeapon2->GetWeaponBox()->SetBoxExtent(MyDataRow->HitBoxExtent);
				EquippedWeapon2->ItemMesh->SetStaticMesh(MyDataRow->WeaponMesh);
				EquippedWeapon2->SetStepSpeed(MyDataRow->StepSpeed);
				EquippedWeapon2->SetDamage(MyDataRow->BaseDamage);
				EquippedWeapon2->GetBoxTraceStart()->SetRelativeLocation(MyDataRow->TraceStart);
				EquippedWeapon2->GetBoxTraceEnd()->SetRelativeLocation(MyDataRow->TraceEnd);
				EquippedWeapon2->SetAnimMontage(MyDataRow->AttackMontage);
				EquippedWeapon2->SetRollAttackAnimMontage(MyDataRow->RollAttackMontage);
				EquippedWeapon2->SetSprintAttackAnimMontage(MyDataRow->SprintAttackMontage);
				EquippedWeapon2->SetMaxComboAmount(MyDataRow->ComboAmount);
				EquippedWeapon2->SetName(MyDataRow->WeaponName);
				EquippedWeapon2->SetSwingSFX(MyDataRow->SwingSFX);
				EquippedWeapon2->SetImpactSFX(MyDataRow->ImpactSFX);
				EquippedWeapon2->SetSkillVfxLocation(MyDataRow->SkillVFXLocation);
				EquippedWeapon2->SetSkillVfxRotation(MyDataRow->SkillVFXRotation);
				EquippedWeapon2->SetWeaponSkillVfx(MyDataRow->WeaponSkillVFX);
				Cast<ABaseWeapon>(EquippedWeapon2)->AttachMeshToSocket(GetMesh(), HandSlot);
				EquippedWeapon2->SetOwner(this);
			}
		}
	}
}


// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseCharacter::SetWeaponCollisionEnabled()
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

}

void ABaseCharacter::SetWeaponCollisionDisabled()
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EquippedWeapon->IgnoreActors.Empty();
	}

}

void ABaseCharacter::SetWeapon2CollisionEnabled()
{
	if(EquippedWeapon2 && EquippedWeapon2->GetWeaponBox())
	{
		EquippedWeapon2->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ABaseCharacter::SetWeapon2CollisionDisabled()
{
	if(EquippedWeapon2 && EquippedWeapon2->GetWeaponBox())
	{
		EquippedWeapon2->GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EquippedWeapon2->IgnoreActors.Empty();
	}
}

void ABaseCharacter::ResetCombo()
{
	ComboCounter=0;
}


