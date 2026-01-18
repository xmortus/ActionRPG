// Copyright Epic Games, Inc. All Rights Reserved.

#include "Items/BeastCoreItem.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Core/SkillDataAsset.h"

bool UBeastCoreItem::CanUse() const
{
	if (!USkillItem::CanUse())
	{
		return false;
	}

	// Additional BeastCore requirement checks will be added in Phase 4.
	UE_LOG(LogTemp, Verbose, TEXT("BeastCoreItem::CanUse - Requirements check (stubbed, Phase 4)"));
	return true;
}
