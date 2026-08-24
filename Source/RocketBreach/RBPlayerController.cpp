#include "RBPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "RBPauseMenuWidget.h"
#include "InputCoreTypes.h"
#include "Components/InputComponent.h"
#include "RBGun.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "RBCrosshairWidget.h"

ARBPlayerController::ARBPlayerController()
{
}

void ARBPlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = false;

    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);

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

    SpawnGun();

    CrosshairWidget = CreateWidget<URBCrosshairWidget>(
        this,
        URBCrosshairWidget::StaticClass()
    );

    if (CrosshairWidget)
    {
        CrosshairWidget->AddToViewport();
        CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
    }

}

void ARBPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    FInputKeyBinding& PauseBinding = InputComponent->BindKey(
        EKeys::P,
        IE_Pressed,
        this,
        &ARBPlayerController::TogglePauseMenu
    );

    PauseBinding.bExecuteWhenPaused = true;

    InputComponent->BindKey(
        EKeys::One,
        IE_Pressed,
        this,
        &ARBPlayerController::ToggleGun
    );

    InputComponent->BindKey(
        EKeys::LeftMouseButton,
        IE_Pressed,
        this,
        &ARBPlayerController::FireGun
    );
}

void ARBPlayerController::TogglePauseMenu()
{
    if (PauseMenuWidget && PauseMenuWidget->IsInViewport())
    {
        ClosePauseMenu();
        return;
    }

    OpenPauseMenu();
}

void ARBPlayerController::OpenPauseMenu()
{
    if (!PauseMenuWidget)
    {
        PauseMenuWidget = CreateWidget<URBPauseMenuWidget>(
            this,
            URBPauseMenuWidget::StaticClass()
        );
    }

    if (!PauseMenuWidget)
    {
        return;
    }

    PauseMenuWidget->AddToViewport();

    SetPause(true);
    bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    SetInputMode(InputMode);
}

void ARBPlayerController::ClosePauseMenu()
{
    SetPause(false);

    if (PauseMenuWidget)
    {
        PauseMenuWidget->RemoveFromParent();
    }

    bShowMouseCursor = false;

    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
}

void ARBPlayerController::SpawnGun()
{
    APawn* PlayerPawn = GetPawn();

    if (!PlayerPawn)
    {
        return;
    }

    UCameraComponent* Camera =
        PlayerPawn->FindComponentByClass<UCameraComponent>();

    if (!Camera)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not find player camera"));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = PlayerPawn;
    SpawnParams.Instigator = PlayerPawn;

    Gun = GetWorld()->SpawnActor<ARBGun>(
        ARBGun::StaticClass(),
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (!Gun)
    {
        return;
    }

    Gun->AttachToComponent(
        Camera,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale
    );

    Gun->SetActorRelativeLocation(FVector(90.0f, 45.0f, -40.0f));
    Gun->SetActorRelativeRotation(FRotator(0.0f, 20.0f, 10.0f));

    Gun->SetEquipped(false);
}

void ARBPlayerController::ToggleGun()
{
    if (!Gun)
    {
        return;
    }

    bGunEquipped = !bGunEquipped;

    Gun->SetEquipped(bGunEquipped);

    if (CrosshairWidget)
    {
        CrosshairWidget->SetVisibility(
            bGunEquipped
            ? ESlateVisibility::Visible
            : ESlateVisibility::Collapsed
        );
    }
}

void ARBPlayerController::FireGun()
{
    if (!Gun || !bGunEquipped)
    {
        return;
    }

    Gun->Fire(this);
}