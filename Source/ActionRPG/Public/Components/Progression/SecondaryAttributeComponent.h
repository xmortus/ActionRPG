// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Progression/Core/ProgressionTypes.h"
#include "Items/Core/ItemTypes.h"
#include "Skills/Core/SkillTypes.h"
#include "TimerManager.h"
#include "SecondaryAttributeComponent.generated.h"

class USecondaryAttributeDataAsset;
class UAttributeComponent;
class UAcquiredAttributeComponent;
class UEquipmentComponent;
class UEquipmentItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSecondaryAttributeChanged, ESecondaryAttribute, Attribute, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCurrentSecondaryAttributeChanged, ESecondaryAttribute, Attribute, float, NewValue, float, OldValue);

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API USecondaryAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USecondaryAttributeComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void RecalculateSecondaryAttributes();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
	float GetSecondaryAttribute(ESecondaryAttribute Attribute) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes|Combat")
	float GetDamageResistancePercent(EDamageType DamageType) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
	float GetCurrentSecondaryAttribute(ESecondaryAttribute Attribute) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetCurrentSecondaryAttribute(ESecondaryAttribute Attribute, float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ModifyCurrentSecondaryAttribute(ESecondaryAttribute Attribute, float Delta);

	UFUNCTION(BlueprintPure, Category = "Attributes|Resources")
	bool CanSpendResource(ESecondaryAttribute Attribute, float Amount) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes|Resources")
	bool TrySpendResource(ESecondaryAttribute Attribute, float Amount);

	UFUNCTION(BlueprintPure, Category = "Attributes|Resources")
	bool CanSpendResources(float ManaCost, float StaminaCost) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes|Resources")
	bool TrySpendResources(float ManaCost, float StaminaCost);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Resources")
	bool RestoreResource(ESecondaryAttribute Attribute, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetClassBonuses(const TMap<ESecondaryAttribute, float>& Bonuses);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Regeneration")
	void SetHealthRegenerationSuppressed(bool bSuppressed);

	UFUNCTION(BlueprintPure, Category = "Attributes|Regeneration")
	float GetRegenerationRate(ESecondaryAttribute Attribute) const;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnSecondaryAttributeChanged OnSecondaryAttributeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnCurrentSecondaryAttributeChanged OnCurrentSecondaryAttributeChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TObjectPtr<USecondaryAttributeDataAsset> SecondaryAttributeDataAsset;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	TMap<ESecondaryAttribute, float> SecondaryAttributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes|Combat")
	TMap<EDamageType, float> DamageResistances;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	TMap<ESecondaryAttribute, float> CurrentSecondaryAttributes;

private:
#if WITH_DEV_AUTOMATION_TESTS
	friend class FResourceRegenerationTest;
#endif

	UPROPERTY()
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY()
	TObjectPtr<UAcquiredAttributeComponent> AcquiredAttributeComponent;

	UFUNCTION()
	void OnPrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue);

	UFUNCTION()
	void OnAcquiredAttributeChanged(EAcquiredAttribute Attribute, int32 NewValue, int32 Cap);

	UFUNCTION()
	void OnEquipmentChanged(EEquipmentSlot Slot, UEquipmentItem* Item);

	void InitializeCurrentFromMax();
	void UpdateRegenerationTimer(ESecondaryAttribute Attribute, bool bResetDelay);
	void RegenerateResource(ESecondaryAttribute Attribute);
	float GetRegenerationDelay(ESecondaryAttribute Attribute) const;
	FTimerHandle* GetRegenerationTimer(ESecondaryAttribute Attribute);

	static constexpr float RegenerationInterval = 0.1f;
	FTimerHandle HealthRegenerationTimer;
	FTimerHandle StaminaRegenerationTimer;
	FTimerHandle ManaRegenerationTimer;
	bool bHealthRegenerationSuppressed = false;

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	TMap<ESecondaryAttribute, float> ClassBonuses;
};
