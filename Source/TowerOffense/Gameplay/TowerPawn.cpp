#include "TowerPawn.h"

#include "TankPawn.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectile.h"
#include "TOGameModeBase.h"

ATowerPawn::ATowerPawn(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("ShootingZone"));
	SphereComponent->SetupAttachment(RootComponent);

	PeriodFire = 2.0f;
}

void ATowerPawn::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

void ATowerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!OverlapedActor.IsEmpty())
	{
		RotateTurret();
	}

	if (!bPlayedTurretRotationSoundIteration && bIsRotate)
	{
		// Play Sound

		if (TurretRotationSound)
		{
			TurretRotationAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), TurretRotationSound);
			TurretRotationAudioComponent->Play();

			bPlayedTurretRotationSoundIteration = true;
		}
	}

	else if (bPlayedTurretRotationSoundIteration && !bIsRotate)
	{
		// Stop and delete sound

		if (TurretRotationSound && TurretRotationAudioComponent && TurretRotationAudioComponent->IsValidLowLevel())
		{
			TurretRotationAudioComponent->Stop();
			TurretRotationAudioComponent->DestroyComponent();

			bPlayedTurretRotationSoundIteration = false;
		}
	}
}

void ATowerPawn::RotateTurret()
{
	ATOGameModeBase* GameModeBase = GetWorld()->GetAuthGameMode<ATOGameModeBase>();

	if (!IsTheSameTeam(OverlapedActor[0]) && GameModeBase->GetGamePhase() == EGamePhase::Playing)
	{
		Super::RotateTurret();

		const FRotator NewRotator = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(), OverlapedActor[0]->GetActorLocation()) - GetActorRotation();

		TargetAngle = FRotator(0.f, NewRotator.Yaw - 90.f, 0.f);
	}
}

void ATowerPawn::Fire()
{
	ATOGameModeBase* GameModeBase = GetWorld()->GetAuthGameMode<ATOGameModeBase>();

	if (IsLookToTank() && !IsTheSameTeam(OverlapedActor[0]) && GameModeBase->GetGamePhase() == EGamePhase::Playing)
	{
		Start = ProjectileSpawnPoint->GetComponentLocation();
		End = OverlapedActor[0]->GetActorLocation();

		Super::Fire();
	}
}

bool ATowerPawn::IsLookToTank()
{
	FHitResult HitResult;

	FVector StartPointLook = ProjectileSpawnPoint->GetComponentLocation();
	FVector EndPointLook = StartPointLook + ProjectileSpawnPoint->GetForwardVector() * 5000.f;

	if (GetWorld()->LineTraceSingleByChannel(HitResult,
		FVector(StartPointLook.X, StartPointLook.Y, OverlapedActor[0]->GetActorLocation().Z),
		FVector(EndPointLook.X, EndPointLook.Y, OverlapedActor[0]->GetActorLocation().Z),
		ECollisionChannel::ECC_Visibility))
	{
		if (HitResult.GetActor()->IsA<ATankPawn>())
		{
			return true;
		}
	}

	return false;
}

bool ATowerPawn::IsTheSameTeam(AActor* Actor)
{
	if (Actor->IsA<ATurretPawn>())
	{
		const ATurretPawn* TurretPawn = Cast<ATurretPawn>(Actor);

		if (Execute_GetTeam(this) == Execute_GetTeam(TurretPawn))
		{
			return true;
		}
	}

	return false;
}

void ATowerPawn::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (!IsTheSameTeam(OtherActor))
	{
		if (OtherActor && OtherActor->IsA<ATankPawn>())
		{
			OverlapedActor.Add(OtherActor);

			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATowerPawn::Fire, PeriodFire, true);
		}
	}
}

void ATowerPawn::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!IsTheSameTeam(OtherActor))
	{
		if (OtherActor && OtherActor->IsA<ATankPawn>())
		{
			OverlapedActor.RemoveSingleSwap(OtherActor); // if the actor leave shooting zone, overlapped actor must be deleted from array

			if (OverlapedActor.IsEmpty())
			{
				GetWorldTimerManager().ClearTimer(FireTimerHandle);

				if (TurretRotationSound && TurretRotationAudioComponent && TurretRotationAudioComponent->IsValidLowLevel()) // if the turret did not have time to aim
				{
					TurretRotationAudioComponent->Stop();
					TurretRotationAudioComponent->DestroyComponent();
				}
			}
		}
	}
}
