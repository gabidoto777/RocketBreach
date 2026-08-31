#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RBEnemySpawner.generated.h"

class ARBEnemyCharacter;
class ARBWaveManager;
class UArrowComponent;
class USceneComponent;

UCLASS()
class ROCKETBREACH_API ARBEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    ARBEnemySpawner();

    ARBEnemyCharacter* SpawnEnemy(
        float EnemyMoveSpeed,
        float EnemyAttackDamage,
        float EnemyMaxHealth
    );

    void SetWaveManager(
        ARBWaveManager* NewWaveManager
    );

    void NotifyEnemyDied(
        ARBEnemyCharacter* DeadEnemy
    );

private:
    UArrowComponent* GetRandomSpawnPoint() const;

    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, Category = "Spawner|Spawn Points")
    TObjectPtr<UArrowComponent> SpawnPointA;

    UPROPERTY(VisibleAnywhere, Category = "Spawner|Spawn Points")
    TObjectPtr<UArrowComponent> SpawnPointB;

    UPROPERTY(VisibleAnywhere, Category = "Spawner|Spawn Points")
    TObjectPtr<UArrowComponent> SpawnPointC;

    UPROPERTY(VisibleAnywhere, Category = "Spawner|Spawn Points")
    TObjectPtr<UArrowComponent> SpawnPointD;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    TSubclassOf<ARBEnemyCharacter> EnemyClass;

    UPROPERTY()
    TObjectPtr<ARBWaveManager> WaveManager;
};