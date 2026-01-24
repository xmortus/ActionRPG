// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProgressionTypes.generated.h"

UENUM(BlueprintType)
enum class EPrimaryAttribute : uint8
{
	Strength		UMETA(DisplayName = "Strength"),
	Agility		UMETA(DisplayName = "Agility"),
	Constitution	UMETA(DisplayName = "Constitution"),
	Dexterity	UMETA(DisplayName = "Dexterity"),
	Intelligence	UMETA(DisplayName = "Intelligence"),
	Wisdom		UMETA(DisplayName = "Wisdom")
};

UENUM(BlueprintType)
enum class ESecondaryAttribute : uint8
{
	MaxHealth	UMETA(DisplayName = "Max Health"),
	MaxMana		UMETA(DisplayName = "Max Mana"),
	Armor		UMETA(DisplayName = "Armor"),
	CritChance	UMETA(DisplayName = "Crit Chance"),
	MoveSpeed	UMETA(DisplayName = "Move Speed")
};

USTRUCT(BlueprintType)
struct ACTIONRPG_API FSecondaryAttributeFormula
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float BaseValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TMap<EPrimaryAttribute, float> AttributeScaling;
};
