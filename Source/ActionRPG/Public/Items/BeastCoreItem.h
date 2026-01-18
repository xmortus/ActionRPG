// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/SkillItem.h"
#include "BeastCoreItem.generated.h"

/**
 * Beast Core item that unlocks a skill if requirements are met.
 * Requirement checks are stubbed until Phase 4 systems are available.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UBeastCoreItem : public USkillItem
{
	GENERATED_BODY()

public:
	virtual bool CanUse() const override;
};
