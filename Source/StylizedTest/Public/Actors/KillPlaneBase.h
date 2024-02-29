// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillPlaneBase.generated.h"

UCLASS()
class STYLIZEDTEST_API AKillPlaneBase : public AActor
{
	GENERATED_BODY()

public:
	AKillPlaneBase();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void CameraRise();
	UFUNCTION()
	void CameraRiseStop();

	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* AreaBox;

	FTimerDelegate Delegate;
	FTimerDelegate Delegate2;
	FTimerHandle CameraTimerHandle;
	FTimerHandle CameraClearTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	float CameraRiseSpeed=1;
};
