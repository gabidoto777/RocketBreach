#include "RBMenuGameMode.h"
#include "RBMenuPlayerController.h"

ARBMenuGameMode::ARBMenuGameMode()
{
    DefaultPawnClass = nullptr;
    PlayerControllerClass = ARBMenuPlayerController::StaticClass();
}