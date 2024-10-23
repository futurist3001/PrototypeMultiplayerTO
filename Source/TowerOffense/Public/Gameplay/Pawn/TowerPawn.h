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

private:
	const FName DetectedBBKeyName = "IsDetected";

	uint8 bPlayedTurretRotationSoundIteration : 1;
	uint8 bIsPlaying : 1;

public:
	ATowerPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FORCEINLINE void SetPlayState(bool IsPlaying)
	{
		bIsPlaying = IsPlaying;
	}

	virtual void RotateTurret(
		const FRotator& Current, const FRotator& Target, float DeltaTime, float InterpSpeed) override; // alternative: make some class friend to that to avoid putting this func into public section
	virtual void Fire() override; // alternative: make some class friend to that to avoid putting this func into public section

	bool IsLookToTank();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, reliable)
	void Multicast_Fire(FVector StartFire, FVector EndFire);

	bool IsTheSameTeam(AActor* Actor);

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
