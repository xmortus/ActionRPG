// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Progression/Core/ProgressionTypes.h"
#include "Curves/CurveFloat.h"
#include "ClassDataAsset.generated.h"

class USkillBase;
class USkillDataAsset;

USTRUCT(BlueprintType)
struct ACTIONRPG_API FClassSkillLevelRequirement
{
	GENERATED_BODY()

	/** Class level earned after this requirement is met. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Requirements", meta = (ClampMin = "1"))
	int32 TargetClassLevel = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Requirements")
	FName SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Requirements", meta = (ClampMin = "1"))
	int32 RequiredSkillLevel = 1;
};

/** A skill granted while this class is selected and has reached the specified level. */
USTRUCT(BlueprintType)
struct ACTIONRPG_API FClassGrantedSkill
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Skills", meta = (ClampMin = "1"))
	int32 TargetClassLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Skills")
	TSubclassOf<USkillBase> SkillClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Skills")
	TObjectPtr<USkillDataAsset> SkillData;
};

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

	/** Optional use-based skill milestones required before earning a class level. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Requirements")
	TArray<FClassSkillLevelRequirement> SkillLevelRequirements;

	/** Class-owned skills are usable while selected; their earned skill rank survives class removal. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Skills")
	TArray<FClassGrantedSkill> GrantedSkills;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Bonuses")
	TMap<EPrimaryAttribute, float> PrimaryAttributeBonuses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Bonuses")
	TMap<ESecondaryAttribute, float> SecondaryAttributeBonuses;

	/** Permanent attribute gain for each earned class level, including level one. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Progression")
	TMap<EPrimaryAttribute, float> AttributeGainsPerLevel;

	/** Basic and Specialized award 4 free attribute points per level; Advanced awards 5. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Progression")
	ECharacterClassTier ClassTier = ECharacterClassTier::Basic;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class|Progression")
	int32 GetFreeAttributePointsPerLevel() const;

	/** Legacy value retained for existing assets; ClassTier determines ordinary level rewards. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Class|Legacy")
	int32 AttributePointsPerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class|Progression")
	TObjectPtr<UCurveFloat> ClassExperienceCurve;
};
