#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "NPC_TowerAIController.generated.h"


UCLASS()
class TOWEROFFENSE_API ANPC_TowerAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTree;

public:
	ANPC_TowerAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
};
