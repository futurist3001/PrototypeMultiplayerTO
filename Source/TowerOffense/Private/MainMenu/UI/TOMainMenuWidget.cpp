#include "TowerOffense/Public/MainMenu/UI/TOMainMenuWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TowerOffense/Public/MainMenu/TOGameInstance.h"

void UTOMainMenuWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);

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
		CreateServerButton->OnClicked.AddDynamic(GameInstance, &UTOGameInstance::StartCreateServer);
		
		JoinServerButton->OnClicked.AddDynamic(GameInstance, &UTOGameInstance::StartJoinServer);
	}
	QuitGameButton->OnClicked.AddDynamic(this, &UTOMainMenuWidget::QuitGame);
}
