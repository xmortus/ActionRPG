#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Progression/Core/ProgressionTypes.h"
#include "StatusEffectDataAsset.generated.h"

UENUM(BlueprintType)
enum class EStatusEffectStackPolicy : uint8
{
	Refresh,
	Stack,
	Ignore
};

UCLASS(BlueprintType)
class ACTIONRPG_API UStatusEffectDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	FName EffectID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (ClampMin = "0.01"))
	float DurationSeconds = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (ClampMin = "0.01"))
	float TickIntervalSeconds = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	float HealthPerTick = 0.0f;
	/** Added to each positive health tick for every effective Wisdom point on the effect source. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Healing", meta = (ClampMin = "0.0"))
	float HealthPerTickPerWisdom = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	float ManaPerTick = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	float StaminaPerTick = 0.0f;
	/** Temporary primary bonuses per stack; removed when the effect ends. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TMap<EPrimaryAttribute, float> PrimaryAttributeBonuses;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	EStatusEffectStackPolicy StackPolicy = EStatusEffectStackPolicy::Refresh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (ClampMin = "1"))
	int32 MaxStacks = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	bool bRemoveOnDeath = true;
};
