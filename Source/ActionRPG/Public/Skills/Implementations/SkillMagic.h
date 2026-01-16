// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Skills/Core/SkillBase.h"
#include "SkillMagic.generated.h"

class AActor;
class ASkillProjectile;

/**
 * Magic skill implementation (Fireball example).
 * Spawns a projectile and applies damage on hit.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API USkillMagic : public USkillBase
{
	GENERATED_BODY()

public:
	USkillMagic();

	// Override Activate to implement magic attack logic
	virtual USkillBase* Activate(AActor* Target = nullptr) override;

protected:
	/**
	 * Spawn projectile actor at specified location and rotation.
	 * @param SpawnLocation World location to spawn projectile
	 * @param SpawnRotation World rotation for projectile direction
	 * @return Spawned projectile actor, or nullptr if spawn failed
	 */
	AActor* SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation);

	/** Get projectile spawn location (owner location + forward offset). */
	FVector GetProjectileSpawnLocation() const;

	/** Get projectile direction (toward target or owner's forward direction). */
	FVector GetProjectileDirection(AActor* Target) const;

	/** Get projectile speed from SkillDataAsset. */
	float GetProjectileSpeed() const;

	/** Get attack damage from SkillDataAsset. */
	float GetAttackDamage() const;

	/** Get area of effect radius from SkillDataAsset. */
	float GetAreaOfEffectRadius() const;
};
