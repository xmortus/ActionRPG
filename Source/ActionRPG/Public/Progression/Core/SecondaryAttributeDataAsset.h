// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Progression/Core/ProgressionTypes.h"
#include "SecondaryAttributeDataAsset.generated.h"

UCLASS(BlueprintType)
class ACTIONRPG_API USecondaryAttributeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TMap<ESecondaryAttribute, FSecondaryAttributeFormula> Formulas;

	bool GetFormula(ESecondaryAttribute Attribute, FSecondaryAttributeFormula& OutFormula) const;
};
