#include "TowerOffense/Public/Generic/TORushItem.h"
#include "TowerOffense/Public/Gameplay/Pawn/TankPawn.h"

ATORushItem::ATORushItem(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	BonusSpeed = 10.f;
	TimeResetSpeed = 10.f;
}

void ATORushItem::SpecialBonus(AActor* Actor)
{
	if (Actor->IsValidLowLevel(); ATankPawn* TankPawn = Cast<ATankPawn>(Actor))
	{
		float DefaultSpeed = TankPawn->Speed;
		TankPawn->Speed += BonusSpeed;

		FTimerHandle ResetSpeedTimer;
		GetWorld()->GetTimerManager().SetTimer(
			ResetSpeedTimer, [TankPawn, DefaultSpeed]()
			{
				TankPawn->Speed = DefaultSpeed;
			}, TimeResetSpeed, false);
	}
}
