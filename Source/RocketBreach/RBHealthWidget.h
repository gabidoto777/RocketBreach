#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RBHealthWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class ROCKETBREACH_API URBHealthWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetHealth(float CurrentHealth, float MaxHealth);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    UPROPERTY()
    TObjectPtr<UProgressBar> HealthBar;

    UPROPERTY()
    TObjectPtr<UTextBlock> HealthText;
};