#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TowerOffense/Public/MainMenu/UI/ServerInfoSlot.h"

#include "TOMainMenuWidget.generated.h"

class UButton;
class UEditableTextBox;
class UScrollBox;
class UTextBlock;
class UWidgetSwitcher;

UCLASS()
class TOWEROFFENSE_API UTOMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateServerButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ServerNameTextBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButtonCreate;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinServerButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Join to server")
	TSubclassOf<UServerInfoSlot> ServerInfoSlotClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ServerScrollBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButtonJoin;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RefreshButtonJoin;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GameNameText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> MMSlideAnimation;

	uint8 bServerCreationInitialized : 1;

public:
	UFUNCTION()
	void CreateServerMenu();

	UFUNCTION()
	void CommitServerName(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void CreateJoinMenu();

	UFUNCTION()
	void GetServersListFromServerManager();
	void GetServersList();

	UServerInfoSlot* CreateServerInfoSlot();

	UFUNCTION()
	void BackToMainMenu();

	UFUNCTION()
	void QuitGame();

protected:
	virtual void NativeConstruct() override;
};
