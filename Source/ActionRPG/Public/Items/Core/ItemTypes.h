// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemTypes.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Consumable		UMETA(DisplayName = "Consumable"),
	Equipment		UMETA(DisplayName = "Equipment"),
	SkillItem		UMETA(DisplayName = "Skill Item"),
	SkillStone		UMETA(DisplayName = "Skill Stone"),
	BeastCore		UMETA(DisplayName = "Beast Core"),
	Misc			UMETA(DisplayName = "Miscellaneous")
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common			UMETA(DisplayName = "Common"),
	Uncommon		UMETA(DisplayName = "Uncommon"),
	Rare			UMETA(DisplayName = "Rare"),
	Epic			UMETA(DisplayName = "Epic"),
	Legendary		UMETA(DisplayName = "Legendary")
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	None		UMETA(DisplayName = "None"),
	MainHand	UMETA(DisplayName = "Main Hand"),
	Offhand		UMETA(DisplayName = "Offhand"),
	Head		UMETA(DisplayName = "Head"),
	Chest		UMETA(DisplayName = "Chest"),
	Legs		UMETA(DisplayName = "Legs"),
	Hands		UMETA(DisplayName = "Hands"),
	Feet		UMETA(DisplayName = "Feet"),
	Ring		UMETA(DisplayName = "Ring"),
	Amulet		UMETA(DisplayName = "Amulet")
};

