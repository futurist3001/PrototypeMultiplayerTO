#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOGameStateBase.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOGameModeBase.h"

#include "TOPlayerController.generated.h"

class UTOHUDWidget;
class UTOPreparationWidget;
class UTOScopeWidget;
class UTOWinLoseWidget;

UCLASS()
class TOWEROFFENSE_API ATOPlayerController : public APlayerController 
{
	GENERATED_BODY()
	
public:
	float HandleTime; // For delay preparation
	ETeam PlayerTeam;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UTOWinLoseWidget> WinLoseWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UTOScopeWidget> ScopeWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UTOPreparationWidget> PreparationWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UTOHUDWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> YouAreDeadWidgetClass;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UTOWinLoseWidget> WinLoseWidget;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UTOScopeWidget> ScopeWidget;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UTOPreparationWidget> PreparationWidget;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UTOHUDWidget> HUDWidget;

	UPROPERTY(Transient, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UUserWidget> YouAreDeadWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	TObjectPtr<USoundBase> GameBackMusic;

private:
	FTimerHandle TimerHandle;

public:
	UFUNCTION(BlueprintCallable)
	void SwitchScopeVisibility();

	UFUNCTION()
	void Restart();

	UFUNCTION()
	void ReturnToMainMenu();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void CreateScopeWidget();

	void CreatePreparationWidget();
	void DestroyPreparationWidget();

	void CreateHUDWidget();
	void UpdateHUDEnergy();

	UFUNCTION()
	void UpdateHUDHealth(
		AActor* HealthKeeper, UTOHealthComponent* ParameterHealthComponent);

private:
	UFUNCTION(Server, reliable)
	void Server_ChangeGamePhase();

	UFUNCTION()
	void CreateDeadWidget(AActor* HealthKeeper, UTOHealthComponent* ParameterHealthComponent);

	UFUNCTION()
	void LimitPlayerMovement();

	UFUNCTION()
	void CreateWinLoseWidget(EGamePhase EndGamePhase);
	void LogicCreateWinLoseWidget(EGamePhase EndGamePhase);
};
