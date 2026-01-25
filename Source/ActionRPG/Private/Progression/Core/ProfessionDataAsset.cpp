// Copyright Epic Games, Inc. All Rights Reserved.

#include "Progression/Core/ProfessionDataAsset.h"

UProfessionDataAsset::UProfessionDataAsset()
{
	ProfessionType = FPrimaryAssetType("Profession");
	ProfessionID = NAME_None;
	ProfessionName = FText::GetEmpty();
	ProfessionDescription = FText::GetEmpty();
	ProfessionIcon = nullptr;
	RequiredLevel = 1;
	RequiredAttributes = {};
	PrimaryAttributeBonuses = {};
	SecondaryAttributeBonuses = {};
}

FPrimaryAssetId UProfessionDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ProfessionType, ProfessionID);
}
