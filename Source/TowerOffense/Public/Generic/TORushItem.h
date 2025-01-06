#pragma once

#include "CoreMinimal.h"
#include "Generic/TOSpawnerItems.h"

#include "TORushItem.generated.h"


UCLASS()
class TOWEROFFENSE_API ATORushItem : public ATOSpawnerItems
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float BonusSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float TimeResetSpeed;

private:
	uint8 bIsUnderRush : 1;

public:
	ATORushItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void SpecialBonus(AActor* Actor) override;
	
};
