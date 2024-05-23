#include "TOGameModeBase.h"
#include "EngineUtils.h"
#include "TowerPawn.h"
#include "TankPawn.h"
#include "Kismet/GameplayStatics.h"

ATOGameModeBase::ATOGameModeBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	NumberTowers = 0;
	NumberTanks = 0;
}

void ATOGameModeBase::Win()
{
	OnEndGame.Broadcast(EEndGameState::Win);
}

void ATOGameModeBase::Lose()
{
	OnEndGame.Broadcast(EEndGameState::Lose);
}

void ATOGameModeBase::TankDestroyed(AActor* DestroyedActor)
{
	
	--NumberTanks;

	if (NumberTanks < 1)
	{
		Lose();
	}
}

void ATOGameModeBase::TowerDestroyed(AActor* DestroyedActor)
{
	--NumberTowers;

	if (NumberTowers < 1)
	{
 		Win();
	}
}

void ATOGameModeBase::InitPlayData()
{
	const auto Init = [this](UClass* Class, int32& Count, void (ThisClass::* Method)(AActor*), FName MethodName)
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(this, Class, Actors);
		Count = Actors.Num();

		for (AActor* Actor : Actors)
		{
			Actor->OnDestroyed.__Internal_AddDynamic(this, Method, MethodName);
		}
	};

	Init(ATowerPawn::StaticClass(), NumberTowers, &ATOGameModeBase::TowerDestroyed, TEXT("TowerDestroyed"));
	Init(ATankPawn::StaticClass(), NumberTanks, &ATOGameModeBase::TankDestroyed, TEXT("TankDestroyed"));
}

void ATOGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	InitPlayData();
}