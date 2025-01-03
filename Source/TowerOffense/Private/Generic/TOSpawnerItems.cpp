#include "TowerOffense/Public/Generic/TOSpawnerItems.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TowerOffense/Public/Gameplay/Pawn/TankPawn.h"

ATOSpawnerItems::ATOSpawnerItems(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
	ItemVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Item"));

	SphereComponent->InitSphereRadius(70.f);

	SetRootComponent(SphereComponent);
	ItemVFX->SetupAttachment(RootComponent);

	ItemVFX->SetAutoActivate(true);

	RespawnTime = 10.0f;
}

void ATOSpawnerItems::Respawn()
{
	ItemVFX->SetVisibility(true);
	//ItemVFX->Activate();

	SetActorEnableCollision(true);
}

void ATOSpawnerItems::SpecialBonus(AActor* Actor)
{
}

void ATOSpawnerItems::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ATOSpawnerItems::OnBeginOverlap);
}

void ATOSpawnerItems::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsValidLowLevel() && OtherActor != this && OtherActor->IsA<ATankPawn>())
	{
		SpecialBonus(OtherActor);

		ItemVFX->SetVisibility(false);
		//ItemVFX->Deactivate();

		SetActorEnableCollision(false);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), DeathVFX, GetActorLocation());

		GetWorld()->GetTimerManager().SetTimer(
			RespawnTimerHandle, this, &ATOSpawnerItems::Respawn, RespawnTime, false);
	}
}

