#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "ActionRPGMagicDamageType.generated.h"

/** Marks elemental and magical skill hits as nonphysical. */
UCLASS()
class ACTIONRPG_API UActionRPGMagicDamageType : public UDamageType
{
	GENERATED_BODY()
};
