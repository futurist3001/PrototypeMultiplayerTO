#include "TowerOffense/Public/MainMenu/UI/TOMainMenuWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TowerOffense/Public/MainMenu/TOGameInstance.h"
#include "TowerOffense/Public/MainMenu/ModeControl/TOMMPlayerController.h"

void UTOMainMenuWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);

	if (UTOGameInstance* GameInstance = GetGameInstance<UTOGameInstance>())
	{
		if (GameInstance->SessionInterface->GetNamedSession("My Session"))
		{
			GameInstance->SessionInterface->DestroySession("My Session");
		}
	}
}

void UTOMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MMSlideAnimation)
	{
		PlayAnimationForward(MMSlideAnimation);
	}

	if (UTOGameInstance* GameInstance = GetGameInstance<UTOGameInstance>())
	{
		CreateServerButton->OnClicked.AddDynamic(GameInstance, &UTOGameInstance::CreateNewServer);
		JoinServerButton->OnClicked.AddDynamic(GameInstance, &UTOGameInstance::StartJoinServer); // old
	}
	QuitGameButton->OnClicked.AddDynamic(this, &UTOMainMenuWidget::QuitGame);
}