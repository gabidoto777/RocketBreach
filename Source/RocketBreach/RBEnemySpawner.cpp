#include "RBEnemySpawner.h"

#include "RBEnemyCharacter.h"
#include "RBWaveManager.h"

#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"

ARBEnemySpawner::ARBEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot =
        CreateDefaultSubobject<USceneComponent>(
            TEXT("SceneRoot")
        );

    RootComponent = SceneRoot;

    SpawnPointA =
        CreateDefaultSubobject<UArrowComponent>(
            TEXT("SpawnPointA")
        );

    SpawnPointA->SetupAttachment(SceneRoot);

    SpawnPointA->SetRelativeLocation(
        FVector(200.0f, 0.0f, 0.0f)
    );

    SpawnPointB =
        CreateDefaultSubobject<UArrowComponent>(
            TEXT("SpawnPointB")
        );

    SpawnPointB->SetupAttachment(SceneRoot);

    SpawnPointB->SetRelativeLocation(
        FVector(-200.0f, 0.0f, 0.0f)
    );

    SpawnPointC =
        CreateDefaultSubobject<UArrowComponent>(
            TEXT("SpawnPointC")
        );

    SpawnPointC->SetupAttachment(SceneRoot);

    SpawnPointC->SetRelativeLocation(
        FVector(0.0f, 200.0f, 0.0f)
    );

    SpawnPointD =
        CreateDefaultSubobject<UArrowComponent>(
            TEXT("SpawnPointD")
        );

    SpawnPointD->SetupAttachment(SceneRoot);

    SpawnPointD->SetRelativeLocation(
        FVector(0.0f, -200.0f, 0.0f)
    );

    EnemyClass = ARBEnemyCharacter::StaticClass();
}

ARBEnemyCharacter* ARBEnemySpawner::SpawnEnemy(
    float EnemyMoveSpeed,
    float EnemyAttackDamage,
    float EnemyMaxHealth
)
{
    if (!EnemyClass)
    {
        return nullptr;
    }

    UArrowComponent* SpawnPoint =
        GetRandomSpawnPoint();

    if (!SpawnPoint)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;

    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::
        AdjustIfPossibleButAlwaysSpawn;

    ARBEnemyCharacter* Enemy =
        GetWorld()->SpawnActor<ARBEnemyCharacter>(
            EnemyClass,
            SpawnPoint->GetComponentLocation(),
            SpawnPoint->GetComponentRotation(),
            SpawnParams
        );

    if (!Enemy)
    {
        return nullptr;
    }

    Enemy->SetMoveSpeed(
        EnemyMoveSpeed
    );

    Enemy->SetAttackDamage(
        EnemyAttackDamage
    );

    Enemy->SetMaxHealth(
        EnemyMaxHealth
    );

    Enemy->SetOwningSpawner(
        this
    );

    return Enemy;
}

void ARBEnemySpawner::SetWaveManager(
    ARBWaveManager* NewWaveManager
)
{
    WaveManager = NewWaveManager;
}

void ARBEnemySpawner::NotifyEnemyDied(
    ARBEnemyCharacter* DeadEnemy
)
{
    if (WaveManager)
    {
        WaveManager->NotifyEnemyDied();
    }
}

UArrowComponent*
ARBEnemySpawner::GetRandomSpawnPoint() const
{
    TArray<UArrowComponent*> SpawnPoints;

    SpawnPoints.Add(SpawnPointA);
    SpawnPoints.Add(SpawnPointB);
    SpawnPoints.Add(SpawnPointC);
    SpawnPoints.Add(SpawnPointD);

    if (SpawnPoints.IsEmpty())
    {
        return nullptr;
    }

    const int32 RandomIndex =
        FMath::RandRange(
            0,
            SpawnPoints.Num() - 1
        );

    return SpawnPoints[RandomIndex];
}