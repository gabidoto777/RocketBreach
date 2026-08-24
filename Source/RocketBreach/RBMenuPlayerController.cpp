#include "RBMenuPlayerController.h"
#include "RBMainMenuWidget.h"

void ARBMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("RB Menu Player Controller is active"));

    MainMenuWidget = CreateWidget<URBMainMenuWidget>(
        this,
        URBMainMenuWidget::StaticClass()
    );

    if (!MainMenuWidget)
    {
        return;
    }

    MainMenuWidget->AddToViewport();

    bShowMouseCursor = true;

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    SetInputMode(InputMode);
}