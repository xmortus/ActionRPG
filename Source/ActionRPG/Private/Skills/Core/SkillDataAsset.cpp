// Copyright Epic Games, Inc. All Rights Reserved.

#include "Skills/Core/SkillDataAsset.h"

USkillDataAsset::USkillDataAsset()
{
	// Set default values
	SkillType = FPrimaryAssetType("Skill");
	SkillID = NAME_None;
	SkillName = FText::GetEmpty();
	SkillDescription = FText::GetEmpty();
	SkillIcon = nullptr;
	Type = ESkillType::Utility;
	Category = ESkillCategory::Combat;
	CooldownDuration = 0.0f;
	ManaCost = 0.0f;
	StaminaCost = 0.0f;
	CastTime = 0.0f;
	Range = 0.0f;
}

FPrimaryAssetId USkillDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(SkillType, SkillID);
}
