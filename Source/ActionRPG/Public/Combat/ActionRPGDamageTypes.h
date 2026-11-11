#pragma once

#include "CoreMinimal.h"
#include "Combat/ActionRPGMagicDamageType.h"
#include "Skills/Core/SkillTypes.h"
#include "ActionRPGDamageTypes.generated.h"

UCLASS() class ACTIONRPG_API UActionRPGFireDamageType : public UActionRPGMagicDamageType { GENERATED_BODY() };
UCLASS() class ACTIONRPG_API UActionRPGColdDamageType : public UActionRPGMagicDamageType { GENERATED_BODY() };
UCLASS() class ACTIONRPG_API UActionRPGLightningDamageType : public UActionRPGMagicDamageType { GENERATED_BODY() };
UCLASS() class ACTIONRPG_API UActionRPGPoisonDamageType : public UActionRPGMagicDamageType { GENERATED_BODY() };
UCLASS() class ACTIONRPG_API UActionRPGDarkDamageType : public UActionRPGMagicDamageType { GENERATED_BODY() };
UCLASS() class ACTIONRPG_API UActionRPGHolyDamageType : public UActionRPGMagicDamageType { GENERATED_BODY() };

/** Maps skill data to Unreal's damage events and back without affecting untyped hits. */
struct ACTIONRPG_API FActionRPGDamageTypes
{
	static TSubclassOf<UDamageType> GetClass(EDamageType Kind);
	static TOptional<EDamageType> GetKind(TSubclassOf<UDamageType> DamageClass);
};
