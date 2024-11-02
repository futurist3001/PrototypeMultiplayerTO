#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TowerOffense/Public/Generic/TeamMemberInterface.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOGameModeBase.h"
#include "TowerOffense/Public/Gameplay/Other/TOHealthComponent.h"

#include "TurretPawn.generated.h"

class AProjectile;
class UAudioComponent;
class UCapsuleComponent;
class UParticleSystem;
class USoundBase;
class UTOCameraShake;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class TOWEROFFENSE_API ATurretPawn : public APawn, public ITeamMemberInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> ProjectileSpawnPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthWidgetComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	TObjectPtr<UTOHealthComponent> HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	TObjectPtr<UParticleSystem> DeathEfect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	TObjectPtr<UParticleSystem> FireEfect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	ETeam Team;

protected:
	UPROPERTY(EditAnywhere, Category = "Base", meta = (GetOptions = "GetBaseMeshMaterialSlotOptions"))
	FName BaseMeshMaterialSlotName;

	UPROPERTY(EditAnywhere, Category = "Base", meta = (GetOptions = "GetBaseMeshMaterialParameterOptions"))
	FName BaseMaterialParameterName;

	UPROPERTY(EditAnywhere, Category = "Base")
	FLinearColor BaseColor;

	UPROPERTY(EditAnywhere, Category = "Turret", meta = (GetOptions = "GetTurretMeshMaterialSlotOptions"))
	FName TurretMeshMaterialSlotName;

	UPROPERTY(EditAnywhere, Category = "Turret", meta = (GetOptions = "GetTurretMeshMaterialParameterOptions"))
	FName TurretMaterialParameterName;

	UPROPERTY(EditAnywhere, Category = "Turret")
	FLinearColor TurretColor;

	UPROPERTY(EditAnywhere, Category = "Turret")
	float TurretRotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	TObjectPtr<USoundBase> ShootSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	TObjectPtr<USoundBase> TurretRotationSound;

	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<UTOCameraShake> TOCameraShakeClass;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* BaseDynamicMaterialInstance;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* TurretDynamicMaterialInstance;

	FRotator TargetAngle; // For rotation
	float RotationCurrentTime;
	uint8 bIsRotate : 1;
	FVector Start; // For fire start point
	FVector End; // For fire end point
	FVector ShootDirection;
	UAudioComponent* TurretRotationAudioComponent;

public:
	ATurretPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FORCEINLINE const ETeam GetTeam_Implementation() const
	{
		return Team;
	}

	FORCEINLINE const FRotator GetTargetAngle() const
	{
		return TargetAngle;
	}

	FORCEINLINE const float GetTurretRotationSpeed() const
	{
		return TurretRotationSpeed;
	}

	FORCEINLINE const float GetRotationCurrentTime() const
	{
		return RotationCurrentTime;
	}

	void ShakeCameraAfterKilling() const;

	UFUNCTION()
	void HealthCheckedDeath(
		AActor* HealthKeeper, UTOHealthComponent* ParameterHealthComponent);
	UFUNCTION(NetMulticast, reliable)
	void Multicast_HealthCheckedDeath(
		AActor* RPCHealthKeeper, float RPCCurrentHealth, float RPCDefaultHealth);
	UFUNCTION()
	void UpdateHealthBarComponent(
		AActor* HealthKeeper, UTOHealthComponent* ParameterHealthComponent);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void RotateTurret(
		const FRotator& Current, const FRotator& Target, float DeltaTime, float InterpSpeed);
	virtual void Fire();
	virtual void DestroyActor(AActor* ActorToDestroy);

	void SetMeshMaterial(
		UStaticMeshComponent* MeshComponent, FName MeshMaterialSlotName, FName MaterialParameterName,
		const FLinearColor& Color, UMaterialInstanceDynamic*& DynamicMaterialInstance);

private:
	UFUNCTION()
	TArray<FName> GetBaseMeshMaterialSlotOptions() const;

	UFUNCTION()
	TArray<FName> GetTurretMeshMaterialSlotOptions() const;

	static TArray<FName> GetMaterialSlotOptions(const UStaticMeshComponent* InputComponentSlot);

	UFUNCTION()
	TArray<FName> GetBaseMeshMaterialParameterOptions() const;

	UFUNCTION()
	TArray<FName> GetTurretMeshMaterialParameterOptions() const;
	
	static TArray<FName> GetMaterialParameterOptions(
		const UStaticMeshComponent* InputComponentParameter, FName MeshMaterialSlotName);
};
