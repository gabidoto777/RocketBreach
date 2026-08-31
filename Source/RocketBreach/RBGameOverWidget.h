#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RBGameOverWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class ROCKETBREACH_API URBGameOverWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetStats(
        int32 EnemiesKilled,
        int32 WavesCompleted,
        int32 BestEnemiesKilled,
        int32 BestWavesCompleted
    );

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void RetryGame();

    UFUNCTION()
    void ReturnToMainMenu();

    UPROPERTY()
    TObjectPtr<UButton> RetryButton;

    UPROPERTY()
    TObjectPtr<UButton> MainMenuButton;

    UPROPERTY()
    TObjectPtr<UTextBlock> EnemiesKilledText;

    UPROPERTY()
    TObjectPtr<UTextBlock> WavesCompletedText;

    UPROPERTY()
    TObjectPtr<UTextBlock> BestEnemiesKilledText;

    UPROPERTY()
    TObjectPtr<UTextBlock> BestWavesCompletedText;
};