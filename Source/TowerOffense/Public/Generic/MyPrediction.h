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
	int64 TimeStamp;

	FSavedMovePosition()
		: Position(FVector::ZeroVector),
		  TimeStamp(0)
	{}
};

UCLASS()
class TOWEROFFENSE_API UMyPrediction : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	FSavedMovePosition Saved_ClientPredictedPosition;

	UPROPERTY(Transient)
	FSavedMovePosition ServerMove;

	UPROPERTY(Transient)
	TArray<FSavedMovePosition> PendingSavedMoves;

public:
	UMyPrediction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SaveClientPredictedPosition(FVector NewPosition, int64 NewTimeStamp); // save client prediction
	void SaveServerSavedMove(FVector NewPosition, int64 NewTimeStamp); // save server state


	void ClearClientPredictedPosition();
	void ClearPendingSavedMoves();
};
