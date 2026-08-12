#include "RBPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

ARBPlayerController::ARBPlayerController()
{
}

void ARBPlayerController::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("NEW RB Player Controller is active!"));

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (DefaultMappingContext)
            {
                InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
            }

            if (MouseLookMappingContext)
            {
                InputSubsystem->AddMappingContext(MouseLookMappingContext, 1);
            }
        }
    }
}

void ARBPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}