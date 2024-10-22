#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "TOBTTask_NPCFire.generated.h"

UCLASS()
class TOWEROFFENSE_API UTOBTTask_NPCFire : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UTOBTTask_NPCFire(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
