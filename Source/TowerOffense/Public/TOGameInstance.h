#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "IWebSocket.h"
#include "IWebSocketsManager.h"
#include "WebSocketsModule.h"

#include "TowerOffense/Public/Gameplay/ModeControl/TOGameModeBase.h"

#include "TOGameInstance.generated.h"

USTRUCT()
struct FUEServerInfo
{
	GENERATED_BODY()

public:
	int32 UEServID;
	FString UEServName;
	int32 UEServPort;
	FString UEServHost;
	int32 UEServMatchStatus;
	FString UEServMatchType;
	FString UEServMapName;
	int32 CurrentPlayers;
	int32 MaximumPlayers;

public:
	FUEServerInfo()
		: UEServID(0), UEServName("DefaultServerName"), UEServPort(0),
		UEServHost("0.0.0.0"), UEServMatchStatus(0), UEServMatchType("DefaultMatch"),
		UEServMapName("DefaultMap"), CurrentPlayers(0), MaximumPlayers(12)
	{
	}
};

UCLASS()
class TOWEROFFENSE_API UTOGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	TSharedPtr<IWebSocket> WebSocket;

	int32 MaxPlayers;
	int32 CurrPlayers;
	int32 MatchStatus;
	int32 ServerID;
	int32 ServerPort;

	FString Host;
	FString ServerName;
	FString MapName;
	FString MatchType;

	uint8 ServerConnected : 1;
	uint8 ServerConnectedError : 1;

	TArray<FUEServerInfo> UEServersInfo;

	UPROPERTY()
	ETeam PlayerTeam;

public:
	UTOGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION()
	void OnWebSocketConnected();
	UFUNCTION()
	void OnWebSocketConnectionError(const FString& Error);
	UFUNCTION()
	void OnWebSocketDisconnectionSuccess(int32 StatusCode, const FString& Reason, bool bWasClean);
	UFUNCTION()
	void OnWebSocketMessageReceived(const FString& Message);

	UFUNCTION()
	void ConnectToServerManager();

	UFUNCTION()
	void ConnecteToDedicatedUEServer(FString ParamHost, int32 PararmServerPort);

	UFUNCTION()
	void DisconnectWebSocket();

	void CloseDedicatedServer();

	UFUNCTION()
	void CreateNewUEServer();

	UFUNCTION()
	void GetServersList();

	UFUNCTION()
	void GetServerInfoByPort(int32 port);

	void UpdateUEServer();

	void SetPlayerTeam(ETeam ParamTeam);

protected:
	virtual void Init() override;
};
