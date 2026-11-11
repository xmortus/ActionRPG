#pragma once

#include "CoreMinimal.h"

/** Shared result for a hit against either actor-owned or component-owned health. */
struct ACTIONRPG_API FActionRPGDamageResult
{
	float AppliedDamage = 0.0f;
	float RemainingHealth = 0.0f;
	bool bKilled = false;
};

/** Resolves the default, untyped damage path used by existing skills and projectiles. */
class ACTIONRPG_API FActionRPGDamageResolver
{
public:
	static FActionRPGDamageResult Resolve(float RequestedDamage, float CurrentHealth, bool bAlreadyDead);
	static FActionRPGDamageResult ResolvePhysical(float RequestedDamage, float CurrentHealth,
		bool bAlreadyDead, float Armor, float ArmorMitigationScale);
};
