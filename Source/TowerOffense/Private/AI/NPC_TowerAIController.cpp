#include "TowerOffense/Public/AI/NPC_TowerAIController.h"

ANPC_TowerAIController::ANPC_TowerAIController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void ANPC_TowerAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunBehaviorTree(BehaviorTree);
}
