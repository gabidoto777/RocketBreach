#include "RBProjectile.h"
#include "Materials/MaterialInterface.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/PointLightComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

ARBProjectile::ARBProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SetRootComponent(Collision);

    Collision->InitSphereRadius(8.0f);
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    Collision->SetNotifyRigidBodyCollision(true);

    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(Collision);
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
        TEXT("/Engine/BasicShapes/Sphere.Sphere")
    );

    if (SphereMesh.Succeeded())
    {
        ProjectileMesh->SetStaticMesh(SphereMesh.Object);
        ProjectileMesh->SetRelativeScale3D(FVector(0.35f, 0.08f, 0.08f));
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface> ProjectileMaterial(
        TEXT("/Game/ROCKETBREACH/Weapons/Materials/M_RBProjectile.M_RBProjectile")
    );

    if (ProjectileMaterial.Succeeded())
    {
        ProjectileMesh->SetMaterial(0, ProjectileMaterial.Object);
    }

    ProjectileLight = CreateDefaultSubobject<UPointLightComponent>(
        TEXT("ProjectileLight")
    );

    ProjectileLight->SetupAttachment(Collision);
    ProjectileLight->SetIntensity(1000.0f);
    ProjectileLight->SetAttenuationRadius(200.0f);
    ProjectileLight->SetLightColor(FLinearColor(0.0f, 1.0f, 0.15f));
    ProjectileLight->SetCastShadows(false);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(
        TEXT("ProjectileMovement")
    );

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ImpactEffectAsset(
        TEXT("/Game/ROCKETBREACH/Weapons/FX/NS_RBProjectileImpact.NS_RBProjectileImpact")
    );

    if (ImpactEffectAsset.Succeeded())
    {
        ImpactEffect = ImpactEffectAsset.Object;
    }

    ProjectileMovement->InitialSpeed = 6000.0f;
    ProjectileMovement->MaxSpeed = 6000.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.0f;

    InitialLifeSpan = 5.0f;

    Collision->OnComponentHit.AddDynamic(this, &ARBProjectile::HandleHit);
}

void ARBProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (GetOwner())
    {
        Collision->IgnoreActorWhenMoving(GetOwner(), true);
    }

    if (GetInstigator())
    {
        Collision->IgnoreActorWhenMoving(GetInstigator(), true);
    }
}

void ARBProjectile::HandleHit(
    UPrimitiveComponent* HitComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    FVector NormalImpulse,
    const FHitResult& Hit
)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }

    UGameplayStatics::ApplyDamage(
        OtherActor,
        Damage,
        GetInstigatorController(),
        this,
        UDamageType::StaticClass()
    );

    if (ImpactEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ImpactEffect,
            Hit.ImpactPoint,
            Hit.ImpactNormal.Rotation()
        );
    }

    Destroy();
}