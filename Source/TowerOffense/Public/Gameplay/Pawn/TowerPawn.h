#pragma once

#include "CoreMinimal.h"
#include "TurretPawn.h"

#include "TowerPawn.generated.h"

class USphereComponent;

UCLASS()
class TOWEROFFENSE_API ATowerPawn : public ATurretPawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(Transient)
	TArray<AActor*> OverlapedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float PeriodFire;

private:
	FTimerHandle FireTimerHandle;
	uint8 bPlayedTurretRotationSoundIteration : 1;
	uint8 bIsPlaying : 1;

public:
	ATowerPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	FORCEINLINE void SetPlayState(bool IsPlaying)
	{
		bIsPlaying = IsPlaying;
	}

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void RotateTurret() override;
	virtual void Fire() override;
	bool IsLookToTank();
	bool IsTheSameTeam(AActor* Actor);

	UFUNCTION(Server, unreliable)
	void Server_SetTurretRotation(UStaticMeshComponent* Turret, FRotator TurretRotation);

private:
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};
