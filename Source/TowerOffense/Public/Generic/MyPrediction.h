#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "MyPrediction.generated.h"

USTRUCT()
struct FSavedMovePosition
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Position;

	UPROPERTY()
	int32 TimeStamp;

	UPROPERTY()
	float Speed;

	FSavedMovePosition()
		: Position(FVector::ZeroVector),
		  TimeStamp(0),
		  Speed(0.0f)
	{}
};

UCLASS()
class TOWEROFFENSE_API UMyPrediction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TArray<FSavedMovePosition> PendingClientSavedMovePosition;

	UPROPERTY(Transient)
	TArray<FSavedMovePosition> PredictionServerSavedMovePosition;

public:
	UMyPrediction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SaveClientMovePosition(FVector PawnPosition, float ActorSpeed);
	void ClearClientMovePosition();
	void PredictServerMovePosition(FVector PawnPosition, float ActorSpeed);
	void ClearPredictedServerMovePosition();
	FVector ClientAdjustPosition(APawn* PawnToAdjust);

	
};
