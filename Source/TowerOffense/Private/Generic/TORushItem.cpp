#include "TowerOffense/Public/Generic/TORushItem.h"
#include "TowerOffense/Public/Gameplay/Pawn/TankPawn.h"

ATORushItem::ATORushItem(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	BonusSpeed = 10.f;
	TimeResetSpeed = 10.f;

	bIsUnderRush = false;
}

void ATORushItem::SpecialBonus(AActor* Actor)
{
	if (Actor->IsValidLowLevel(); ATankPawn* TankPawn = Cast<ATankPawn>(Actor))
	{
		if (!bIsUnderRush)
		{
			bIsUnderRush = true;

			float DefaultSpeed = TankPawn->Speed;
			TankPawn->Speed += BonusSpeed;

			FTimerHandle ResetSpeedTimer;
			GetWorld()->GetTimerManager().SetTimer(
				ResetSpeedTimer, [this, TankPawn, DefaultSpeed]()
				{
					TankPawn->Speed = DefaultSpeed;
					bIsUnderRush = false;
				}, TimeResetSpeed, false);
		}
	}
}
