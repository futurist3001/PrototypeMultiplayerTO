#pragma once

#include "CoreMinimal.h"
#include "Generic/TOSpawnerItems.h"

#include "TOHealthItem.generated.h"


UCLASS()
class TOWEROFFENSE_API ATOHealthItem : public ATOSpawnerItems
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float BonusHealth;

public:
	ATOHealthItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void SpecialBonus(AActor* Actor) override;
	
};
