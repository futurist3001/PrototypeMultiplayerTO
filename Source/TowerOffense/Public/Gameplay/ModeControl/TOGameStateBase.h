#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "TOGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Preparation,
	Playing,
	FirstTeamLose,
	SecondTeamLose,
	ThirdTeamLose,
	FourthTeamLose,
	FirstTeamWin,
	SecondTeamWin,
	ThirdTeamWin,
	FourthTeamWin
};

UCLASS()
class TOWEROFFENSE_API ATOGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseChanged, EGamePhase, DelEndGameState);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFirstTeamTankDestroyed, int32, TanksRemain);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSecondTeamTankDestroyed, int32, TanksRemain);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThirdTeamTankDestroyed, int32, TanksRemain);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFourthTeamTankDestroyed, int32, TanksRemain);

	UPROPERTY(BlueprintAssignable)
	FOnGamePhaseChanged OnGamePhaseChanged;
	FOnFirstTeamTankDestroyed OnFirstTeamTankDestroyed;
	FOnSecondTeamTankDestroyed OnSecondTeamTankDestroyed;
	FOnThirdTeamTankDestroyed OnThirdTeamTankDestroyed;
	FOnFourthTeamTankDestroyed OnFourthTeamTankDestroyed;

	UPROPERTY(ReplicatedUsing=OnRep_GamePhase, BlueprintReadOnly, Category = "GameState")
	EGamePhase GamePhase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MainMenuMapName;

	UPROPERTY(ReplicatedUsing = OnRep_FirstTeamPlayers)
	int32 FirstTeamPlayers;

	UPROPERTY(ReplicatedUsing = OnRep_SecondTeamPlayers)
	int32 SecondTeamPlayers;

	UPROPERTY(ReplicatedUsing = OnRep_ThirdTeamPlayers)
	int32 ThirdTeamPlayers;

	UPROPERTY(ReplicatedUsing = OnRep_FourthTeamPlayers)
	int32 FourthTeamPlayers;

public:
	ATOGameStateBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SetGamePhase(EGamePhase NewPhase);

	const EGamePhase GetGamePhase() const
	{
		return GamePhase;
	}

	void SetNumberFirstTeamPlayers(int32 FirstTeam);
	void SetNumberSecondTeamPlayers(int32 SecondTeam);
	void SetNumberThirdTeamPlayers(int32 ThirdTeam);
	void SetNumberFourthTeamPlayers(int32 FourthTeam);

	UFUNCTION()
	void OnRep_GamePhase();

	UFUNCTION()
	void OnRep_FirstTeamPlayers();

	UFUNCTION()
	void OnRep_SecondTeamPlayers();

	UFUNCTION()
	void OnRep_ThirdTeamPlayers();

	UFUNCTION()
	void OnRep_FourthTeamPlayers();

protected:

};
