#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOGameModeBase.h"

#include "TOWinLoseWidget.generated.h"

class UButton;
class UOverlay;
class UTextBlock;

UCLASS()
class TOWEROFFENSE_API UTOWinLoseWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> TextOverlay;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EndGameStateText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton; // change to Menu

public:
	void SetEndGameStateTextColor(EGamePhase EndGameState);

protected:
	virtual void NativeConstruct() override;
};
