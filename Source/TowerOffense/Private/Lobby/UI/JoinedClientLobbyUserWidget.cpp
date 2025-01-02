#include "Lobby/UI/JoinedClientLobbyUserWidget.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "TowerOffense/Public/TOGameInstance.h"
#include "TowerOffense/Public/Lobby/ModeControl/LobbyPlayerController.h"

void UJoinedClientLobbyUserWidget::SetMapImageTexture(UTexture2D* NewTexture)
{
	if (MapImage && NewTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(NewTexture);
		MapImage->SetBrush(Brush);
	}
}

void UJoinedClientLobbyUserWidget::ChooseProperImage(FString MapName)
{
	if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		if (MapName == "Map1")
		{
			SetMapImageTexture(TextureMap1);
		}

		else if (MapName == "Map2")
		{
			SetMapImageTexture(TextureMap2);
		}

		else if (MapName == "Map3")
		{
			SetMapImageTexture(TextureMap3);
		}

		else if (MapName == "Map4")
		{
			SetMapImageTexture(TextureMap4);
		}
	}
}

void UJoinedClientLobbyUserWidget::OnTeamSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
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

void UJoinedClientLobbyUserWidget::OnRefreshTeamNumbersButton()
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

void UJoinedClientLobbyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ComboBoxTeams->AddOption("Team1");
	ComboBoxTeams->AddOption("Team2");
	ComboBoxTeams->AddOption("Team3");
	ComboBoxTeams->AddOption("Team4");

	if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		LobbyPlayerController->Server_GetServerName();
		FTimerHandle SetServerNameTextTimer;
		GetWorld()->GetTimerManager().SetTimer(
			SetServerNameTextTimer, [this, LobbyPlayerController]()
			{
				TextServerName->SetText(FText::FromString(LobbyPlayerController->ServerName));
			}, 0.1f, false);
	}

	ComboBoxTeams->OnSelectionChanged.AddDynamic(this, &UJoinedClientLobbyUserWidget::OnTeamSelectionChanged);
	RefreshTeamNumbersButton->OnClicked.AddDynamic(this, &UJoinedClientLobbyUserWidget::OnRefreshTeamNumbersButton);

	if (ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		LeaveServerButton->OnClicked.AddDynamic(LobbyPlayerController, &ALobbyPlayerController::ReturnToMM);
	}

	if (UTOGameInstance* TOGameInstance = GetGameInstance<UTOGameInstance>())
	{
		ChooseProperImage(TOGameInstance->MapName);
	}
}
