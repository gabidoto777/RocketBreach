#include "RBWaveManager.h"

#include "RBEnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "RBWaveWidget.h"
#include "GameFramework/PlayerController.h"

ARBWaveManager::ARBWaveManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ARBWaveManager::BeginPlay()
{
    Super::BeginPlay();

    FindSpawners();

    APlayerController* PlayerController =
        UGameplayStatics::GetPlayerController(
            this,
            0
        );

    if (PlayerController)
    {
        WaveWidget =
            CreateWidget<URBWaveWidget>(
                PlayerController,
                URBWaveWidget::StaticClass()
            );

        if (WaveWidget)
        {
            WaveWidget->AddToViewport(50);
        }
    }

    GetWorldTimerManager().SetTimer(
        NextWaveTimer,
        this,
        &ARBWaveManager::StartNextWave,
        1.0f,
        false
    );
}

void ARBWaveManager::FindSpawners()
{
    TArray<AActor*> FoundActors;

    UGameplayStatics::GetAllActorsOfClass(
        this,
        ARBEnemySpawner::StaticClass(),
        FoundActors
    );

    for (AActor* Actor : FoundActors)
    {
        ARBEnemySpawner* Spawner =
            Cast<ARBEnemySpawner>(Actor);

        if (!Spawner)
        {
            continue;
        }

        Spawners.Add(Spawner);

        Spawner->SetWaveManager(this);
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Wave Manager found %d spawners"),
        Spawners.Num()
    );
}

void ARBWaveManager::StartNextWave()
{
    if (Spawners.IsEmpty())
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Wave Manager has no enemy spawners!")
        );

        return;
    }

    ++CurrentWave;

    EnemiesSpawned = 0;
    EnemiesAlive = 0;

    EnemiesToSpawn =
        BaseEnemiesPerWave +
        ((CurrentWave - 1) * EnemiesAddedPerWave);

    CurrentEnemyMoveSpeed =
        BaseEnemyMoveSpeed +
        ((CurrentWave - 1) *
            MoveSpeedIncreasePerWave);

    CurrentEnemyAttackDamage =
        BaseEnemyAttackDamage +
        ((CurrentWave - 1) *
            AttackDamageIncreasePerWave);

    CurrentSpawnInterval =
        FMath::Max(
            MinimumSpawnInterval,
            BaseSpawnInterval -
            ((CurrentWave - 1) *
                SpawnIntervalReductionPerWave)
        );

    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "WAVE %d | Enemies: %d | Speed: %.1f | Damage: %.1f"
        ),
        CurrentWave,
        EnemiesToSpawn,
        CurrentEnemyMoveSpeed,
        CurrentEnemyAttackDamage
    );

    SpawnNextEnemy();

    if (EnemiesSpawned < EnemiesToSpawn)
    {
        GetWorldTimerManager().SetTimer(
            SpawnTimer,
            this,
            &ARBWaveManager::SpawnNextEnemy,
            CurrentSpawnInterval,
            true
        );
    }

    const int32 HealthTier =
        (CurrentWave - 1) / WavesPerHealthTier;

    CurrentEnemyHealth =
        BaseEnemyHealth +
        (HealthTier * HealthIncreasePerTier);
}

void ARBWaveManager::SpawnNextEnemy()
{
    if (EnemiesSpawned >= EnemiesToSpawn)
    {
        GetWorldTimerManager().ClearTimer(
            SpawnTimer
        );

        return;
    }

    ARBEnemySpawner* Spawner =
        GetRandomSpawner();

    if (!Spawner)
    {
        return;
    }

    Spawner->SpawnEnemy(
        CurrentEnemyMoveSpeed,
        CurrentEnemyAttackDamage,
        CurrentEnemyHealth
    );
    {
        ++EnemiesSpawned;
        ++EnemiesAlive;
    }

    if (EnemiesSpawned >= EnemiesToSpawn)
    {
        GetWorldTimerManager().ClearTimer(
            SpawnTimer
        );
    }
}

void ARBWaveManager::NotifyEnemyDied()
{
    ++TotalEnemiesKilled;

    EnemiesAlive =
        FMath::Max(
            0,
            EnemiesAlive - 1
        );

    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "Wave %d | Remaining: %d"
        ),
        CurrentWave,
        EnemiesAlive
    );

    const bool bAllSpawned =
        EnemiesSpawned >= EnemiesToSpawn;

    if (bAllSpawned &&
        EnemiesAlive == 0)
    {
        ++WavesCompleted;

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("WAVE %d COMPLETE"),
            CurrentWave
        );

        if (WaveWidget)
        {
            WaveWidget->ShowWaveComplete(
                CurrentWave
            );

            GetWorldTimerManager().SetTimer(
                WaveMessageTimer,
                WaveWidget.Get(),
                &URBWaveWidget::HideMessage,
                WaveCompleteMessageDuration,
                false
            );
        }

        GetWorldTimerManager().SetTimer(
            NextWaveTimer,
            this,
            &ARBWaveManager::StartNextWave,
            TimeBetweenWaves,
            false
        );
    }
}

ARBEnemySpawner* ARBWaveManager::GetRandomSpawner() const
{
    if (Spawners.IsEmpty())
    {
        return nullptr;
    }

    const int32 RandomIndex =
        FMath::RandRange(
            0,
            Spawners.Num() - 1
        );

    return Spawners[RandomIndex];
}

int32 ARBWaveManager::GetCurrentWave() const
{
    return CurrentWave;
}

int32 ARBWaveManager::GetEnemiesKilled() const
{
    return TotalEnemiesKilled;
}

int32 ARBWaveManager::GetWavesCompleted() const
{
    return WavesCompleted;
}