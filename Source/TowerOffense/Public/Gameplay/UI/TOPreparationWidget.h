#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "TOPreparationWidget.generated.h"

class UOverlay;
class UTextBlock;

UCLASS()
class TOWEROFFENSE_API UTOPreparationWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> OverlayTextPreparation;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextPreparation;

public:
	void SetPreparationText();
	FText GetPreparationText();
};
