#include "Generic/MyPrediction.h"

UMyPrediction::UMyPrediction(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UMyPrediction::SaveClientPredictedPosition(FVector NewPosition, float NewTimeStamp)
{
	Saved_ClientPredictedPosition.Position = NewPosition;
	Saved_ClientPredictedPosition.TimeStamp = NewTimeStamp;
}

void UMyPrediction::ContemporaryServerPositionState(FVector ActorPosition, float Speed, float ParamTimeStamp)
{
	if (!PredictionServerSavedMovePosition.IsEmpty() &&
		!ensure(PredictionServerSavedMovePosition.IsValidIndex(0)))
	{
		PredictionServerSavedMovePosition.Reset();
	}

	for (int i = 0; i <= 20; ++i)
	{
		FSavedMovePosition SavedMovePosition;
		SavedMovePosition.Position = ActorPosition * ((i + 1) * Speed);
		SavedMovePosition.TimeStamp = ParamTimeStamp + i;

		PredictionServerSavedMovePosition.Add(SavedMovePosition);
	}
}

FVector UMyPrediction::ContemporaryServerActorPosition()
{
	for (FSavedMovePosition& SavedMovePosition : PredictionServerSavedMovePosition)
	{
		if (Saved_ClientPredictedPosition.TimeStamp == SavedMovePosition.TimeStamp)
		{
			return SavedMovePosition.Position;
		}
	}

	return FVector::ZeroVector;
}

void UMyPrediction::ClearClientPredictedPosition()
{
	Saved_ClientPredictedPosition.Position = FVector::ZeroVector;
}

void UMyPrediction::ClearPredictedServerSavedMovePositionArray()
{
	PredictionServerSavedMovePosition.Reset();
}