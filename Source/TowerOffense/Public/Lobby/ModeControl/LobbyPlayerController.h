#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TowerOffense/Public/Lobby/ModeControl/LobbyGameModeBase.h"

#include "LobbyPlayerController.generated.h"

class UMatchSettingsUserWidget;
class UJoinedClientLobbyUserWidget;

UCLASS()
class TOWEROFFENSE_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Transient)
	TObjectPtr<UMatchSettingsUserWidget> MatchSettingsUserWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UMatchSettingsUserWidget> MatchSettingsUserWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UJoinedClientLobbyUserWidget> JoinedClientLobbyUserWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UJoinedClientLobbyUserWidget> JoinedClientLobbyUserWidgetClass;

	FString ControlMapName;
	FString ServerName;
	ETeam LobbyTeam;
	FTeamMembersInfo TeamMembersInfo;

	TObjectPtr<APlayerController> HostPlayerController;

public:
	void LimitPlayerMovement();
	
	void CreateMatchSettingsUserWidget();
	void CreateJoinedClientLobbyUserWidget();

	void SyncClientMatchSettingsPreferences(FString FromWidgetMapName);
	UFUNCTION(Server, Reliable)
	void Server_SyncClientMatchSettingsPreferences(const FString& ForServerMapName);

	UFUNCTION()
	void CreateSessionGame();
	UFUNCTION(Server, Reliable)
	void Server_CreateSessionGame();

	UFUNCTION(Server, Reliable)
	void Server_InitializeClientsReturnToMM();

	UFUNCTION()
	void ReturnToMM();
	UFUNCTION(Client, Reliable)
	void Client_ClientReturnToMM();

	void FindHostPlayer(APlayerController* ParamHost);
	UFUNCTION(Client, Reliable)
	void Client_FindHostPlayer(APlayerController* ClientHost);

	void SetLobbyTeam(ETeam ParamTeam);
	UFUNCTION(Server, Reliable)
	void Server_SetLobbyTeam(ETeam ServLobbyTeam);

	UFUNCTION(Server, Reliable)
	void Server_GetTeamMembersInfo();
	UFUNCTION(Client, Reliable)
	void Client_SetTeamMembersInfo(const FTeamMembersInfo& ClientTeamMembersInfo);

	UFUNCTION(Server, Reliable)
	void Server_GetServerName();
	UFUNCTION(Client, Reliable)
	void Client_SetServerName(const FString& ParamServName);

protected:
	virtual void BeginPlay() override;
};
