// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/LockInterface.h"
#include "GenericStructs.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/BossFightOverlay.h"
#include "EnemyCharacter.generated.h"

class UAttributesComponent;
class UPawnSensingComponent;
class UEnemyActionComponent;
class UCombatOverlay;
UCLASS()
class STYLIZEDTEST_API AEnemyCharacter : public ABaseCharacter, public ILockInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;


	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetActor() const;

	UEnemyActionComponent* GetActionComp();

	//HUD
	void InitializeBossOverlay();



	UPROPERTY(EditAnywhere, Category="AI")
	AActor* FocusTarget;

	bool bFocusTarget = true;

	float MoveRotationSpeed;

	UPROPERTY(EditDefaultsOnly,Category="Player")
	float PlayerHealChargeMultiplier=3;

	UPROPERTY(EditDefaultsOnly,Category="Player")
	float PlayerAbility1ChargeMultiplier=3;

	UPROPERTY(EditDefaultsOnly,Category="Player")
	float PlayerAbility2ChargeMultiplier=3;

	UPROPERTY()
	FVector LockOnLocation;

	UPROPERTY(EditDefaultsOnly,Category="Player")
	class UBillboardComponent* LockOnTargetLocation;


protected:
	virtual void GetLocked_Implementation(AActor* Locker, bool Release) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter, bool CauseStagger) override;
	virtual bool IsLockable_Implementation() override;
	/* Key for AI Blackboard 'TargetActor' */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TargetActorKey;
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName MoveToLocationKey;

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UAttributesComponent* OwningComp, float NewHealth, float Delta);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UPawnSensingComponent> PawnSensingComp;

	UPROPERTY(EditAnywhere)
	ECharacterLegPosition CharacterLegPosition = ECharacterLegPosition::ECL_Neutral;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	void SetHUDHealth();
	


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UEnemyActionComponent* EnemyActionComponent;

	UPROPERTY(EditAnywhere)
	bool AIActive;

	UPROPERTY(EditAnywhere)
	bool IsLockable =true;

	UPROPERTY(EditDefaultsOnly,Category="Starter Info")
	FName BossName;
	
	UPROPERTY()
	class UBlackboardComponent* BlackBoardComp;

	UPROPERTY()
	UBossFightOverlay* BossFightOverlay;
	UPROPERTY()
	class UVictoryDeathOverlay* VictoryDeathOverlay;
	UPROPERTY()
	UCombatOverlay* CombatOverlay;

	FTimerHandle RemoveOverLayHandle;
	FTimerHandle RemoveVictoryOverLayHandle;

	UFUNCTION()
	void RemoveBossOverlay();
	UFUNCTION()
	void RemoveVictoryOverlay();

	//GETTERS AND SETTERS
public:
	UFUNCTION(BlueprintGetter)
	ECharacterLegPosition GetCharacterLegPosition()
	{
		return CharacterLegPosition;
	}

	UFUNCTION(BlueprintSetter)
	void SetCharacterLegPosition(ECharacterLegPosition CLP)
	{
		CharacterLegPosition = CLP;
	}

	bool GetAIActive()
	{
		return AIActive;
	}

	bool GetIsLockable()
	{
		return IsLockable;
	}

	FName GetBossName()
	{
		return BossName;
	}

	void SetAiActive(bool Active)
	{
		AIActive=Active;
	}
};
