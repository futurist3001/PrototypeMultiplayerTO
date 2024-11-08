#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "TOGameInstance.generated.h"

UCLASS()
class TOWEROFFENSE_API UTOGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	IOnlineSessionPtr SessionInterface;
	class ATOMMPlayerController* CreateServerPlayer;

public:
	UTOGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION()
	void StartCreateServer();

	UFUNCTION()
	void StartJoinServer();

protected:
	virtual void Init() override;
	virtual void OnCreateSessionComplete(FName ServerName, bool Succeeded);
};
