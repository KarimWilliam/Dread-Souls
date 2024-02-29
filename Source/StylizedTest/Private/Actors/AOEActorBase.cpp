// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AOEActorBase.h"
#include "Interfaces/HitInterface.h"
#include "GameplayFunctionLibrary.h"
#include "Components/BoxComponent.h"

// Sets default values
AAOEActorBase::AAOEActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Area Box"));
	RootComponent = AreaBox;
	AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called when the game starts or when spawned
void AAOEActorBase::BeginPlay()
{
	Super::BeginPlay();
}

 void AAOEActorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AreaBox->OnComponentBeginOverlap.AddDynamic(this, &AAOEActorBase::OnBoxOverlap);
}



void AAOEActorBase::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FHitResult HitResult = SweepResult;
	//only hurt an actor once
	if(!IgnoreActors.Contains(OtherActor))
	{
		UGameplayFunctionLibrary::ApplyDamage(GetOwner(), OtherActor, Damage);
		IgnoreActors.Add(OtherActor);
	}
	ExecuteGetHit(HitResult); //this will be used for special effects.
}

void AAOEActorBase::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());

	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner(), true);
	}
}


// Called every frame
void AAOEActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
