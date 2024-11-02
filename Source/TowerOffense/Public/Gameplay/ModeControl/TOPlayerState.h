#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOGameModeBase.h"

#include "TOPlayerState.generated.h"


UCLASS()
class TOWEROFFENSE_API ATOPlayerState : public APlayerState // don`t use this class
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ETeam PlayerTeam;

public:
	ATOPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
};
