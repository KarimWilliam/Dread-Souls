// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AOEActorBase.generated.h"


UCLASS()
class STYLIZEDTEST_API AAOEActorBase : public AActor
{
	GENERATED_BODY()

public:
	AAOEActorBase();
	virtual void Tick(float DeltaTime) override;
	TArray<AActor*> IgnoreActors;

protected:
	virtual void BeginPlay() override;
	void ExecuteGetHit(FHitResult& BoxHit);
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void PostInitializeComponents() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	float Damage = 5.f;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UBoxComponent* AreaBox;
};
