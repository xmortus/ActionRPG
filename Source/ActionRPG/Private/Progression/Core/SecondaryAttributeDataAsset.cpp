// Copyright Epic Games, Inc. All Rights Reserved.

#include "Progression/Core/SecondaryAttributeDataAsset.h"

bool USecondaryAttributeDataAsset::GetFormula(ESecondaryAttribute Attribute, FSecondaryAttributeFormula& OutFormula) const
{
	if (const FSecondaryAttributeFormula* Formula = Formulas.Find(Attribute))
	{
		OutFormula = *Formula;
		return true;
	}

	return false;
}
