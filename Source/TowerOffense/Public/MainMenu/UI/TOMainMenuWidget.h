#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "TOMainMenuWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class TOWEROFFENSE_API UTOMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateServerButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinServerButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GameNameText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> MMSlideAnimation;

public:
	UFUNCTION()
	void QuitGame();

	UFUNCTION()
	void SetCreateServerPlayer();

protected:
	virtual void NativeConstruct() override;
};
