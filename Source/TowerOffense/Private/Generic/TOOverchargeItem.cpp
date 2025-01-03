#include "TowerOffense/Public/Generic/TOOverchargeItem.h"
#include "TowerOffense/Public/Gameplay/Pawn/TankPawn.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOPlayerController.h"

ATOOverchargeItem::ATOOverchargeItem(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	BonusEnergy = 1000.f;
	TimeOverChargeEnd = 10.f;
}

void ATOOverchargeItem::SpecialBonus(AActor* Actor)
{
	if (Actor->IsValidLowLevel(); ATankPawn * TankPawn = Cast<ATankPawn>(Actor))
	{
		float StartEnergyCount = TankPawn->GetCurrentEnergy();
		TankPawn->CurrentEnergy = BonusEnergy;

		FTimerHandle EndOverchargeTimer;
		GetWorld()->GetTimerManager().SetTimer(
			EndOverchargeTimer, [TankPawn, StartEnergyCount]()
			{
				TankPawn->CurrentEnergy = StartEnergyCount;
				if (ATOPlayerController* TOPlayerController = Cast<ATOPlayerController>(TankPawn))
				{
					TOPlayerController->UpdateHUDEnergy();
				}
			}, TimeOverChargeEnd, false);
	}
}
