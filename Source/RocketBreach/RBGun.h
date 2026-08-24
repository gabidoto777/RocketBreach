#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RBGun.generated.h"

class UStaticMeshComponent;
class USceneComponent;
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

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float AimTraceRange = 10000.0f;
};