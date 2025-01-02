#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TowerOffense/Public/TOGameInstance.h"

#include "ServerInfoSlot.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class TOWEROFFENSE_API UServerInfoSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextIDInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextServerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextMapName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextMatchType;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextPlayerInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;

	FUEServerInfo ServerInfo;

public:
	void FillTextBlocks(FUEServerInfo ParamUEServerInfo);

	UFUNCTION()
	void JoinToUEServerFunc();

protected:
	virtual void NativeConstruct() override;
};
