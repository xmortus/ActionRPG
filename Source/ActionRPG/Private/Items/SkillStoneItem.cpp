// Copyright Epic Games, Inc. All Rights Reserved.

#include "Items/SkillStoneItem.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Core/SkillDataAsset.h"

bool USkillStoneItem::CanUse() const
{
	if (!UItemBase::CanUse())
	{
		return false;
	}

	AActor* OwnerActor = nullptr;
	USkillManagerComponent* SkillManager = FindSkillManager(OwnerActor);
	if (!SkillManager || !OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillStoneItem::CanUse - SkillManager or OwnerActor missing"));
		return false;
	}

	USkillDataAsset* SkillData = nullptr;
	TSubclassOf<USkillBase> SkillClass;
	if (!ResolveSkillDefinition(SkillData, SkillClass))
	{
		return false;
	}

	if (IsSkillAlreadyUnlocked(SkillManager, SkillData, SkillClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillStoneItem::CanUse - Skill already unlocked"));
		return false;
	}

	// Skill stones bypass requirement checks.
	return true;
}
