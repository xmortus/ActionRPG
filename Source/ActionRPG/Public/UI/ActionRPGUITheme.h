// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UWidgetTree;

namespace ActionRPGUITheme
{
	inline FLinearColor Panel() { return FLinearColor(0.035f, 0.055f, 0.085f, 0.97f); }
	inline FLinearColor Surface() { return FLinearColor(0.07f, 0.10f, 0.15f, 1.0f); }
	inline FLinearColor Button() { return FLinearColor(0.20f, 0.26f, 0.33f, 1.0f); }
	inline FLinearColor Accent() { return FLinearColor(0.12f, 0.38f, 0.28f, 1.0f); }
	inline FLinearColor Slot() { return FLinearColor(0.04f, 0.07f, 0.11f, 1.0f); }
	inline FLinearColor SlotHover() { return FLinearColor(0.10f, 0.15f, 0.21f, 1.0f); }

	// Styles the authored Blueprint chrome without changing its bindings, layout, or item icons.
	ACTIONRPG_API void StyleBlueprintWidgetTree(UWidgetTree* WidgetTree, bool bCompact = false);
}
