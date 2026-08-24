#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RBCrosshairWidget.generated.h"

UCLASS()
class ROCKETBREACH_API URBCrosshairWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
};
