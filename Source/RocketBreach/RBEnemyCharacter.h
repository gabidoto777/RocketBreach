#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "RBEnemyCharacter.generated.h"

class APawn;
class UAnimSequence;
class ARBEnemySpawner;
class USoundBase;
class UAudioComponent;

UCLASS()
class ROCKETBREACH_API ARBEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ARBEnemyCharacter();

    virtual float TakeDamage(
        float DamageAmount,
        FDamageEvent const& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser
    ) override;

    void SetMoveSpeed(float NewMoveSpeed);
    void SetAttackDamage(float NewAttackDamage);
    void SetMaxHealth(float NewMaxHealth);
    void SetOwningSpawner(ARBEnemySpawner* NewSpawner);

protected:
    virtual void BeginPlay() override;

private:
    void ChasePlayer();
    void CheckCombatState();

    void StartAttack();
    void DealAttackDamage();
    void FinishAttack();

    void TryJumpToPlayer();
    void FinishPhysicalJump();

    void StartHitFlash();
    void EndHitFlash();

    void Die();

    UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    float MaxHealth = 50.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    float MoveSpeed = 350.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Combat")
    float AttackRange = 125.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Combat")
    float AttackDamage = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Combat")
    float AttackCooldown = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Combat")
    float AttackHitDelay = 0.25f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Combat")
    float AttackPlayRate = 1.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Jump")
    float JumpTriggerDistance = 180.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Jump")
    float JumpHeightRequirement = 70.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Jump")
    float JumpUpVelocity = 700.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Jump")
    float JumpForwardVelocity = 250.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Jump")
    float JumpRecoveryTime = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Feedback")
    float HitFlashDuration = 0.08f;

    FVector ChaseOffset = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Movement")
    float ChaseOffsetRadius = 120.0f;

    UPROPERTY()
    TObjectPtr<ARBEnemySpawner> OwningSpawner;

    UPROPERTY()
    TObjectPtr<APawn> PlayerPawn;

    UPROPERTY()
    TObjectPtr<UAnimSequence> RunAnimation;

    UPROPERTY()
    TObjectPtr<UAnimSequence> AttackAnimation;

    UPROPERTY()
    TObjectPtr<USoundBase> AlienVoiceSound;

    UPROPERTY()
    TObjectPtr<UAudioComponent> AlienVoiceAudioComponent;

    float CurrentHealth = 0.0f;

    bool bIsAttacking = false;
    bool bIsChasing = false;
    bool bIsDead = false;
    bool bIsJumpingToPlayer = false;

    FTimerHandle CombatCheckTimer;
    FTimerHandle AttackHitTimer;
    FTimerHandle AttackCooldownTimer;
    FTimerHandle JumpRecoveryTimer;
    FTimerHandle HitFlashTimer;
};