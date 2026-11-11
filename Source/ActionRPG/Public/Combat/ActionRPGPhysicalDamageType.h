#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "ActionRPGPhysicalDamageType.generated.h"

/** Marks a hit as physical so the target can apply Armor. */
UCLASS()
class ACTIONRPG_API UActionRPGPhysicalDamageType : public UDamageType
{
	GENERATED_BODY()
};
