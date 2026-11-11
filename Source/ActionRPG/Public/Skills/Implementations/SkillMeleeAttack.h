// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Skills/Core/SkillBase.h"
#include "SkillMeleeAttack.generated.h"

class AActor;
class UDamageType;
class UItemDataAsset;

/**
 * Melee Attack skill implementation.
 * Performs a short-range melee overlap for hit detection.
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

	/** Uses the equipped damaging weapon's type, or the skill's type when unarmed. */
	TSubclassOf<UDamageType> GetMeleeDamageTypeClass() const;

	float GetAttackDamage() const;
	virtual float GetEffectiveCooldownDuration() const override;

protected:
	const UItemDataAsset* GetMainHandWeaponData() const;
	/**
	 * Find unique pawn targets in melee reach, favoring the facing direction.
	 * @return Actors that can be damaged by this swing
	 */
	TArray<AActor*> PerformMeleeHitDetection() const;

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
};
