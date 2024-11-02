#include "TowerOffense/Public/Generic/MyBlueprintFunctionLibrary.h"

FColor UMyBlueprintFunctionLibrary::GetTeamColor(ETeam Team)
{
	switch (Team)
	{
		case ETeam::Team1:
			return FColor::Green;
		case ETeam::Team2:
			return FColor::Yellow;
		case ETeam::Team3:
			return FColor::Purple;
		case ETeam::Team4:
			return FColor::Blue;
		case ETeam::AI:
			return FColor::Red;

		default:
			return FColor::White;
	}
}
