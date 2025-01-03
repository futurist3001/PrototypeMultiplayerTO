#pragma once

#include "CoreMinimal.h"
#include "Generic/TOSpawnerItems.h"

#include "TOOverchargeItem.generated.h"

UCLASS()
class TOWEROFFENSE_API ATOOverchargeItem : public ATOSpawnerItems
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overcharge")
	float BonusEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overcharge")
	float TimeOverChargeEnd;

public:
	ATOOverchargeItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void SpecialBonus(AActor* Actor) override;
};
