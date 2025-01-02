#include "TowerOffense/Public/Lobby/UI/MatchSettingsUserWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "TowerOffense/Public/Lobby/ModeControl/LobbyPlayerController.h"
#include "TowerOffense/Public/TOGameInstance.h"

void UMatchSettingsUserWidget::OnMapSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::OnMouseClick)
	{
		ChooseProperImage(SelectedItem);
	}
}

void UMatchSettingsUserWidget::OnTeamSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::OnMouseClick)
	{
		if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>();
			ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetOwningPlayer()))
		{
			if (SelectedItem == "Team1")
			{
				TOGameInstance->SetPlayerTeam(ETeam::Team1);
				LobbyPlayerController->SetLobbyTeam(ETeam::Team1);
			}

			else if (SelectedItem == "Team2")
			{
				TOGameInstance->SetPlayerTeam(ETeam::Team2);
				LobbyPlayerController->SetLobbyTeam(ETeam::Team2);
			}

			else if (SelectedItem == "Team3")
			{
				TOGameInstance->SetPlayerTeam(ETeam::Team3);
				LobbyPlayerController->SetLobbyTeam(ETeam::Team3);
			}

			else if (SelectedItem == "Team4")
			{
				TOGameInstance->SetPlayerTeam(ETeam::Team4);
				LobbyPlayerController->SetLobbyTeam(ETeam::Team4);
			}
		}
	}
}

void UMatchSettingsUserWidget::ChooseProperImage(FString MapName)
{
	if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		if (MapName == "Map1")
		{
			SetMapImageTexture(TextureMap1);

			LobbyPlayerController->SyncClientMatchSettingsPreferences(MapName);
		}

		else if (MapName == "Map2")
		{
			SetMapImageTexture(TextureMap2);

			LobbyPlayerController->SyncClientMatchSettingsPreferences(MapName);
		}

		else if (MapName == "Map3")
		{
			SetMapImageTexture(TextureMap3);

			LobbyPlayerController->SyncClientMatchSettingsPreferences(MapName);
		}

		else if (MapName == "Map4")
		{
			SetMapImageTexture(TextureMap4);

			LobbyPlayerController->SyncClientMatchSettingsPreferences(MapName);
		}
	}
}

void UMatchSettingsUserWidget::OnRefreshTeamNumbersButton()
{
	if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		LobbyPlayerController->Server_GetTeamMembersInfo();

		FTimerHandle SetTeamMembersInfoTimer;
		GetWorld()->GetTimerManager().SetTimer(
			SetTeamMembersInfoTimer, [this, LobbyPlayerController]()
			{
				TextFirstTeamNumbers->SetText(FText::AsNumber(LobbyPlayerController->TeamMembersInfo.FirstTeamNumbers));
				TextSecondTeamNumbers->SetText(FText::AsNumber(LobbyPlayerController->TeamMembersInfo.SecondTeamNumbers));
				TextThirdTeamNumbers->SetText(FText::AsNumber(LobbyPlayerController->TeamMembersInfo.ThirdTeamNumbers));
				TextFourthTeamNumbers->SetText(FText::AsNumber(LobbyPlayerController->TeamMembersInfo.FourthTeamNumbers));
			}, 0.1f, false);
	}
}

void UMatchSettingsUserWidget::SetMapImageTexture(UTexture2D* NewTexture)
{
	if (MapImage && NewTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(NewTexture);
		MapImage->SetBrush(Brush);
	}
}

void UMatchSettingsUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ComboBoxMaps->AddOption("Map1");
	ComboBoxMaps->AddOption("Map2");
	ComboBoxMaps->AddOption("Map3");
	ComboBoxMaps->AddOption("Map4");

	ComboBoxTeams->AddOption("Team1");
	ComboBoxTeams->AddOption("Team2");
	ComboBoxTeams->AddOption("Team3");
	ComboBoxTeams->AddOption("Team4");

	ComboBoxMaps->OnSelectionChanged.AddDynamic(this, &UMatchSettingsUserWidget::OnMapSelectionChanged);
	ComboBoxTeams->OnSelectionChanged.AddDynamic(this, &UMatchSettingsUserWidget::OnTeamSelectionChanged);
	RefreshTeamNumbersButton->OnClicked.AddDynamic(this, &UMatchSettingsUserWidget::OnRefreshTeamNumbersButton);

	if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>())
	{
		TextServerName->SetText(FText::FromString(TOGameInstance->ServerName));
	}

	if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		CreateGameSessionButton->OnClicked.AddDynamic(LobbyPlayerController, &ALobbyPlayerController::CreateSessionGame);
		CloseServerButton->OnClicked.AddDynamic(LobbyPlayerController, &ALobbyPlayerController::Server_InitializeClientsReturnToMM);
	}
}
