// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainCharacter.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ActionComponent.h"
#include "Components/MainActionComponent.h"
#include "Components/AttributesComponent.h"
#include "Components/InputComponent.h"
#include "Components/QueueComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/DataTable.h"
#include "Components/Action.h"
#include "HUD/MainHUD.h"
#include "Math/Vector.h"
#include "HUD/MainOverlay.h"
#include "MainGameMode.h"
#include "Characters/EnemyCharacter.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameInstance.h"
#include "VectorTypes.h"
#include "Interfaces/LockInterface.h"
#include "HUD/CombatOverlay.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/BaseWeapon.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
	//UE_LOG(LogTemp, Warning, TEXT("You are looking Y %f"),X);
	//UE_LOG(LogTemp, Warning, TEXT("You are looking Y"));
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bSuspendForcedLookRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
	GetCharacterMovement()->RotationRate.Yaw = 600;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->TargetOffset.Z = 60.f;
	DefaultCameraBoomLength = CameraBoom->TargetArmLength;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	ActionComp = CreateDefaultSubobject<UMainActionComponent>("ActionComp");
	AttributesComp = CreateDefaultSubobject<UAttributesComponent>("AttributesComp");
	QueueComp = CreateDefaultSubobject<UQueueComponent>("QueueComp");
	InitialSocketOffset = CameraBoom->SocketOffset.X;


	bIsLockedOn = false;
	bIsIdle = false;
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CameraSprintZoomOut(DeltaTime);
	//Keeps camera fixed on Target when we are locked on.
	LockRotationToTarget(DeltaTime);
	//Character is constantly moving a little to keep the collision detection on movement component active at all time.
	FHitResult OutHit;
	GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, 0.01f), GetActorRotation(), true, OutHit);
	GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, -0.01f), GetActorRotation(), true, OutHit);

	//Camera collision system;
	//CameraCollisionCalc(DeltaTime);
}


void AMainCharacter::BeginPlay()
{
	InitializeMainOverlay();
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMin = PitchUp;
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = PitchDown;
	CameraCollisionActorsToIgnore.Add(this);
	Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = GetCurrentMaxSpeed();
	AttributesComp->OnHealthChanged.AddDynamic(this, &AMainCharacter::OnHealthChanged);
	AttributesComp->OnStaminaChanged.AddDynamic(this, &AMainCharacter::OnStaminaChanged);
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MainContext, 10);
			Subsystem->AddMappingContext(BossEntranceContext, 1);
		}
	}

	// Long timer idle

	IdleDelegate.BindUFunction(this, "PlayerIdle");
	GetWorld()->GetTimerManager().SetTimer(IdleTimerHandle, IdleDelegate, LongIdleDelay, true, LongIdleDelay);
}


void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AssignWeapon("Sword_R", "RightHandWeapon", 1);
	AssignWeapon("Sword_L", "LeftHandWeapon", 2);

	GI = Cast<UMainGameInstance>(GetGameInstance());
}

void AMainCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter, bool CauseStagger)
{
	IHitInterface::GetHit_Implementation(ImpactPoint, Hitter, CauseStagger);
	if (CauseStagger && !ActionComp->ActiveGamePlayTags.HasTag(FGameplayTag::RequestGameplayTag("Rolling")))
	{
		DirectionalHitReact(ImpactPoint);
	}
	//Play impact sound effect
	if (!ActionComp->ActiveGamePlayTags.HasTag(FGameplayTag::RequestGameplayTag("Rolling")))
	{
		UFMODBlueprintStatics::PlayEvent2D(this, HurtGrunt, true);
	}
}

void AMainCharacter::CameraCollisionCalc(float DeltaTime)
{
	const FVector Start = CameraBoom->GetComponentLocation();
	const FVector End = ViewCamera->GetComponentLocation() + CameraBoom->TargetOffset - CameraBoom->GetComponentLocation().Normalize() * DefaultCameraBoomLength + CameraBoom->GetComponentLocation();
	UKismetSystemLibrary::SphereTraceSingle(this, Start, End, CameraProbeRadius, TraceTypeQuery1, false, CameraCollisionActorsToIgnore, EDrawDebugTrace::None, SphereHit, true);
	if (SphereHit.bBlockingHit)
	{
		CameraSphereImpactPoint = SphereHit.ImpactPoint;


		CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, UE::Geometry::Distance(CameraBoom->GetComponentLocation(), CameraSphereImpactPoint) - CameraProbeRadius, DeltaTime,
		                                               CalcInterpSpeed());
	}
	else
	{
		if (!FMath::IsNearlyEqual(CameraBoom->TargetArmLength, DefaultCameraBoomLength, 1))
		{
			CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, DefaultCameraBoomLength, DeltaTime, CalcInterpSpeed());
		}
	}
}

float AMainCharacter::CalcInterpSpeed()
{
	float Dist = UE::Geometry::Distance(ViewCamera->GetComponentLocation() + CameraBoom->TargetOffset, CameraSphereImpactPoint);
	return UKismetMathLibrary::MapRangeClamped(Dist, 0, DefaultCameraBoomLength, CameraInterpSpeed, CameraInterpSpeed * 10);
}


void AMainCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	LatestImpactPoint = ImpactPoint;
	ActionComp->StartActionByName(this, "StaggerAction");
}

void AMainCharacter::ReturnToMainMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("Back to the main menu"));
	UGameplayStatics::OpenLevel(this, "MainMenu");
}

void AMainCharacter::ReturnToLevelBegin()
{
	UGameplayStatics::OpenLevel(this, "Iudex");
}

void AMainCharacter::InitializeMainOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		AMainHUD* MainHUD = Cast<AMainHUD>(PlayerController->GetHUD());
		if (MainHUD)
		{
			VictoryDeathOverlay = MainHUD->GetVictoryDeathOverlay();
			MainOverlay = MainHUD->GetMainOverlay();
			CombatOverlay = MainHUD->GetCombatOverlay();
			if (CombatOverlay && AttributesComp)
			{
				CombatOverlay->SetPlayerHealthBarPercent(AttributesComp->GetHealthPercent());
				CombatOverlay->SetStaminaBarPercent(1.f);
				CombatOverlay->SetHealthChargeAmount(AttributesComp->GetHeals());
				CombatOverlay->SetHealProgressBar(AttributesComp->GetHealChargeProgress());
				CombatOverlay->EnableAbility1(false);
				CombatOverlay->EnableAbility2(false);
			}
		}
	}
}


// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		EnhancedInputComponent->BindAction(DodgeRollAction, ETriggerEvent::Triggered, this, &AMainCharacter::DodgeRoll);
		EnhancedInputComponent->BindAction(HealAction, ETriggerEvent::Triggered, this, &AMainCharacter::HealSelf);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMainCharacter::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMainCharacter::SprintStop);
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Triggered, this, &AMainCharacter::LightAttack);
		EnhancedInputComponent->BindAction(AbilityAction, ETriggerEvent::Triggered, this, &AMainCharacter::SpecialAbility);
		EnhancedInputComponent->BindAction(AbilityAction2, ETriggerEvent::Triggered, this, &AMainCharacter::SpecialAbility2);
		EnhancedInputComponent->BindAction(AbilityAction3, ETriggerEvent::Triggered, this, &AMainCharacter::SpecialAbility3);
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &AMainCharacter::LockOn);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &AMainCharacter::PauseGame);
		EnhancedInputComponent->BindAction(StepAction, ETriggerEvent::Triggered, this, &AMainCharacter::SideStep);
	}
}

void AMainCharacter::PauseGame(const FInputActionValue& Value)
{
	AMainGameMode* GameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->TogglePause(Cast<APlayerController>(GetController()));
}


void AMainCharacter::Move(const FInputActionValue& Value)
{
	if (bIsMovementDisabled) return;

	bIsIdle = false;
	GetWorld()->GetTimerManager().SetTimer(IdleTimerHandle, IdleDelegate, LongIdleDelay, true, LongIdleDelay);
	OnMove.Broadcast(this);
	SharpTurnCheck();
	const FVector2d MovementVector = Value.Get<FVector2d>();
	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Forward, MovementVector.Y);

	const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Right, MovementVector.X);
}

void AMainCharacter::SharpTurnCheck()
{
	if ((GetDirectionalIntent() | GetActorForwardVector()) < -0.9 && GetCharacterMovement()->Velocity.Size() > TurningSpeed && !bIsLockedOn && CanSharpTurn)
	{
		CanSharpTurn = false;
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "CanSharpTurnTrigger");
		GetWorld()->GetTimerManager().SetTimer(SharpTurnHandle, Delegate, 1, false, SharpTurnCD);
		ActionComp->StartActionByName(this, "TurnAction");
	}
	else
	{
		b180Turn = false;
	}
}


void AMainCharacter::CanSharpTurnTrigger()
{
	CanSharpTurn = true;
}


void AMainCharacter::OnHealthChanged(AActor* InstigatorActor, UAttributesComponent* OwningComp, float NewHealth, float Delta)
{
	SetHUDHealth();
	//if we got hurt reduce the health charge progress
	if (Delta < 0)
	{
		const float PercentDamage = Delta / GetAttributesComp()->GetMaxHealth() * ChargeDecreaseMultiplier;
		GetAttributesComp()->ModifyHealChargeProgress(PercentDamage);
		if (!bIsRaging)
		{
			if (GetAttributesComp()->GetAbility1ChargeProgress() < 1)
			{
				GetAttributesComp()->ModifyAbility1ChargeProgress(PercentDamage);
			}
		}
		if (GetAttributesComp()->GetAbility2ChargeProgress() < 1)
		{
			GetAttributesComp()->ModifyAbility2ChargeProgress(PercentDamage);
		}
	}
	//we died
	if (NewHealth <= 0)
	{
		if (DieOnce)
		{
			DieOnce = false;
			CombatOverlay->SetHealProgressBar(AttributesComp->GetHealChargeProgress());
			VictoryDeathOverlay->SetText(FText::FromName("You Died"));
			VictoryDeathOverlay->PlayAnimation(VictoryDeathOverlay->FadeOut, 0, 1, EUMGSequencePlayMode::Reverse);
			VictoryDeathOverlay->AddToViewport();

			//Return to main menu after a few seconds
			FTimerDelegate Delegate2;
			FTimerHandle StartTimerHandle;
			Delegate2.BindUFunction(this, "ReturnToLevelBegin");
			GetWorld()->GetTimerManager().SetTimer(StartTimerHandle, Delegate2, 1, false, 5);
		}
	}
}

void AMainCharacter::Seppuku()
{
	AttributesComp->ApplyHealthChange(this,-10000,false,false,false,false);
}


void AMainCharacter::OnStaminaChanged(UAttributesComponent* OwningComp, float NewHealth, float Delta)
{
	SetHUDStamina();
}


void AMainCharacter::Look(const FInputActionValue& Value)
{
	const FVector2d LookAxisVector = Value.Get<FVector2d>();
	AddControllerPitchInput(LookAxisVector.Y * GI->GetMouseSensitivityVertical() * GI->GetMouseSensitivity() * 2);
	AddControllerYawInput(LookAxisVector.X * GI->GetMouseSensitivityHorizontal() * GI->GetMouseSensitivity() * 2);
}

void AMainCharacter::DodgeRoll(const FInputActionValue& Value)
{
	ActionComp->StartActionByName(this, "DodgeRollAction");
}

void AMainCharacter::SideStep(const FInputActionValue& Value)
{
	ActionComp->StartActionByName(this, "SideStepAction");
}

void AMainCharacter::SprintStart(const FInputActionValue& Value)
{
	ActionComp->StartActionByName(this, "SprintAction");
}


void AMainCharacter::SprintStop(const FInputActionValue& Value)
{
	ActionComp->StopActionByName(this, "SprintAction");
}

void AMainCharacter::HealSelf(const FInputActionValue& Value)
{
	ActionComp->StartActionByName(this, "HealAction");
}


void AMainCharacter::LightAttack(const FInputActionValue& Value)
{
	ActionComp->StartActionByName(this, "LightAttackAction");
}

void AMainCharacter::SpecialAbility(const FInputActionValue& Value)
{
	if (EquippedWeapon2 && EquippedWeapon2->GetWeaponName() == "Sword_L")
	{
		ActionComp->StartActionByName(this, "SwordAbilityAction");
	}
}

void AMainCharacter::SpecialAbility2(const FInputActionValue& Value)
{
	//TODO DARK make this call a function that decides which ability to actually use instead
	ActionComp->StartActionByName(this, "SwordSweepAction");
}

void AMainCharacter::SpecialAbility3(const FInputActionValue& Value)
{
	//TODO DARK make this call a function that decides which ability to actually use instead
	ActionComp->StartActionByName(this, "RageAction");
}


FVector AMainCharacter::GetDirectionalIntent()
{
	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	//TODO DARK CONTROLLER SUPPORT 
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			TArray<FKey> KeyArray = Subsystem->QueryKeysMappedToAction(MovementAction);
			const bool Up = PlayerController->IsInputKeyDown(KeyArray[0]);
			const bool Down = PlayerController->IsInputKeyDown(KeyArray[1]);
			const bool Left = PlayerController->IsInputKeyDown(KeyArray[2]);
			const bool Right = PlayerController->IsInputKeyDown(KeyArray[3]);
			const bool UpRight = Up && Right;
			const bool UpLeft = Up && Left;
			const bool DownRight = Down && Right;
			const bool DownLeft = Down && Left;

			if (UpRight)return ForwardDirection + RightDirection;
			if (UpLeft)return ForwardDirection - RightDirection;
			if (DownRight)return -ForwardDirection + RightDirection;
			if (DownLeft) return -ForwardDirection - RightDirection;
			if (Right) return RightDirection;
			if (Left) return -RightDirection;
			if (Down) return -ForwardDirection;
			if (Up) return ForwardDirection;
		}
	}
	// can return forwardDirection if u want the intended direction to be the controller forward instead of the mesh forward
	return GetActorForwardVector();
}

void AMainCharacter::LockOn(const FInputActionValue& Value)
{
	if (bIsLockedOn)
	{
		UnlockTarget();
	}
	else
	{
		LockOnTarget();
	}
}

void AMainCharacter::LockOnTarget()
{
	//make sure we are not sprinting. it is dangerous to sprint while locked on.
	if (GetCharacterMovement() && GetCharacterMovement()->Velocity.Size() > RunSpeed + 20) return;
	if (AActor* ClosestTarget = FindClosestLockTarget(ULockInterface::StaticClass()))
	{
		ILockInterface* LockInterface = Cast<ILockInterface>(ClosestTarget);
		if (!LockInterface->Execute_IsLockable(ClosestTarget)) return;
		if (CombatOverlay) CombatOverlay->SetLockOnVisibility(true);
		CombatOverlay->PlayAnimation(CombatOverlay->LockOnFadeIn, 0, 1, EUMGSequencePlayMode::Forward);
		bIsLockedOn = true;
		CurrentLockTarget = ClosestTarget;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetController()->SetIgnoreLookInput(true);
		SetControllerDesiredRotation(true);
		if (LockInterface)LockInterface->Execute_GetLocked(CurrentLockTarget, this, false);
	}
}

void AMainCharacter::UnlockTarget()
{
	if (CombatOverlay)CombatOverlay->SetLockOnVisibility(false);
	CombatOverlay->PlayAnimation(CombatOverlay->LockOnFadeIn, 0, 1, EUMGSequencePlayMode::Reverse);
	GetController()->ResetIgnoreLookInput();
	bIsLockedOn = false;
	SetControllerDesiredRotation(false);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	ILockInterface* LockInterface = Cast<ILockInterface>(CurrentLockTarget);
	if (LockInterface)LockInterface->Execute_GetLocked(CurrentLockTarget, this, true);
	CurrentLockTarget = nullptr;
}


void AMainCharacter::SetControllerDesiredRotation(bool Desire)
{
	if (GetActionComp()->GetRunningAction() && GetActionComp()->GetRunningAction()->bActionCanLockDirection)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}
	else { GetCharacterMovement()->bUseControllerDesiredRotation = Desire; }
}

void AMainCharacter::UpdateMainOverlayHealText(bool Healed)
{
	if (Healed)
	{
		CombatOverlay->SetHealthChargeAmount(AttributesComp->GetHeals());
		if (AttributesComp->GetHeals() < 1) CombatOverlay->EnableHealAbility(false);
		if (AttributesComp->GetHeals() > 0) CombatOverlay->EnableHealAbility(true);
	}
	else
	{
		CombatOverlay->PlayAnimation(CombatOverlay->HealBlink, 0, 1);
	}
}

void AMainCharacter::UpdateMainOverlayHealCharge(float Percent)
{
	CombatOverlay->SetHealProgressBar(Percent);
}

void AMainCharacter::UpdateMainOverlayAbility1Charge(float Percent, bool success)
{
	if (success)
	{
		CombatOverlay->SetAbility1ProgressBar(Percent);
		if (AttributesComp->GetAbility1ChargeProgress() == 1) CombatOverlay->EnableAbility1(true);
		if (!bIsRaging)if (AttributesComp->GetAbility1ChargeProgress() != 1) CombatOverlay->EnableAbility1(false);
	}
	else
	{
		CombatOverlay->PlayAnimation(CombatOverlay->Ability1Blink, 0, 1);
	}
}

void AMainCharacter::UpdateMainOverlayAbility2Charge(float Percent, bool success)
{
	if (success)
	{
		CombatOverlay->SetAbility2ProgressBar(Percent);
		if (AttributesComp->GetAbility2ChargeProgress() == 1) CombatOverlay->EnableAbility2(true);
		if (AttributesComp->GetAbility2ChargeProgress() != 1) CombatOverlay->EnableAbility2(false);
	}
	else
	{
		CombatOverlay->PlayAnimation(CombatOverlay->Ability2Blink, 0, 1);
	}
}


AActor* AMainCharacter::FindClosestLockTarget(TSubclassOf<ULockInterface> TargetInterface)
{
	//TODO Dark Make this more efficient. maybe shoot a cone ray and find target based on that.
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), TargetInterface, FoundActors);

	AActor* ClosestTarget = nullptr;
	float MinDistance = MAX_FLT;

	for (AActor* Actor : FoundActors)
	{
		float Distance = (Actor->GetActorLocation() - GetActorLocation()).Size();
		if (Distance < MinDistance && Distance < MaxLockDistance)
		{
			MinDistance = Distance;
			ClosestTarget = Actor;
		}
	}

	return ClosestTarget;
}

void AMainCharacter::LockRotationToTarget(float Deltatime)
{
	if (bIsLockedOn && !bSuspendForcedLookRotation)
	{
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(CurrentLockTarget);
		if (Enemy && Enemy->LockOnTargetLocation)
		{
			FVector Location = Enemy->LockOnTargetLocation->GetComponentLocation();
			//make sure we dont look directly up in the center of the  boss. will not work if there is an elevation diff. will need a more complex formula IG.
			GetController()->SetControlRotation((Location - GetActorLocation()).Rotation());
		}
	}
}

void AMainCharacter::PlayerIdle(AMainCharacter* MainCharacter, FVector Direction)
{
	bIsIdle = true;
}


//HELPER FUNCTIONS

void AMainCharacter::ResolveSpeed()
{
	{
		if (bIsRaging)
		{
			Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = GetCurrentMaxSpeed() * RageSpeedMultiplier;
		}
		else
		{
			Cast<UCharacterMovementComponent>(GetMovementComponent())->MaxWalkSpeed = GetCurrentMaxSpeed();
		}
	}
}


void AMainCharacter::Jump()
{
	Super::Jump();
}

void AMainCharacter::CameraSprintZoomOut(float DeltaTime)
{
	CameraBoom->CameraLagSpeed = RegularCameraLagSpeed;
}


UActionComponent* AMainCharacter::GetActionComp() const
{
	return ActionComp;
}


UQueueComponent* AMainCharacter::GetQueueComp() const
{
	return QueueComp;
}

UCombatOverlay* AMainCharacter::GetCombatOverlay() const
{
	return CombatOverlay;
}


void AMainCharacter::DisableMovement()
{
	bIsMovementDisabled = true;
}

void AMainCharacter::EnableMovement()
{
	bIsMovementDisabled = false;
}

void AMainCharacter::SetHUDHealth()
{
	if (CombatOverlay && AttributesComp)
	{
		CombatOverlay->SetPlayerHealthBarPercent(AttributesComp->GetHealthPercent());
	}
}

void AMainCharacter::SetHUDStamina()
{
	if (CombatOverlay && AttributesComp)
	{
		CombatOverlay->SetStaminaBarPercent(AttributesComp->GetStaminaPercent());
	}
}

void AMainCharacter::SetAnimPauseDuration(float PauseDuration)
{
	AnimPauseDuration = PauseDuration;
}

float AMainCharacter::GetAnimPauseDuration()
{
	return AnimPauseDuration;
}
