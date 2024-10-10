#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOGameStateBase.h"

#include "TOGameModeBase.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	Team1 UMETA(DisplayName = "Team 1"),
	Team2 UMETA(DisplayName = "Team 2")
};

class USoundBase;

UCLASS()
class TOWEROFFENSE_API ATOGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	
protected:
	
private:
	int32 NumberTowers;
	int32 NumberTanks;
	uint8 bIsGameStarted : 1;

public:
	ATOGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(Exec, Category = "Levels")
	void OpenRelativeLevelCC(int32 LevelIndex) const; // Console command

	void GameStarted();

protected:
	virtual void BeginPlay() override;

	void InitPlayData();

	void Win();
	void Lose();

	void SetEndGameState(EGamePhase Phase);

private:
	UFUNCTION()
	void TankDestroyed(AActor* DestroyedActor);

	UFUNCTION()
	void TowerDestroyed(AActor* DestroyedActor);
};