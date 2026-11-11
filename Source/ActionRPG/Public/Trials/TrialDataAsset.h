// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TrialDataAsset.generated.h"

class UItemDataAsset;
class USkillBase;
class USkillDataAsset;

USTRUCT(BlueprintType)
struct ACTIONRPG_API FTrialItemReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards")
	TObjectPtr<UItemDataAsset> Item;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards", meta = (ClampMin = "1"))
	int32 Quantity = 1;
};

USTRUCT(BlueprintType)
struct ACTIONRPG_API FTrialSkillReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards")
	TSubclassOf<USkillBase> SkillClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards")
	TObjectPtr<USkillDataAsset> SkillData;
};

/** One mutually exclusive reward package. A trial uses either two choices or its legacy fixed reward. */
USTRUCT(BlueprintType)
struct ACTIONRPG_API FTrialRewardChoice
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards")
	FText ChoiceName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards", meta = (ClampMin = "0.0"))
	float ExperienceReward = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards")
	FTrialItemReward ItemReward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards")
	FTrialSkillReward SkillReward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards", meta = (ClampMin = "0"))
	int32 AttributePointReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards", meta = (ClampMin = "0"))
	int32 SkillPointReward = 0;

	/** Book-style acquired Endurance, capped by persistent Constitution. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards", meta = (ClampMin = "0"))
	int32 EnduranceReward = 0;
};

UENUM(BlueprintType)
enum class ETrialType : uint8
{
	Combat,
	Skill,
	Exploration,
	Crafting
};

USTRUCT(BlueprintType)
struct ACTIONRPG_API FTrialObjectiveDefinition
{
	GENERATED_BODY()

	// Gameplay code reports progress using this stable objective identifier.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial")
	FName ObjectiveId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial", meta = (ClampMin = "1"))
	int32 TargetCount = 1;
};

/** Data-only definition of a trial. Gameplay rules and rewards live in UTrialComponent. */
UCLASS(BlueprintType)
class ACTIONRPG_API UTrialDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial")
	FName TrialId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial")
	FText TrialName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial")
	ETrialType TrialType = ETrialType::Combat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial")
	TArray<FTrialObjectiveDefinition> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Requirements", meta = (ClampMin = "1"))
	int32 RequiredPlayerLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Requirements")
	TArray<TObjectPtr<UTrialDataAsset>> RequiredTrials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial")
	bool bRepeatable = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards", meta = (ClampMin = "0.0"))
	float ExperienceReward = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards")
	FTrialItemReward ItemReward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards")
	FTrialSkillReward SkillReward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards", meta = (ClampMin = "0"))
	int32 AttributePointReward = 0;

	/** Free skill points; independent of free attribute points and XP. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards", meta = (ClampMin = "0"))
	int32 SkillPointReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards", meta = (ClampMin = "0"))
	int32 EnduranceReward = 0;

	/** Exactly two choices when used. Each option replaces the fixed reward fields above. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trial|Rewards")
	TArray<FTrialRewardChoice> RewardChoices;
};
