// Copyright Epic Games, Inc. All Rights Reserved.

#include "RocketBreachGameMode.h"
#include "RBPlayerController.h"

ARocketBreachGameMode::ARocketBreachGameMode()
{
    PlayerControllerClass = ARBPlayerController::StaticClass();
}
