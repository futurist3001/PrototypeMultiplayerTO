#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "TOGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Preparation,
	Playing,
	Win,
	Lose
};

UCLASS()
class TOWEROFFENSE_API ATOGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EGamePhase, DelEndGameState);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTowerDestroyed, int32, TowersRemain);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTankDestroyed, int32, TanksRemain);

	UPROPERTY(BlueprintAssignable)
	FOnGamePhaseChanged OnGamePhaseChanged;

	FOnTowerDestroyed OnTowerDestroyed;
	FOnTankDestroyed OnTankDestroyed;

	UPROPERTY(ReplicatedUsing=OnRep_GamePhase, BlueprintReadOnly, Category = "GameState")
	EGamePhase GamePhase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MainMenuMapName;

	UPROPERTY(ReplicatedUsing=OnRep_NumberTanks)
	int32 NumberTanksState;

	UPROPERTY(ReplicatedUsing = OnRep_NumberTowers)
	int32 NumberTowersState;

	

public:
	ATOGameStateBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SetGamePhase(EGamePhase NewPhase);

	const EGamePhase GetGamePhase() const
	{
		return GamePhase;
	}

	void SetNumberTowers(int32 Towers);
	void SetNumberTanks(int32 Tanks);

	UFUNCTION()
	void OnRep_GamePhase();

	UFUNCTION()
	void OnRep_NumberTanks();

	UFUNCTION()
	void OnRep_NumberTowers();

protected:

};
