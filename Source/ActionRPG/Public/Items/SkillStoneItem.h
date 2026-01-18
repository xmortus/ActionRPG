// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/SkillItem.h"
#include "SkillStoneItem.generated.h"

/**
 * Skill Stone item that unlocks a skill without requirements.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API USkillStoneItem : public USkillItem
{
	GENERATED_BODY()

public:
	virtual bool CanUse() const override;
};
