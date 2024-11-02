#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "TOMainMenuWidget.generated.h"

class UButton;
class UOverlay;
class UTextBlock;

UCLASS()
class TOWEROFFENSE_API UTOMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextStartGame;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LevelButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResetLevelAccessButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextQuitGame;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> OverlayGameNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GameNameText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> MMSlideAnimation;

public:
	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void QuitGame();

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void DestroyMMWidget();
};
