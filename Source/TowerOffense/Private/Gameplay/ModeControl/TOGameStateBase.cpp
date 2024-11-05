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

void ATOGameStateBase::SetNumberFirstTeamPlayers(int32 FirstTeam)
{
	if (HasAuthority())
	{
		FirstTeamPlayers = FirstTeam;
	}
}

void ATOGameStateBase::SetNumberSecondTeamPlayers(int32 SecondTeam)
{
	if (HasAuthority())
	{
		SecondTeamPlayers = SecondTeam;
	}
}

void ATOGameStateBase::SetNumberThirdTeamPlayers(int32 ThirdTeam)
{
	if (HasAuthority())
	{
		ThirdTeamPlayers = ThirdTeam;
	}
}

void ATOGameStateBase::SetNumberFourthTeamPlayers(int32 FourthTeam)
{
	if (HasAuthority())
	{
		FourthTeamPlayers = FourthTeam;
	}
}

void ATOGameStateBase::OnRep_GamePhase()
{// client side; execute some code when GamePhase is replicated
	if (OnGamePhaseChanged.IsBound())
	{
		OnGamePhaseChanged.Broadcast(GamePhase);
	}
}

void ATOGameStateBase::OnRep_FirstTeamPlayers()
{
	if (OnFirstTeamTankDestroyed.IsBound())
	{
		OnFirstTeamTankDestroyed.Broadcast(FirstTeamPlayers);
	}
}

void ATOGameStateBase::OnRep_SecondTeamPlayers()
{
	if (OnSecondTeamTankDestroyed.IsBound())
	{
		OnSecondTeamTankDestroyed.Broadcast(SecondTeamPlayers);
	}
}

void ATOGameStateBase::OnRep_ThirdTeamPlayers()
{
	if (OnThirdTeamTankDestroyed.IsBound())
	{
		OnThirdTeamTankDestroyed.Broadcast(ThirdTeamPlayers);
	}
}

void ATOGameStateBase::OnRep_FourthTeamPlayers()
{
	if (OnFourthTeamTankDestroyed.IsBound())
	{
		OnFourthTeamTankDestroyed.Broadcast(FourthTeamPlayers);
	}
}

void ATOGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATOGameStateBase, GamePhase);

	DOREPLIFETIME(ATOGameStateBase, FirstTeamPlayers);
	DOREPLIFETIME(ATOGameStateBase, SecondTeamPlayers);
	DOREPLIFETIME(ATOGameStateBase, ThirdTeamPlayers);
	DOREPLIFETIME(ATOGameStateBase, FourthTeamPlayers);
}