// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ActionRPGCharacter.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Skills/Core/SkillTypes.h"
#include "TimerManager.h"
#include "ActionRPGEnemyCharacter.generated.h"

class AActionRPGPlayerCharacter;
class AActionRPGEnemyCharacter;
class UTextRenderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyDied, AActionRPGEnemyCharacter*, Enemy, AActor*, Killer);

/** Native damage/death source for enemies. A death is reported at most once. */
UCLASS(Blueprintable)
class ACTIONRPG_API AActionRPGEnemyCharacter : public AActionRPGCharacter
{
	GENERATED_BODY()

public:
	AActionRPGEnemyCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "Enemy|Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Enemy|Health")
	bool IsDead() const { return bIsDead; }

	/** Restores a defeated enemy for a new combat encounter. Does not restart a completed trial. */
	UFUNCTION(BlueprintCallable, Category = "Enemy|Health")
	bool Revive();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Trial")
	FName EnemyId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Health", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	/** Reduces tagged physical hits. Zero preserves existing enemy damage. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "0.0"))
	float Armor = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "1.0"))
	float ArmorMitigationScale = 100.0f;

	/** Percentage reduction for recognized damage types. Empty values leave damage unchanged. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat")
	TMap<EDamageType, float> DamageResistancePercent;

	UFUNCTION(BlueprintPure, Category = "Enemy|Combat")
	float GetDamageResistancePercent(EDamageType DamageType) const;

	/** Enable only on enemies intended for repeatable practice. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Training")
	bool bRespawnAfterDeath = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Training", meta = (ClampMin = "0.1", EditCondition = "bRespawnAfterDeath"))
	float RespawnDelaySeconds = 4.0f;

	/** Enables a simple close-range strike for enemies used in combat practice. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat")
	bool bAttacksPlayers = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "0.1", EditCondition = "bAttacksPlayers"))
	float AttackIntervalSeconds = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "1.0", EditCondition = "bAttacksPlayers"))
	float AttackRange = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "0.0", EditCondition = "bAttacksPlayers"))
	float AttackDamage = 8.0f;

	UPROPERTY(BlueprintAssignable, Category = "Enemy|Events")
	FOnEnemyDied OnEnemyDied;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Health")
	bool bIsDead = false;

private:
	void FaceLabelTowardPlayerCamera();

	UPROPERTY(Transient)
	TObjectPtr<UTextRenderComponent> CameraFacingLabel;

#if WITH_DEV_AUTOMATION_TESTS
	friend class FConstitutionTrainingTest;
#endif
	AActionRPGPlayerCharacter* ResolveKiller(AController* EventInstigator, AActor* DamageCauser) const;
	void AttemptMeleeAttack();
	void RespawnAfterDelay();
	FTimerHandle RespawnTimer;
	FTimerHandle AttackTimer;
	ECollisionEnabled::Type InitialCollisionEnabled = ECollisionEnabled::QueryAndPhysics;
	EMovementMode InitialMovementMode = MOVE_Walking;
};
