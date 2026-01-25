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

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetAttribute(EPrimaryAttribute Attribute, float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ModifyAttribute(EPrimaryAttribute Attribute, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetClassBonuses(const TMap<EPrimaryAttribute, float>& Bonuses);

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

	void ApplyEquipmentBonuses();

	void RecalculateAttributes();

	TMap<EPrimaryAttribute, float> BaseAttributes;
	TMap<EPrimaryAttribute, float> AllocatedAttributes;
	TMap<EPrimaryAttribute, float> ClassBonuses;

	int32 UnspentAttributePoints = 0;
};
