#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RBMainMenuWidget.generated.h"

class UButton;

UCLASS()
class ROCKETBREACH_API URBMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void HandlePlayClicked();

    UFUNCTION()
    void HandleExitClicked();

    UPROPERTY()
    TObjectPtr<UButton> PlayButton;

    UPROPERTY()
    TObjectPtr<UButton> ExitButton;
};