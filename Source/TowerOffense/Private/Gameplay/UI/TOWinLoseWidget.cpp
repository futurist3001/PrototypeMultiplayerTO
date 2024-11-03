#include "TowerOffense/Public/Gameplay/UI/TOWinLoseWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOPlayerController.h"

void UTOWinLoseWidget::SetEndGameStateTextColor(EGamePhase EndGamePhase)
{
	if (EndGameStateText)
	{
 		const FString RealEndGameStateString = UEnum::GetDisplayValueAsText(EndGamePhase).ToString();
		EndGameStateText->SetText(FText::FromString(RealEndGameStateString));

		FColor Color;

		if (EndGamePhase == EGamePhase::FirstTeamWin || EndGamePhase == EGamePhase::SecondTeamWin ||
			EndGamePhase == EGamePhase::ThirdTeamWin || EndGamePhase == EGamePhase::FourthTeamWin)
		{
			Color = FColor::Green;
		}

		else if (EndGamePhase == EGamePhase::FirstTeamLose || EndGamePhase == EGamePhase::SecondTeamLose ||
			EndGamePhase == EGamePhase::ThirdTeamLose || EndGamePhase == EGamePhase::FourthTeamLose)
		{
			Color = FColor::Red;
		}

		EndGameStateText->SetColorAndOpacity(FSlateColor(Color));
	}
}

void UTOWinLoseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ATOPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ATOPlayerController>();

	RestartButton->OnClicked.AddDynamic(PlayerController, &ATOPlayerController::Restart);
	QuitButton->OnClicked.AddDynamic(PlayerController, &ATOPlayerController::ReturnToMainMenu);
}