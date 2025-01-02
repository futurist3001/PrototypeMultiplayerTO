#include "TowerOffense/Public/MainMenu/UI/ServerInfoSlot.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UServerInfoSlot::FillTextBlocks(FUEServerInfo ParamUEServerInfo)
{
	if (TextIDInfo && TextServerName && TextMapName && TextMatchType && TextPlayerInfo)
	{
		TextIDInfo->SetText(FText::AsNumber(ParamUEServerInfo.UEServID));
		TextServerName->SetText(FText::FromString(ParamUEServerInfo.UEServName));
		TextMapName->SetText(FText::FromString(ParamUEServerInfo.UEServMapName));
		TextMatchType->SetText(FText::FromString(ParamUEServerInfo.UEServMatchType));

		FString PlayerInfo = FString::Printf(
			TEXT("%d/%d"), ParamUEServerInfo.CurrentPlayers, ParamUEServerInfo.MaximumPlayers);
		TextPlayerInfo->SetText(FText::FromString(PlayerInfo));

		ServerInfo = ParamUEServerInfo;
	}
}

void UServerInfoSlot::JoinToUEServerFunc()
{
	if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>())
	{
		TOGameInstance->GetServerInfoByPort(ServerInfo.UEServPort);

		if (TOGameInstance->MatchStatus < 2) // if we are in lobby (not in game)
		{
			FTimerHandle ConnectTimer;
			GetWorld()->GetTimerManager().SetTimer(
				ConnectTimer, [this, TOGameInstance]()
				{
					if (TOGameInstance->CurrPlayers < TOGameInstance->MaxPlayers)
					{
						TOGameInstance->ConnecteToDedicatedUEServer(ServerInfo.UEServHost, ServerInfo.UEServPort);
					}
				}, 1.f, false);
		}
	}
}

void UServerInfoSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (!JoinButton->OnClicked.IsBound())
	{
		JoinButton->OnClicked.AddDynamic(this, &UServerInfoSlot::JoinToUEServerFunc);
	}
}
