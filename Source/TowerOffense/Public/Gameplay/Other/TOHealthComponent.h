#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "TOHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FHealthChanged, AActor*, HealthKeeper, UTOHealthComponent*, ParameterHealthComponent);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOWEROFFENSE_API UTOHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DefaultHealth;

	UPROPERTY(ReplicatedUsing = OnRep_HealthChanged, BlueprintReadWrite)
	float Health;

	UPROPERTY(BlueprintAssignable)
	FHealthChanged HealthChanged;

public:	
	UTOHealthComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION()
	void TakeDamage(
		AActor* DamagedActor, float Damage, const UDamageType* DamageType,
		AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnRep_HealthChanged();

protected:
	virtual void BeginPlay() override;

private:
	void BroadcastOnServerSide();
};
