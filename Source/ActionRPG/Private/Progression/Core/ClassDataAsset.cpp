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
	RequiredAttributes = {};
	PrimaryAttributeBonuses = {};
	SecondaryAttributeBonuses = {};
	AttributePointsPerLevel = 0;
	ClassExperienceCurve = nullptr;
}

FPrimaryAssetId UClassDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ClassType, ClassID);
}
