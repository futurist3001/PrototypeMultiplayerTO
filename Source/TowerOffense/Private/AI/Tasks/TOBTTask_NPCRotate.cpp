#include "TowerOffense/Public/AI/Tasks/TOBTTask_NPCRotate.h"

#include"TowerOffense/Public/AI/NPC_TowerAIController.h"
#include "TowerOffense/Public/Gameplay/Pawn/TowerPawn.h"

UTOBTTask_NPCRotate::UTOBTTask_NPCRotate(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

EBTNodeResult::Type UTOBTTask_NPCRotate::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ANPC_TowerAIController* AIController = Cast<ANPC_TowerAIController>(OwnerComp.GetAIOwner()))
	{
		if (ATowerPawn* TowerPawn = Cast<ATowerPawn>(AIController->GetPawn()))
		{
			if (!TowerPawn->IsLookToTank())
			{
				TowerPawn->RotateTurret(
					TowerPawn->TurretMesh->GetComponentRotation(), TowerPawn->GetTargetAngle(),
					TowerPawn->GetRotationCurrentTime(), TowerPawn->GetTurretRotationSpeed());

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}
