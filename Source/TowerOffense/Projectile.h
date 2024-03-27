#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Projectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class TOWEROFFENSE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

public:	
	AProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	//virtual void PostInitializeComponents() override;

private:
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);
};
