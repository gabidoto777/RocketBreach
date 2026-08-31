#include "RBGun.h"
#include "RBProjectile.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ARBGun::ARBGun()
{
    PrimaryActorTick.bCanEverTick = true;

    GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
    SetRootComponent(GunMesh);

    GunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GunMesh->SetLightingChannels(
        true,
        false,
        false
    );

    static ConstructorHelpers::FObjectFinder<UStaticMesh> RayGunMesh(
        TEXT("/Game/ROCKETBREACH/Weapons/RayGun/SM_RayGun.SM_RayGun")
    );

    if (RayGunMesh.Succeeded())
    {
        GunMesh->SetStaticMesh(RayGunMesh.Object);
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundAsset(
        TEXT("/Game/ROCKETBREACH/Audio/Gun/SC_RayGunShot.SC_RayGunShot")
    );

    if (FireSoundAsset.Succeeded())
    {
        FireSound = FireSoundAsset.Object;
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
    if (!PlayerController || !bCanFire)
    {
        return;
    }

    bCanFire = false;

    GetWorldTimerManager().SetTimer(
        FireCooldownTimer,
        this,
        &ARBGun::ResetFire,
        FireCooldown,
        false
    );

    bRecoilActive = true;
    bReturningFromRecoil = false;

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

    PlayerController->GetViewportSize(
        ViewportX,
        ViewportY
    );

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

    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            FireSound,
            MuzzleLocation
        );
    }

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

void ARBGun::SetEquipped(bool bEquipped, bool bInstant)
{
    bRecoilActive = false;
    bReturningFromRecoil = false;

    bTargetEquipped = bEquipped;

    const FVector HolsterLocation =
        RestLocation + HolsterLocationOffset;

    const FRotator HolsterRotation =
        RestRotation + HolsterRotationOffset;

    if (bInstant)
    {
        if (bEquipped)
        {
            SetActorRelativeLocation(RestLocation);
            SetActorRelativeRotation(RestRotation);
            SetActorHiddenInGame(false);
        }
        else
        {
            SetActorRelativeLocation(HolsterLocation);
            SetActorRelativeRotation(HolsterRotation);
            SetActorHiddenInGame(true);
        }

        bEquipAnimating = false;
        return;
    }

    if (bEquipped)
    {
        SetActorHiddenInGame(false);

        SetActorRelativeLocation(HolsterLocation);
        SetActorRelativeRotation(HolsterRotation);
    }

    bEquipAnimating = true;
}

void ARBGun::HideMuzzleFlash()
{
    if (MuzzleFlashLight)
    {
        MuzzleFlashLight->SetVisibility(false);
    }
}

void ARBGun::ResetFire()
{
    bCanFire = true;
}

void ARBGun::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bEquipAnimating)
    {
        UpdateEquipAnimation(DeltaSeconds);
    }
    else if (IsInspecting())
    {
        UpdateInspectAnimation(DeltaSeconds);
    }
    else
    {
        UpdateRecoil(DeltaSeconds);
    }
}

void ARBGun::SetRestTransform(
    const FVector& Location,
    const FRotator& Rotation
)
{
    RestLocation = Location;
    RestRotation = Rotation;
}

void ARBGun::UpdateRecoil(float DeltaSeconds)
{
    if (!bRecoilActive)
    {
        return;
    }

    const FVector RecoilLocation =
        RestLocation + RecoilLocationOffset;

    const FRotator RecoilRotation =
        RestRotation + RecoilRotationOffset;

    const FVector TargetLocation =
        bReturningFromRecoil
        ? RestLocation
        : RecoilLocation;

    const FRotator TargetRotation =
        bReturningFromRecoil
        ? RestRotation
        : RecoilRotation;

    const float InterpSpeed =
        bReturningFromRecoil
        ? RecoilReturnSpeed
        : RecoilKickSpeed;

    const FVector NewLocation = FMath::VInterpTo(
        GetRootComponent()->GetRelativeLocation(),
        TargetLocation,
        DeltaSeconds,
        InterpSpeed
    );

    const FRotator NewRotation = FMath::RInterpTo(
        GetRootComponent()->GetRelativeRotation(),
        TargetRotation,
        DeltaSeconds,
        InterpSpeed
    );

    SetActorRelativeLocation(NewLocation);
    SetActorRelativeRotation(NewRotation);

    if (!bReturningFromRecoil)
    {
        if (NewLocation.Equals(RecoilLocation, 0.5f) &&
            NewRotation.Equals(RecoilRotation, 0.5f))
        {
            bReturningFromRecoil = true;
        }
    }
    else
    {
        if (NewLocation.Equals(RestLocation, 0.5f) &&
            NewRotation.Equals(RestRotation, 0.5f))
        {
            SetActorRelativeLocation(RestLocation);
            SetActorRelativeRotation(RestRotation);

            bRecoilActive = false;
        }
    }
}

void ARBGun::UpdateEquipAnimation(float DeltaSeconds)
{
    if (!bEquipAnimating)
    {
        return;
    }

    const FVector HolsterLocation =
        RestLocation + HolsterLocationOffset;

    const FRotator HolsterRotation =
        RestRotation + HolsterRotationOffset;

    const FVector TargetLocation =
        bTargetEquipped
        ? RestLocation
        : HolsterLocation;

    const FRotator TargetRotation =
        bTargetEquipped
        ? RestRotation
        : HolsterRotation;

    const FVector CurrentLocation =
        GetRootComponent()->GetRelativeLocation();

    const FRotator CurrentRotation =
        GetRootComponent()->GetRelativeRotation();

    const FVector NewLocation = FMath::VInterpTo(
        CurrentLocation,
        TargetLocation,
        DeltaSeconds,
        EquipSpeed
    );

    const FRotator NewRotation = FMath::RInterpTo(
        CurrentRotation,
        TargetRotation,
        DeltaSeconds,
        EquipSpeed
    );

    SetActorRelativeLocation(NewLocation);
    SetActorRelativeRotation(NewRotation);

    if (NewLocation.Equals(TargetLocation, 0.5f) &&
        NewRotation.Equals(TargetRotation, 0.5f))
    {
        SetActorRelativeLocation(TargetLocation);
        SetActorRelativeRotation(TargetRotation);

        bEquipAnimating = false;

        if (!bTargetEquipped)
        {
            SetActorHiddenInGame(true);
        }
    }
}

void ARBGun::StartInspectAnimation()
{
    if (bEquipAnimating || IsInspecting())
    {
        return;
    }

    bRecoilActive = false;
    bReturningFromRecoil = false;

    InspectState = EInspectState::MoveToPoseOne;
    InspectStateTime = 0.0f;
}

bool ARBGun::IsInspecting() const
{
    return InspectState != EInspectState::None;
}

void ARBGun::UpdateInspectAnimation(float DeltaSeconds)
{
    InspectStateTime += DeltaSeconds;

    const FVector PoseOneLocation =
        RestLocation + InspectPoseOneLocationOffset;

    const FRotator PoseOneRotation =
        RestRotation + InspectPoseOneRotationOffset;

    const FVector PoseTwoLocation =
        RestLocation + InspectPoseTwoLocationOffset;

    const FRotator PoseTwoRotation =
        RestRotation + InspectPoseTwoRotationOffset;

    FVector CurrentLocation =
        GetRootComponent()->GetRelativeLocation();

    FRotator CurrentRotation =
        GetRootComponent()->GetRelativeRotation();

    switch (InspectState)
    {
    case EInspectState::MoveToPoseOne:
    {
        const FVector NewLocation = FMath::VInterpTo(
            CurrentLocation,
            PoseOneLocation,
            DeltaSeconds,
            InspectMoveSpeed
        );

        const FRotator NewRotation = FMath::RInterpTo(
            CurrentRotation,
            PoseOneRotation,
            DeltaSeconds,
            InspectMoveSpeed
        );

        SetActorRelativeLocation(NewLocation);
        SetActorRelativeRotation(NewRotation);

        if (NewLocation.Equals(PoseOneLocation, 0.5f) &&
            NewRotation.Equals(PoseOneRotation, 0.5f))
        {
            InspectState = EInspectState::HoldPoseOne;
            InspectStateTime = 0.0f;
        }

        break;
    }

    case EInspectState::HoldPoseOne:
    {
        const float Wave =
            FMath::Sin(InspectStateTime * InspectWaveSpeed)
            * InspectWaveAmount;

        SetActorRelativeLocation(
            PoseOneLocation + FVector(0.0f, 0.0f, Wave)
        );

        SetActorRelativeRotation(
            PoseOneRotation +
            FRotator(0.0f, Wave * 0.25f, Wave * 0.15f)
        );

        if (InspectStateTime >= PoseOneHoldTime)
        {
            InspectState = EInspectState::MoveToPoseTwo;
            InspectStateTime = 0.0f;
        }

        break;
    }

    case EInspectState::MoveToPoseTwo:
    {
        const FVector NewLocation = FMath::VInterpTo(
            CurrentLocation,
            PoseTwoLocation,
            DeltaSeconds,
            InspectMoveSpeed
        );

        const FRotator NewRotation = FMath::RInterpTo(
            CurrentRotation,
            PoseTwoRotation,
            DeltaSeconds,
            InspectMoveSpeed
        );

        SetActorRelativeLocation(NewLocation);
        SetActorRelativeRotation(NewRotation);

        if (NewLocation.Equals(PoseTwoLocation, 0.5f) &&
            NewRotation.Equals(PoseTwoRotation, 0.5f))
        {
            InspectState = EInspectState::HoldPoseTwo;
            InspectStateTime = 0.0f;
        }

        break;
    }

    case EInspectState::HoldPoseTwo:
    {
        const float Wave =
            FMath::Sin(InspectStateTime * InspectWaveSpeed)
            * (InspectWaveAmount * 0.5f);

        SetActorRelativeLocation(
            PoseTwoLocation + FVector(0.0f, 0.0f, Wave)
        );

        if (InspectStateTime >= PoseTwoHoldTime)
        {
            InspectState = EInspectState::ReturnToRest;
            InspectStateTime = 0.0f;
        }

        break;
    }

    case EInspectState::ReturnToRest:
    {
        const FVector NewLocation = FMath::VInterpTo(
            CurrentLocation,
            RestLocation,
            DeltaSeconds,
            InspectMoveSpeed
        );

        const FRotator NewRotation = FMath::RInterpTo(
            CurrentRotation,
            RestRotation,
            DeltaSeconds,
            InspectMoveSpeed
        );

        SetActorRelativeLocation(NewLocation);
        SetActorRelativeRotation(NewRotation);

        if (NewLocation.Equals(RestLocation, 0.5f) &&
            NewRotation.Equals(RestRotation, 0.5f))
        {
            SetActorRelativeLocation(RestLocation);
            SetActorRelativeRotation(RestRotation);

            InspectState = EInspectState::None;
            InspectStateTime = 0.0f;
        }

        break;
    }

    default:
        break;
    }
}