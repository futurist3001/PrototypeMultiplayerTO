#include "TowerOffense/Public/TOGameInstance.h"

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
	MaxPlayers = 0;
	CurrPlayers = 0;
	MatchStatus = 1; // 0 - Entrace, 1 - Lobby, 2 - PickStage, 3 - Running, 4 - Finished
	ServerID = 0;
	ServerPort = 0;

	Host = "0.0.0.0";
	MapName = "DefaultMapName";
	ServerName = "DefaultServerName";
	MatchType = "RegularMatch";

	PlayerTeam = ETeam::Team1;
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
	if (Message.StartsWith(TEXT("[")) && Message.EndsWith(TEXT("]")))
	{
		if (!UEServersInfo.IsEmpty())
		{
			UEServersInfo.Reset();
		}

		TArray<TSharedPtr<FJsonValue>> JsonArray;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

		if (FJsonSerializer::Deserialize(Reader, JsonArray))
		{
			for (const TSharedPtr<FJsonValue>& JsonValue : JsonArray)
			{
				if (TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject())
				{
					if (JsonObject.IsValid())
					{
						FUEServerInfo UEServerInfo;
						UEServerInfo.UEServID = JsonObject->GetIntegerField(TEXT("Id"));
						UEServerInfo.UEServName = JsonObject->GetStringField(TEXT("serverName"));
						UEServerInfo.UEServPort = JsonObject->GetIntegerField(TEXT("port"));
						UEServerInfo.UEServHost = JsonObject->GetStringField(TEXT("host"));
						UEServerInfo.UEServMatchStatus = JsonObject->GetIntegerField(TEXT("matchStatus"));
						UEServerInfo.UEServMatchType = JsonObject->GetStringField(TEXT("matchType"));
						UEServerInfo.UEServMapName = JsonObject->GetStringField(TEXT("mapName"));
						UEServerInfo.CurrentPlayers = JsonObject->GetIntegerField(TEXT("currentPlayers"));
						UEServerInfo.MaximumPlayers = JsonObject->GetIntegerField(TEXT("maxPlayers"));

						UEServersInfo.Add(UEServerInfo);
					}
				}
			}
		}
	}

	else
	{
		TSharedPtr<FJsonObject> json;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

		if (FJsonSerializer::Deserialize(Reader, json))
		{
			FString ID = json->GetStringField("Id");
			ServerID = FCString::Atoi(*ID);

			FString ServName = json->GetStringField("serverName");
			ServerName = ServName;

			FString ServPort = json->GetStringField("port");
			ServerPort = FCString::Atoi(*ServPort);

			FString ServHost = json->GetStringField("host");
			Host = ServHost;

			FString ServMatchStatus = json->GetStringField("matchStatus");
			MatchStatus = FCString::Atoi(*ServMatchStatus);

			FString ServMatchType = json->GetStringField("matchType");
			MatchType = ServMatchType;

			FString GameMapName = json->GetStringField("mapName");
			MapName = GameMapName;

			FString ServCurrPlayers = json->GetStringField("currentPlayers");
			CurrPlayers = FCString::Atoi(*ServCurrPlayers);

			FString ServMaxPlayers = json->GetStringField("maxPlayers");
			MaxPlayers = FCString::Atoi(*ServMaxPlayers);
		}
	}
}

void UTOGameInstance::ConnectToServerManager()
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

void UTOGameInstance::ConnecteToDedicatedUEServer(FString ParamHost, int32 PararmServerPort)
{
	if (WebSocket && WebSocket->IsConnected())
	{
		if (!Host.IsEmpty() && PararmServerPort > 0)
		{
			FString ServerAdress = FString::Printf(TEXT("%s:%d"), *ParamHost, PararmServerPort);

			if (UWorld* World = GetWorld())
			{
				World->GetFirstPlayerController()->ClientTravel(ServerAdress, ETravelType::TRAVEL_Absolute);
			}
		}

		else
		{
			UE_LOG(LogTemp, Error, TEXT("Server adress or port is invalid!"));
		}
	}
}

void UTOGameInstance::DisconnectWebSocket()
{
	if (WebSocket)
	{
		if (WebSocket->IsConnected())
		{
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

void UTOGameInstance::CreateNewUEServer()
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		FString Request = FString::Printf(
				TEXT("{\"Action\":\"ReqestServer\",\"ServerName\":\"%s\"}"),
				*ServerName);

		WebSocket->Send(Request); // request for creating separate UEServer

		FTimerHandle JoinToCreatedServerTimer;
		GetWorld()->GetTimerManager().SetTimer(
			JoinToCreatedServerTimer, [this]()
			{
				ConnecteToDedicatedUEServer(Host, ServerPort);
			}, 4.f, false);
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WebSocket is not connected!"));
	}
}

void UTOGameInstance::GetServersList()
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		FString Request = FString::Printf(
			TEXT("{\"Action\":\"GetServersList\"}"));

		WebSocket->Send(Request);
	}
}

void UTOGameInstance::GetServerInfoByPort(int32 port)
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		FString Request = FString::Printf(
			TEXT("{\"Action\":\"ReqestServerInfoByPort\",\"ServerPort\":\"%d\"}"),
			port);

		UE_LOG(LogTemp, Error, TEXT("Port: %d"), port);

		WebSocket->Send(Request);
	}
}

void UTOGameInstance::UpdateUEServer()
{
	if (WebSocket.IsValid() && WebSocket->IsConnected())
	{
		FString Update = FString::Printf(
				TEXT("{\"Update\":\"HeartBeat\",\"MatchStatus\":\"%d\",\"MatchType\":\"%s\",\"MapName\":\"%s\",\"MaxPlayers\":\"%d\",\"ServerID\":\"%d\",\"CurrPlayers\":\"%d\"}"),
				MatchStatus, *MatchType, *MapName, MaxPlayers, ServerID, CurrPlayers);
			
		WebSocket->Send(Update); // Update info
	}
}

void UTOGameInstance::SetPlayerTeam(ETeam ParamTeam)
{
	PlayerTeam = ParamTeam;
}

void UTOGameInstance::Init()
{
	Super::Init();

	ConnectToServerManager();
}