// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KillPlaneBase.h"

#include "Camera/CameraComponent.h"
#include "Characters/MainCharacter.h"
#include "Components/BoxComponent.h"
// Sets default values
AKillPlaneBase::AKillPlaneBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Area Box"));
	RootComponent = AreaBox;
	AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called when the game starts or when spawned
void AKillPlaneBase::BeginPlay()
{
	Super::BeginPlay();
}

void AKillPlaneBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AreaBox->OnComponentBeginOverlap.AddDynamic(this, &AKillPlaneBase::OnBoxOverlap);
}

void AKillPlaneBase::CameraRise()
{
	if (Camera)
	{
		Camera->AddWorldOffset(FVector(0,0,CameraRiseSpeed));
	
	}
}

void AKillPlaneBase::CameraRiseStop()
{
	GetWorld()->GetTimerManager().ClearTimer(CameraTimerHandle);
}

void AKillPlaneBase::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FHitResult HitResult = SweepResult;
	AMainCharacter* MC = Cast<AMainCharacter>(OtherActor);
	if(!MC) return;
	Camera = MC->GetCamera();
	//Camera->RemoveFromRoot();
	Camera->DetachFromComponent((FDetachmentTransformRules::KeepWorldTransform));
	FVector NewVector= FVector(OtherActor->GetActorLocation().X,OtherActor->GetActorLocation().Y,Camera->GetComponentLocation().Z);
	FRotator NewRotation = FRotator(-90.0f, 0.0f, 0.0f);
	Camera->SetWorldRotation(NewRotation);
	Camera->SetWorldLocation(NewVector);
	MC->Seppuku();
	Delegate.BindUFunction(this, "CameraRise");
	GetWorld()->GetTimerManager().SetTimer(CameraTimerHandle, Delegate, 0.01, true, -1);
	//	Delegate2.BindUFunction(this, "CameraRiseStop");
	//GetWorld()->GetTimerManager().SetTimer(CameraClearTimerHandle, Delegate2, 6, true, -1);
}


// Called every frame
void AKillPlaneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
