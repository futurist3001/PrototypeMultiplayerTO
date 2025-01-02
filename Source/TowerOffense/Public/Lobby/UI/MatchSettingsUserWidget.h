#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MatchSettingsUserWidget.generated.h"

class UButton;
class UComboBoxString;
class UImage;
class UTextBlock;

UCLASS()
class TOWEROFFENSE_API UMatchSettingsUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ComboBoxMaps;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ComboBoxTeams;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MapImage;

	UPROPERTY(EditAnywhere, Category = "MapImage")
	TObjectPtr<UTexture2D> TextureMap1;

	UPROPERTY(EditAnywhere, Category = "MapImage")
	TObjectPtr<UTexture2D> TextureMap2;

	UPROPERTY(EditAnywhere, Category = "MapImage")
	TObjectPtr<UTexture2D> TextureMap3;

	UPROPERTY(EditAnywhere, Category = "MapImage")
	TObjectPtr<UTexture2D> TextureMap4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateGameSessionButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseServerButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextServerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextFirstTeamNumbers;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextSecondTeamNumbers;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextThirdTeamNumbers;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextFourthTeamNumbers;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RefreshTeamNumbersButton;

public:
	UFUNCTION()
	void OnMapSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnTeamSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	void SetMapImageTexture(UTexture2D* NewTexture);

	void ChooseProperImage(FString MapName);

	UFUNCTION()
	void OnRefreshTeamNumbersButton();

protected:
	virtual void NativeConstruct() override;
	
};
