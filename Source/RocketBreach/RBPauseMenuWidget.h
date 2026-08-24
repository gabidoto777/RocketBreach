#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RBPauseMenuWidget.generated.h"

class UButton;

UCLASS()
class ROCKETBREACH_API URBPauseMenuWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void HandleResumeClicked();

    UFUNCTION()
    void HandleMainMenuClicked();

    UFUNCTION()
    void HandleExitClicked();

    UPROPERTY()
    TObjectPtr<UButton> ResumeButton;

    UPROPERTY()
    TObjectPtr<UButton> MainMenuButton;

    UPROPERTY()
    TObjectPtr<UButton> ExitButton;
};