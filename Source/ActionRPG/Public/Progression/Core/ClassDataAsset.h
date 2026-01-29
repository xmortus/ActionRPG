// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Progression/Core/ProgressionTypes.h"
#include "Curves/CurveFloat.h"
#include "ClassDataAsset.generated.h"

/**
 * Data Asset for defining class properties and bonuses.
 */
UCLASS(BlueprintType)
class ACTIONRPG_API UClassDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UClassDataAsset();

	// Primary Data Asset Interface
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class")
	FPrimaryAssetType ClassType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class")
	FName ClassID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class")
	FText ClassName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class", meta = (MultiLine = true))
	FText ClassDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class")
	TObjectPtr<UTexture2D> ClassIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Requirements", meta = (ClampMin = "1"))
	int32 RequiredLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Requirements", meta = (ClampMin = "1"))
	int32 RequiredClassLevelToUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Requirements")
	TMap<EPrimaryAttribute, float> RequiredAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Bonuses")
	TMap<EPrimaryAttribute, float> PrimaryAttributeBonuses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Bonuses")
	TMap<ESecondaryAttribute, float> SecondaryAttributeBonuses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Progression", meta = (ClampMin = "0"))
	int32 AttributePointsPerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Progression")
	TObjectPtr<UCurveFloat> ClassExperienceCurve;
};
