#include "Gameplay/ModeControl/TOGameStateBase.h"

#include "Net/UnrealNetwork.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOGameModeBase.h"

ATOGameStateBase::ATOGameStateBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true;
}

void ATOGameStateBase::SetGamePhase(EGamePhase NewPhase)
{
	if (HasAuthority())
	{
		GamePhase = NewPhase;

		if (ATOGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ATOGameModeBase>())
		{
			GameMode->GameStarted();
		}
	}
}

void ATOGameStateBase::SetNumberTowers(int32 Towers)
{
	if (HasAuthority())
	{
		NumberTowersState = Towers;
	}
}

void ATOGameStateBase::SetNumberTanks(int32 Tanks)
{
	if (HasAuthority())
	{
		NumberTanksState = Tanks;
	}
}

void ATOGameStateBase::OnRep_GamePhase()
{// client side; execute some code when GamePhase is replicated
	if (OnGamePhaseChanged.IsBound())
	{
		OnGamePhaseChanged.Broadcast(GamePhase);
	}
}

void ATOGameStateBase::OnRep_NumberTanks()
{
	if (OnTankDestroyed.IsBound())
	{
		OnTankDestroyed.Broadcast(NumberTanksState);
	}
}

void ATOGameStateBase::OnRep_NumberTowers()
{
	if (OnTowerDestroyed.IsBound())
	{
		OnTowerDestroyed.Broadcast(NumberTowersState);
	}
}

void ATOGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATOGameStateBase, GamePhase);
	DOREPLIFETIME(ATOGameStateBase, NumberTanksState);
	DOREPLIFETIME(ATOGameStateBase, NumberTowersState);
}