#include "TowerOffense/Public/Generic/TOCharacterMovementComponent.h"

#include "GameFramework/Character.h"

UTOCharacterMovementComponent::UTOCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

bool UTOCharacterMovementComponent::FSavedMove_Tank::CanCombineWith(
	const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Tank* NewTankMove = static_cast<FSavedMove_Tank*>(NewMove.Get());

	if (Saved_bWantsToDrive != NewTankMove->Saved_bWantsToDrive)
	{
		return false;
	}
	
	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UTOCharacterMovementComponent::FSavedMove_Tank::Clear()
{
	Super::Clear();

	Saved_bWantsToDrive = 0;
}

uint8 UTOCharacterMovementComponent::FSavedMove_Tank::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (Saved_bWantsToDrive) Result |= FLAG_Custom_0;

	return Result;
}

void UTOCharacterMovementComponent::FSavedMove_Tank::SetMoveFor(
	ACharacter* Character, float InDeltaTime, FVector const& NewAccel,
	FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UTOCharacterMovementComponent* CharacterMovement = Cast<UTOCharacterMovementComponent>(Character->GetCharacterMovement());
	
	Saved_bWantsToDrive = CharacterMovement->Safe_bWantsToDrive;
}

void UTOCharacterMovementComponent::FSavedMove_Tank::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UTOCharacterMovementComponent* CharacterMovement = Cast<UTOCharacterMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWantsToDrive = Saved_bWantsToDrive;
}

UTOCharacterMovementComponent::FNetworkPredictionData_Client_Tank::FNetworkPredictionData_Client_Tank(
	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{
}

FSavedMovePtr UTOCharacterMovementComponent::FNetworkPredictionData_Client_Tank::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Tank);
}

FNetworkPredictionData_Client* UTOCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);

	if (ClientPredictionData == nullptr)
	{
		UTOCharacterMovementComponent* MutableThis = const_cast<UTOCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Tank(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UTOCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToDrive = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

void UTOCharacterMovementComponent::OnMovementUpdated(
	float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaTime, OldLocation, OldVelocity);

	if (MovementMode == MOVE_Walking)
	{
		if (Safe_bWantsToDrive)
		{
			MaxWalkSpeed = Ride_MaxWalkSpeed;
		}

		else
		{
			MaxWalkSpeed = 0.0f;
		}
	}
}
