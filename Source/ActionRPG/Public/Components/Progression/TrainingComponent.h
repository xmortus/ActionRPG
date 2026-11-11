#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Progression/Core/ProgressionTypes.h"
#include "TrainingComponent.generated.h"

class UAttributeComponent;

USTRUCT(BlueprintType)
struct ACTIONRPG_API FAttributeTrainingRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training")
	EPrimaryAttribute Attribute = EPrimaryAttribute::Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training")
	FName Activity = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", meta = (ClampMin = "1"))
	int32 PracticePerGain = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", meta = (ClampMin = "1"))
	int32 AttributeGain = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", meta = (ClampMin = "0"))
	float ActivityCooldownSeconds = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", meta = (ClampMin = "0"))
	float TargetCooldownSeconds = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training")
	bool bRequiresTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", meta = (ClampMin = "0"))
	float WindowSeconds = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Training", meta = (ClampMin = "0"))
	int32 MaxPracticePerWindow = 10;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnAttributePractice,
	EPrimaryAttribute, Attribute, FName, Activity, int32, Progress, int32, Required, bool, bGrantedGrowth);

/** Converts validated gameplay practice into permanent attribute modifier sources. */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UTrainingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTrainingComponent();

	/** Call after a meaningful gameplay action succeeds. A rule must explicitly allow a null target. */
	UFUNCTION(BlueprintCallable, Category = "Progression|Training")
	bool RecordPractice(EPrimaryAttribute Attribute, FName Activity, AActor* Target);

	UFUNCTION(BlueprintPure, Category = "Progression|Training")
	int32 GetPracticeProgress(EPrimaryAttribute Attribute, FName Activity) const;

	UFUNCTION(BlueprintPure, Category = "Progression|Training")
	int32 GetEarnedTrainingGain(EPrimaryAttribute Attribute) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Progression|Training")
	TArray<FAttributeTrainingRule> Rules;

	UPROPERTY(BlueprintAssignable, Category = "Progression|Training")
	FOnAttributePractice OnAttributePractice;

private:
	struct FPracticeState
	{
		int32 Progress = 0;
		int32 AcceptedInWindow = 0;
		double WindowStart = 0.0;
		double LastActivityTime = 0.0;
		bool bHasActivityTime = false;
		TMap<uint32, double> LastTargetTimes;
	};

	TMap<FName, FPracticeState> PracticeStates;
	TMap<EPrimaryAttribute, int32> EarnedGains;

	static FName MakeRuleKey(EPrimaryAttribute Attribute, FName Activity);
	static FName MakeSourceId(EPrimaryAttribute Attribute);
};
