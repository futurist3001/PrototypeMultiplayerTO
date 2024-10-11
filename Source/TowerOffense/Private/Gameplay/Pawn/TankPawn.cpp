#include "TowerOffense/Public/GamePlay/Pawn/TankPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Landscape.h"
#include "NiagaraFunctionLibrary.h"
#include "TowerOffense/Public/Gameplay/Other/Projectile.h"
#include "TowerOffense/Public/Gameplay/Other/TOCameraShake.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOPlayerController.h"

ATankPawn::ATankPawn(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));

	RightTankTrack = CreateDefaultSubobject<USceneComponent>(TEXT("Right Tank Track"));
	LeftTankTrack = CreateDefaultSubobject<USceneComponent>(TEXT("Left Tank Track"));
	RightTankTrackRotation = CreateDefaultSubobject<USceneComponent>(TEXT("Right Tank Track Rotation"));
	LeftTankTrackRotation = CreateDefaultSubobject<USceneComponent>(TEXT("Left Tank Track Rotation"));
	TankTop = CreateDefaultSubobject<USceneComponent>(TEXT("Top of the tank"));
	TankBottom = CreateDefaultSubobject<USceneComponent>(TEXT("Bottom of the tank"));

	SpringArmComponent->SetupAttachment(RootComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);

	RightTankTrack->SetupAttachment(BaseMesh);
	LeftTankTrack->SetupAttachment(BaseMesh);
	RightTankTrackRotation->SetupAttachment(BaseMesh);
	LeftTankTrackRotation->SetupAttachment(BaseMesh);

	TankTop->SetupAttachment(TurretMesh);
	TankBottom->SetupAttachment(BaseMesh);

	CurrentTime = 0.f;
	CurrentSpeed = 0.f;
	SpeedStopGas = 0.f;
	SpeedStopBraking = 0.f;
	YawTurnRotator = 0.f;
	MaxEnergy = 50.f;
	CurrentEnergy = MaxEnergy;
	OldShootTime = 10.f;
	MaxFireHorizontalAngle = 15.f;

	bIsStopMoving = false;
	bReverseAttempt = false;
	bPlayedTurretRotationSoundIteration = false;
	bIsOldShoot = false;
	bIsCollision = false;
	bIsUpsideDown = false;

	MovementEffect = nullptr;

	bReplicates = true;
	SetReplicateMovement(true);
}

void ATankPawn::MoveStarted()
{
	if (MovementSound)
	{
		MovementAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), MovementSound);
		MovementAudioComponent->Play();
	}
}

void ATankPawn::MoveTriggeredValue(const FInputActionValue& Value)
{
	MovementVector = Value.Get<FVector>();

	if (!bReverseAttempt) // this will release once
	{
		if (MovementVector == PreviousMovementVector)
		{
			SpeedStopBraking *= -1;
		}
		bReverseAttempt = true;
	}

	if (MovementEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), MovementEffect, RightTankTrack->GetComponentLocation());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), MovementEffect, LeftTankTrack->GetComponentLocation());
	}

	UpsideDownTank();
}

void ATankPawn::MoveTriggeredInstance(const FInputActionInstance& Instance)
{
	bIsStopMoving = false;

	if (!bIsCollision)
	{
		CurrentTime = Instance.GetElapsedTime();
		CurrentSpeed = FMath::Lerp(SpeedStopBraking, Speed, FMath::Clamp(CurrentTime / AccelerationDuration, 0.f, 1.f));
		AddActorLocalOffset(MovementVector * CurrentSpeed, true, nullptr);

		SpeedStopGas = CurrentSpeed;
	}
}

void ATankPawn::Server_SetActorLocation_Implementation(FVector ActorLocation)
{
	SetActorLocation(ActorLocation);
}

void ATankPawn::Turn(const FInputActionValue& Value)
{
	if (!HasAuthority())
	{//CLIENT
		YawTurnRotator = Value.Get<float>();
		AddActorLocalRotation(FRotator(0.f, YawTurnRotator, 0.f), true, nullptr);

		TurretMesh->AddLocalRotation(FRotator(0.f, -YawTurnRotator, 0.f), false, nullptr);
		TargetAngle.Yaw -= YawTurnRotator;

		if (MovementEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(), MovementEffect, RightTankTrackRotation->GetComponentLocation());
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(), MovementEffect, LeftTankTrackRotation->GetComponentLocation());
		}

		Server_SetActorRotation(YawTurnRotator);
	}

	else
	{//SERVER
		Multicast_SetActorRotation(YawTurnRotator);
	}
}

void ATankPawn::Server_SetActorRotation_Implementation(float ServerYawTurnRotator)
{
	Multicast_SetActorRotation(ServerYawTurnRotator);

	AddActorLocalRotation(FRotator(0.f, ServerYawTurnRotator, 0.f), true, nullptr);

	TurretMesh->AddLocalRotation(FRotator(0.f, -ServerYawTurnRotator, 0.f), false, nullptr);
	TargetAngle.Yaw -= ServerYawTurnRotator;

	if (MovementEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), MovementEffect, RightTankTrackRotation->GetComponentLocation());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), MovementEffect, LeftTankTrackRotation->GetComponentLocation());
	}
}

void ATankPawn::Multicast_SetActorRotation_Implementation(float MultiServerYawTurnRotator)
{
	AddActorLocalRotation(FRotator(0.f, MultiServerYawTurnRotator, 0.f), true, nullptr);

	TurretMesh->AddLocalRotation(FRotator(0.f, -MultiServerYawTurnRotator, 0.f), false, nullptr);
	TargetAngle.Yaw -= MultiServerYawTurnRotator;

	if (MovementEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), MovementEffect, RightTankTrackRotation->GetComponentLocation());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), MovementEffect, LeftTankTrackRotation->GetComponentLocation());
	}
}

void ATankPawn::MoveCompleted()
{
	bIsStopMoving = true;
	CurrentTime = 0;
	bReverseAttempt = false;

	PreviousMovementVector = MovementVector;

	if (MovementSound && MovementAudioComponent && MovementAudioComponent->IsValidLowLevel())
	{
		MovementAudioComponent->Stop();
		MovementAudioComponent->DestroyComponent();
	}
}

void ATankPawn::StopCollision()
{
	if (bIsStopMoving)
	{
		bIsCollision = false;
		GetWorldTimerManager().ClearTimer(CollisionTimerHandle);
	}
}

void ATankPawn::Fire()
{
	if (CurrentTimeFire >= FireInterval && CurrentEnergy > 0.f)
	{
		Start = ProjectileSpawnPoint->GetComponentLocation();
		End = Start + (FRotator(
			ProjectileSpawnPoint->GetForwardVector().Rotation().Pitch + PitchAimingRotator,
			ProjectileSpawnPoint->GetForwardVector().Rotation().Yaw,
			ProjectileSpawnPoint->GetForwardVector().Rotation().Roll)).GetNormalized().Vector() * 1000.f;

		Super::Fire();

		if (TOCameraShakeClass)
		{
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayWorldCameraShake(
				GetWorld(), TOCameraShakeClass, GetActorLocation(), 0.0f, 1000.0f, 1.f);
		}

		CurrentTimeFire = 0.0f;
		CurrentEnergy -= 10.f;

		bIsOldShoot = false;
	}
}

void ATankPawn::NotifyHit(
	UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(
		MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Other && !Other->IsA<AProjectile>() && !Other->IsA<ALandscape>())
	{
		MoveCompleted();

		bIsCollision = true;
		CurrentSpeed = 0.0f;
		SpeedStopBraking = 0.0f;
		SpeedStopGas = 0.0f;

		MovementVector = FVector::ZeroVector;

		GetWorld()->GetTimerManager().SetTimer(
			CollisionTimerHandle, this, &ATankPawn::StopCollision, 0.2f, true);

		GetWorldTimerManager().SetTimer(
			AdjustingTurretPositionTimerHandle, this, &ATankPawn::AdjustTurretPosition, 1.0f, true);
	}
}

void ATankPawn::RotateTurret(
	const FRotator& Current, const FRotator& Target, float DeltaTime, float InterpSpeed)
{
	Super::RotateTurret(Current, Target, DeltaTime, InterpSpeed);
}

void ATankPawn::Server_SetTurretRotation_Implementation(
	const FRotator& Current, const FRotator& Target, float DeltaTime, float InterpSpeed) // doesn`t work correctly
{
	RotateTurret(Current, Target, DeltaTime, InterpSpeed);
	Multicast_SetTurretRotation(Current, Target, DeltaTime, InterpSpeed);
}

void ATankPawn::Multicast_SetTurretRotation_Implementation(
	const FRotator& Current, const FRotator& Target, float DeltaTime, float InterpSpeed) // doesn`t work correctly
{
	if (!IsLocallyControlled())
	{
		RotateTurret(Current, Target, DeltaTime, InterpSpeed);
	}
}

void ATankPawn::Rotate(const FInputActionValue& Value)
{
	float RotateValue = Value.Get<float>();
	YawCameraRotator += RotateValue;
	TargetAngle.Yaw += RotateValue;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetControlRotation(FRotator(0.f, YawCameraRotator, 0.f));
}

void ATankPawn::RotateCompleted()
{
	GetWorldTimerManager().SetTimer(RotComplAdjustingTurretPositionTimerHandle, this,
		&ATankPawn::AdjustTurretPosition, 3.0f, true);
}

void ATankPawn::Aiming(const FInputActionValue& Value)
{
	float AimingValue = Value.Get<float>();
	PitchAimingRotator += AimingValue;

	if (PitchAimingRotator > MaxFireHorizontalAngle) // for limiting range of horizontal aiming
	{
		PitchAimingRotator = MaxFireHorizontalAngle;
	}
	else if (PitchAimingRotator < 0.f) // for limiting range of horizontal aiming
	{
		PitchAimingRotator = 0.f;
	}
}

void ATankPawn::AdjustTurretPosition()
{
	if (!bIsRotate)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

		if (PlayerController->GetControlRotation() != ProjectileSpawnPoint->GetForwardVector().Rotation())
		{
			PlayerController->SetControlRotation(ProjectileSpawnPoint->GetForwardVector().Rotation());
			YawCameraRotator = ProjectileSpawnPoint->GetForwardVector().Rotation().Yaw;
		}

		if (PlayerController->GetControlRotation() == ProjectileSpawnPoint->GetForwardVector().Rotation())
		{
			GetWorldTimerManager().SetTimer(
				ClearAdjustingTurretPositionTimerHandle, this,
				&ATankPawn::ClearAdjustingTurretPositionTimer, 0.001f, false);
		}
	}
}

void ATankPawn::ClearAdjustingTurretPositionTimer()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (!bIsRotate)
	{
		if (AdjustingTurretPositionTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(
				AdjustingTurretPositionTimerHandle);
		}

		if (ClearAdjustingTurretPositionTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(
				ClearAdjustingTurretPositionTimerHandle);
		}

		if (RotComplAdjustingTurretPositionTimerHandle.IsValid())
		{
			GetWorldTimerManager().ClearTimer(
				RotComplAdjustingTurretPositionTimerHandle);
		}
	}
}

void ATankPawn::UpsideDownTank()
{
	if (TankTop->GetComponentLocation().Z < TankBottom->GetComponentLocation().Z && !bIsUpsideDown)
	{
		ATOPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ATOPlayerController>();
		GetWorldTimerManager().SetTimer(
			ReloadLevelTimerHandle, PlayerController, &ATOPlayerController::Restart, 3.0f, false);

		bIsUpsideDown = true;
	}
}

void ATankPawn::BeginPlay()
{
	Super::BeginPlay();
	
	BaseMesh->SetWorldRotation(GetActorRotation() - FRotator(0.f, 90.f, 0.f));
	YawCameraRotator = GetActorRotation().Yaw;

	if (const auto* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(TankMappingContext, 0);
			}
		}
	}
}

void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsStopMoving)
	{
		CurrentTime += DeltaTime;
		CurrentSpeed = FMath::Lerp(SpeedStopGas, 0, FMath::Clamp(CurrentTime / AccelerationDuration, 0.f, 1.f));
		AddActorLocalOffset(MovementVector * CurrentSpeed, true, nullptr);

		SpeedStopBraking = -CurrentSpeed;
	}

	if (!bPlayedTurretRotationSoundIteration && bIsRotate)
	{
		// Play Sound

		if (TurretRotationSound && !HasAuthority())
		{
			TurretRotationAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), TurretRotationSound);
			TurretRotationAudioComponent->Play();

			bPlayedTurretRotationSoundIteration = true;
		}
	}

	else if (bPlayedTurretRotationSoundIteration && !bIsRotate)
	{
		// Stop and delete sound

		if (TurretRotationSound && TurretRotationAudioComponent)
		{
			TurretRotationAudioComponent->Stop();
			TurretRotationAudioComponent->DestroyComponent();

			bPlayedTurretRotationSoundIteration = false;
		}
	}

	GetWorld()->LineTraceSingleByChannel(
		ShootingPoint, ProjectileSpawnPoint->GetComponentLocation(), 
		ProjectileSpawnPoint->GetComponentLocation() + (FRotator(
		ProjectileSpawnPoint->GetForwardVector().Rotation().Pitch + PitchAimingRotator,
		ProjectileSpawnPoint->GetForwardVector().Rotation().Yaw,
		ProjectileSpawnPoint->GetForwardVector().Rotation().Roll)).GetNormalized().Vector() * 100000.f,
		ECollisionChannel::ECC_Camera);

	if (ShootingPoint.Location != FVector(0.f,0.f,0.f))
	{
		DrawDebugSphere(
			GetWorld(), FVector(ShootingPoint.Location.X, ShootingPoint.Location.Y, ShootingPoint.Location.Z),
			35, 15, FColor::Blue, false, 0.03f, 0, 0.5);
	}

	CurrentTimeFire += DeltaTime;

	if (CurrentTimeFire > OldShootTime)
	{
		bIsOldShoot = true;
	}

	if (CurrentEnergy < MaxEnergy)
	{
		RechargeTimeProjectile += DeltaTime;
	}

	if (RechargeTimeProjectile >= RechargeInterval && CurrentEnergy < MaxEnergy)
	{
		CurrentEnergy += 10.f;
		RechargeTimeProjectile = 0.0f;
	}

	Server_SetActorLocation(GetActorLocation());

	if (!HasAuthority())
	{
		Server_SetTurretRotation(
			TurretMesh->GetRelativeRotation(), TargetAngle, RotationCurrentTime, TurretRotationSpeed);

		RotateTurret(
			TurretMesh->GetRelativeRotation(), TargetAngle, RotationCurrentTime, TurretRotationSpeed);
	}

	else
	{
		Multicast_SetTurretRotation(
			TurretMesh->GetRelativeRotation(), TargetAngle, RotationCurrentTime, TurretRotationSpeed);
	}
}

void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Started, this, &ATankPawn::MoveStarted);
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ATankPawn::MoveTriggeredValue);
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ATankPawn::MoveTriggeredInstance);
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this, &ATankPawn::MoveCompleted);

		EnhancedInputComponent->BindAction(TurnRightAction, ETriggerEvent::Triggered, this, &ATankPawn::Turn);
		EnhancedInputComponent->BindAction(RotateTurretAction, ETriggerEvent::Triggered, this, &ATankPawn::Rotate);
		EnhancedInputComponent->BindAction(RotateTurretAction, ETriggerEvent::Completed, this, &ATankPawn::RotateCompleted);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Triggered, this, &ATankPawn::Aiming);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATankPawn::Fire);
	}
}