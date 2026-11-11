#include "Combat/DamageResolution.h"

FActionRPGDamageResult FActionRPGDamageResolver::Resolve(
	float RequestedDamage, float CurrentHealth, bool bAlreadyDead)
{
	FActionRPGDamageResult Result;
	Result.RemainingHealth = FMath::IsFinite(CurrentHealth) ? FMath::Max(CurrentHealth, 0.0f) : 0.0f;
	if (bAlreadyDead || !FMath::IsFinite(RequestedDamage) || RequestedDamage <= 0.0f
		|| Result.RemainingHealth <= 0.0f)
	{
		return Result;
	}

	Result.AppliedDamage = FMath::Min(RequestedDamage, Result.RemainingHealth);
	Result.RemainingHealth -= Result.AppliedDamage;
	Result.bKilled = Result.RemainingHealth <= 0.0f;
	if (Result.bKilled)
	{
		Result.RemainingHealth = 0.0f;
	}
	return Result;
}

FActionRPGDamageResult FActionRPGDamageResolver::ResolvePhysical(
	float RequestedDamage, float CurrentHealth, bool bAlreadyDead, float Armor, float ArmorMitigationScale)
{
	if (!FMath::IsFinite(Armor) || Armor < 0.0f) Armor = 0.0f;
	if (!FMath::IsFinite(ArmorMitigationScale) || ArmorMitigationScale <= 0.0f) ArmorMitigationScale = 100.0f;
	const float MitigationMultiplier = ArmorMitigationScale / (ArmorMitigationScale + Armor);
	return Resolve(RequestedDamage * MitigationMultiplier, CurrentHealth, bAlreadyDead);
}
