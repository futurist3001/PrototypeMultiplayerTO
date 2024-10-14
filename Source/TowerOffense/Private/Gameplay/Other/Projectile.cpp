#include "TowerOffense/Public/Gameplay/Other/Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TowerOffense/Public/Generic/TeamMemberInterface.h"
#include "TowerOffense/Public/Gameplay/Pawn/TurretPawn.h"

AProjectile::AProjectile(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMovementComponent =
		CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));

	SetRootComponent(SphereComponent);
	ProjectileMesh->SetupAttachment(RootComponent);

	ProjectileMesh->SetRelativeLocation(FVector(-30.f, 0.f, 0.f));
	ProjectileMesh->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));

	ProjectileMovementComponent->InitialSpeed = 4000.f;
	ProjectileMovementComponent->MaxSpeed = 4000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->ProjectileGravityScale = 0.1f;

	Damage = 10.0f;
	DamageEffect = nullptr;
	ExplosionSound = nullptr;
}

void AProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::OnHit(
	UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, FVector, const FHitResult& HitResult)
{
	if (OtherActor && (OtherActor != GetOwner()) && (OtherActor != GetInstigator()))
	{                                                                                             
		if (GetInstigator() && GetInstigator()->IsA<ATurretPawn>() && OtherActor->IsA<ATurretPawn>())
		{
			const ATurretPawn* TransmitterTurretPawn = Cast<ATurretPawn>(GetInstigator());
			const ATurretPawn* ReceiverTurretPawn = Cast<ATurretPawn>(OtherActor);

			if (TransmitterTurretPawn->Execute_GetTeam(TransmitterTurretPawn) != ReceiverTurretPawn->Execute_GetTeam(ReceiverTurretPawn))
			{
				OtherActor->TakeDamage(Damage, {}, nullptr, this);

				if (ReceiverTurretPawn->HealthComponent->Health <= 0)
				{
					TransmitterTurretPawn->ShakeCameraAfterKilling();
				}
			}
		}

		if (DamageEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DamageEffect, HitResult.Location);
		}

		if (ExplosionSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ExplosionSound);
		}

		this->Destroy();
	}
}
