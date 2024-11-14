#include "Generic/MyPrediction.h"

UMyPrediction::UMyPrediction(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UMyPrediction::SaveClientMovePosition(FVector ActorPosition, float ActorSpeed)
{
	if (PendingClientSavedMovePosition.IsEmpty())
	{
		FSavedMovePosition ClientSavedMovePosition;
		ClientSavedMovePosition.Position = ActorPosition;
		ClientSavedMovePosition.TimeStamp = PendingClientSavedMovePosition.Num();
		ClientSavedMovePosition.Speed = ActorSpeed;

		PendingClientSavedMovePosition.Add(ClientSavedMovePosition);
	}

	else if (PendingClientSavedMovePosition.Num() > 0 && PendingClientSavedMovePosition.Num() > 22) // Saved not more than 21 states from the client side
	{
		FSavedMovePosition ClientSavedMovePosition;
		ClientSavedMovePosition.Position = ActorPosition;
		ClientSavedMovePosition.TimeStamp = 0;
		ClientSavedMovePosition.Speed = ActorSpeed;

		if (!PendingClientSavedMovePosition.IsEmpty())
		{
			PendingClientSavedMovePosition.Add(ClientSavedMovePosition);
		}
	}
}

void UMyPrediction::ClearClientMovePosition()
{
	if (PendingClientSavedMovePosition.Num() > 0)
	{
		PendingClientSavedMovePosition.Empty(); // think about Reset()
	}
}

void UMyPrediction::PredictServerMovePosition(FVector ActorPosition, float ActorSpeed) // we predict 20 states ahead compared with input state
{
	for (int i = 0; i <= 20; ++i)
	{
		FSavedMovePosition PredictedServerSavedMovePosition;
		PredictedServerSavedMovePosition.Position = ActorPosition * ((i + 1) * ActorSpeed);
		PredictedServerSavedMovePosition.TimeStamp = i;
		PredictedServerSavedMovePosition.Speed = ActorSpeed;

		PredictionServerSavedMovePosition.Add(PredictedServerSavedMovePosition);
	}
}

void UMyPrediction::ClearPredictedServerMovePosition()
{
	if (PredictionServerSavedMovePosition.Num() > 0)
	{
		PredictionServerSavedMovePosition.Empty(); // think about Reset()
	}
}

FVector UMyPrediction::ClientAdjustPosition(APawn* PawnToAdjust) // return correct position
{
	if (!PredictionServerSavedMovePosition.IsEmpty() && !PendingClientSavedMovePosition.IsEmpty())
	{
		for (FSavedMovePosition& SavedMovePosition : PredictionServerSavedMovePosition)
		{
			if (SavedMovePosition.TimeStamp == PendingClientSavedMovePosition.Num())
			{
				return PendingClientSavedMovePosition.Last().Position;
			}
		}

		return FVector::ZeroVector;
	}

	return FVector::ZeroVector;
}
