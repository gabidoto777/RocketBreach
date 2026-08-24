#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RBMenuPlayerController.generated.h"

class URBMainMenuWidget;

UCLASS()
class ROCKETBREACH_API ARBMenuPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    TObjectPtr<URBMainMenuWidget> MainMenuWidget;
};