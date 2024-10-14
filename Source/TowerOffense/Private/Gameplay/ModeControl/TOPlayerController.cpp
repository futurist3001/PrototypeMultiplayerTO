#include "TowerOffense/Public/Gameplay/ModeControl/TOPlayerController.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "TowerOffense/Public/Gameplay/Pawn/TankPawn.h"
#include "TowerOffense/Public/Gameplay/UI/TOHUDWidget.h"
#include "TowerOffense/Public/Gameplay/UI/TOPreparationWidget.h"
#include "TowerOffense/Public/Gameplay/UI/TOScopeWidget.h"
#include "TowerOffense/Public/Gameplay/UI/TOWinLoseWidget.h"
#include "TowerOffense/Public/Generic/LevelSystem.h"

void ATOPlayerController::SwitchScopeVisibility()
{
	const ESlateVisibility Visibility = ScopeWidget->GetVisibility() != ESlateVisibility::Visible
		? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	ScopeWidget->SetVisibility(Visibility);
}

void ATOPlayerController::Restart()
{
	ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();
	LevelSystem->OpenRelativeLevel(GetWorld(), LevelSystem->ActualCurrentLevel);

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetWorld()->GetFirstPlayerController());

	ATOGameStateBase* GameState = GetWorld()->GetGameState<ATOGameStateBase>();

	if (GameState)
	{
		GameState->SetGamePhase(EGamePhase::Preparation);
	}
}

void ATOPlayerController::ReturnToMainMenu()
{
	ATOGameStateBase* GameState = GetWorld()->GetGameState<ATOGameStateBase>();
	UGameplayStatics::OpenLevel(GetWorld(), GameState->MainMenuMapName, true);
}

void ATOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GameBackMusic)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), GameBackMusic);
	}

	if (IsLocalController())
	{
		CreateScopeWidget();
		CreatePreparationWidget();
		CreateHUDWidget();

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATOPlayerController::DestroyPreparationWidget, 4.f, false);

		if (ATOGameStateBase* GameState = GetWorld()->GetGameState<ATOGameStateBase>())
		{
			GameState->OnGamePhaseChanged.AddDynamic(this, &ThisClass::CreateWinLoseWidget);
		}
	}

	if (ATankPawn* TankPawn = GetPawn<ATankPawn>(); TankPawn && HUDWidget)
	{
		TankPawn->HealthComponent->HealthChanged.AddDynamic(this, &ATOPlayerController::UpdateHUDHealth);
		TankPawn->HealthComponent->HealthChanged.AddDynamic(this, &ATOPlayerController::CreateDeadWidget);
	}

	FTimerHandle TimerChangePhase;
	GetWorldTimerManager().SetTimer(
		TimerChangePhase, this, &ATOPlayerController::Server_ChangeGamePhase, 3.10f, false);
}

void ATOPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleTime += DeltaTime;

	if (PreparationWidget)
	{
		PreparationWidget->SetPreparationText();
	}

	if (HUDWidget)
	{
		UpdateHUDEnergy();
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
		ATOGameStateBase* GameState = GetWorld()->GetGameState<ATOGameStateBase>();

		if (PreparationWidget && GameState->GetGamePhase() == EGamePhase::Playing)
		{
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);

			PreparationWidget->RemoveFromParent();
			PreparationWidget = nullptr;
		}
	}
}

void ATOPlayerController::CreateHUDWidget()
{
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UTOHUDWidget>(this, HUDWidgetClass);
		HUDWidget->AddToViewport();
		HUDWidget->HealthBar->SetVisibility(ESlateVisibility::Hidden);
		HUDWidget->EnergyBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void ATOPlayerController::UpdateHUDEnergy()
{
	if (HUDWidget)
	{
		if (APawn* ControllerPawn = GetPawn())
		{
			if (ATankPawn* TankPawn = Cast<ATankPawn>(ControllerPawn))
			{
				HUDWidget->SetEnergy(TankPawn->GetCurrentEnergy(), TankPawn->GetMaxEnergy());

				TankPawn->IsOldShoot() ? HUDWidget->EnergyBar->SetVisibility(ESlateVisibility::Hidden) :
					HUDWidget->EnergyBar->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void ATOPlayerController::UpdateHUDHealth(AActor* HealthKeeper, UTOHealthComponent* ParameterHealthComponent)
{
	if (HUDWidget && HealthKeeper)
	{
		HUDWidget->SetHealth(
			ParameterHealthComponent->Health, ParameterHealthComponent->DefaultHealth);

		if (HUDWidget->HealthBar->GetVisibility() != ESlateVisibility::Visible)
		{
			HUDWidget->HealthBar->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ATOPlayerController::Server_ChangeGamePhase_Implementation()
{
	if (HandleTime > 3.f)
	{
		ATOGameStateBase* GameState = GetWorld()->GetGameState<ATOGameStateBase>();

		if (GameState)
		{
			GameState->SetGamePhase(EGamePhase::Playing);
		}
	}
}

void ATOPlayerController::CreateDeadWidget(AActor* HealthKeeper, UTOHealthComponent* ParameterHealthComponent)
{
	if (ParameterHealthComponent->Health <= 0.0f)
	{
		if (HUDWidget)
		{
			HUDWidget->RemoveFromParent();
			HUDWidget = nullptr;
		}

		YouAreDeadWidget = CreateWidget<UUserWidget>(this, YouAreDeadWidgetClass);
		YouAreDeadWidget->AddToViewport();
		YouAreDeadWidget->SetVisibility(ESlateVisibility::Visible);
		LimitPlayerMovement();
	}
}

void ATOPlayerController::LimitPlayerMovement()
{
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(this);

	bShowMouseCursor = true;
}

void ATOPlayerController::CreateWinLoseWidget(EGamePhase EndGamePhase)
{
	if ((GetWorld() && !GetWorld()->bIsTearingDown && GetLocalRole() != ROLE_Authority)
		&& (EndGamePhase == EGamePhase::Win || EndGamePhase == EGamePhase::Lose))
	{
		if (YouAreDeadWidget)
		{
			YouAreDeadWidget->RemoveFromParent();
			YouAreDeadWidget = nullptr;
		}

		if (HUDWidget)
		{
			HUDWidget->RemoveFromParent();
			HUDWidget = nullptr;
		}

		WinLoseWidget = CreateWidget<UTOWinLoseWidget>(this, WinLoseWidgetClass);
		WinLoseWidget->SetEndGameStateTextColor(EndGamePhase);
		WinLoseWidget->AddToViewport();
		WinLoseWidget->SetVisibility(ESlateVisibility::Visible);
		UGameplayStatics::SetGamePaused(GetWorld(), true); // not sure that this func executes
		LimitPlayerMovement();
	}
}
