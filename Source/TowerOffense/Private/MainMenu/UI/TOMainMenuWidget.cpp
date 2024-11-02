#include "TowerOffense/Public/MainMenu/UI/TOMainMenuWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TowerOffense/Public/MainMenu/ModeControl/TOMMPlayerController.h"
#include "TowerOffense/Public/Generic/LevelSystem.h"

void UTOMainMenuWidget::StartGame()
{
	ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();
	LevelSystem->OpenRelativeLevel(GetWorld(), 1);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetWorld()->GetFirstPlayerController());
}

void UTOMainMenuWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}

void UTOMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MMSlideAnimation)
	{
		PlayAnimationForward(MMSlideAnimation);
	}

	auto* PlayerController = GetWorld()->GetFirstPlayerController<ATOMMPlayerController>();
	ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();

	StartGameButton->OnClicked.AddDynamic(this, &UTOMainMenuWidget::StartGame);
	LevelButton->OnClicked.AddDynamic(PlayerController, &ATOMMPlayerController::CreatePageLevelWidget);
	LevelButton->OnClicked.AddDynamic(this, &UTOMainMenuWidget::DestroyMMWidget);
	ResetLevelAccessButton->OnClicked.AddDynamic(LevelSystem, &ULevelSystem::ResetLevelAccess);
	QuitGameButton->OnClicked.AddDynamic(this, &UTOMainMenuWidget::QuitGame);
}

void UTOMainMenuWidget::DestroyMMWidget()
{
	RemoveFromParent();
}
