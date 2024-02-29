// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacter.h"
#include "AI/EnemyAIController.h"
#include "Components/AttributesComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "Characters/MainCharacter.h"
#include "Components/BillboardComponent.h"
#include "Components/EnemyActionComponent.h"
#include "HUD/CombatOverlay.h"
#include "HUD/MainHUD.h"
#include "HUD/ProgressBarBase.h"
#include "Kismet/GameplayStatics.h"

AEnemyCharacter::AEnemyCharacter()
{
	AttributesComp = CreateDefaultSubobject<UAttributesComponent>("AttributesComp");

	// Ensures we receive a controller when spawned in the level by our gamemode
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	TargetActorKey = "TargetActor";
	MoveToLocationKey = "MoveToLocation";
	EnemyActionComponent = CreateDefaultSubobject<UEnemyActionComponent>("EnemyActionComp");
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	LockOnTargetLocation = CreateDefaultSubobject<UBillboardComponent>("LockOnTargetBillboard");
	LockOnTargetLocation->SetupAttachment(GetRootComponent());
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
	MoveRotationSpeed = GetCharacterMovement()->RotationRate.Yaw;
	bUseControllerRotationYaw = false;
	AIActive = false;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	AssignWeapon("Gun", "RightHandWeapon", 1);
}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void AEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnPawnSeen);
	AttributesComp->OnHealthChanged.AddDynamic(this, &AEnemyCharacter::OnHealthChanged);
	LockOnLocation = LockOnTargetLocation->GetComponentLocation();
}

void AEnemyCharacter::OnHealthChanged(AActor* InstigatorActor, UAttributesComponent* OwningComp, float NewHealth, float Delta)
{
	SetHUDHealth();

	// Died
	if (NewHealth <= 0.0f)
	{
		Cast<AMainCharacter>(GetTargetActor())->UnlockTarget();
	IsLockable=false;
		if (VictoryDeathOverlay)
		{
			VictoryDeathOverlay->PlayAnimation(VictoryDeathOverlay->FadeOut, 0, 1, EUMGSequencePlayMode::Reverse);
			VictoryDeathOverlay->AddToViewport();
		}

		BossFightOverlay->PlayAnimation(BossFightOverlay->FadeIn, 0, 1, EUMGSequencePlayMode::Reverse);
		FTimerDelegate Delegate;
		FTimerDelegate Delegate2;
		Delegate2.BindUFunction(this, "RemoveVictoryOverlay");
		Delegate.BindUFunction(this, "RemoveBossOverlay");
		GetWorld()->GetTimerManager().SetTimer(RemoveOverLayHandle, Delegate, 1, false, 3);
		GetWorld()->GetTimerManager().SetTimer(RemoveVictoryOverLayHandle, Delegate2, 1, false, 6);

		// stop BT
		AEnemyAIController* AIC = Cast<AEnemyAIController>(GetController());
		if (AIC)
		{
			AIC->GetBrainComponent()->StopLogic("Killed");
		}
		StopMontage(NULL);
		SetIsDead(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();
	}
}

void AEnemyCharacter::RemoveBossOverlay()
{
	BossFightOverlay->RemoveFromParent();
	VictoryDeathOverlay->PlayAnimation(VictoryDeathOverlay->FadeOut);
}

void AEnemyCharacter::RemoveVictoryOverlay()
{
	VictoryDeathOverlay->RemoveFromParent();
}

void AEnemyCharacter::OnPawnSeen(APawn* Pawn)
{
	// Ignore if target already set
	if (GetTargetActor() != Pawn)
	{
		SetTargetActor(Pawn);
	}
}

void AEnemyCharacter::SetHUDHealth()
{
	if (CombatOverlay && AttributesComp)
	{
		CombatOverlay->SetBossHealthBarPercent(AttributesComp->GetHealthPercent());
		//BossFightOverlay->SetHealthBarPercent(AttributesComp->GetHealthPercent());
	}
}


void AEnemyCharacter::InitializeBossOverlay()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		AMainHUD* MainHUD = Cast<AMainHUD>(PlayerController->GetHUD());
		if (MainHUD)
		{
			BossFightOverlay = MainHUD->GetBossFightOverlay();
			VictoryDeathOverlay = MainHUD->GetVictoryDeathOverlay();
			CombatOverlay = MainHUD->GetCombatOverlay();
			if (CombatOverlay && AttributesComp)
			{
				CombatOverlay->SetBossHealthBarPercent(AttributesComp->GetHealthPercent());
				CombatOverlay->SetBossName(FText::FromName(BossName));
				//BossFightOverlay->AddToViewport();
			}
		}
	}
}


void AEnemyCharacter::GetLocked_Implementation(AActor* Locker, bool Release)
{
	ILockInterface::GetLocked_Implementation(Locker, Release);
}

void AEnemyCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter, bool CauseStagger)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter, CauseStagger);
}

bool AEnemyCharacter::IsLockable_Implementation()
{
	return IsLockable;
}

void AEnemyCharacter::SetTargetActor(AActor* NewTarget)
{
	AEnemyAIController* AIC = Cast<AEnemyAIController>(GetController());
	if (AIC)
	{
		AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);
	}
}

AActor* AEnemyCharacter::GetTargetActor() const
{
	AEnemyAIController* AIC = Cast<AEnemyAIController>(GetController());
	if (AIC)
	{
		return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
	}

	return nullptr;
}

UEnemyActionComponent* AEnemyCharacter::GetActionComp()
{
	return EnemyActionComponent;
}
