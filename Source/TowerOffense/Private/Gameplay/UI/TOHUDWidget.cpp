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

void UTOHUDWidget::SetFirstTeamText(int32 FirstTeamTanksRemain)
{
	if (FirstTeamText)
	{
		FString FormattedString = FString::Printf(TEXT("First team tanks remain: %d"), FirstTeamTanksRemain);

		FirstTeamText->SetText(FText::FromString(FormattedString));
	}
}

void UTOHUDWidget::SetSecondTeamText(int32 SecondTeamTanksRemain)
{
	if (SecondTeamText)
	{
		FString FormattedString = FString::Printf(TEXT("Second team tanks remain: %d"), SecondTeamTanksRemain);

		SecondTeamText->SetText(FText::FromString(FormattedString));
	}
}

void UTOHUDWidget::SetThirdTeamText(int32 ThirdTeamTanksRemain)
{
	if (ThirdTeamText)
	{
		FString FormattedString = FString::Printf(TEXT("Third team tanks remain: %d"), ThirdTeamTanksRemain);

		ThirdTeamText->SetText(FText::FromString(FormattedString));
	}
}

void UTOHUDWidget::SetFourthTeamText(int32 FourthTeamTanksRemain)
{
	if (FourthTeamText)
	{
		FString FormattedString = FString::Printf(TEXT("Fourth team tanks remain: %d"), FourthTeamTanksRemain);

		FourthTeamText->SetText(FText::FromString(FormattedString));
	}
}

void UTOHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ATOGameStateBase* GameState = GetWorld()->GetGameState<ATOGameStateBase>())
	{
		GameState->OnFirstTeamTankDestroyed.AddDynamic(this, &UTOHUDWidget::SetFirstTeamText);
		GameState->OnSecondTeamTankDestroyed.AddDynamic(this, &UTOHUDWidget::SetSecondTeamText);
		GameState->OnThirdTeamTankDestroyed.AddDynamic(this, &UTOHUDWidget::SetThirdTeamText);
		GameState->OnFourthTeamTankDestroyed.AddDynamic(this, &UTOHUDWidget::SetFourthTeamText);

		SetFirstTeamText(GameState->FirstTeamPlayers);
		SetSecondTeamText(GameState->SecondTeamPlayers);
		SetThirdTeamText(GameState->ThirdTeamPlayers);
		SetFourthTeamText(GameState->FourthTeamPlayers);
	}
}
