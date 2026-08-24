#include "RBGun.h"
#include "RBProjectile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ARBGun::ARBGun()
{
    PrimaryActorTick.bCanEverTick = false;

    GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
    SetRootComponent(GunMesh);

    GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> RayGunMesh(
        TEXT("/Game/ROCKETBREACH/Weapons/RayGun/SM_RayGun.SM_RayGun")
    );

    if (RayGunMesh.Succeeded())
    {
        GunMesh->SetStaticMesh(RayGunMesh.Object);
    }

    MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
    MuzzlePoint->SetupAttachment(GunMesh);

    MuzzleFlashLight = CreateDefaultSubobject<USpotLightComponent>(
        TEXT("MuzzleFlashLight")
    );

    MuzzleFlashLight->SetupAttachment(GunMesh, TEXT("Muzzle"));

    MuzzleFlashLight->SetIntensity(5000.0f);
    MuzzleFlashLight->SetAttenuationRadius(250.0f);

    MuzzleFlashLight->SetInnerConeAngle(20.0f);
    MuzzleFlashLight->SetOuterConeAngle(45.0f);

    MuzzleFlashLight->SetLightColor(
        FLinearColor(0.0f, 1.0f, 0.15f)
    );

    MuzzleFlashLight->SetVisibility(false);
}
void ARBGun::Fire(APlayerController* PlayerController)
{
    if (!PlayerController)
    {
        return;
    }

    MuzzleFlashLight->SetVisibility(true);

    GetWorldTimerManager().ClearTimer(MuzzleFlashTimer);

    GetWorldTimerManager().SetTimer(
        MuzzleFlashTimer,
        this,
        &ARBGun::HideMuzzleFlash,
        0.06f,
        false
    );

    int32 ViewportX;
    int32 ViewportY;
    PlayerController->GetViewportSize(ViewportX, ViewportY);

    FVector WorldLocation;
    FVector WorldDirection;

    PlayerController->DeprojectScreenPositionToWorld(
        ViewportX * 0.5f,
        ViewportY * 0.5f,
        WorldLocation,
        WorldDirection
    );

    const FVector TraceEnd =
        WorldLocation + WorldDirection * AimTraceRange;

    FHitResult HitResult;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    if (APawn* PlayerPawn = PlayerController->GetPawn())
    {
        QueryParams.AddIgnoredActor(PlayerPawn);
    }

    GetWorld()->LineTraceSingleByChannel(
        HitResult,
        WorldLocation,
        TraceEnd,
        ECC_Visibility,
        QueryParams
    );

    const FVector TargetLocation =
        HitResult.bBlockingHit
        ? HitResult.ImpactPoint
        : TraceEnd;

    const FVector MuzzleLocation =
        GunMesh->GetSocketLocation(TEXT("Muzzle"));

    const FVector ProjectileDirection =
        (TargetLocation - MuzzleLocation).GetSafeNormal();

    const FRotator ProjectileRotation =
        ProjectileDirection.Rotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = PlayerController->GetPawn();

    GetWorld()->SpawnActor<ARBProjectile>(
        ARBProjectile::StaticClass(),
        MuzzleLocation,
        ProjectileRotation,
        SpawnParams
    );
}

void ARBGun::SetEquipped(bool bEquipped)
{
    SetActorHiddenInGame(!bEquipped);
}

void ARBGun::HideMuzzleFlash()
{
    if (MuzzleFlashLight)
    {
        MuzzleFlashLight->SetVisibility(false);
    }
}