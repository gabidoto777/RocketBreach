#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RBPlayerController.generated.h"

class UInputMappingContext;
class URBPauseMenuWidget;
class ARBGun;
class URBCrosshairWidget;

UCLASS()
class ROCKETBREACH_API ARBPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ARBPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* MouseLookMappingContext;

private:
    void TogglePauseMenu();
    void OpenPauseMenu();
    void ClosePauseMenu();

    void SpawnGun();
    void ToggleGun();
    void FireGun();

    UPROPERTY()
    TObjectPtr<URBPauseMenuWidget> PauseMenuWidget;

    UPROPERTY()
    TObjectPtr<ARBGun> Gun;

    UPROPERTY()
    TObjectPtr<URBCrosshairWidget> CrosshairWidget;

    bool bGunEquipped = false;
};