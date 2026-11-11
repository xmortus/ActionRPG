// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Progression/Core/ProgressionTypes.h"
#include "Items/Core/ItemTypes.h"
#include "AttributeComponent.generated.h"

class UPrimaryAttributeDataAsset;
class UEquipmentComponent;
class UEquipmentItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPrimaryAttributeChanged, EPrimaryAttribute, Attribute, float, NewValue, float, OldValue);

USTRUCT(BlueprintType)
struct ACTIONRPG_API FAttributeSourceContribution
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Sources")
	FName SourceId;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Sources")
	float Amount = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Sources")
	bool bPersistent = false;
};

USTRUCT(BlueprintType)
struct ACTIONRPG_API FAttributeBreakdown
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Sources")
	float Base = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Sources")
	float Allocated = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Sources")
	TArray<FAttributeSourceContribution> Sources;

	/** Final total after the attribute data asset's minimum/maximum limits. */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Sources")
	float FinalTotal = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Sources")
	float LimitAdjustment = 0.0f;
};

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void InitializeFromDataAsset();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
	float GetAttribute(EPrimaryAttribute Attribute) const;

	/** Base, allocated points, and permanent earned grants; excludes equipment and timed effects. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
	float GetPersistentAttribute(EPrimaryAttribute Attribute) const;

	/** Replaces all bonuses from one stable source. Empty bonuses remove the source. */
	UFUNCTION(BlueprintCallable, Category = "Attributes|Modifiers")
	bool SetModifierSource(FName SourceId, const TMap<EPrimaryAttribute, float>& Bonuses, bool bPersistent);

	UFUNCTION(BlueprintCallable, Category = "Attributes|Modifiers")
	bool RemoveModifierSource(FName SourceId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes|Modifiers")
	float GetModifierSourceBonus(FName SourceId, EPrimaryAttribute Attribute) const;

	/** Read-only explanation of one final attribute value, including temporary sources and limits. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes|Sources")
	FAttributeBreakdown GetAttributeBreakdown(EPrimaryAttribute Attribute) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetAttribute(EPrimaryAttribute Attribute, float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ModifyAttribute(EPrimaryAttribute Attribute, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetClassBonuses(const TMap<EPrimaryAttribute, float>& Bonuses);

	/** Applies removable selected-class bonuses and permanent earned level gains together. */
	void SetClassProgressionBonuses(const TMap<EPrimaryAttribute, float>& SelectedBonuses,
		const TMap<EPrimaryAttribute, float>& EarnedBonuses);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void AddAttributePoints(int32 Points);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
	int32 GetUnspentAttributePoints() const { return UnspentAttributePoints; }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool AllocateAttributePoints(EPrimaryAttribute Attribute, int32 Points);

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnPrimaryAttributeChanged OnPrimaryAttributeChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TObjectPtr<UPrimaryAttributeDataAsset> AttributeDataAsset;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	TMap<EPrimaryAttribute, float> Attributes;

	float ClampAttributeValue(float Value) const;

private:
	UPROPERTY()
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UFUNCTION()
	void OnEquipmentChanged(EEquipmentSlot Slot, UEquipmentItem* Item);

	void RefreshEquipmentSources();

	void RecalculateAttributes();

	TMap<EPrimaryAttribute, float> BaseAttributes;
	TMap<EPrimaryAttribute, float> AllocatedAttributes;
	struct FModifierSource
	{
		TMap<EPrimaryAttribute, float> Bonuses;
		bool bPersistent = false;
	};
	TMap<FName, FModifierSource> ModifierSources;
	TSet<FName> EquipmentSourceIds;
	float CalculateAttribute(EPrimaryAttribute Attribute, bool bPersistentOnly) const;

	int32 UnspentAttributePoints = 0;
};
