#include "TowerOffense/Public/MainMenu/TOGameInstance.h"

#include "Engine/World.h"
#include "Misc/DefaultValueHelper.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "TowerOffense/Public/MainMenu/ModeControl/TOMMPlayerController.h"

UTOGameInstance::UTOGameInstance(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	ServerConnected = false;
	ServerConnectedError = false;
	MaxPlayers = 12;
	CurrPlayers = 0;
	MatchStatus = 1; // 0 - Entrace, 1 - Lobby, 2 - PickStage, 3 - Running, 4 - Finished
	ServerID = 0;

	MapName = "DefaultMapName";
	ServerName = "DefaultServerName";
	MatchType = "RegularMatch";
}

void UTOGameInstance::OnWebSocketConnected()
{
	if (!WebSocket->IsConnected())
	{
		return;
	}

	ServerConnected = true;
}

void UTOGameInstance::OnWebSocketConnectionError(const FString& Error)
{
	ServerConnectedError = true;
}

void UTOGameInstance::OnWebSocketDisconnectionSuccess(
	int32 StatusCode, const FString& Reason, bool bWasClean)
{
}

void UTOGameInstance::OnWebSocketMessageReceived(const FString& Message)
{
	TSharedPtr<FJsonObject> json;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

	if (FJsonSerializer::Deserialize(Reader, json))
	{
		FString ID = json->GetStringField("Id");
		ServerID = FCString::Atoi(*ID);
	}
}

void UTOGameInstance::ConnectToServer()
{
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) // it`s related with build.cs file
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}

	WebSocket = FWebSocketsModule::Get().CreateWebSocket("ws://127.0.0.1:3030");

	WebSocket->OnConnected().AddUFunction(this, FName("OnWebSocketConnected"));
	WebSocket->OnConnectionError().AddUFunction(this, FName("OnWebSocketConnectionError"));
	WebSocket->OnClosed().AddUFunction(this, FName("OnWebSocketDisconnectionSuccess"));
	WebSocket->OnMessage().AddUFunction(this, FName("OnWebSocketMessageReceived"));

	WebSocket->Connect();
}

void UTOGameInstance::DisconnectWebSocket()
{
	if (WebSocket)
	{
		if (WebSocket->IsConnected())
		{

			//CloseDedicatedServer();
			WebSocket->Close();
		}
	}
}

void UTOGameInstance::CloseDedicatedServer() // only from server side
{
	if (WebSocket && WebSocket->IsConnected())
	{
		FString Request = FString::Printf(
				TEXT("{\"Action\":\"StopServerByID\",\"ServerID\":\"%d\"}"),
				ServerID);

		WebSocket->Send(Request);
	}
}

void UTOGameInstance::CreateNewServer()
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		FString Request = FString::Printf(
				TEXT("{\"Action\":\"ReqestServer\",\"ServerName\":\"%s\"}"),
				*ServerName);

		WebSocket->Send(Request); // request for creating separate UEServer
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WebSocket is not connected!"));
	}
}

void UTOGameInstance::UpdateUEServer()
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		FString Update = FString::Printf(
				TEXT("{\"Update\":\"HeartBeat\",\"MatchStatus\":\"%d\",\"MatchType\":\"%s\",\"MapName\":\"%s\",\"CurrPlayers\":\"%d\",\"MaxPlayers\":\"%d\",\"ServerID\":\"%d\"}"),
				MatchStatus, *MatchType, *MapName, CurrPlayers, MaxPlayers, ServerID);

		WebSocket->Send(Update); // Update info
	}
}

void UTOGameInstance::Init()
{
	Super::Init();

	ConnectToServer();

	FTimerHandle UpdateTimer;
	GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, &ThisClass::UpdateUEServer, 2.f, true, 3.0f);

#if UE_SERVER
	UE_LOG(LogTemp, Error, TEXT("This is a server build."));
#endif

	/*if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UTOGameInstance::OnCreateSessionComplete);
		}
	}*/
}

void UTOGameInstance::StartCreateServer() // old and delete later
{
	/*if (CreateServerPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Start Create Server"));

		FOnlineSessionSettings SessionSettings;

		SessionSettings.bIsDedicated = true;
		SessionSettings.bAllowJoinInProgress = false;
		SessionSettings.bIsLANMatch = true;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.NumPublicConnections = 12;

		SessionInterface->CreateSession(0, "My Session", SessionSettings);
	}*/
}

void UTOGameInstance::StartJoinServer()
{
}

void UTOGameInstance::OnCreateSessionComplete(FName ServerNameParam, bool Succeeded)
{
	/*UE_LOG(LogTemp, Warning, TEXT("On create session complete, Succeeded: %d"), Succeeded);

	if (Succeeded && IsDedicatedServerInstance())
	{// in case if server migrate to another level
		GetWorld()->ServerTravel("/Game/TowerOffense/TowerOffenseLevel/Level_0");
	}

	else if (CreateServerPlayer) // true if this client pressed button to create server
	{// in case if it`s client that pressed "create"
		FString ServerAddress;
		if (SessionInterface->GetResolvedConnectString(ServerName, ServerAddress))
		{
			CreateServerPlayer->ClientTravel(ServerAddress, ETravelType::TRAVEL_Absolute);
		}
	}*/
}