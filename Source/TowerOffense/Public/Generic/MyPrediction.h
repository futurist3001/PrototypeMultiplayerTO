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
	float TimeStamp;

	FSavedMovePosition()
		: Position(FVector::ZeroVector),
		  TimeStamp(0.0f)
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
	TArray<FSavedMovePosition> PredictionServerSavedMovePosition;

public:
	UMyPrediction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SaveClientPredictedPosition(FVector NewPosition, float NewTimeStamp); // save client prediction
	void ContemporaryServerPositionState(FVector ActorPosition, float Speed, float ParamTimeStamp); // here we predict 20 state ahead compare with input state
	FVector ContemporaryServerActorPosition(); // return contemporary position from the server based on server prediction

	void ClearClientPredictedPosition();
	void ClearPredictedServerSavedMovePositionArray();
};
