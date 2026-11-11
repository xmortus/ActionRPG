// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QuestDataAsset.generated.h"

class UItemDataAsset;

/** A quest is separate from a Trial: collect a required item, optionally defeat an enemy, then return to the marker. */
UCLASS(BlueprintType)
class ACTIONRPG_API UQuestDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName QuestID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FText QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objectives")
	TObjectPtr<UItemDataAsset> RequiredItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objectives", meta = (ClampMin = "1"))
	int32 RequiredItemCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objectives")
	FName OptionalEnemyID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objectives", meta = (ClampMin = "0"))
	int32 OptionalEnemyDefeatCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Rewards", meta = (ClampMin = "0.0"))
	float ExperienceReward = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Rewards")
	TObjectPtr<UItemDataAsset> RewardItem;

	/** Paid only if the optional enemy objective is complete before turn-in. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Rewards", meta = (ClampMin = "0"))
	int32 OptionalSkillPointReward = 0;
};
