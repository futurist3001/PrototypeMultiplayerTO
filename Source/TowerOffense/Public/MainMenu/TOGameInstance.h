#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "IWebSocket.h"
#include "IWebSocketsManager.h"
#include "WebSocketsModule.h"

#include "TOGameInstance.generated.h"

UCLASS()
class TOWEROFFENSE_API UTOGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	TSharedPtr<IWebSocket> WebSocket;

	IOnlineSessionPtr SessionInterface;
	class ATOMMPlayerController* CreateServerPlayer;

	int32 MaxPlayers;
	int32 CurrPlayers;
	int32 MatchStatus;
	int32 ServerID;

	FString ServerName;
	FString MapName;
	FString MatchType;

	uint8 ServerConnected : 1;
	uint8 ServerConnectedError : 1;

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
	void ConnectToServer();

	UFUNCTION()
	void DisconnectWebSocket();

	void CloseDedicatedServer();

	UFUNCTION()
	void CreateNewServer();

	void UpdateUEServer();

	UFUNCTION()
	void StartCreateServer(); // old

	UFUNCTION()
	void StartJoinServer(); // old

protected:
	virtual void Init() override;
	virtual void OnCreateSessionComplete(FName ServerNameParam, bool Succeeded); // old
};
