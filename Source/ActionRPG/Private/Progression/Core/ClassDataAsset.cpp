// Copyright Epic Games, Inc. All Rights Reserved.

#include "Progression/Core/ClassDataAsset.h"

UClassDataAsset::UClassDataAsset()
{
	ClassType = FPrimaryAssetType("Class");
	ClassID = NAME_None;
	ClassName = FText::GetEmpty();
	ClassDescription = FText::GetEmpty();
	ClassIcon = nullptr;
	RequiredLevel = 1;
	RequiredClassLevelToUnlock = 1;
	RequiredAttributes = {};
	PrimaryAttributeBonuses = {};
	SecondaryAttributeBonuses = {};
	AttributePointsPerLevel = 0;
	ClassExperienceCurve = nullptr;
}

int32 UClassDataAsset::GetFreeAttributePointsPerLevel() const
{
	return ClassTier == ECharacterClassTier::Advanced ? 5 : 4;
}

FPrimaryAssetId UClassDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ClassType, ClassID);
}
