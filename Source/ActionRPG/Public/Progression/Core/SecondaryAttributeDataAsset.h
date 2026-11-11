// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Progression/Core/ProgressionTypes.h"
#include "Skills/Core/SkillTypes.h"
#include "SecondaryAttributeDataAsset.generated.h"

UCLASS(BlueprintType)
class ACTIONRPG_API USecondaryAttributeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TMap<ESecondaryAttribute, FSecondaryAttributeFormula> Formulas;

	/** Physical damage multiplier is Scale / (Scale + Armor). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "1.0"))
	float ArmorMitigationScale = 100.0f;

	/** Percent of each typed hit prevented. Empty by default; untyped hits bypass this map. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TMap<EDamageType, float> DamageResistancePercent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Regeneration", meta = (ClampMin = "0.0"))
	float HealthRegenerationPerSecond = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Regeneration", meta = (ClampMin = "0.0"))
	float HealthRegenerationDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Regeneration", meta = (ClampMin = "0.0"))
	float StaminaRegenerationPerSecond = 20.0f;

	/** Additional Stamina restored per second for each earned Endurance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Regeneration", meta = (ClampMin = "0.0"))
	float StaminaRegenerationPerEndurance = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Regeneration", meta = (ClampMin = "0.0"))
	float StaminaRegenerationDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Regeneration", meta = (ClampMin = "0.0"))
	float ManaRegenerationPerSecond = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Regeneration", meta = (ClampMin = "0.0"))
	float ManaRegenerationDelay = 2.0f;

	bool GetFormula(ESecondaryAttribute Attribute, FSecondaryAttributeFormula& OutFormula) const;
};
