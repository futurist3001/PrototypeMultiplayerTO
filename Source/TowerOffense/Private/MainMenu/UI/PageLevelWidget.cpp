#include "TowerOffense/Public/MainMenu/UI/PageLevelWidget.h"

#include "TowerOffense/Public/MainMenu/UI/ButtonLevelWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "TowerOffense/Public/MainMenu/ModeControl/TOMMPlayerController.h"
#include "TowerOffense/Public/Generic/LevelSystem.h"

void UPageLevelWidget::InitializePage(int32 MaxButtonsPerPage, int32 TotalLevels)
{
	TotalPages = FMath::CeilToInt(
		static_cast<float>(TotalLevels) / MaxButtonsPerPage); // calculate number of pages
	CurrentPage = 0;

	CreateButtons(0, FMath::Min(MaxButtonsPerPage, TotalLevels)); // fisrt page;
}

void UPageLevelWidget::NextPage()
{
	if (CurrentPage < TotalPages - 1)
	{
		CurrentPage++;

		if (SecondPageSlideAnimation)
		{
			PlayAnimationReverse(SecondPageSlideAnimation);
		}

		UpdatePageButtons();
	}
}

void UPageLevelWidget::PreviousPage()
{
	if (CurrentPage > 0)
	{
		CurrentPage--;

		if (FirstPageSlideAnimation)
		{
			PlayAnimationReverse(FirstPageSlideAnimation);
		}

		UpdatePageButtons();
	}
}

void UPageLevelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ATOMMPlayerController* PlayerController = Cast<ATOMMPlayerController>(GetOwningPlayer());

	CopyVerticalBox = VerticalBox;

	NextButton->OnClicked.AddDynamic(this, &UPageLevelWidget::NextPage);
	PreviousButton->OnClicked.AddDynamic(this, &UPageLevelWidget::PreviousPage);
	HomeButton->OnClicked.AddDynamic(PlayerController, &ATOMMPlayerController::CreateMainMenuWidget);
	HomeButton->OnClicked.AddDynamic(this, &UPageLevelWidget::DestroyPageLevelWidget);
}

void UPageLevelWidget::CreateButtons(int32 StartIndex, int32 EndIndex)
{
	if (ButtonLevelWidgetClass)
	{
		ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();

		UHorizontalBox* UpperHorizontalBox = NewObject<UHorizontalBox>(this);
		UHorizontalBox* LowerHorizontalBox = NewObject<UHorizontalBox>(this);

		for (int32 i = StartIndex; i < EndIndex; i++)
		{
			if (i >= StartIndex && i < EndIndex - 5 && UpperHorizontalBox) // subtract 5 because in every row 5 buttons
			{
				UButtonLevelWidget* LevelButton =
					CreateWidget<UButtonLevelWidget>(this, ButtonLevelWidgetClass);
				LevelButton->InitializeButton(i + 1);

				LevelButton->OnLevelSelected.AddDynamic(
					this, &UPageLevelWidget::OnLevelSelected);

				UpperHorizontalBox->AddChildToHorizontalBox(LevelButton);
				VerticalBox->AddChildToVerticalBox(UpperHorizontalBox);

				if (LevelSystem->Levels[LevelButton->GetCurrentLevelIndex() - 1] == true) // we check which level is unlocked and highlight it with relative color
				{
					LevelButton->SetColorAndOpacity(FColor::Green);
				}
		
			}

			else if (i >= EndIndex - 5 && i < EndIndex && LowerHorizontalBox) // subtract 5 because in every row 5 buttons
			{
				UButtonLevelWidget* LevelButton =
					CreateWidget<UButtonLevelWidget>(this, ButtonLevelWidgetClass);
				LevelButton->InitializeButton(i + 1);

				LevelButton->OnLevelSelected.AddDynamic(
					this, &UPageLevelWidget::OnLevelSelected);

				LowerHorizontalBox->AddChildToHorizontalBox(LevelButton);
				VerticalBox->AddChildToVerticalBox(LowerHorizontalBox);

				if (LevelSystem->Levels[LevelButton->GetCurrentLevelIndex() - 1] == true) // we check which level is unlocked and highlight it with relative color
				{
					LevelButton->SetColorAndOpacity(FColor::Green);
				}
			}
		}
	}
}

void UPageLevelWidget::UpdatePageButtons()
{
	ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();

	int32 StartIndex = CurrentPage * 10;
	int32 EndIndex = FMath::Min(StartIndex + 10, LevelSystem->GetNumberLevels());

	VerticalBox->ClearChildren();

	VerticalBox = CopyVerticalBox;

	CreateButtons(StartIndex, EndIndex);
}

void UPageLevelWidget::OnLevelSelected(int32 LevelIndex)
{
	ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();
	LevelSystem->OpenRelativeLevel(GetWorld(), LevelIndex);
}

void UPageLevelWidget::DestroyPageLevelWidget()
{
	RemoveFromParent();
}
