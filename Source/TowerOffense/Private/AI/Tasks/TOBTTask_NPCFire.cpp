#include "TowerOffense/Public/AI/Tasks/TOBTTask_NPCFire.h"

#include"TowerOffense/Public/AI/NPC_TowerAIController.h"
#include "TowerOffense/Public/Gameplay/Pawn/TowerPawn.h"

UTOBTTask_NPCFire::UTOBTTask_NPCFire(const FObjectInitializer& ObjectInitializer)
{
}

EBTNodeResult::Type UTOBTTask_NPCFire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ANPC_TowerAIController* AIController = Cast<ANPC_TowerAIController>(OwnerComp.GetAIOwner()))
	{
		if (ATowerPawn* TowerPawn = Cast<ATowerPawn>(AIController->GetPawn()))
		{
			if (TowerPawn->IsLookToTank())
			{
				TowerPawn->Fire();

				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}
