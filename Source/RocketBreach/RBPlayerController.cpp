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
#include "RBHealthWidget.h"
#include "RBGameOverWidget.h"
#include "RBWaveManager.h"
#include "RBScoreSaveGame.h"
#include "Kismet/GameplayStatics.h"

ARBPlayerController::ARBPlayerController()
{
}

void ARBPlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = false;

    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("NEW RB Player Controller is active!")
    );

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (DefaultMappingContext)
            {
                InputSubsystem->AddMappingContext(
                    DefaultMappingContext,
                    0
                );
            }

            if (MouseLookMappingContext)
            {
                InputSubsystem->AddMappingContext(
                    MouseLookMappingContext,
                    1
                );
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
        CrosshairWidget->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }

    HealthWidget = CreateWidget<URBHealthWidget>(
        this,
        URBHealthWidget::StaticClass()
    );

    if (HealthWidget)
    {
        HealthWidget->AddToViewport(10);

        HealthWidget->SetHealth(
            CurrentHealth,
            MaxHealth
        );
    }

    CurrentHealth = MaxHealth;
    bPlayerDead = false;

    if (APawn* PlayerPawn = GetPawn())
    {
        PlayerPawn->OnTakeAnyDamage.AddDynamic(
            this,
            &ARBPlayerController::HandlePlayerDamage
        );
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

    InputComponent->BindKey(
        EKeys::Three,
        IE_Pressed,
        this,
        &ARBPlayerController::StartGunInspect
    );
}

void ARBPlayerController::TogglePauseMenu()
{
    if (PauseMenuWidget &&
        PauseMenuWidget->IsInViewport())
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
        PauseMenuWidget =
            CreateWidget<URBPauseMenuWidget>(
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
    InputMode.SetWidgetToFocus(
        PauseMenuWidget->TakeWidget()
    );

    InputMode.SetLockMouseToViewportBehavior(
        EMouseLockMode::DoNotLock
    );

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
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Could not find player camera")
        );

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

    Gun->SetActorRelativeLocation(
        FVector(90.0f, 45.0f, -40.0f)
    );

    Gun->SetActorRelativeRotation(
        FRotator(0.0f, 20.0f, 10.0f)
    );

    Gun->SetRestTransform(
        FVector(90.0f, 45.0f, -40.0f),
        FRotator(0.0f, 20.0f, 10.0f)
    );

    Gun->SetEquipped(false, true);
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
    if (!Gun ||
        !bGunEquipped ||
        Gun->IsInspecting())
    {
        return;
    }

    Gun->Fire(this);
}

void ARBPlayerController::StartGunInspect()
{
    if (!Gun ||
        !bGunEquipped ||
        Gun->IsInspecting())
    {
        return;
    }

    Gun->StartInspectAnimation();

    if (CrosshairWidget)
    {
        CrosshairWidget->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }
}

void ARBPlayerController::PlayerTick(
    float DeltaTime
)
{
    Super::PlayerTick(DeltaTime);

    if (!CrosshairWidget || !Gun)
    {
        return;
    }

    const bool bShouldShowCrosshair =
        bGunEquipped &&
        !Gun->IsInspecting();

    CrosshairWidget->SetVisibility(
        bShouldShowCrosshair
        ? ESlateVisibility::Visible
        : ESlateVisibility::Collapsed
    );
}

void ARBPlayerController::HandlePlayerDamage(
    AActor* DamagedActor,
    float Damage,
    const UDamageType* DamageType,
    AController* InstigatedBy,
    AActor* DamageCauser
)
{
    if (bPlayerDead || Damage <= 0.0f)
    {
        return;
    }

    CurrentHealth = FMath::Clamp(
        CurrentHealth - Damage,
        0.0f,
        MaxHealth
    );

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Player Health: %.1f / %.1f"),
        CurrentHealth,
        MaxHealth
    );

    if (HealthWidget)
    {
        HealthWidget->SetHealth(
            CurrentHealth,
            MaxHealth
        );
    }

    if (CurrentHealth <= 0.0f)
    {
        HandlePlayerDeath();
    }
}

void ARBPlayerController::HandlePlayerDeath()
{
    if (bPlayerDead)
    {
        return;
    }

    bPlayerDead = true;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("PLAYER DEAD")
    );

    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);

    if (CrosshairWidget)
    {
        CrosshairWidget->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }

    if (HealthWidget)
    {
        HealthWidget->SetVisibility(
            ESlateVisibility::Collapsed
        );
    }

    // -------------------------
    // CURRENT RUN STATS
    // -------------------------

    int32 EnemiesKilled = 0;
    int32 WavesCompleted = 0;

    ARBWaveManager* WaveManager =
        Cast<ARBWaveManager>(
            UGameplayStatics::GetActorOfClass(
                this,
                ARBWaveManager::StaticClass()
            )
        );

    if (WaveManager)
    {
        EnemiesKilled =
            WaveManager->GetEnemiesKilled();

        WavesCompleted =
            WaveManager->GetWavesCompleted();
    }

    // -------------------------
    // LOAD HIGH SCORE
    // -------------------------

    const FString SaveSlot =
        TEXT("RocketBreachHighScore");

    URBScoreSaveGame* ScoreSave = nullptr;

    if (UGameplayStatics::DoesSaveGameExist(
        SaveSlot,
        0
    ))
    {
        ScoreSave =
            Cast<URBScoreSaveGame>(
                UGameplayStatics::LoadGameFromSlot(
                    SaveSlot,
                    0
                )
            );
    }

    // First time playing - create save.
    if (!ScoreSave)
    {
        ScoreSave =
            Cast<URBScoreSaveGame>(
                UGameplayStatics::CreateSaveGameObject(
                    URBScoreSaveGame::StaticClass()
                )
            );
    }

    int32 BestEnemiesKilled = EnemiesKilled;
    int32 BestWavesCompleted = WavesCompleted;

    if (ScoreSave)
    {
        ScoreSave->BestEnemiesKilled =
            FMath::Max(
                ScoreSave->BestEnemiesKilled,
                EnemiesKilled
            );

        ScoreSave->BestWavesCompleted =
            FMath::Max(
                ScoreSave->BestWavesCompleted,
                WavesCompleted
            );

        BestEnemiesKilled =
            ScoreSave->BestEnemiesKilled;

        BestWavesCompleted =
            ScoreSave->BestWavesCompleted;

        UGameplayStatics::SaveGameToSlot(
            ScoreSave,
            SaveSlot,
            0
        );
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "RUN | Kills: %d | Waves: %d | BEST Kills: %d | BEST Waves: %d"
        ),
        EnemiesKilled,
        WavesCompleted,
        BestEnemiesKilled,
        BestWavesCompleted
    );

    // -------------------------
    // GAME OVER UI
    // -------------------------

    if (!GameOverWidget)
    {
        GameOverWidget =
            CreateWidget<URBGameOverWidget>(
                this,
                URBGameOverWidget::StaticClass()
            );
    }

    if (!GameOverWidget)
    {
        return;
    }

    GameOverWidget->AddToViewport(100);

    GameOverWidget->SetStats(
        EnemiesKilled,
        WavesCompleted,
        BestEnemiesKilled,
        BestWavesCompleted
    );

    GameOverWidget->AddToViewport(100);

    SetPause(true);

    bShowMouseCursor = true;

    FInputModeGameAndUI InputMode;

    InputMode.SetWidgetToFocus(
        GameOverWidget->TakeWidget()
    );

    InputMode.SetLockMouseToViewportBehavior(
        EMouseLockMode::DoNotLock
    );

    SetInputMode(InputMode);
}