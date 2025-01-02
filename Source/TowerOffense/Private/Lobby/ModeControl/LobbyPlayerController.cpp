#include "TowerOffense/Public/Lobby/ModeControl/LobbyPlayerController.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "TowerOffense/Public/TOGameInstance.h"
#include "TowerOffense/Public/Lobby/UI/MatchSettingsUserWidget.h"
#include "TowerOffense/Public/Lobby/UI/JoinedClientLobbyUserWidget.h"
#include "TowerOffense/Public/Lobby/ModeControl/LobbyGameModeBase.h"

void ALobbyPlayerController::LimitPlayerMovement()
{
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this);

	bShowMouseCursor = true;
}

void ALobbyPlayerController::CreateMatchSettingsUserWidget()
{
	if (MatchSettingsUserWidgetClass)
	{
		MatchSettingsUserWidget = CreateWidget<UMatchSettingsUserWidget>(this, MatchSettingsUserWidgetClass);
		MatchSettingsUserWidget->AddToViewport();
		MatchSettingsUserWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ALobbyPlayerController::CreateJoinedClientLobbyUserWidget()
{
	if (JoinedClientLobbyUserWidgetClass)
	{
		JoinedClientLobbyUserWidget = CreateWidget<UJoinedClientLobbyUserWidget>(this, JoinedClientLobbyUserWidgetClass);
		JoinedClientLobbyUserWidget->AddToViewport();
		JoinedClientLobbyUserWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ALobbyPlayerController::SyncClientMatchSettingsPreferences(FString FromWidgetMapName)
{
	ControlMapName = FromWidgetMapName;

	Server_SyncClientMatchSettingsPreferences(FromWidgetMapName);
}

void ALobbyPlayerController::CreateSessionGame()
{
	Server_CreateSessionGame();
}

void ALobbyPlayerController::Server_InitializeClientsReturnToMM_Implementation()
{
	if (ALobbyGameModeBase* LobbyGameMode = GetWorld()->GetAuthGameMode<ALobbyGameModeBase>())
	{
		LobbyGameMode->ReturnAllClientsToMM();
	}
}

void ALobbyPlayerController::Server_CreateSessionGame_Implementation()
{
	if (ALobbyGameModeBase* LobbyGameMode = GetWorld()->GetAuthGameMode<ALobbyGameModeBase>();
		UTOGameInstance* GameInstance = GetGameInstance<UTOGameInstance>())
	{
		LobbyGameMode->PerformServerTravel(GameInstance->MapName);
	}
}

void ALobbyPlayerController::Server_SyncClientMatchSettingsPreferences_Implementation(const FString& ForServerMapName)
{
	ControlMapName = ForServerMapName;

	if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>())
	{
		TOGameInstance->MapName = ControlMapName;
		TOGameInstance->UpdateUEServer();
	}
}

void ALobbyPlayerController::ReturnToMM()
{
	ClientTravel(TEXT("/Game/TowerOffense/TowerOffenseLevel/TOMainMenuLevel"), ETravelType::TRAVEL_Absolute);
}

void ALobbyPlayerController::Client_ClientReturnToMM_Implementation()
{
	ReturnToMM();
}

void ALobbyPlayerController::FindHostPlayer(APlayerController* ParamHost)
{
	if (ParamHost->IsValidLowLevel() && GetLocalRole() == ROLE_Authority)
	{
		HostPlayerController = ParamHost;

		FTimerHandle ClientFindHostRPCTimer;
		GetWorld()->GetTimerManager().SetTimer(
			ClientFindHostRPCTimer, [this, ParamHost]()
			{
				Client_FindHostPlayer(ParamHost);
			}, 0.1f, false);
	}
}

void ALobbyPlayerController::SetLobbyTeam(ETeam ParamTeam)
{
	LobbyTeam = ParamTeam;
	Server_SetLobbyTeam(ParamTeam);
}

void ALobbyPlayerController::Server_GetTeamMembersInfo_Implementation()
{
	if (ALobbyGameModeBase* LobbyGameModeBase = GetWorld()->GetAuthGameMode<ALobbyGameModeBase>();
		GetLocalRole() == ROLE_Authority)
	{
		TeamMembersInfo = LobbyGameModeBase->GetTeamMembersStat();

		Client_SetTeamMembersInfo(TeamMembersInfo);
	}
}

void ALobbyPlayerController::Client_SetTeamMembersInfo_Implementation(const FTeamMembersInfo& ClientTeamMembersInfo)
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		TeamMembersInfo = ClientTeamMembersInfo;
	}
}

void ALobbyPlayerController::Server_SetLobbyTeam_Implementation(ETeam ServLobbyTeam)
{
	LobbyTeam = ServLobbyTeam;
}

void ALobbyPlayerController::Client_FindHostPlayer_Implementation(APlayerController* ClientHost) // calling this clientRPC must be earlier than creating lobby widget
{
	HostPlayerController = ClientHost;
}

void ALobbyPlayerController::Server_GetServerName_Implementation()
{
	if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>())
	{
		ServerName = TOGameInstance->ServerName;
		Client_SetServerName(ServerName);
	}
}

void ALobbyPlayerController::Client_SetServerName_Implementation(const FString& ParamServName)
{
	ServerName = ParamServName;
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	LimitPlayerMovement();

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		FTimerHandle CreateLobbyTimer;
		GetWorld()->GetTimerManager().SetTimer(
			CreateLobbyTimer, [this]()
			{
				if (HostPlayerController && HostPlayerController == Cast<APlayerController>(this))
				{
					CreateMatchSettingsUserWidget();
				}

				else if (!HostPlayerController && HostPlayerController != Cast<APlayerController>(this))
				{
					CreateJoinedClientLobbyUserWidget();
				}

			}, 0.15f, false);
	}
}