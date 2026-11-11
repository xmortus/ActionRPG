// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrialComponent.generated.h"

class UTrialDataAsset;

USTRUCT(BlueprintType)
struct ACTIONRPG_API FTrialRecord
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trial")
	TObjectPtr<UTrialDataAsset> Trial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trial")
	TArray<int32> ObjectiveCounts;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trial")
	int32 CompletionCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trial")
	int32 FailureCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trial")
	bool bActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trial")
	int32 LastRewardChoiceIndex = INDEX_NONE;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrialStateChanged, UTrialDataAsset*, Trial);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTrialProgressChanged, UTrialDataAsset*, Trial, float, Progress);

/** Tracks one active trial and awards its XP only after every objective is complete. */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UTrialComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTrialComponent();

	/** Rejects malformed definitions, unmet prerequisites, and completed one-time trials. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trials")
	bool CanStartTrial(const UTrialDataAsset* Trial) const;

	/** Starts a trial and resets its objective counters. */
	UFUNCTION(BlueprintCallable, Category = "Trials")
	bool StartTrial(UTrialDataAsset* Trial);

	/** Reports a gameplay event against one objective of the active trial. */
	UFUNCTION(BlueprintCallable, Category = "Trials")
	bool AdvanceObjective(FName ObjectiveId, int32 Amount = 1);

	/** Credits a successful skill use only when a Skill trial objective ID matches the skill ID. */
	bool ReportSkillActivated(FName SkillId);

	/** Credits a Combat trial only for a real enemy death with a matching enemy ID. */
	bool ReportEnemyDefeated(FName EnemyId, AActor* DefeatedEnemy);

	/** Credits a successfully crafted recipe with a matching stable recipe ID. */
	bool ReportRecipeCrafted(FName RecipeId);

	/** Ends the active attempt without granting rewards. */
	UFUNCTION(BlueprintCallable, Category = "Trials")
	bool FailActiveTrial();

	/** Retries completion if objectives are done but an XP reward could not be granted. */
	UFUNCTION(BlueprintCallable, Category = "Trials")
	bool TryCompleteActiveTrial();

	/** Selects one of a completed trial's two reward packages and claims it if possible. */
	UFUNCTION(BlueprintCallable, Category = "Trials")
	bool ChooseActiveTrialReward(int32 ChoiceIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trials")
	int32 GetActiveRewardChoiceIndex() const { return ActiveRewardChoiceIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trials")
	float GetTrialProgress(const UTrialDataAsset* Trial) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trials")
	int32 GetCompletionCount(const UTrialDataAsset* Trial) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trials")
	UTrialDataAsset* GetActiveTrial() const { return ActiveTrial; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Trials")
	const TArray<FTrialRecord>& GetTrialRecords() const { return TrialRecords; }

	UPROPERTY(BlueprintAssignable, Category = "Trials|Events")
	FOnTrialStateChanged OnTrialStarted;

	UPROPERTY(BlueprintAssignable, Category = "Trials|Events")
	FOnTrialStateChanged OnTrialCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Trials|Events")
	FOnTrialStateChanged OnTrialFailed;

	UPROPERTY(BlueprintAssignable, Category = "Trials|Events")
	FOnTrialProgressChanged OnTrialProgressChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trials")
	TArray<FTrialRecord> TrialRecords;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trials")
	TObjectPtr<UTrialDataAsset> ActiveTrial;

private:
	const FTrialRecord* FindRecord(const UTrialDataAsset* Trial) const;
	FTrialRecord* FindRecord(UTrialDataAsset* Trial);
	TSet<TWeakObjectPtr<AActor>> CreditedEnemies;
	int32 ActiveRewardChoiceIndex = INDEX_NONE;
};
