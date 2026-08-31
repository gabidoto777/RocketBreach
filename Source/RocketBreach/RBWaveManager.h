#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "RBWaveManager.generated.h"

class ARBEnemySpawner;
class URBWaveWidget;

UCLASS()
class ROCKETBREACH_API ARBWaveManager : public AActor
{
    GENERATED_BODY()

public:
    ARBWaveManager();

    void NotifyEnemyDied();

    int32 GetCurrentWave() const;
    int32 GetEnemiesKilled() const;
    int32 GetWavesCompleted() const;

protected:
    virtual void BeginPlay() override;

private:
    void FindSpawners();
    void StartNextWave();
    void SpawnNextEnemy();

    ARBEnemySpawner* GetRandomSpawner() const;

    int32 TotalEnemiesKilled = 0;
    int32 WavesCompleted = 0;

    UPROPERTY(EditAnywhere, Category = "Wave")
    int32 BaseEnemiesPerWave = 3;

    UPROPERTY(EditAnywhere, Category = "Wave")
    int32 EnemiesAddedPerWave = 2;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float TimeBetweenWaves = 3.0f;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float WaveCompleteMessageDuration = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Enemy Speed")
    float BaseEnemyMoveSpeed = 350.0f;

    UPROPERTY(EditAnywhere, Category = "Enemy Speed")
    float MoveSpeedIncreasePerWave = 35.0f;

    UPROPERTY(EditAnywhere, Category = "Enemy Damage")
    float BaseEnemyAttackDamage = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Enemy Damage")
    float AttackDamageIncreasePerWave = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Spawn Rate")
    float BaseSpawnInterval = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Spawn Rate")
    float SpawnIntervalReductionPerWave = 0.1f;

    UPROPERTY(EditAnywhere, Category = "Spawn Rate")
    float MinimumSpawnInterval = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Enemy Health")
    float BaseEnemyHealth = 50.0f;

    UPROPERTY(EditAnywhere, Category = "Enemy Health")
    float HealthIncreasePerTier = 25.0f;

    UPROPERTY(EditAnywhere, Category = "Enemy Health")
    int32 WavesPerHealthTier = 2;

    float CurrentEnemyHealth = 50.0f;

    UPROPERTY()
    TArray<TObjectPtr<ARBEnemySpawner>> Spawners;

    UPROPERTY()
    TObjectPtr<URBWaveWidget> WaveWidget;

    int32 CurrentWave = 0;

    int32 EnemiesToSpawn = 0;
    int32 EnemiesSpawned = 0;
    int32 EnemiesAlive = 0;

    float CurrentEnemyMoveSpeed = 350.0f;
    float CurrentEnemyAttackDamage = 10.0f;
    float CurrentSpawnInterval = 1.5f;

    FTimerHandle SpawnTimer;
    FTimerHandle NextWaveTimer;
    FTimerHandle WaveMessageTimer;
};