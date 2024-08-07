#include "TOMMPlayerController.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "FirstBlockLevelsWidget.h"
#include "SecondBlockLevelsWidget.h"
#include "TOMainMenuWidget.h"
#include "TowerOffense/Generic/LevelSystem.h"

void ATOMMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	LimitPlayerMovement();

	CreateMainMenuWidget();
	//CreateFirstBlockLevelsWidget();
	//CreateSecondBlockLevelsWidget();
}

void ATOMMPlayerController::LimitPlayerMovement()
{
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this);

	bShowMouseCursor = true;
}

void ATOMMPlayerController::CreateMainMenuWidget()
{
	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UTOMainMenuWidget>(this, MainMenuWidgetClass);
		MainMenuWidget->AddToViewport();
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ATOMMPlayerController::CreateFirstBlockLevelsWidget()
{
	if (FirstBlockLevelsWidgetClass)
	{
		FirstBlockLevelsWidget = CreateWidget<UFirstBlockLevelsWidget>(this, FirstBlockLevelsWidgetClass);
		FirstBlockLevelsWidget->AddToViewport();
		FirstBlockLevelsWidget->SetVisibility(ESlateVisibility::Visible);

		ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();
		FirstBlockLevelsWidget->OnPressedFirstBlockButton.AddDynamic(LevelSystem, &ULevelSystem::OpenRelativeLevel);
	}
}

void ATOMMPlayerController::CreateSecondBlockLevelsWidget()
{
	if (SecondBlockLevelsWidgetClass)
	{
		SecondBlockLevelsWidget = CreateWidget<USecondBlockLevelsWidget>(this, SecondBlockLevelsWidgetClass);
		SecondBlockLevelsWidget->AddToViewport();
		SecondBlockLevelsWidget->SetVisibility(ESlateVisibility::Visible);

		ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();
		SecondBlockLevelsWidget->OnPressedSecondBlockButton.AddDynamic(LevelSystem, &ULevelSystem::OpenRelativeLevel);
	}
}
