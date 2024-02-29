// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/LockInterface.h"
#include "UObject/UnrealTypePrivate.h"
#include "MainCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UActionComponent;
class UMainActionComponent;
class UAttributesComponent;
class UQueueComponent;
class UMainOverlay;
class UCombatOverlay;
class UVictoryDeathOverlay;
class UMainGameInstance;


UCLASS()

class STYLIZEDTEST_API AMainCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	void SetControllerDesiredRotation(bool Desire);
	void UpdateMainOverlayHealText(bool Healed);
	void UpdateMainOverlayHealCharge(float Percent);
	void UpdateMainOverlayAbility1Charge(float Percent,bool success);
	void UpdateMainOverlayAbility2Charge(float Percent,bool success);

	FVector GetDirectionalIntent();

	//Getters & Setters
	UActionComponent* GetActionComp() const;
	UQueueComponent* GetQueueComp() const;
	UCombatOverlay* GetCombatOverlay() const;

	UFUNCTION(BlueprintCallable)
	void DisableMovement();
	UFUNCTION(BlueprintCallable)
	void EnableMovement();
	UFUNCTION(BlueprintCallable)
	void ResolveSpeed();
	void UnlockTarget();

	void SetAnimPauseDuration(float PauseDuration);
	float GetAnimPauseDuration();

	//Public Variables
	bool b180Turn;
	bool bIsLockedOn;
	bool bSuspendForcedLookRotation;
	bool bIsRaging=false;
	float RageSpeedMultiplier=1;
	UPROPERTY(BlueprintReadWrite)
	bool bIsIdle;
	

	//Idle timer
	UFUNCTION()
	void PlayerIdle(AMainCharacter* MainCharacter, FVector Direction);
	FTimerDelegate IdleDelegate;
	FTimerHandle IdleTimerHandle;
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	float LongIdleDelay = 10;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMove, AActor*, InstigatorActor);

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnMove OnMove;

	UCameraComponent* GetCamera()
	{
		return ViewCamera;
	}

	void Seppuku();

protected:
	virtual void BeginPlay() override;
	//Components
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* MainContext;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* BossEntranceContext;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UMainActionComponent* ActionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UQueueComponent* QueueComp;

	//Actions
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MovementAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* DodgeRollAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LightAttackAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* HealAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AbilityAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AbilityAction2;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AbilityAction3;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LockOnAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* PauseAction;
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* StepAction;


	virtual void PostInitializeComponents() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter, bool CauseStagger) override;
	UFUNCTION(BlueprintCallable)
	void DirectionalHitReact(const FVector& ImpactPoint);
	UFUNCTION()
	void ReturnToMainMenu();
	UFUNCTION()
void ReturnToLevelBegin();


	UPROPERTY(EditDefaultsOnly, Category="Lag")
	float RegularCameraLagSpeed = 10.f;
	UPROPERTY(EditDefaultsOnly, Category="Lag")
	float SprintCameraLagSpeed = 4.0f;
	UPROPERTY(EditDefaultsOnly, Category = "FMOD")
	class UFMODEvent* HurtGrunt;
	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float MaxLockDistance = 2500;


private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	UPROPERTY(EditDefaultsOnly)
	float CameraMaxZoomOut = 100;
	UPROPERTY(EditDefaultsOnly)
	float CameraZoomOutSpeed = 2;
	UPROPERTY(EditDefaultsOnly)
	float CameraZoomInSpeed = 4;
	float InitialSocketOffset = 0;
	bool bIsMovementDisabled = false;
	//Camera collision
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float CameraProbeRadius=20;
	UPROPERTY()
	TArray<AActor*> CameraCollisionActorsToIgnore;
	FHitResult SphereHit;
	UPROPERTY()
	float DefaultCameraBoomLength;
	UPROPERTY()
	FVector CameraSphereImpactPoint;
	UPROPERTY()
	float CameraInterpSpeed=1;
	UFUNCTION()
	float CalcInterpSpeed();

	//Sharop turning cooldown
	UPROPERTY(EditDefaultsOnly, Category="Ability")
	float SharpTurnCD = 1;
	FTimerHandle SharpTurnHandle;
	bool CanSharpTurn = true;
	UFUNCTION()
	void CanSharpTurnTrigger();
	

	UFUNCTION()
	void CameraCollisionCalc(float Deltatime);

	UPROPERTY()
	UMainGameInstance* GI;
	UPROPERTY()
	AActor* CurrentLockTarget;

	
UPROPERTY(EditAnywhere,Category="Heal Charge")
	float ChargeDecreaseMultiplier=1;
	UPROPERTY()
	float AnimPauseDuration = 0;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float PitchDown = 30;
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float PitchUp = -50;
	bool DieOnce=true;

	//User Input
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void DodgeRoll(const FInputActionValue& Value);
	void SideStep(const FInputActionValue& Value);
	void LightAttack(const FInputActionValue& Value);
	void SpecialAbility(const FInputActionValue& Value);
	void SpecialAbility2(const FInputActionValue& Value);
	void SpecialAbility3(const FInputActionValue& Value);
	void SprintStart(const FInputActionValue& Value);
	void SprintStop(const FInputActionValue& Value);
	void HealSelf(const FInputActionValue& Value);
	void LockOn(const FInputActionValue& Value);
	void PauseGame(const FInputActionValue& Value);

	//Utility Functions
	void CameraSprintZoomOut(float DeltaTime);
	void SharpTurnCheck();
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UAttributesComponent* OwningComp, float NewHealth, float Delta);
	UFUNCTION()
	void OnStaminaChanged(UAttributesComponent* OwningComp, float NewHealth, float Delta);
	//HUD
	void InitializeMainOverlay();
	UPROPERTY()
	UMainOverlay* MainOverlay;

	
	UPROPERTY()
	UVictoryDeathOverlay* VictoryDeathOverlay;
	UPROPERTY()
	UCombatOverlay* CombatOverlay;

	void SetHUDHealth();
	void SetHUDStamina();
	//Other Functions
	void LockOnTarget();
	AActor* FindClosestLockTarget(TSubclassOf<ULockInterface> TargetInterface);
	void LockRotationToTarget(float Deltatime);


};
