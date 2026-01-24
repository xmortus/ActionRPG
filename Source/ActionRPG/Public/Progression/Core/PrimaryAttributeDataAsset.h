// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Progression/Core/ProgressionTypes.h"
#include "PrimaryAttributeDataAsset.generated.h"

UCLASS(BlueprintType)
class ACTIONRPG_API UPrimaryAttributeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TMap<EPrimaryAttribute, float> BaseValues;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float MinValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	float MaxValue = 999.0f;

	float GetBaseValue(EPrimaryAttribute Attribute) const;
};
