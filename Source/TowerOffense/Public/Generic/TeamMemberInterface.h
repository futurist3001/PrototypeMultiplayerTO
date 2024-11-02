#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "TeamMemberInterface.generated.h"

enum class ETeam : uint8;

UINTERFACE(MinimalAPI, Blueprintable)
class UTeamMemberInterface : public UInterface
{
	GENERATED_BODY()
};

class TOWEROFFENSE_API ITeamMemberInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	const ETeam GetTeam() const;
};
