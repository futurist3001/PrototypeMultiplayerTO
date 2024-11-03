#include "TowerOffense/Public/Gameplay/Pawn/TurretPawn.h"

#include "Components/CapsuleComponent.h"
#include "Components/ProgressBar.h"
#include "Components/WidgetComponent.h"
#include "TowerOffense/Public/Gameplay/UI/HealthTurretWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TowerOffense/Public/Gameplay/Other/Projectile.h"
#include "TowerOffense/Public/Gameplay/Other/TOCameraShake.h"
#include "TowerOffense/Public/Generic/MyBlueprintFunctionLibrary.h"

ATurretPawn::ATurretPawn(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Component"));
	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Widget Component"));
	HealthComponent = CreateDefaultSubobject<UTOHealthComponent>(TEXT("HealthComponent"));

	SetRootComponent(CapsuleComponent);

	HealthWidgetComponent->SetupAttachment(RootComponent);
	BaseMesh->SetupAttachment(RootComponent);
	TurretMesh->SetupAttachment(BaseMesh);
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);

	bIsRotate = false;
	TurretRotationSpeed = 1.f;
	DeathEfect = nullptr;
	FireEfect = nullptr;

	bReplicates = true;
}

TArray<FName> ATurretPawn::GetBaseMeshMaterialSlotOptions() const
{
	return GetMaterialSlotOptions(BaseMesh);
}

TArray<FName> ATurretPawn::GetTurretMeshMaterialSlotOptions() const
{
	return GetMaterialSlotOptions(TurretMesh);
}

TArray<FName> ATurretPawn::GetMaterialSlotOptions(const UStaticMeshComponent* InputComponentSlot)
{
	TArray<FName> OptionSlotNames;

	if (const UStaticMesh* StaticMeshComponent = InputComponentSlot->GetStaticMesh())
	{
		for (const FStaticMaterial& Material : StaticMeshComponent->GetStaticMaterials())
		{
			OptionSlotNames.Add(Material.MaterialSlotName);
		}
	}

	return OptionSlotNames;
}

TArray<FName> ATurretPawn::GetBaseMeshMaterialParameterOptions() const
{
	return GetMaterialParameterOptions(BaseMesh, BaseMeshMaterialSlotName);
}

TArray<FName> ATurretPawn::GetTurretMeshMaterialParameterOptions() const
{
	return GetMaterialParameterOptions(TurretMesh, TurretMeshMaterialSlotName);
}

TArray<FName> ATurretPawn::GetMaterialParameterOptions(
	const UStaticMeshComponent* InputComponentParameter, FName MeshMaterialSlotName)
{
	TArray<FName> OptionParameterNames;

	if (const UStaticMesh* StaticMeshComponent = InputComponentParameter->GetStaticMesh())
	{
		int32 MaterialIndex = StaticMeshComponent->GetMaterialIndex(MeshMaterialSlotName);
		MaterialIndex = FMath::Max(MaterialIndex, 0);

		const UMaterialInterface* Material = StaticMeshComponent->GetMaterial(MaterialIndex);
		TArray<FMaterialParameterInfo> MaterialParameterInfo;
		TArray<FGuid> Guid;

		Material->GetAllVectorParameterInfo(MaterialParameterInfo, Guid);

		for (const FMaterialParameterInfo& Parameter : MaterialParameterInfo)
		{
			OptionParameterNames.Add(Parameter.Name);
		}
	}

	return OptionParameterNames;
}

void ATurretPawn::SetMeshMaterial(
	UStaticMeshComponent* MeshComponent, FName MeshMaterialSlotName, FName MaterialParameterName,
	const FLinearColor& Color, UMaterialInstanceDynamic*& DynamicMaterialInstance)
{
	if (IsValid(MeshComponent))
	{
		if (DynamicMaterialInstance)
		{// If the dynamic material already exists, simply update the color parameter
			DynamicMaterialInstance->SetVectorParameterValue(MaterialParameterName, Color);
		}

		else
		{// Get the base material to create a new dynamic material
			const int32 MaterialIndex = MeshComponent->GetMaterialIndex(MeshMaterialSlotName);
			UMaterialInterface* Material = MeshComponent->GetMaterial(MaterialIndex);

			if (Material && !Material->IsA<UMaterialInstanceDynamic>())
			{
				DynamicMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
				DynamicMaterialInstance->SetVectorParameterValue(MaterialParameterName, Color);
				MeshComponent->SetMaterial(MaterialIndex, DynamicMaterialInstance);
			}
		}
	}
}

void ATurretPawn::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->HealthChanged.AddDynamic(this, &ATurretPawn::HealthCheckedDeath);

	if (UHealthTurretWidget* HealthBarWidget = Cast<UHealthTurretWidget>(HealthWidgetComponent->GetWidget()))
	{
		HealthBarWidget->SetHealthBar(
			HealthComponent->Health, HealthComponent->DefaultHealth);
	}
}

void ATurretPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotationCurrentTime = DeltaTime;

	if (HealthWidgetComponent && !HasAuthority())
	{
		HealthWidgetComponent->SetWorldRotation(
			UKismetMathLibrary::FindLookAtRotation(
				HealthWidgetComponent->GetComponentLocation(),
				GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation()));
	}
}

void ATurretPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetMeshMaterial(
		BaseMesh, BaseMeshMaterialSlotName, BaseMaterialParameterName,
		UMyBlueprintFunctionLibrary::GetTeamColor(Team), BaseDynamicMaterialInstance);

	SetMeshMaterial(
		TurretMesh, TurretMeshMaterialSlotName, TurretMaterialParameterName,
		UMyBlueprintFunctionLibrary::GetTeamColor(Team), TurretDynamicMaterialInstance);
}

void ATurretPawn::RotateTurret(const FRotator& Current, const FRotator& Target, float DeltaTime, float InterpSpeed)
{
	TurretMesh->SetWorldRotation(FMath::RInterpTo(Current, Target, DeltaTime, InterpSpeed));

	float LimitTargetAngleYaw = FMath::Fmod(Target.Yaw, 360.f);

	if (LimitTargetAngleYaw > 180.f) // For limit: from -180.f to 180.f 
	{
		LimitTargetAngleYaw -= 360.f;
	}

	else if (LimitTargetAngleYaw < -180.f) // For limit: from -180.f to 180.f
	{
		LimitTargetAngleYaw += 360.f;
	}

	if (FMath::Abs(TurretMesh->GetRelativeRotation().Yaw - LimitTargetAngleYaw) < 2.f) // Condition when it is no rotation
	{
		bIsRotate = false;
	}

	else if (FMath::Abs(TurretMesh->GetRelativeRotation().Yaw - LimitTargetAngleYaw) >= 2.f) // Condition when rotation exist
	{
		bIsRotate = true;
	}
}

void ATurretPawn::Fire()
{
	ShootDirection = (End - Start).GetSafeNormal();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
		ProjectileActor, Start, ShootDirection.Rotation(), SpawnParameters);
	Projectile->FireInDirection(ShootDirection);

	if (FireEfect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEfect, ProjectileSpawnPoint->GetComponentLocation());
	}

	if(ShootSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(), ShootSound, ProjectileSpawnPoint->GetComponentLocation());
	}
}

void ATurretPawn::DestroyActor(AActor* ActorToDestroy)
{
	ActorToDestroy->Destroy();
}

void ATurretPawn::ShakeCameraAfterKilling() const
{
	if (TOCameraShakeClass)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayWorldCameraShake(
			GetWorld(), TOCameraShakeClass, GetActorLocation(), 6000.0f, 10000.0f, 10.f);
	}
}

void ATurretPawn::HealthCheckedDeath(AActor* HealthKeeper, UTOHealthComponent* ParameterHealthComponent)
{
    if (IsValid(HealthKeeper))
	{
		if (ParameterHealthComponent->Health <= 0)
		{
			if (DeathEfect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathEfect, GetActorLocation());
			}

			if (DeathSound)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
			}

			if (GetLocalRole() == ROLE_Authority)
			{
				Multicast_HealthCheckedDeath(
					HealthKeeper, ParameterHealthComponent->Health, ParameterHealthComponent->DefaultHealth);
			}

			DestroyActor(HealthKeeper);
		}

		UpdateHealthBarComponent(HealthKeeper, ParameterHealthComponent);

		if (GetLocalRole() == ROLE_Authority)
		{
			Multicast_HealthCheckedDeath(
				HealthKeeper, ParameterHealthComponent->Health, ParameterHealthComponent->DefaultHealth);
		}
	}
}

void ATurretPawn::Multicast_HealthCheckedDeath_Implementation(
	AActor* RPCHealthKeeper, float RPCCurrentHealth, float RPCDefaultHealth)
{
	if (IsValid(RPCHealthKeeper) && GetLocalRole() != ROLE_Authority)
	{
		if (RPCCurrentHealth <= 0)
		{
			if (DeathEfect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathEfect, GetActorLocation());
			}

			if (DeathSound)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());
			}

			DestroyActor(RPCHealthKeeper);
		}

		if (UHealthTurretWidget* HealthBarWidget = Cast<UHealthTurretWidget>(HealthWidgetComponent->GetWidget()))
		{
			HealthBarWidget->SetHealthBar(
				RPCCurrentHealth, RPCDefaultHealth);
		}
	}
}

void ATurretPawn::UpdateHealthBarComponent(
	AActor* HealthKeeper, UTOHealthComponent* ParameterHealthComponent)
{
	if (HealthKeeper)
	{
		if (UHealthTurretWidget* HealthBarWidget = Cast<UHealthTurretWidget>(HealthWidgetComponent->GetWidget()))
		{
			HealthBarWidget->SetHealthBar(
				ParameterHealthComponent->Health, ParameterHealthComponent->DefaultHealth);
		}
	}
}