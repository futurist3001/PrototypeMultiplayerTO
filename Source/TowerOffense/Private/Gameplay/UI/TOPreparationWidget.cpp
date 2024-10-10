#include "TowerOffense/Public/Gameplay/UI/TOPreparationWidget.h"

#include "Components/TextBlock.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOGameStateBase.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOPlayerController.h"

void UTOPreparationWidget::SetPreparationText()
{
	TextPreparation->SetText(GetPreparationText());
}

FText UTOPreparationWidget::GetPreparationText()
{
	ATOPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ATOPlayerController>();

	if (PlayerController->HandleTime < 1.f)
	{
		return FText::FromString(TEXT("3"));
	}

	else if (PlayerController->HandleTime >= 1.f && PlayerController->HandleTime < 2.f)
	{
		return FText::FromString(TEXT("2"));
	}

	else if (PlayerController->HandleTime >= 2.f && PlayerController->HandleTime < 3.f)
	{
		return FText::FromString(TEXT("1"));
	}

	else if (PlayerController->HandleTime > 3.f)
	{
		return FText::FromString(TEXT("Play!"));
	}

	return FText();
}
