// Copyright Epic Games, Inc. All Rights Reserved.

#include "Progression/Core/PrimaryAttributeDataAsset.h"

float UPrimaryAttributeDataAsset::GetBaseValue(EPrimaryAttribute Attribute) const
{
	if (const float* Value = BaseValues.Find(Attribute))
	{
		return *Value;
	}

	return 0.0f;
}
