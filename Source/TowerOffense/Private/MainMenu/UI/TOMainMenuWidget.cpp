#include "TowerOffense/Public/MainMenu/UI/TOMainMenuWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components\ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "TowerOffense/Public/TOGameInstance.h"
#include "TowerOffense/Public/MainMenu/ModeControl/TOMMPlayerController.h"

void UTOMainMenuWidget::CreateServerMenu()
{
	if (Switcher)
	{
		Switcher->SetActiveWidgetIndex(1);
	}
}

void UTOMainMenuWidget::CommitServerName(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (UTOGameInstance* GameInstance = GetGameInstance<UTOGameInstance>())
	{
		if (!bServerCreationInitialized && GameInstance->WebSocket &&
			GameInstance->WebSocket.IsValid() && GameInstance->WebSocket->IsConnected())
		{
			switch (CommitMethod)
			{
			case ETextCommit::OnEnter:
				GameInstance->ServerName = Text.ToString();
				GameInstance->CreateNewUEServer();
				bServerCreationInitialized = true;

				return;
			}
		}

		else
		{
			GameInstance->ConnectToServerManager();
		}
	}
}

void UTOMainMenuWidget::CreateJoinMenu()
{
	if (Switcher)
	{
		Switcher->SetActiveWidgetIndex(2);
	}
}

void UTOMainMenuWidget::GetServersListFromServerManager()
{
	if (UTOGameInstance* GameInstance = GetGameInstance<UTOGameInstance>())
	{
		if (GameInstance->WebSocket && GameInstance->WebSocket.IsValid() &&
			GameInstance->WebSocket->IsConnected())
		{
			GameInstance->GetServersList();

			FTimerHandle FillScrollBoxTimer;
			GetWorld()->GetTimerManager().SetTimer(
				FillScrollBoxTimer, this, &UTOMainMenuWidget::GetServersList, 4.f, false);
		}

		else
		{
			GameInstance->ConnectToServerManager();
		}
	}
}

void UTOMainMenuWidget::GetServersList()
{
	if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>())
	{
		if (ServerScrollBox->HasAnyChildren())
		{
			ServerScrollBox->ClearChildren();
		}

		for (const FUEServerInfo& UEServerInfo : TOGameInstance->UEServersInfo)
		{
			if (UEServerInfo.UEServMatchStatus < 2) // if server is in lobby stage we can see this server in servers list
			{
				if (UServerInfoSlot* ServerInfoSlot = CreateServerInfoSlot())
				{
					ServerInfoSlot->FillTextBlocks(UEServerInfo);
					ServerScrollBox->AddChild(ServerInfoSlot);
				}
			}
		}
	}
}

UServerInfoSlot* UTOMainMenuWidget::CreateServerInfoSlot()
{
	if (ServerInfoSlotClass && ServerScrollBox)
	{
		UServerInfoSlot* ServerInfoSlotWidget = CreateWidget<UServerInfoSlot>(this, ServerInfoSlotClass);
		ServerInfoSlotWidget->AddToViewport();
		ServerInfoSlotWidget->SetVisibility(ESlateVisibility::Visible);

		return ServerInfoSlotWidget;
	}

	return nullptr;
}

void UTOMainMenuWidget::BackToMainMenu()
{
	if (Switcher)
	{
		Switcher->SetActiveWidgetIndex(0);
	}
}

void UTOMainMenuWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UTOMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bServerCreationInitialized = false;

	if (MMSlideAnimation)
	{
		PlayAnimationForward(MMSlideAnimation);
	}

	CreateServerButton->OnClicked.AddDynamic(this, &UTOMainMenuWidget::CreateServerMenu);
	ServerNameTextBox->OnTextCommitted.AddDynamic(this, &UTOMainMenuWidget::CommitServerName);
	BackButtonCreate->OnClicked.AddDynamic(this, &UTOMainMenuWidget::BackToMainMenu);
	JoinServerButton->OnClicked.AddDynamic(this, &UTOMainMenuWidget::CreateJoinMenu);
	RefreshButtonJoin->OnClicked.AddDynamic(this, &UTOMainMenuWidget::GetServersListFromServerManager);
	BackButtonJoin->OnClicked.AddDynamic(this, &UTOMainMenuWidget::BackToMainMenu);

	if (UTOGameInstance* GameInstance = GetGameInstance<UTOGameInstance>())
	{
		QuitGameButton->OnClicked.AddDynamic(GameInstance, &UTOGameInstance::DisconnectWebSocket);
	}

	QuitGameButton->OnClicked.AddDynamic(this, &UTOMainMenuWidget::QuitGame);
}