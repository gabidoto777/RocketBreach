#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RBWaveWidget.generated.h"

class UTextBlock;

UCLASS()
class ROCKETBREACH_API URBWaveWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void ShowWaveComplete(int32 WaveNumber);
    void HideMessage();

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    UPROPERTY()
    TObjectPtr<UTextBlock> WaveText;
};
