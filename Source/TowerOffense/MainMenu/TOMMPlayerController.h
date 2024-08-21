#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "TOMMPlayerController.generated.h"

class UPageLevelWidget;
class UTOMainMenuWidget;

UCLASS()
class TOWEROFFENSE_API ATOMMPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Levels")
	TSubclassOf<UPageLevelWidget> PageLevelClass;
	
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Levels")
	TObjectPtr<UPageLevelWidget> PageLevelWidget;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UTOMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UTOMainMenuWidget> MainMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	TObjectPtr<USoundBase> MMBackSound;

public:
	UFUNCTION()
	void CreatePageLevelWidget();

	UFUNCTION()
	void CreateMainMenuWidget();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void LimitPlayerMovement();
};
