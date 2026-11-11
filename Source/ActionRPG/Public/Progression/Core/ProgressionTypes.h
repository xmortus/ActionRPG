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
enum class ECharacterClassTier : uint8
{
	Basic UMETA(DisplayName = "Basic"),
	Specialized UMETA(DisplayName = "Specialized"),
	Advanced UMETA(DisplayName = "Advanced")
};

UENUM(BlueprintType)
enum class ESecondaryAttribute : uint8
{
	MaxHealth	UMETA(DisplayName = "Max Health"),
	MaxMana		UMETA(DisplayName = "Max Mana"),
	MaxStamina	UMETA(DisplayName = "Max Stamina"),
	Armor		UMETA(DisplayName = "Armor"),
	CritChance	UMETA(DisplayName = "Crit Chance"),
	MoveSpeed	UMETA(DisplayName = "Move Speed")
};

/** Earned attributes shown on Status, distinct from calculated combat stats. */
UENUM(BlueprintType)
enum class EAcquiredAttribute : uint8
{
	Endurance UMETA(DisplayName = "Endurance")
};

USTRUCT(BlueprintType)
struct ACTIONRPG_API FSecondaryAttributeFormula
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float BaseValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TMap<EPrimaryAttribute, float> AttributeScaling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TMap<EAcquiredAttribute, float> AcquiredAttributeScaling;
};
