// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BaseWeapon.h"
#include "GameplayFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "FMODBlueprintStatics.h"
#include "Characters/EnemyCharacter.h"
#include "Characters/MainCharacter.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/AttributesComponent.h"

ABaseWeapon::ABaseWeapon()
{
	//PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());

	//Box start & End
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
	WeaponSkillEffectComponent=CreateDefaultSubobject<UNiagaraComponent>(TEXT("Weapon Skill Effect Component"));
	WeaponSkillEffectComponent->SetupAttachment(GetRootComponent());
	WeaponSkillEffectComponent->SetRelativeLocation(FVector(-8,11,-4));
	WeaponSkillEffectComponent->SetRelativeRotation(SkillVFXRotation);
//	UE_LOG(LogTemp, Warning, TEXT("Skill vfx rotation: %f"),SkillVFXRotation.Roll);


	MC = Cast<AMainCharacter>(GetOwner());
}


void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
}


void ABaseWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();


	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnBoxOverlap);
	//Sets my trace to be the same size as me hitbox
	BoxTraceExtent = WeaponBox->GetScaledBoxExtent();
	//only activate collision when the attack is ongoing to avoid damaging things without explicitly attacking.
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


bool ABaseWeapon::ActorIsSameType(AActor* OtherActor)
{
	return false;
}

void ABaseWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}


void ABaseWeapon::BoxTrace(TArray<FHitResult>& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	ActorsToIgnore.Add(this);


	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}
	UKismetSystemLibrary::BoxTraceMulti(
		this,
		Start,
		End,
		WeaponBox->GetScaledBoxExtent(),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true
	);
	for (FHitResult Hit : BoxHit)
	{
		IgnoreActors.AddUnique(Hit.GetActor());
	}
}

void ABaseWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TArray<FHitResult> BoxHit;
	TArray<AActor*> ActorArray;
	int count = 0;
	int ArraySize = 0;
	BoxTrace(BoxHit);
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetOwner());
	AMainCharacter* Mc = Cast<AMainCharacter>(GetOwner());
	bool dam = false;
	for (FHitResult Hit : BoxHit)
	{
		ActorArray.AddUnique(Hit.GetActor());
		ArraySize = ActorArray.Num();
		if (ArraySize > count)
		{
			count = ArraySize;
			FString classname = Hit.GetComponent()->GetClass()->GetName();
			//UE_LOG(LogTemp, Warning, TEXT("You Hit: %s"), *classname);

			if (Hit.GetActor()->IsA(AMainCharacter::StaticClass()))
			{
				dam = UGameplayFunctionLibrary::ApplyDamage(GetOwner(), Hit.GetActor(), Damage * EnemyCharacter->GetLatestDamageMulti());
			}

			if (Hit.GetActor()->IsA(AEnemyCharacter::StaticClass()))
			{
				EnemyCharacter = Cast<AEnemyCharacter>(Hit.GetActor());
				dam = UGameplayFunctionLibrary::ApplyDamage(GetOwner(), Hit.GetActor(), Damage * Mc->GetLatestDamageMulti());
				//UE_LOG(LogTemp, Warning, TEXT("Enemy takes damage: %f"), Damage * Mc->GetLatestDamageMulti());
				//Pause animation for a second when we hit an enemy
				if (Mc->GetAnimPauseDuration()) Mc->GetMesh()->bPauseAnims = true;
				if (BossAnimPause && Mc->GetAnimPauseDuration()) EnemyCharacter->GetMesh()->bPauseAnims = true;
				FTimerDelegate Delegate;
				Delegate.BindUFunction(this, "UnpauseAnimation", Mc, EnemyCharacter);
				GetWorld()->GetTimerManager().SetTimer(RollTimerHandle, Delegate, Mc->GetAnimPauseDuration(), false, Mc->GetAnimPauseDuration());
				// Blood effects
				if (BloodSplatterEffect)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodSplatterEffect, Hit.ImpactPoint, ItemMesh->GetForwardVector().Rotation());

					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SparkEffect, Hit.ImpactPoint, ItemMesh->GetForwardVector().Rotation());
				}

				//Charge Health Progress
				if (dam && Mc && EnemyCharacter)
				{
					Mc->GetAttributesComp()->ModifyHealChargeProgress(Damage * EnemyCharacter->PlayerHealChargeMultiplier / EnemyCharacter->GetAttributesComp()->GetMaxHealth());
					if (!Mc->bIsRaging)
					{
						Mc->GetAttributesComp()->ModifyAbility1ChargeProgress(Damage * EnemyCharacter->PlayerAbility1ChargeMultiplier / EnemyCharacter->GetAttributesComp()->GetMaxHealth());
					}
					Mc->GetAttributesComp()->ModifyAbility2ChargeProgress(Damage * EnemyCharacter->PlayerAbility2ChargeMultiplier / EnemyCharacter->GetAttributesComp()->GetMaxHealth());
				}
			}
			//Play impact sound effect

			if (dam)
			{
				UFMODBlueprintStatics::PlayEventAtLocation(this, ImpactSFX, this->GetTransform(), true);
				ExecuteGetHit(Hit); //this will be used for special effects.
			}
		}
	}
	ActorArray.Empty();
	count = 0;
	ArraySize = 0;
}


void ABaseWeapon::UnpauseAnimation(AMainCharacter* MainCharacter, AEnemyCharacter* EnemyCharacter)
{
	MainCharacter->GetMesh()->bPauseAnims = false;
	EnemyCharacter->GetMesh()->bPauseAnims = false;
}

void ABaseWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());

	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner(), true);
	}
}

void ABaseWeapon::SetWeaponBoxExtentY(float Extent)
{
	FVector NewExtent = FVector(WeaponBox->GetScaledBoxExtent().X, Extent, WeaponBox->GetScaledBoxExtent().Z);
	WeaponBox->SetBoxExtent(NewExtent);
}

void ABaseWeapon::SetBoxLocationY(float Loc)
{
	WeaponBox->SetRelativeLocation(FVector(WeaponBox->GetRelativeLocation().X, Loc, WeaponBox->GetRelativeLocation().Z));
}

void ABaseWeapon::SetBoxTraceEndY(float Loc)
{
	BoxTraceEnd->SetRelativeLocation(FVector(BoxTraceEnd->GetRelativeLocation().X, Loc, BoxTraceEnd->GetRelativeLocation().Z));
}
