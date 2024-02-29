// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FMODEvent.h"
#include "GenericStructs.generated.h" 

class UNiagaraSystem;
USTRUCT(BlueprintType)
struct FWeaponsStruct  : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WeaponName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage *AttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage *RollAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage *SprintAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* WeaponMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HitBoxLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator HitBoxRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HitBoxExtent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TraceStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TraceEnd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 ComboAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StepSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFMODEvent* SwingSFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFMODEvent* ImpactSFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* WeaponSkillVFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SkillVFXLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator SkillVFXRotation;
};

UENUM(BlueprintType)
enum class ECharacterLegPosition : uint8
{
	ECL_Neutral UMETA(DisplayName = "Neutral"),
	ECL_Right UMETA(DisplayName = "Right"),
	ECL_Left UMETA(DisplayName = "Left")
};



class STYLIZEDTEST_API GenericStructs
{
public:
	GenericStructs();
	~GenericStructs();
};
