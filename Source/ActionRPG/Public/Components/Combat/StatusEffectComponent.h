#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/StatusEffectDataAsset.h"
#include "StatusEffectComponent.generated.h"

class AActor;

USTRUCT(BlueprintType)
struct FActiveStatusEffect
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UStatusEffectDataAsset> Definition;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> Source;
	UPROPERTY(BlueprintReadOnly)
	int32 Stacks = 1;
	UPROPERTY(BlueprintReadOnly)
	float RemainingSeconds = 0.0f;
	UPROPERTY(BlueprintReadOnly)
	FName AttributeSourceId;
	float TimeUntilTick = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatusEffectsChanged);

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class ACTIONRPG_API UStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UStatusEffectComponent();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UFUNCTION(BlueprintCallable, Category = "Combat|Effects")
	bool ApplyEffect(UStatusEffectDataAsset* Definition, AActor* Source);
	UFUNCTION(BlueprintCallable, Category = "Combat|Effects")
	bool RemoveEffect(FName EffectID);
	UFUNCTION(BlueprintCallable, Category = "Combat|Effects")
	void RemoveEffectsOnDeath();
	UFUNCTION(BlueprintPure, Category = "Combat|Effects")
	const TArray<FActiveStatusEffect>& GetActiveEffects() const { return ActiveEffects; }
	/** Signed health change per tick, including stacks and opt-in source Wisdom scaling. */
	float GetEffectiveHealthPerTick(const FActiveStatusEffect& Effect) const;
	UPROPERTY(BlueprintAssignable, Category = "Combat|Effects")
	FOnStatusEffectsChanged OnStatusEffectsChanged;
	/** Fired only when a displayed whole second changes, without per-frame UI polling. */
	UPROPERTY(BlueprintAssignable, Category = "Combat|Effects")
	FOnStatusEffectsChanged OnStatusEffectTimeChanged;
private:
#if WITH_DEV_AUTOMATION_TESTS
	friend class FStatusEffectTest;
	friend class FRemainingAttributeTrainingTest;
	friend class FWisdomHealingScalingTest;
#endif
	void AdvanceEffects();
	void ApplyPeriodicValues(const FActiveStatusEffect& Effect);
	void UpdateAttributeBonuses(const FActiveStatusEffect& Effect);
	void ClearAttributeBonuses(FName SourceId);
	UPROPERTY(VisibleInstanceOnly, Category = "Combat|Effects")
	TArray<FActiveStatusEffect> ActiveEffects;
	FTimerHandle EffectTimer;
	static constexpr float UpdateInterval = 0.1f;
};
