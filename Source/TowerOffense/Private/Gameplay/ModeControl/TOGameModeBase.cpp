#include "TowerOffense/Public/GamePlay/ModeControl/TOGameModeBase.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TowerOffense/Public/Gameplay/Pawn/TankPawn.h"
#include "TowerOffense/Public/Gameplay/Pawn/TowerPawn.h"
#include "TowerOffense/Public/Generic/LevelSystem.h"
#include "TowerOffense/Public/Generic/UActorMoverComponent.h"
#include "TowerOffense/Public/Generic/MeshMoverAlongSplineComponent.h"
#include "TowerOffense/Public/MainMenu/TOGameInstance.h"

ATOGameModeBase::ATOGameModeBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

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

	ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();

	/*UKismetSystemLibrary::PrintString(this, FString::Printf(
			TEXT("Current Level: %d"), LevelSystem->ActualCurrentLevel),
		true, false, FColor::Purple, 4.f);*/
}

void ATOGameModeBase::PostLogin(APlayerController* NewPlayerController)
{
	Super::PostLogin(NewPlayerController);

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
}

void ATOGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UTOGameInstance* GameInstance = GetGameInstance<UTOGameInstance>()) // delete current session when end play
	{
		if (GameInstance->SessionInterface->GetNamedSession("My Session"))
		{
			GameInstance->SessionInterface->DestroySession("My Session");
		}
	}
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

			if (ATOGameStateBase* TOGameState = GetGameState<ATOGameStateBase>())
			{
				TOGameState->SetNumberFirstTeamPlayers(FirstTeamPlayers);
				TOGameState->SetNumberSecondTeamPlayers(SecondTeamPlayers);
				TOGameState->SetNumberThirdTeamPlayers(ThirdTeamPlayers);
				TOGameState->SetNumberFourthTeamPlayers(FourthTeamPlayers);
			}
		}
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

void ATOGameModeBase::AlternativeTankDestroyed(AActor* DestroyedActor)
{
	if (ATankPawn* TankPawn = Cast<ATankPawn>(DestroyedActor))
	{
		ATOGameStateBase* TOGameState = GetGameState<ATOGameStateBase>();

		switch (TankPawn->Execute_GetTeam(TankPawn))
		{
			case ETeam::Team1:
				--FirstTeamPlayers;
				TOGameState->SetNumberFirstTeamPlayers(FirstTeamPlayers);
				break;

			case ETeam::Team2:
				--SecondTeamPlayers;
				TOGameState->SetNumberSecondTeamPlayers(SecondTeamPlayers);
				break;

			case ETeam::Team3:
				--ThirdTeamPlayers;
				TOGameState->SetNumberThirdTeamPlayers(ThirdTeamPlayers);
				break;

			case ETeam::Team4:
				--FourthTeamPlayers;
				TOGameState->SetNumberFourthTeamPlayers(FourthTeamPlayers);
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