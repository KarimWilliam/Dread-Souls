// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class UNiagaraSystem;
class UBoxComponent;
class UNiagaraComponent;

UCLASS()
class STYLIZEDTEST_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABaseWeapon();
	virtual void Tick(float DeltaTime) override;

	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);
	void DisableSphereCollision();

	TArray<AActor*> IgnoreActors;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, Category="Animation")
	bool BossAnimPause =true;

	UPROPERTY(EditAnywhere, Category="VFX")
	UNiagaraComponent* WeaponSkillEffectComponent;

protected:
	virtual void BeginPlay() override;

	bool ActorIsSameType(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& BoxHit);

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* BloodSplatterEffect;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* SparkEffect;

private:
	void BoxTrace(TArray<FHitResult>& BoxHit);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(10.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = true;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UAnimMontage* AttackMontage;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UAnimMontage* RollAttackMontage;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UAnimMontage* SprintAttackMontage;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	uint8 MaxComboAmount = 0;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	float StepSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	FName Name = "";

	UPROPERTY(VisibleAnywhere, Category = "FMOD")
	class UFMODEvent* SwingSFX;

	UPROPERTY(VisibleAnywhere, Category = "FMOD")
	class UFMODEvent* ImpactSFX;

	UPROPERTY(VisibleAnywhere, Category = "VFX")
	class UNiagaraSystem* WeaponSkillVFX;

	UPROPERTY(VisibleAnywhere, Category = "VFX")
	FVector SkillVFXLocation;


private:
	UPROPERTY(VisibleAnywhere, Category = "VFX")
	FRotator SkillVFXRotation;

	UPROPERTY(EditDefaultsOnly,Category="Effects")
	float AnimPauseDuration=0.1;

	UPROPERTY()
	class AMainCharacter* MC;


	UFUNCTION()
void UnpauseAnimation(class AMainCharacter* MainCharacter, class AEnemyCharacter* EnemyCharacter);
	FTimerHandle RollTimerHandle;


	//GETTERS AND SETTERS
public:
	USceneComponent* GetBoxTraceStart() const
	{
		return BoxTraceStart;
	}

	USceneComponent* GetBoxTraceEnd() const
	{
		return BoxTraceEnd;
	}

	void SetAnimMontage(UAnimMontage* Montage)
	{
		AttackMontage = Montage;
	}

	UAnimMontage* GetAnimMontage() const
	{
		return AttackMontage;
	}

	void SetSprintAttackAnimMontage(UAnimMontage* Montage)
	{
		SprintAttackMontage = Montage;
	}

	UAnimMontage* GetSprintAttackAnimMontage() const
	{
		return SprintAttackMontage;
	}

	void SetRollAttackAnimMontage(UAnimMontage* Montage)
	{
		RollAttackMontage = Montage;
	}

	UAnimMontage* GetRollAttackAnimMontage() const
	{
		return RollAttackMontage;
	}

	uint8 GetMaxComboAmount() const
	{
		return MaxComboAmount;
	}

	void SetMaxComboAmount(uint8 Combo)
	{
		MaxComboAmount = Combo;
	}

	float GetStepSpeed() const
	{
		return StepSpeed;
	}

	void SetStepSpeed(float Speed)
	{
		StepSpeed = Speed;
	}

	void SetDamage(float SetterDamage)
	{
		Damage = SetterDamage;
	}

	float GetDamage() const
	{
		return Damage;
	}

	FName GetWeaponName()
	{
		return Name;
	}

	void SetName(FName WeaponName)
	{
		Name = WeaponName;
	}

	void SetSwingSFX(UFMODEvent* SwingSoundFile)
	{
		SwingSFX = SwingSoundFile;
	}

	UFUNCTION(BlueprintCallable)
	UFMODEvent* GetSwingSFX() const
	{
		return SwingSFX;
	}

	void SetImpactSFX(UFMODEvent* ImpactSoundFile)
	{
		ImpactSFX = ImpactSoundFile;
	}

	UFUNCTION(BlueprintCallable)
	UFMODEvent* GetImpactSFX() const
	{
		return ImpactSFX;
	}

	UFUNCTION(BlueprintCallable)
	USceneComponent* GetBoxTraceEnd()
	{
		return BoxTraceEnd;
	}

public:
	UNiagaraSystem* GetWeaponSkillVfx() const
	{
		return WeaponSkillVFX;
	}

	void SetWeaponSkillVfx(UNiagaraSystem* WeaponSkillVfx)
	{
		WeaponSkillVFX = WeaponSkillVfx;
	}

	FVector GetSkillVfxLocation() const
	{
		return SkillVFXLocation;
	}

	void SetSkillVfxLocation(const FVector SkillVfxLocation)
	{
		SkillVFXLocation = SkillVfxLocation;
	}

	FRotator GetSkillVfxRotation() const
	{
		return SkillVFXRotation;
	}

	void SetSkillVfxRotation(const FRotator SkillVfxRotation)
	{
		SkillVFXRotation = SkillVfxRotation;
	}

	UBoxComponent* GetWeaponBox() const { return WeaponBox; }

	void SetWeaponBoxExtentY(float Extent);
	void SetBoxLocationY(float Loc);
	void SetBoxTraceEndY(float Loc);


	
};


