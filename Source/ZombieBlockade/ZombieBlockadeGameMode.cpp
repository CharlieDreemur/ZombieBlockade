// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZombieBlockadeGameMode.h"
#include "ZombieBlockadePlayerController.h"
#include "ZombieBlockadeCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "CustomHUD.h"

AZombieBlockadeGameMode::AZombieBlockadeGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AZombieBlockadePlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<ACustomHUD> HUDClassFinder(TEXT("/Game/TopDown/Blueprints/Widget_OverallHud"));
	if (HUDClassFinder.Succeeded()) {
		HUDClass = HUDClassFinder.Class;
	}
}