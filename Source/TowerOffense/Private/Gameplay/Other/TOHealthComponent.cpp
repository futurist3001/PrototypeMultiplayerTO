#include "TowerOffense/Public/Gameplay/Other/TOHealthComponent.h"

#include "Net/UnrealNetwork.h"

UTOHealthComponent::UTOHealthComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	DefaultHealth = 100.0f;
}

void UTOHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = DefaultHealth;

	AActor* Owner = GetOwner();

	if (IsValid(Owner))
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UTOHealthComponent::TakeDamage);
	}
}

void UTOHealthComponent::TakeDamage(
	AActor* DamagedActor, float Damage, const UDamageType* DamageType, 
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0)
	{
		return;
	}

 	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	
	if (Health <= 0.0f)
	{// Broadcast on server side. Must be only for destroying owner of this component. Delay must be to ensure that all widgets update in time
		FTimerHandle DelayTimer;
		GetWorld()->GetTimerManager().SetTimer(
			DelayTimer, this, &UTOHealthComponent::BroadcastOnServerSide, 0.1f, false);
	}
}

void UTOHealthComponent::OnRep_HealthChanged()
{// Broadcast only on client side
	if (HealthChanged.IsBound())
	{
		HealthChanged.Broadcast(GetOwner(), this);
	}
}

void UTOHealthComponent::BroadcastOnServerSide()
{
	HealthChanged.Broadcast(GetOwner(), this);
}

void UTOHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTOHealthComponent, Health);
}