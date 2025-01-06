#include "TowerOffense/Public/GamePlay/ModeControl/TOGameModeBase.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework\PlayerStart.h"
#include "TowerOffense/Public/Gameplay/Pawn/TankPawn.h"
#include "TowerOffense/Public/Gameplay/Pawn/TowerPawn.h"
#include "TowerOffense/Public/Generic/LevelSystem.h"
#include "TowerOffense/Public/Generic/UActorMoverComponent.h"
#include "TowerOffense/Public/Generic/MeshMoverAlongSplineComponent.h"
#include "TowerOffense/Public/TOGameInstance.h"

ATOGameModeBase::ATOGameModeBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	FirstTeamPlayers = 0;
	SecondTeamPlayers = 0;
	ThirdTeamPlayers = 0;
	FourthTeamPlayers = 0;

	bIsSetPlayersPosition = false;
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

	GetWorld()->GetTimerManager().SetTimer(
		SetPlayersTeamTimer, [this]()
		{
			TArray<AActor*> Tanks;
			UGameplayStatics::GetAllActorsOfClass(this, ATankPawn::StaticClass(), Tanks);
			for (AActor* Tank : Tanks)
			{
				if (ATankPawn* TankPawn = Cast<ATankPawn>(Tank))
				{
					TankPawn->Client_GetPlayerTeam();
				}
			}
		}, 5.f, true, 1.f); // execute when all player`s pawns are constructed (time must be less than time in TOPlayerController (means the last parameter here))

	FTimerHandle SetProperStartPosTimer;
	GetWorld()->GetTimerManager().SetTimer(
		SetProperStartPosTimer, [this]()
		{
			SetPlayersPosition();
		}, 2.5f, false); // must execute after all server tanks will get proper team value (previous timer)

	FTimerHandle InitTimer;
	GetWorld()->GetTimerManager().SetTimer(
		InitTimer, [this]()
		{
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
		}, 3.0f, false); // this time must be increased if numbers of players increased
}

void ATOGameModeBase::PostLogin(APlayerController* NewPlayerController)
{
	Super::PostLogin(NewPlayerController);

	UE_LOG(LogTemp, Error, TEXT("Players are in a battle."));

	FTimerHandle InitTimer;
	GetWorld()->GetTimerManager().SetTimer(
		InitTimer, [this]()
		{
			AlternativeInitPlayData();
		}, 2.0f, false);

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

APlayerController* ATOGameModeBase::Login(
	UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* PlayerController =
		Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	if (!PlayerController)
	{
		ErrorMessage = TEXT("Failed to create PlayerController");
		return nullptr;
	}

	return PlayerController;
}

void ATOGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	TArray<AActor*> Tanks;
	UGameplayStatics::GetAllActorsOfClass(this, ATankPawn::StaticClass(), Tanks);
	TotalPlayerNumbers = Tanks.Num();

	if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>(); TotalPlayerNumbers <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(SetPlayersTeamTimer);

		TOGameInstance->CloseDedicatedServer();

		FTimerHandle LastCloseServerTimer;
		GetWorld()->GetTimerManager().SetTimer(
			LastCloseServerTimer, []()
			{
				FGenericPlatformMisc::RequestExit(false);
			}, 2.0f, false);
	}
}

void ATOGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void ATOGameModeBase::AlternativeInitPlayData()
{
	TArray<AActor*> Tanks;
	UGameplayStatics::GetAllActorsOfClass(this, ATankPawn::StaticClass(), Tanks);
	TotalPlayerNumbers = Tanks.Num();

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

		if ((Phase != EGamePhase::Preparation) || (Phase != EGamePhase::Playing))
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
		DeactivateTowers();
	}

	else if (SecondTeamPlayers >= 1 && FirstTeamPlayers <= 0 &&
			ThirdTeamPlayers <= 0 && FourthTeamPlayers <= 0)
	{
		SetEndGameState(EGamePhase::SecondTeamWin);
		DeactivateTowers();
	}

	else if (ThirdTeamPlayers >= 1 && FirstTeamPlayers <= 0 &&
			SecondTeamPlayers <= 0 && FourthTeamPlayers <= 0)
	{
		SetEndGameState(EGamePhase::ThirdTeamWin);
		DeactivateTowers();
	}

	else if (FourthTeamPlayers >= 1 && FirstTeamPlayers <= 0 &&
			SecondTeamPlayers <= 0 && ThirdTeamPlayers <= 0)
	{
		SetEndGameState(EGamePhase::FourthTeamWin);
		DeactivateTowers();
	}
}

void ATOGameModeBase::DeactivateTowers()
{
	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATowerPawn::StaticClass(), Towers);

	for (AActor* Tower : Towers)
	{
		ATowerPawn* TowerPawn = Cast<ATowerPawn>(Tower);
		TowerPawn->SetPlayState(false);
	}
}

void ATOGameModeBase::SetPlayersPosition()
{
	TArray<AActor*> Tanks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATankPawn::StaticClass(), Tanks);

	for (AActor* Tank : Tanks)
	{
		if (ATankPawn* TankPawn = Cast<ATankPawn>(Tank))
		{
			TArray<AActor*> PlayerStarts;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

			for (AActor* PlayerSart : PlayerStarts)
			{
				if (APlayerStart* PlayerStartPos = Cast<APlayerStart>(PlayerSart))
				{
					if ((TankPawn->Team == ETeam::Team1 && PlayerStartPos->PlayerStartTag == FName("Team1")) ||
						(TankPawn->Team == ETeam::Team2 && PlayerStartPos->PlayerStartTag == FName("Team2")) ||
						(TankPawn->Team == ETeam::Team3 && PlayerStartPos->PlayerStartTag == FName("Team3")) ||
						(TankPawn->Team == ETeam::Team4 && PlayerStartPos->PlayerStartTag == FName("Team4")))
					{
						TankPawn->CapsuleComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
						TankPawn->CapsuleComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
						TankPawn->CapsuleComponent->SetSimulatePhysics(false);
						TankPawn->SetActorLocation(PlayerStartPos->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
						TankPawn->SetActorRotation(PlayerStartPos->GetActorRotation(), ETeleportType::TeleportPhysics);
						TankPawn->CapsuleComponent->SetSimulatePhysics(true);
						PlayerStartPos->Destroy();
						break;
					}
				}
			}
		}
	}
}
