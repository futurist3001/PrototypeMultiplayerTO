#include "TowerOffense/Public/Gameplay/ModeControl/TOPlayerState.h"

ATOPlayerState::ATOPlayerState(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PlayerTeam = ETeam::Team1;
}
