// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TargetDummy.h"
#include "Components/AttributesComponent.h"

// Sets default values
ATargetDummy::ATargetDummy()
{

	PrimaryActorTick.bCanEverTick = false;
	DummyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	DummyMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	AttributesComp= CreateDefaultSubobject<UAttributesComponent>("AttributesComp");

}

void ATargetDummy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATargetDummy::GetLocked_Implementation(AActor* Locker, bool Release)
{
	ILockInterface::GetLocked_Implementation(Locker, Release);
}

void ATargetDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

