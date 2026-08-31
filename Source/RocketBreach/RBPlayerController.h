#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RBPlayerController.generated.h"

class UInputMappingContext;
class URBPauseMenuWidget;
class URBCrosshairWidget;
class UDamageType;
class ARBGun;
class URBHealthWidget;
class URBGameOverWidget;


UCLASS()
class ROCKETBREACH_API ARBPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ARBPlayerController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void PlayerTick(float DeltaTime) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* MouseLookMappingContext;

    UFUNCTION()
    void HandlePlayerDamage(
        AActor* DamagedActor,
        float Damage,
        const UDamageType* DamageType,
        AController* InstigatedBy,
        AActor* DamageCauser
    );

private:
    void TogglePauseMenu();
    void OpenPauseMenu();
    void ClosePauseMenu();

    void SpawnGun();
    void ToggleGun();
    void FireGun();
    void StartGunInspect();

    void HandlePlayerDeath();

    UPROPERTY()
    TObjectPtr<URBPauseMenuWidget> PauseMenuWidget;

    UPROPERTY()
    TObjectPtr<ARBGun> Gun;

    UPROPERTY()
    TObjectPtr<URBCrosshairWidget> CrosshairWidget;

    UPROPERTY()
    TObjectPtr<URBHealthWidget> HealthWidget;

    UPROPERTY(EditDefaultsOnly, Category = "Player|Health")
    float MaxHealth = 100.0f;

    UPROPERTY()
    TObjectPtr<URBGameOverWidget> GameOverWidget;

    bool bGunEquipped = false;
    float CurrentHealth = 100.0f;
    bool bPlayerDead = false;
};