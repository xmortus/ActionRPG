// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Skills/Core/SkillBase.h"
#include "SkillMeleeAttack.generated.h"

class AActor;
struct FHitResult;

/**
 * Melee Attack skill implementation.
 * Performs a melee attack using sphere trace for hit detection.
 * Applies damage to all valid targets within range.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API USkillMeleeAttack : public USkillBase
{
	GENERATED_BODY()

public:
	USkillMeleeAttack();

	// Override Activate to implement melee attack logic
	virtual USkillBase* Activate(AActor* Target = nullptr) override;

protected:
	/**
	 * Perform melee hit detection using sphere trace.
	 * @return Array of hit results for valid targets
	 */
	TArray<FHitResult> PerformMeleeHitDetection() const;

	/**
	 * Apply damage to a target actor.
	 * @param Target The target actor to damage
	 * @param Damage The amount of damage to apply
	 * @return True if damage was applied successfully
	 */
	bool ApplyDamageToTarget(AActor* Target, float Damage) const;

	/**
	 * Get the effective attack range.
	 * May scale with skill level in the future.
	 * @return Attack range in units
	 */
	float GetAttackRange() const;

	/**
	 * Get the effective attack damage.
	 * May scale with skill level in the future.
	 * @return Attack damage value
	 */
	float GetAttackDamage() const;
};
