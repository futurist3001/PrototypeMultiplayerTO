#include "TowerOffense/Public/GamePlay/ModeControl/TOGameModeBase.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TowerOffense/Public/Gameplay/Pawn/TankPawn.h"
#include "TowerOffense/Public/Gameplay/Pawn/TowerPawn.h"
#include "TowerOffense/Public/Generic/LevelSystem.h"
#include "TowerOffense/Public/Generic/UActorMoverComponent.h"
#include "TowerOffense/Public/Generic/MeshMoverAlongSplineComponent.h"

ATOGameModeBase::ATOGameModeBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	NumberTowers = 0;
	NumberTanks = 0;

	FirstTeamPlayers = 0;
	SecondTeamPlayers = 0;
	ThirdTeamPlayers = 0;
	FourthTeamPlayers = 0;
}

void ATOGameModeBase::OpenRelativeLevelCC(int32 LevelIndex) const
{
	ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();
	LevelSystem->OpenRelativeLevel(GetWorld(), LevelIndex);
}

void ATOGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ATOGameStateBase* TOGameState = GetGameState<ATOGameStateBase>();

	if (TOGameState)
	{
		TOGameState->SetGamePhase(EGamePhase::Preparation);
	}

	FTimerHandle PostponeInitilezeTimer;
	GetWorldTimerManager().SetTimer(
		PostponeInitilezeTimer, this, &ThisClass::PostponeInitilize, 1.5f, false);

	ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();

	UKismetSystemLibrary::PrintString(this, FString::Printf(
			TEXT("Current Level: %d"), LevelSystem->ActualCurrentLevel),
		true, false, FColor::Purple, 4.f);
}

void ATOGameModeBase::InitPlayData()
{
#define INIT_PLAY_DATA(Class, Count, Method)                        \
	do {                                                            \
		TArray<AActor*> Actors;                                     \
		UGameplayStatics::GetAllActorsOfClass(this, Class, Actors); \
		Count = Actors.Num();                                       \
		for (AActor* Actor : Actors)                                \
		{                                                           \
			Actor->OnDestroyed.AddDynamic(this, Method);            \
		}                                                           \
	} while (0)

	INIT_PLAY_DATA(ATowerPawn::StaticClass(), NumberTowers, &ATOGameModeBase::TowerDestroyed);
	INIT_PLAY_DATA(ATankPawn::StaticClass(), NumberTanks, &ATOGameModeBase::TankDestroyed);
#undef INIT_PLAY_DATA
}

void ATOGameModeBase::AlternativeInitPlayData()
{
	TArray<AActor*> Tanks;
	UGameplayStatics::GetAllActorsOfClass(this, ATankPawn::StaticClass(), Tanks);

	FirstTeamPlayers = 0;
	SecondTeamPlayers = 0;
	ThirdTeamPlayers = 0;
	FourthTeamPlayers = 0; // in case for update when changes team for players

	for (AActor* Tank : Tanks)
	{
		if (ATankPawn* TankPlayer = Cast<ATankPawn>(Tank))
		{
			switch (TankPlayer->Execute_GetTeam(TankPlayer))
			{
				case ETeam::Team1:
					++FirstTeamPlayers;
					break;

				case ETeam::Team2:
					++SecondTeamPlayers;
					break;

				case ETeam::Team3:
					++ThirdTeamPlayers;
					break;

				case ETeam::Team4:
					++FourthTeamPlayers;
					break;
					
				default:
					break;
			}

			if (!TankPlayer->OnDestroyed.Contains(this, "AlternativeTankDestroyed"))
			{
				TankPlayer->OnDestroyed.AddDynamic(this, &ATOGameModeBase::AlternativeTankDestroyed);
			}
		}
	}
}

void ATOGameModeBase::PostponeInitilize()
{
	if (ATOGameStateBase* TOGameState = GetGameState<ATOGameStateBase>())
	{
		//InitPlayData();
		AlternativeInitPlayData();

		TArray<AActor*> Tanks;
		UGameplayStatics::GetAllActorsOfClass(this, ATankPawn::StaticClass(), Tanks);
		for (AActor* Tank : Tanks)
		{
			if (ATankPawn* TankPawn = Cast<ATankPawn>(Tank); !TankPawn->OnChangeTeam.IsBound())
			{
				TankPawn->OnChangeTeam.AddDynamic(this, &ATOGameModeBase::AlternativeInitPlayData);
			}
		}

		TOGameState->SetNumberTowers(NumberTowers);
		TOGameState->SetNumberTanks(NumberTanks);
	}
}

void ATOGameModeBase::GameStarted()
{
	ATOGameStateBase* TOGameState = GetGameState<ATOGameStateBase>();

	if (TOGameState && TOGameState->GetGamePhase() == EGamePhase::Playing)
	{
		TArray<AActor*> Towers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATowerPawn::StaticClass(), Towers);

		for (AActor* Tower : Towers)
		{
			ATowerPawn* TowerPawn = Cast<ATowerPawn>(Tower);
			TowerPawn->SetPlayState(true);
		}

		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

		for (AActor* Actor : AllActors)
		{
			UUActorMoverComponent* ActorMoverComponent = 
				Actor->FindComponentByClass<UUActorMoverComponent>();

			if (ActorMoverComponent)
			{
				ActorMoverComponent->SetPlayState(true);
			}

			UMeshMoverAlongSplineComponent* MeshMoverAlongSplineComponent =
				Actor->FindComponentByClass<UMeshMoverAlongSplineComponent>();

			if (MeshMoverAlongSplineComponent)
			{
				MeshMoverAlongSplineComponent->SetPlayState(true);
			}
		}
	}
}

/*void ATOGameModeBase::Win()
{
	SetEndGameState(EGamePhase::Win);

	ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();
	LevelSystem->OpenNextLevel(GetWorld(), LevelSystem->ActualCurrentLevel + 1);
	LevelSystem->SaveLevelState();
}

void ATOGameModeBase::Lose()
{
	SetEndGameState(EGamePhase::Lose);
}*/

void ATOGameModeBase::SetEndGameState(EGamePhase Phase)
{
	ATOGameStateBase* TOGameState = GetGameState<ATOGameStateBase>();

	if (GameState)
	{
		TOGameState->SetGamePhase(Phase);

		if (Phase != EGamePhase::Preparation || Phase != EGamePhase::Playing)
		{
			TOGameState->OnGamePhaseChanged.Broadcast(Phase); // Broadcast on server side for correct pause game at the end
		}
	}
}

void ATOGameModeBase::TankDestroyed(AActor* DestroyedActor)
{
	ATOGameStateBase* TOGameState = GetGameState<ATOGameStateBase>();

	if (--NumberTanks < 1 && TOGameState->GetGamePhase() == EGamePhase::Playing)
	{
		//Lose();
	}

	TOGameState->SetNumberTanks(NumberTanks); // think about transfer this to alternative version
}

void ATOGameModeBase::AlternativeTankDestroyed(AActor* DestroyedActor)
{
	if (ATankPawn* TankPawn = Cast<ATankPawn>(DestroyedActor))
	{
		ATOGameStateBase* TOGameState = GetGameState<ATOGameStateBase>();

		switch (TankPawn->Execute_GetTeam(TankPawn))
		{
			case ETeam::Team1:
				--FirstTeamPlayers;
				break;

			case ETeam::Team2:
				--SecondTeamPlayers;
				break;

			case ETeam::Team3:
				--ThirdTeamPlayers;
				break;

			case ETeam::Team4:
				--FourthTeamPlayers;
				break;

			default:
				break;
		}

		AlternativeWinCase();
	}
}

void ATOGameModeBase::AlternativeWinCase()
{
	if (FirstTeamPlayers >= 1 && SecondTeamPlayers <= 0 &&
		ThirdTeamPlayers <= 0 && FourthTeamPlayers <= 0)
	{
		SetEndGameState(EGamePhase::FirstTeamWin);
	}

	else if (SecondTeamPlayers >= 1 && FirstTeamPlayers <= 0 &&
		ThirdTeamPlayers <= 0 && FourthTeamPlayers <= 0)
	{
		SetEndGameState(EGamePhase::SecondTeamWin);
	}

	else if (ThirdTeamPlayers >= 1 && FirstTeamPlayers <= 0 &&
		SecondTeamPlayers <= 0 && FourthTeamPlayers <= 0)
	{
		SetEndGameState(EGamePhase::ThirdTeamWin);
	}

	else if (FourthTeamPlayers >= 1 && FirstTeamPlayers <= 0 &&
		SecondTeamPlayers <= 0 && ThirdTeamPlayers <= 0)
	{
		SetEndGameState(EGamePhase::FourthTeamWin);
	}
}

void ATOGameModeBase::TowerDestroyed(AActor* DestroyedActor) // it`s not important
{
	ATOGameStateBase* TOGameState = GetGameState<ATOGameStateBase>();

 	if (--NumberTowers < 1 && TOGameState->GetGamePhase() == EGamePhase::Playing)
	{
 		//Win();
	}

	TOGameState->SetNumberTowers(NumberTowers);
}