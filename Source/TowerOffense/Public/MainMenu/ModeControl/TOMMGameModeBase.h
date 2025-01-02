#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "TOMMGameModeBase.generated.h"

class USoundBase;

UCLASS()
class TOWEROFFENSE_API ATOMMGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATOMMGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(Exec, Category = "Levels")
	void OpenRelativeLevelCC(int32 LevelIndex) const; // Console command

protected:

};
