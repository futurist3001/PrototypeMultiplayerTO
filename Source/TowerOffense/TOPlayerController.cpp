#include "TOPlayerController.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TOPreparationWidget.h"
#include "TOScopeWidget.h"
#include "TOWinLoseWidget.h"

void ATOPlayerController::SwitchScopeVisibility()
{
	const ESlateVisibility Visibility = ScopeWidget->GetVisibility() != ESlateVisibility::Visible
		? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	ScopeWidget->SetVisibility(Visibility);
}

void ATOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreateScopeWidget();
	CreatePreparationWidget();

	auto* GameMode = Cast<ATOGameModeBase>(GetWorld()->GetAuthGameMode());
	GameMode->OnGamePhaseChanged.AddDynamic(this, &ThisClass::LimitPlayerMovement);
	GameMode->OnGamePhaseChanged.AddDynamic(this, &ThisClass::CreateWinLoseWidget);
}

void ATOPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PreparationWidget)
	{
		PreparationWidget->SetPreparationText();
	}

	if (auto* GameMode = GetWorld()->GetAuthGameMode<ATOGameModeBase>())
	{
		if (GameMode->HandleTime < 5.f && GameMode->HandleTime > 4.f)
		{
			DestroyPreparationWidget();
		}
	}
}

void ATOPlayerController::CreateScopeWidget()
{
	ScopeWidget = CreateWidget<UTOScopeWidget>(this, ScopeWidgetClass);
	ScopeWidget->AddToViewport();
	ScopeWidget->SetVisibility(ESlateVisibility::Visible);
}

void ATOPlayerController::CreatePreparationWidget()
{
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this);

	PreparationWidget = CreateWidget<UTOPreparationWidget>(this, PreparationWidgetClass);
	PreparationWidget->AddToViewport();
	PreparationWidget->SetVisibility(ESlateVisibility::Visible);
}

void ATOPlayerController::DestroyPreparationWidget()
{
	if (PreparationWidget)
	{
		auto* GameMode = GetWorld()->GetAuthGameMode<ATOGameModeBase>();

		if (GameMode->HandleTime > 4.f && PreparationWidget)
		{
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);

			PreparationWidget->RemoveFromViewport();
			PreparationWidget = nullptr;
		}
	}
}

void ATOPlayerController::LimitPlayerMovement(EGamePhase)
{
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this);

	bShowMouseCursor = true;
}

void ATOPlayerController::CreateWinLoseWidget(EGamePhase EndGameState)
{
	WinLoseWidget = CreateWidget<UTOWinLoseWidget>(this, WinLoseWidgetClass);
	WinLoseWidget->SetEndGameStateTextColor(EndGameState);
	WinLoseWidget->AddToViewport();
	WinLoseWidget->SetVisibility(ESlateVisibility::Visible);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}