#include "TowerOffense/Public/Gameplay/UI/TOHUDWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOGameStateBase.h"

void UTOHUDWidget::SetHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

void UTOHUDWidget::SetEnergy(float CurrentEnergy, float MaxEnergy)
{
	if (EnergyBar)
	{
		EnergyBar->SetPercent(CurrentEnergy / MaxEnergy);
	}
}

void UTOHUDWidget::SetEnemiesText(int32 TowersRemain)
{
	if (EnemiesTowersText)
	{
		FString FormattedString = FString::Printf(TEXT("Towers remain: %d"), TowersRemain);

		EnemiesTowersText->SetText(FText::FromString(FormattedString));
	}
}

void UTOHUDWidget::SetAlliesText(int32 TanksRemain)
{
	if (AlliesTanksText)
	{
		FString FormattedString = FString::Printf(TEXT("Tanks remain: %d"), TanksRemain);

		AlliesTanksText->SetText(FText::FromString(FormattedString));
	}
}

void UTOHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ATOGameStateBase* GameState = GetWorld()->GetGameState<ATOGameStateBase>())
	{
		GameState->OnTowerDestroyed.AddDynamic(this, &UTOHUDWidget::SetEnemiesText);
		GameState->OnTankDestroyed.AddDynamic(this, &UTOHUDWidget::SetAlliesText);

		SetEnemiesText(GameState->NumberTowersState);
		SetAlliesText(GameState->NumberTanksState);
	}
}
