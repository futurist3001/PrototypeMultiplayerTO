#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "TOCharacterMovementComponent.generated.h"

UCLASS()
class TOWEROFFENSE_API UTOCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	bool Safe_bWantsToDrive;
	
	UPROPERTY(EditDefaultsOnly)
	float Ride_MaxWalkSpeed;

private:
	class FSavedMove_Tank : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;

		uint8 Saved_bWantsToDrive : 1;

		virtual bool CanCombineWith(
			const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel,
			class FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_Tank : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Tank(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	UTOCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity) override;
};
