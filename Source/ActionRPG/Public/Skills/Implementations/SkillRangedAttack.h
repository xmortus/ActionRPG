// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Skills/Core/SkillBase.h"
#include "SkillRangedAttack.generated.h"

class AActor;
class ASkillProjectile;

/**
 * Ranged Attack skill implementation.
 * Spawns a projectile that travels toward the target and applies damage on hit.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API USkillRangedAttack : public USkillBase
{
	GENERATED_BODY()

public:
	USkillRangedAttack();

	// Override Activate to implement ranged attack logic
	virtual USkillBase* Activate(AActor* Target = nullptr) override;

protected:
	/**
	 * Spawn projectile actor at specified location and rotation.
	 * @param SpawnLocation World location to spawn projectile
	 * @param SpawnRotation World rotation for projectile direction
	 * @return Spawned projectile actor, or nullptr if spawn failed
	 */
	AActor* SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation);

	/**
	 * Get the projectile spawn location (owner location + forward offset).
	 * @return World location to spawn projectile
	 */
	FVector GetProjectileSpawnLocation() const;

	/**
	 * Get the projectile direction (toward target or owner's forward direction).
	 * @param Target Optional target actor to aim at
	 * @return Normalized direction vector
	 */
	FVector GetProjectileDirection(AActor* Target) const;

	/**
	 * Get the projectile speed from SkillDataAsset.
	 * @return Projectile speed in units per second
	 */
	float GetProjectileSpeed() const;

	/**
	 * Get the effective attack damage.
	 * May scale with skill level in the future.
	 * @return Attack damage value
	 */
	float GetAttackDamage() const;
};
