#include "Combat/ActionRPGDamageTypes.h"
#include "Combat/ActionRPGPhysicalDamageType.h"

TSubclassOf<UDamageType> FActionRPGDamageTypes::GetClass(EDamageType Kind)
{
	switch (Kind)
	{
	case EDamageType::Physical: return UActionRPGPhysicalDamageType::StaticClass();
	case EDamageType::Fire: return UActionRPGFireDamageType::StaticClass();
	case EDamageType::Cold: return UActionRPGColdDamageType::StaticClass();
	case EDamageType::Lightning: return UActionRPGLightningDamageType::StaticClass();
	case EDamageType::Poison: return UActionRPGPoisonDamageType::StaticClass();
	case EDamageType::Dark: return UActionRPGDarkDamageType::StaticClass();
	case EDamageType::Holy: return UActionRPGHolyDamageType::StaticClass();
	case EDamageType::Magic: return UActionRPGMagicDamageType::StaticClass();
	default: return UDamageType::StaticClass();
	}
}

TOptional<EDamageType> FActionRPGDamageTypes::GetKind(TSubclassOf<UDamageType> DamageClass)
{
	if (!DamageClass) return {};
	if (DamageClass->IsChildOf(UActionRPGPhysicalDamageType::StaticClass())) return EDamageType::Physical;
	if (DamageClass->IsChildOf(UActionRPGFireDamageType::StaticClass())) return EDamageType::Fire;
	if (DamageClass->IsChildOf(UActionRPGColdDamageType::StaticClass())) return EDamageType::Cold;
	if (DamageClass->IsChildOf(UActionRPGLightningDamageType::StaticClass())) return EDamageType::Lightning;
	if (DamageClass->IsChildOf(UActionRPGPoisonDamageType::StaticClass())) return EDamageType::Poison;
	if (DamageClass->IsChildOf(UActionRPGDarkDamageType::StaticClass())) return EDamageType::Dark;
	if (DamageClass->IsChildOf(UActionRPGHolyDamageType::StaticClass())) return EDamageType::Holy;
	if (DamageClass->IsChildOf(UActionRPGMagicDamageType::StaticClass())) return EDamageType::Magic;
	return {};
}
