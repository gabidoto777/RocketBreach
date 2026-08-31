#include "RBEnemyCharacter.h"
#include "AIController.h"
#include "Animation/AnimSequence.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "RBEnemySpawner.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

ARBEnemyCharacter::ARBEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AAIController::StaticClass();

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    GetCharacterMovement()->bUseRVOAvoidance = true;
    GetCharacterMovement()->AvoidanceConsiderationRadius = 100.0f;

    bUseControllerRotationYaw = false;

    GetCapsuleComponent()->SetCapsuleSize(
        22.0f,
        50.0f
    );

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> AlienMesh(
        TEXT("/Game/ROCKETBREACH/Enemy/RB_Alien.RB_Alien")
    );

    if (AlienMesh.Succeeded())
    {
        GetMesh()->SetSkeletalMeshAsset(AlienMesh.Object);
    }

    GetMesh()->SetRelativeLocation(
        FVector(0.0f, 0.0f, -55.0f)
    );

    static ConstructorHelpers::FObjectFinder<UAnimSequence> RunAnim(
        TEXT("/Game/ROCKETBREACH/Enemy/RB_AlienArmature_Run.RB_AlienArmature_Run")
    );

    if (RunAnim.Succeeded())
    {
        RunAnimation = RunAnim.Object;
    }

    static ConstructorHelpers::FObjectFinder<UAnimSequence> AttackAnim(
        TEXT("/Game/ROCKETBREACH/Enemy/RB_AlienArmature_Attack.RB_AlienArmature_Attack")
    );

    if (AttackAnim.Succeeded())
    {
        AttackAnimation = AttackAnim.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> AlienVoiceAsset(
        TEXT("/Game/ROCKETBREACH/Audio/SC_AlienVoice.SC_AlienVoice")
    );

    if (AlienVoiceAsset.Succeeded())
    {
        AlienVoiceSound = AlienVoiceAsset.Object;
    }
}

void ARBEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (AlienVoiceSound)
    {
        AlienVoiceAudioComponent = UGameplayStatics::SpawnSoundAttached(
            AlienVoiceSound,
            GetMesh(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            true
        );
    }

    GetMesh()->SetRelativeRotation(
        FRotator(0.0f, -90.0f, 0.0f)
    );

    CurrentHealth = MaxHealth;

    if (RunAnimation)
    {
        GetMesh()->PlayAnimation(
            RunAnimation,
            true
        );
    }

    PlayerPawn = UGameplayStatics::GetPlayerPawn(
        this,
        0
    );

    const float RandomAngle =
        FMath::FRandRange(0.0f, 360.0f);

    const float RandomRadius =
        FMath::FRandRange(40.0f, ChaseOffsetRadius);

    ChaseOffset = FVector(
        FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
        FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
        0.0f
    );

    ChasePlayer();

    GetWorldTimerManager().SetTimer(
        CombatCheckTimer,
        this,
        &ARBEnemyCharacter::CheckCombatState,
        0.25f,
        true
    );
}

void ARBEnemyCharacter::ChasePlayer()
{
    if (!PlayerPawn || bIsDead)
    {
        return;
    }

    TryJumpToPlayer();

    if (bIsJumpingToPlayer)
    {
        return;
    }

    AAIController* AIController =
        Cast<AAIController>(GetController());

    if (!AIController)
    {
        SpawnDefaultController();

        AIController =
            Cast<AAIController>(GetController());
    }

    if (!AIController)
    {
        return;
    }

    const FVector TargetLocation =
        PlayerPawn->GetActorLocation() +
        ChaseOffset;

    AIController->MoveToLocation(
        TargetLocation,
        25.0f,
        true,
        true,
        false,
        true
    );

    bIsChasing = true;
}

void ARBEnemyCharacter::CheckCombatState()
{
    if (!PlayerPawn || bIsDead)
    {
        return;
    }

    const float DistanceToPlayer = FVector::Dist(
        GetActorLocation(),
        PlayerPawn->GetActorLocation()
    );

    if (DistanceToPlayer <= AttackRange)
    {
        if (!bIsAttacking)
        {
            StartAttack();
        }

        return;
    }

    if (bIsAttacking)
    {
        return;
    }

    if (RunAnimation && !bIsChasing)
    {
        GetMesh()->PlayAnimation(
            RunAnimation,
            true
        );

        GetMesh()->SetPlayRate(1.0f);
    }

    AAIController* AIController =
        Cast<AAIController>(GetController());

    if (!AIController)
    {
        return;
    }

    // Only request another path if the AI is no longer moving.
    if (AIController->GetMoveStatus() ==
        EPathFollowingStatus::Idle)
    {
        bIsChasing = false;
        ChasePlayer();
    }
}

void ARBEnemyCharacter::StartAttack()
{
    if (!PlayerPawn)
    {
        return;
    }

    bIsAttacking = true;
    bIsChasing = false;

    if (AAIController* AIController =
        Cast<AAIController>(GetController()))
    {
        AIController->StopMovement();
    }

    if (AttackAnimation)
    {
        GetMesh()->PlayAnimation(
            AttackAnimation,
            false
        );

        GetMesh()->SetPlayRate(AttackPlayRate);
    }

    GetWorldTimerManager().SetTimer(
        AttackHitTimer,
        this,
        &ARBEnemyCharacter::DealAttackDamage,
        AttackHitDelay,
        false
    );

    GetWorldTimerManager().SetTimer(
        AttackCooldownTimer,
        this,
        &ARBEnemyCharacter::FinishAttack,
        AttackCooldown,
        false
    );
}

void ARBEnemyCharacter::DealAttackDamage()
{
    if (!PlayerPawn)
    {
        return;
    }

    const float DistanceToPlayer = FVector::Dist(
        GetActorLocation(),
        PlayerPawn->GetActorLocation()
    );

    if (DistanceToPlayer > AttackRange + 25.0f)
    {
        return;
    }

    UGameplayStatics::ApplyDamage(
        PlayerPawn,
        AttackDamage,
        GetController(),
        this,
        UDamageType::StaticClass()
    );

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Alien attacked player for %.1f damage"),
        AttackDamage
    );
}

void ARBEnemyCharacter::FinishAttack()
{
    bIsAttacking = false;

    if (!PlayerPawn || bIsDead)
    {
        return;
    }

    const float DistanceToPlayer = FVector::Dist(
        GetActorLocation(),
        PlayerPawn->GetActorLocation()
    );

    if (DistanceToPlayer > AttackRange)
    {
        if (RunAnimation)
        {
            GetMesh()->PlayAnimation(
                RunAnimation,
                true
            );

            GetMesh()->SetPlayRate(1.0f);
        }

        ChasePlayer();
    }
}

float ARBEnemyCharacter::TakeDamage(
    float DamageAmount,
    FDamageEvent const& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    if (bIsDead)
    {
        return 0.0f;
    }

    const float ActualDamage = Super::TakeDamage(
        DamageAmount,
        DamageEvent,
        EventInstigator,
        DamageCauser
    );

    if (ActualDamage <= 0.0f)
    {
        return 0.0f;
    }

    CurrentHealth -= ActualDamage;

    StartHitFlash();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Enemy Health: %.1f"),
        CurrentHealth
    );

    if (CurrentHealth <= 0.0f)
    {
        Die();
    }

    return ActualDamage;
}

void ARBEnemyCharacter::Die()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;
    bIsAttacking = false;

    GetWorldTimerManager().ClearTimer(HitFlashTimer);

    GetMesh()->SetScalarParameterValueOnMaterials(
        TEXT("HitFlash"),
        0.0f
    );

    if (AlienVoiceAudioComponent)
    {
        AlienVoiceAudioComponent->Stop();
        AlienVoiceAudioComponent = nullptr;
    }

    if (OwningSpawner)
    {
        OwningSpawner->NotifyEnemyDied(this);
        OwningSpawner = nullptr;
    }

    GetWorldTimerManager().ClearTimer(CombatCheckTimer);
    GetWorldTimerManager().ClearTimer(AttackHitTimer);
    GetWorldTimerManager().ClearTimer(AttackCooldownTimer);
    GetWorldTimerManager().ClearTimer(JumpRecoveryTimer);

    if (AAIController* AIController =
        Cast<AAIController>(GetController()))
    {
        AIController->StopMovement();
    }

    GetCharacterMovement()->DisableMovement();

    // Stop current run/attack animation
    GetMesh()->Stop();

    // Disable the Character capsule
    GetCapsuleComponent()->SetCollisionEnabled(
        ECollisionEnabled::NoCollision
    );

    // Let the Physics Asset control the dead body
    GetMesh()->SetCollisionProfileName(
        TEXT("Ragdoll")
    );

    GetMesh()->SetCollisionEnabled(
        ECollisionEnabled::QueryAndPhysics
    );

    GetMesh()->SetAllBodiesSimulatePhysics(true);
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->WakeAllRigidBodies();

    DetachFromControllerPendingDestroy();

    SetLifeSpan(4.0f);
}

/*void ARBEnemyCharacter::TryFrustratedJump()
{
    if (!PlayerPawn ||
        bIsDead ||
        bIsAttacking ||
        bIsFrustratedJumping)
    {
        return;
    }

    const FVector EnemyLocation = GetActorLocation();
    const FVector PlayerLocation = PlayerPawn->GetActorLocation();

    const float HeightDifference =
        PlayerLocation.Z - EnemyLocation.Z;

    const float HorizontalDistance =
        FVector::Dist2D(
            EnemyLocation,
            PlayerLocation
        );

    // Player needs to be above the alien.
    if (HeightDifference < FrustratedJumpHeightDifference)
    {
        return;
    }

    // Alien keeps chasing until it gets close.
    if (HorizontalDistance > FrustratedJumpTriggerDistance)
    {
        return;
    }

    bIsFrustratedJumping = true;
    bIsChasing = false;

    if (AAIController* AIController =
        Cast<AAIController>(GetController()))
    {
        AIController->StopMovement();
    }

    // VISUAL ONLY.
    // Character capsule does not move.
    if (JumpAnimation)
    {
        GetMesh()->PlayAnimation(
            JumpAnimation,
            false
        );

        GetMesh()->SetPlayRate(1.0f);
    }

    GetWorldTimerManager().SetTimer(
        FrustratedJumpTimer,
        this,
        &ARBEnemyCharacter::FinishFrustratedJump,
        FrustratedJumpCooldown,
        false
    );
}*/

/*void ARBEnemyCharacter::FinishFrustratedJump()
{
    bIsFrustratedJumping = false;

    if (!PlayerPawn || bIsDead)
    {
        return;
    }

    if (RunAnimation)
    {
        GetMesh()->PlayAnimation(
            RunAnimation,
            true
        );

        GetMesh()->SetPlayRate(1.0f);
    }

    ChasePlayer();
}*/

void ARBEnemyCharacter::TryJumpToPlayer()
{
    if (!PlayerPawn ||
        bIsDead ||
        bIsAttacking ||
        bIsJumpingToPlayer)
    {
        return;
    }

    const FVector EnemyLocation = GetActorLocation();
    const FVector PlayerLocation = PlayerPawn->GetActorLocation();

    const float HorizontalDistance =
        FVector::Dist2D(EnemyLocation, PlayerLocation);

    const float HeightDifference =
        PlayerLocation.Z - EnemyLocation.Z;

    // Player isn't high enough to justify jumping.
    if (HeightDifference < JumpHeightRequirement)
    {
        return;
    }

    // Still too far away. Keep chasing instead.
    if (HorizontalDistance > JumpTriggerDistance)
    {
        return;
    }

    AAIController* AIController =
        Cast<AAIController>(GetController());

    if (!AIController)
    {
        return;
    }

    // Only jump once navigation has basically reached its limit.
    if (AIController->GetMoveStatus() != EPathFollowingStatus::Idle)
    {
        return;
    }

    bIsJumpingToPlayer = true;
    bIsChasing = false;

    AIController->StopMovement();

    FVector DirectionToPlayer =
        PlayerLocation - EnemyLocation;

    DirectionToPlayer.Z = 0.0f;
    DirectionToPlayer.Normalize();

    const FVector LaunchVelocity =
        (DirectionToPlayer * JumpForwardVelocity) +
        FVector(0.0f, 0.0f, JumpUpVelocity);

    LaunchCharacter(
        LaunchVelocity,
        true,
        true
    );

    GetWorldTimerManager().SetTimer(
        JumpRecoveryTimer,
        this,
        &ARBEnemyCharacter::FinishPhysicalJump,
        JumpRecoveryTime,
        false
    );
}

void ARBEnemyCharacter::FinishPhysicalJump()
{
    bIsJumpingToPlayer = false;

    if (!PlayerPawn || bIsDead)
    {
        return;
    }

    ChasePlayer();
}

void ARBEnemyCharacter::SetMoveSpeed(float NewMoveSpeed)
{
    MoveSpeed = NewMoveSpeed;

    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
}

void ARBEnemyCharacter::SetOwningSpawner(ARBEnemySpawner* NewSpawner)
{
    OwningSpawner = NewSpawner;
}

void ARBEnemyCharacter::SetAttackDamage(float NewAttackDamage)
{
    AttackDamage = NewAttackDamage;
}

void ARBEnemyCharacter::SetMaxHealth(float NewMaxHealth)
{
    MaxHealth = NewMaxHealth;
    CurrentHealth = MaxHealth;
}

void ARBEnemyCharacter::StartHitFlash()
{
    GetMesh()->SetScalarParameterValueOnMaterials(
        TEXT("HitFlash"),
        1.0f
    );

    GetWorldTimerManager().ClearTimer(HitFlashTimer);

    GetWorldTimerManager().SetTimer(
        HitFlashTimer,
        this,
        &ARBEnemyCharacter::EndHitFlash,
        HitFlashDuration,
        false
    );
}

void ARBEnemyCharacter::EndHitFlash()
{
    GetMesh()->SetScalarParameterValueOnMaterials(
        TEXT("HitFlash"),
        0.0f
    );
}