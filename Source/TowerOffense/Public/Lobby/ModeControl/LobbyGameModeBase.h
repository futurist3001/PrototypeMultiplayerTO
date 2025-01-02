#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "LobbyGameModeBase.generated.h"

USTRUCT(BlueprintType)
struct FTeamMembersInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 FirstTeamNumbers;

	UPROPERTY(BlueprintReadWrite)
	int32 SecondTeamNumbers;

	UPROPERTY(BlueprintReadWrite)
	int32 ThirdTeamNumbers;

	UPROPERTY(BlueprintReadWrite)
	int32 FourthTeamNumbers;
};

UCLASS()
class TOWEROFFENSE_API ALobbyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	TObjectPtr<APlayerController> HostPlayer;

	FTeamMembersInfo TeamMembersInfo;

public:
	ALobbyGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GetUEServerInfo();

	void PerformServerTravel(const FString& MapName);

	void ReturnAllClientsToMM();

	FTeamMembersInfo GetTeamMembersStat();
	void ResetTeamMembersInfo();
	
protected:
	virtual APlayerController* Login(
		UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
		const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void Logout(AController* Exiting) override;
};
