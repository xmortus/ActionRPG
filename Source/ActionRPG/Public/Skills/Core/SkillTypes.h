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
	Utility			UMETA(DisplayName = "Utility"),
	Movement		UMETA(DisplayName = "Movement")
};

UENUM(BlueprintType)
enum class ESkillCategory : uint8
{
	Combat			UMETA(DisplayName = "Combat"),
	Support			UMETA(DisplayName = "Support"),
	Movement		UMETA(DisplayName = "Movement"),
	Passive			UMETA(DisplayName = "Passive")
};
