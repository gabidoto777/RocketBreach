#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "RBGun.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class USpotLightComponent;
class ARBProjectile;
class APlayerController;

UCLASS()
class ROCKETBREACH_API ARBGun : public AActor
{
    GENERATED_BODY()

public:
    ARBGun();

    void Fire(APlayerController* PlayerController);
    void SetEquipped(bool bEquipped);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<UStaticMeshComponent> GunMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USceneComponent> MuzzlePoint;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    TObjectPtr<USpotLightComponent> MuzzleFlashLight;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float AimTraceRange = 10000.0f;

private:
    void HideMuzzleFlash();

    FTimerHandle MuzzleFlashTimer;
};