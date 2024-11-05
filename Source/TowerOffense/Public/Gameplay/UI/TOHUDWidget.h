#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "TOHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class TOWEROFFENSE_API UTOHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UProgressBar> EnergyBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> FirstTeamText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> SecondTeamText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> ThirdTeamText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<UTextBlock> FourthTeamText;

public:
	void SetHealth(float CurrentHealth, float MaxHealth);
	void SetEnergy(float CurrentEnergy, float MaxEnergy);

	UFUNCTION()
	void SetFirstTeamText(int32 FirstTeamTanksRemain);

	UFUNCTION()
	void SetSecondTeamText(int32 SecondTeamTanksRemain);

	UFUNCTION()
	void SetThirdTeamText(int32 ThirdTeamTanksRemain);

	UFUNCTION()
	void SetFourthTeamText(int32 FourthTeamTanksRemain);

protected:
	virtual void NativeConstruct() override;
};
