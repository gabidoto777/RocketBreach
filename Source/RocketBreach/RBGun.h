#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "RBGun.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class USpotLightComponent;
class USoundBase;
class ARBProjectile;
class APlayerController;

UCLASS()
class ROCKETBREACH_API ARBGun : public AActor
{
    GENERATED_BODY()

public:
    ARBGun();

    virtual void Tick(float DeltaSeconds) override;

    void Fire(APlayerController* PlayerController);
    void SetEquipped(bool bEquipped, bool bInstant = false);
    void SetRestTransform(const FVector& Location, const FRotator& Rotation);

    void StartInspectAnimation();
    bool IsInspecting() const;

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
    void ResetFire();

    void UpdateRecoil(float DeltaSeconds);
    void UpdateEquipAnimation(float DeltaSeconds);
    void UpdateInspectAnimation(float DeltaSeconds);

    FTimerHandle MuzzleFlashTimer;
    FTimerHandle FireCooldownTimer;

    UPROPERTY()
    TObjectPtr<USoundBase> FireSound;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Fire")
    float FireCooldown = 0.25f;

    bool bCanFire = true;

    FVector RestLocation;
    FRotator RestRotation;

    FVector RecoilLocationOffset = FVector(-8.0f, 0.0f, -1.0f);
    FRotator RecoilRotationOffset = FRotator(-1.5f, 0.0f, 0.0f);

    float RecoilKickSpeed = 45.0f;
    float RecoilReturnSpeed = 15.0f;

    bool bRecoilActive = false;
    bool bReturningFromRecoil = false;

    FVector HolsterLocationOffset = FVector(0.0f, 0.0f, -80.0f);
    FRotator HolsterRotationOffset = FRotator(10.0f, 0.0f, 0.0f);

    float EquipSpeed = 16.0f;

    bool bTargetEquipped = false;
    bool bEquipAnimating = false;

    enum class EInspectState : uint8
    {
        None,
        MoveToPoseOne,
        HoldPoseOne,
        MoveToPoseTwo,
        HoldPoseTwo,
        ReturnToRest
    };

    EInspectState InspectState = EInspectState::None;

    float InspectStateTime = 0.0f;

    float InspectMoveSpeed = 7.0f;

    float PoseOneHoldTime = 2.0f;
    float PoseTwoHoldTime = 1.5f;

    FVector InspectPoseOneLocationOffset = FVector(10.0f, -20.0f, 18.0f);
    FRotator InspectPoseOneRotationOffset = FRotator(20.0f, -75.0f, -50.0f);

    FVector InspectPoseTwoLocationOffset = FVector(10.0f, -25.0f, 12.0f);
    FRotator InspectPoseTwoRotationOffset = FRotator(2.0f, -50.0f, -16.0f);

    float InspectWaveAmount = 1.0f;
    float InspectWaveSpeed = 2.0f;
};