// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AOEActorSphere.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "GameplayFunctionLibrary.h"

// Sets default values
AAOEActorSphere::AAOEActorSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Box"));
	RootComponent = AreaSphere;
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

// Called when the game starts or when spawned
void AAOEActorSphere::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAOEActorSphere::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());

	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner(), true);
	}
}

void AAOEActorSphere::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult)
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

void AAOEActorSphere::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AAOEActorSphere::OnSphereOverlap);
}

// Called every frame
void AAOEActorSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

