#include "Generic/MyPrediction.h"

UMyPrediction::UMyPrediction(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UMyPrediction::SaveClientPredictedPosition(FVector NewPosition, int64 NewTimeStamp)
{
	Saved_ClientPredictedPosition.Position = NewPosition;
	Saved_ClientPredictedPosition.TimeStamp = NewTimeStamp;
	
	if (PendingSavedMoves.Num() >= 20)
	{
		ClearPendingSavedMoves();
	}

	PendingSavedMoves.Add(Saved_ClientPredictedPosition); // it`s not reliable
}

void UMyPrediction::SaveServerSavedMove(FVector NewPosition, int64 NewTimeStamp)
{
	ServerMove.Position = NewPosition;
	ServerMove.TimeStamp = NewTimeStamp;
}

void UMyPrediction::ClearClientPredictedPosition()
{
	Saved_ClientPredictedPosition.Position = FVector::ZeroVector;
}

void UMyPrediction::ClearPendingSavedMoves()
{
	PendingSavedMoves.Reset();
}
