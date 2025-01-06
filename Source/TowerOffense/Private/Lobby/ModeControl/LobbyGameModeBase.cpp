#include "TowerOffense/Public/Lobby/ModeControl/LobbyGameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "TowerOffense/Public/TOGameInstance.h"
#include "TowerOffense/Public/Lobby/ModeControl/LobbyPlayerController.h"

ALobbyGameModeBase::ALobbyGameModeBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bUseSeamlessTravel = true;
}

void ALobbyGameModeBase::GetUEServerInfo()
{
	int32 UEPort = GetWorld()->URL.Port;

	if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>())
	{
		TOGameInstance->GetServerInfoByPort(UEPort);
	}
}

void ALobbyGameModeBase::PerformServerTravel(const FString& MapName)
{
	if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>())
	{
		TOGameInstance->MatchStatus = 2; // here we inform that from this stage we are in game and other players can`t join to us
		TOGameInstance->UpdateUEServer();
	}

	FString TravelPath = FString::Printf(TEXT("/Game/TowerOffense/GameMaps/%s"), *MapName);
	GetWorld()->ServerTravel(TravelPath, true);
}

void ALobbyGameModeBase::ReturnAllClientsToMM()
{
	for (APlayerState* PlayerState : GetGameState<AGameStateBase>()->PlayerArray)
	{
		if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(PlayerState->GetOwner()))
		{
			LobbyPlayerController->Client_ClientReturnToMM();
		}
	}
}

FTeamMembersInfo ALobbyGameModeBase::GetTeamMembersStat()
{
	ResetTeamMembersInfo();

	for (APlayerState* PlayerState : GetGameState<AGameStateBase>()->PlayerArray)
	{
		if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(PlayerState->GetOwner()))
		{
			switch (LobbyPlayerController->LobbyTeam)
			{
				case ETeam::Team1:
					++TeamMembersInfo.FirstTeamNumbers;
					break;

				case ETeam::Team2:
					++TeamMembersInfo.SecondTeamNumbers;
					break;

				case ETeam::Team3:
					++TeamMembersInfo.ThirdTeamNumbers;
					break;

				case ETeam::Team4:
					++TeamMembersInfo.FourthTeamNumbers;
					break;
			}
		}
	}

	return TeamMembersInfo;
}

void ALobbyGameModeBase::ResetTeamMembersInfo()
{
	TeamMembersInfo.FirstTeamNumbers = 0;
	TeamMembersInfo.SecondTeamNumbers = 0;
	TeamMembersInfo.ThirdTeamNumbers = 0;
	TeamMembersInfo.FourthTeamNumbers = 0;
}

APlayerController* ALobbyGameModeBase::Login(
	UPlayer* NewPlayer, ENetRole InRemoteRole,const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* PlayerController =
		Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	if (!PlayerController)
	{
		ErrorMessage = TEXT("Failed to create PlayerController");
		return nullptr;
	}

	if (!HostPlayer)
	{
		HostPlayer = Cast<APlayerController>(PlayerController);
	}

	if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(PlayerController))
	{
		LobbyPlayerController->FindHostPlayer(HostPlayer);
	}

	if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>())
	{
		if (TOGameInstance->CurrPlayers == 0)
		{
			GetUEServerInfo();

			FTimerHandle PauseUpdateTimer;
			GetWorld()->GetTimerManager().SetTimer(
				PauseUpdateTimer, [TOGameInstance]()
				{
					++TOGameInstance->CurrPlayers;
					TOGameInstance->MaxPlayers = 12;
					TOGameInstance->MatchStatus = 1; // we inform that we in lobby stage and other players can join to us

					TOGameInstance->UpdateUEServer();
				}, 1.f, false);
		}

		else
		{
			++TOGameInstance->CurrPlayers;
			TOGameInstance->UpdateUEServer();
		}
	}

	return PlayerController;
}

void ALobbyGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>())
	{
		--TOGameInstance->CurrPlayers;
		TOGameInstance->UpdateUEServer();

		if (TOGameInstance->CurrPlayers <= 0)
		{
			TOGameInstance->CloseDedicatedServer();

			FTimerHandle LastCloseServerTimer;
			GetWorld()->GetTimerManager().SetTimer(
				LastCloseServerTimer, []()
				{
					FGenericPlatformMisc::RequestExit(false);
				}, 2.0f, false);
		}
	}
}