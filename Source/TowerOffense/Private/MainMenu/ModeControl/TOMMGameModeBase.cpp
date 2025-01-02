#include "TowerOffense/Public/MainMenu/ModeControl/TOMMGameModeBase.h"

#include "TowerOffense/Public/Generic/LevelSystem.h"

ATOMMGameModeBase::ATOMMGameModeBase(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATOMMGameModeBase::OpenRelativeLevelCC(int32 LevelIndex) const
{
	ULevelSystem* LevelSystem = GEngine->GetEngineSubsystem<ULevelSystem>();
	LevelSystem->OpenRelativeLevel(GetWorld(), LevelIndex);
}
