// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/LockInterface.h"
#include "TargetDummy.generated.h"

class UAttributesComponent;
UCLASS()
class STYLIZEDTEST_API ATargetDummy : public AActor , public ILockInterface
{
	GENERATED_BODY()
	
public:	
	ATargetDummy();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Components")
	UAttributesComponent* AttributesComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* DummyMesh;

	virtual void GetLocked_Implementation(AActor* Locker, bool Release) override;
	
public:	

	virtual void Tick(float DeltaTime) override;

};
