#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOGameStateBase.h"

#include "TOGameModeBase.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	Team1 UMETA(DisplayName = "Team 1"),
	Team2 UMETA(DisplayName = "Team 2"),
	Team3 UMETA(DisplayName = "Team 3"),
	Team4 UMETA(DisplayName = "Team 4"),
	AI UMETA(DisplayName = "AI")
};

class USoundBase;

UCLASS()
class TOWEROFFENSE_API ATOGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	
protected:
	
private:
	int32 FirstTeamPlayers;
	int32 SecondTeamPlayers;
	int32 ThirdTeamPlayers;
	int32 FourthTeamPlayers;
	int32 TotalPlayerNumbers;
	uint8 bIsGameStarted : 1;
	uint8 bIsSetPlayersPosition : 1;

	FTimerHandle SetPlayersTeamTimer;

public:
	ATOGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(Exec, Category = "Levels")
	void OpenRelativeLevelCC(int32 LevelIndex) const; // Console command

	void GameStarted();

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayerController) override;
	virtual APlayerController* Login(
		UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
		const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void Logout(AController* Exiting) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void AlternativeInitPlayData();

	void SetEndGameState(EGamePhase Phase);

private:
	UFUNCTION()
	void AlternativeTankDestroyed(AActor* DestroyedActor);
	void AlternativeWinCase();
	void DeactivateTowers();

	void SetPlayersPosition();
};