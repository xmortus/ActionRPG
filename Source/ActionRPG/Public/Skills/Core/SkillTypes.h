// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SkillTypes.generated.h"

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	Melee			UMETA(DisplayName = "Melee"),
	Ranged			UMETA(DisplayName = "Ranged"),
	Magic			UMETA(DisplayName = "Magic"),
	Utility			UMETA(DisplayName = "Utility")
};

UENUM(BlueprintType)
enum class ESkillCategory : uint8
{
	Combat			UMETA(DisplayName = "Combat"),
	Support			UMETA(DisplayName = "Support"),
	Movement		UMETA(DisplayName = "Movement"),
	Passive			UMETA(DisplayName = "Passive")
};

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Physical		UMETA(DisplayName = "Physical"),
	Fire			UMETA(DisplayName = "Fire"),
	Cold			UMETA(DisplayName = "Cold"),
	Lightning		UMETA(DisplayName = "Lightning"),
	Poison			UMETA(DisplayName = "Poison"),
	Magic			UMETA(DisplayName = "Magic"),
	Dark			UMETA(DisplayName = "Dark"),
	Holy			UMETA(DisplayName = "Holy")
};