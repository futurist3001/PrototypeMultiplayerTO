#pragma once
#include "CoreMinimal.h"
#include "TurretPawn.h"

#include "TankPawn.generated.h"

class UAudioComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UNiagaraSystem;
class USoundBase;
class USpringArmComponent;
struct FHitResult;
struct FInputActionValue;
struct FInputActionInstance;

UCLASS()
class TOWEROFFENSE_API ATankPawn : public ATurretPawn
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeTeam);
	FOnChangeTeam OnChangeTeam;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> TankMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveForwardAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> TurnRightAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> RotateTurretAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AimingAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AccelerationDuration;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float MaxEnergy;

	UPROPERTY(BlueprintReadOnly, Category = "Fire")
	float CurrentEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
	float OldShootTime; // After this time the last shoot is considered as old shoot

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float FireInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float RechargeInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float MaxFireHorizontalAngle;

	UPROPERTY(VisibleAnywhere, Category = "VFX")
	TObjectPtr<USceneComponent> RightTankTrack;

	UPROPERTY(VisibleAnywhere, Category = "VFX")
	TObjectPtr<USceneComponent> LeftTankTrack;

	UPROPERTY(VisibleAnywhere, Category = "VFX")
	TObjectPtr<USceneComponent> LeftTankTrackRotation;

	UPROPERTY(VisibleAnywhere, Category = "VFX")
	TObjectPtr<USceneComponent> RightTankTrackRotation;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> TankTop;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> TankBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	TObjectPtr<UNiagaraSystem> MovementEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
	TObjectPtr<USoundBase> MovementSound;

private:
	FTimerHandle ReloadLevelTimerHandle;
	FTimerHandle AdjustingTurretPositionTimerHandle;
	FTimerHandle RotComplAdjustingTurretPositionTimerHandle;
	FTimerHandle ClearAdjustingTurretPositionTimerHandle;
	FTimerHandle CollisionTimerHandle; // For detect when collision ends
	FVector MovementVector;
	FVector PreviousMovementVector; // previous pressed button
	float CurrentTime;
	float CurrentSpeed;
	float SpeedStopGas; // speed after acceleration
	float SpeedStopBraking; // speed after braking
	float YawCameraRotator;
	float YawTurnRotator;
	float CurrentTimeFire; // For calculating fire interval
	float PitchAimingRotator; // For calculating aiming process
	float RechargeTimeProjectile; // For calculating recharge projectile interval
	uint8 bIsStopMoving : 1;
	uint8 bReverseAttempt : 1;
	uint8 bPlayedTurretRotationSoundIteration : 1;
	uint8 bIsOldShoot : 1;
	uint8 bIsCollision : 1;
	uint8 bIsUpsideDown : 1;
	FHitResult ShootingPoint;
	UAudioComponent* MovementAudioComponent;
	
public:
	ATankPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	float GetCurrentEnergy() const
	{
		return CurrentEnergy;
	}

	float GetMaxEnergy() const
	{
		return MaxEnergy;
	}

	bool IsOldShoot() const
	{
		return bIsOldShoot;
	}

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void RotateTurret(
		const FRotator& Current, const FRotator& Target, float DeltaTime, float InterpSpeed) override;

	virtual void Fire() override;
	UFUNCTION(Server, reliable)
	void Server_Fire(FVector FireStart, FVector FireEnd, float RPCTimeFire, float RPCFireEnergy, bool bRPCIsOldShoot);
	UFUNCTION(NetMulticast, reliable)
	void Multicast_Fire(FVector FireStart, FVector FireEnd, float RPCTimeFire, float RPCFireEnergy, bool bRPCIsOldShoot);

	virtual void NotifyHit(
		UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
		FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void StopCollision();

	void MoveStarted();
	void MoveTriggeredValue(const FInputActionValue& Value);
	void MoveTriggeredInstance(const FInputActionInstance& Instance);
	UFUNCTION(Server, unreliable)
	void Server_SetActorLocation(FVector ActorLocation);
	UFUNCTION(Server, unreliable)
	void Server_PlayVFXSFXMoveAnim(
		bool RPCIsReverseAtempt, FVector RPCMovementVector, FVector RPCPreviousMovVector, float RPCSpeedStopBraking);
	UFUNCTION(NetMulticast, unreliable)
	void Multicast_PlayVFXSFXMoveAnim(
		bool RPCIsReverseAtempt, FVector RPCMovementVector, FVector RPCPreviousMovVector, float RPCSpeedStopBraking);
	void MoveCompleted();

	void Turn(const FInputActionValue& Value);
	UFUNCTION(Server, unreliable)
	void Server_SetActorRotation(float ServerYawTurnRotator);
	UFUNCTION(NetMulticast, unreliable)
	void Multicast_SetActorRotation(float MultiYawTurnRotator);

	void Rotate(const FInputActionValue& Value);
	UFUNCTION(Server, unreliable)
	void Server_SetControlRotation(const float YawValue);
	void RotateCompleted();

	void AdjustTurretPosition();
	void ClearAdjustingTurretPositionTimer();
	void UpsideDownTank();

	void Aiming(const FInputActionValue& Value);

	UFUNCTION(Exec, Category = "Team")
	void ChangeTeam(ETeam TeamChange); // console command
	UFUNCTION(Server, reliable)
	void Server_ChangeTeam(ETeam TeamChange);
	UFUNCTION(NetMulticast, reliable)
	void Multicast_ChangeTeam(ETeam TeamChange);
};