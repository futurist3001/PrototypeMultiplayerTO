#include "TowerOffense/Public/MainMenu/TOGameInstance.h"

#include "Engine/World.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "TowerOffense/Public/MainMenu/ModeControl/TOMMPlayerController.h"

UTOGameInstance::UTOGameInstance(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UTOGameInstance::Init()
{
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UTOGameInstance::OnCreateSessionComplete);
		}
	}
}

void UTOGameInstance::StartCreateServer()
{
	if (CreateServerPlayer)
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
	}
}

void UTOGameInstance::StartJoinServer()
{
}

void UTOGameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("On create session complete, Succeeded: %d"), Succeeded);

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
	}
}
