#include "TowerOffense/Public/Generic/TOHealthItem.h"

#include "TowerOffense/Public/Gameplay/Pawn/TankPawn.h"
#include "TowerOffense/Public/Gameplay/ModeControl/TOPlayerController.h"
#include "TowerOffense/Public/Gameplay/UI/HealthTurretWidget.h"
#include "TowerOffense/Public/Gameplay/UI/TOHUDWidget.h"

ATOHealthItem::ATOHealthItem(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATOHealthItem::SpecialBonus(AActor* Actor)
{
	if (Actor->IsValidLowLevel(); ATankPawn* TankPawn = Cast<ATankPawn>(Actor))
	{
		if (TankPawn->HealthComponent->Health + BonusHealth > TankPawn->HealthComponent->DefaultHealth)
		{
			TankPawn->HealthComponent->Health = 100.f;
		}
		
		else
		{
			TankPawn->HealthComponent->Health += BonusHealth;
		}

		if (GetLocalRole() != ROLE_Authority)
		{
			if (ATOPlayerController* TOPlayerController = Cast<ATOPlayerController>(TankPawn->GetController()))
			{
				TOPlayerController->HUDWidget->SetHealth(TankPawn->HealthComponent->Health, TankPawn->HealthComponent->DefaultHealth);
				TankPawn->UpdateHealthBarComponent(TankPawn, TankPawn->HealthComponent);
			}
		}
	}
}
