#include "TowerOffense/Public/Gameplay/UI/TOWinLoseWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOPlayerController.h"

void UTOWinLoseWidget::SetEndGameStateTextColor(EGamePhase EndGameState)
{
	if (EndGameStateText)
	{
 		const FString RealEndGameStateString = UEnum::GetDisplayValueAsText(EndGameState).ToString();
		EndGameStateText->SetText(FText::FromString(RealEndGameStateString));

		const FColor Color = EndGameState == EGamePhase::Win ? FColor::Green : FColor::Red;
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