// Copyright Epic Games, Inc. All Rights Reserved.

#include "Trials/TrialDataAsset.h"

FPrimaryAssetId UTrialDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType(TEXT("Trial")), TrialId.IsNone() ? GetFName() : TrialId);
}
