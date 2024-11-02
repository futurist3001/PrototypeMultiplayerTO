#include "TowerOffense/Public/MainMenu/ModeControl/TOMMPlayerController.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "TowerOffense/Public/MainMenu/UI/PageLevelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TowerOffense/Public/MainMenu/UI/TOMainMenuWidget.h"
#include "TowerOffense/Public/Generic/LevelSystem.h"

void ATOMMPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (MMBackSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), MMBackSound);
	}

	LimitPlayerMovement();

	CreateMainMenuWidget();
}

void ATOMMPlayerController::LimitPlayerMovement()
{
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this);

	bShowMouseCursor = true;
}

void ATOMMPlayerController::CreateMainMenuWidget()
{
	if (MainMenuWidgetClass && GetLocalRole() != ROLE_Authority)
	{
		MainMenuWidget = CreateWidget<UTOMainMenuWidget>(this, MainMenuWidgetClass);
		MainMenuWidget->AddToViewport();
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ATOMMPlayerController::CreatePageLevelWidget()
{
	if (PageLevelClass && GetLocalRole() != ROLE_Authority)
	{
		ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();

		PageLevelWidget = CreateWidget<UPageLevelWidget>(this, PageLevelClass);
		PageLevelWidget->InitializePage(10, LevelSystem->GetNumberLevels());
		PageLevelWidget->AddToViewport();
		PageLevelWidget->SetVisibility(ESlateVisibility::Visible);
		PageLevelWidget->PlayAnimationReverse(PageLevelWidget->InitializeAnimation);
	}
}
