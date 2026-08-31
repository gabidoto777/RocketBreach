#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RBScoreSaveGame.generated.h"

UCLASS()
class ROCKETBREACH_API URBScoreSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY()
    int32 BestEnemiesKilled = 0;

    UPROPERTY()
    int32 BestWavesCompleted = 0;
};