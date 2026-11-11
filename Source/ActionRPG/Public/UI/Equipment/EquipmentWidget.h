// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Core/ItemTypes.h"
#include "Progression/Core/ProgressionTypes.h"
#include "EquipmentWidget.generated.h"

class UEquipmentComponent;
class UInventoryComponent;
class UAttributeComponent;
class USecondaryAttributeComponent;
class UEquipmentItem;
class UItemBase;
class UEquipmentInventoryWidget;
class UEquipmentStatsWidget;

/**
 * Equipment UI root widget (paper-doll + stats + inventory).
 * Provides component references and update events for UMG binding.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UEquipmentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	UEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	UAttributeComponent* GetAttributeComponent() const { return AttributeComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	USecondaryAttributeComponent* GetSecondaryAttributeComponent() const { return SecondaryAttributeComponent; }

	UFUNCTION(BlueprintCallable, Category = "Equipment UI")
	void RefreshEquipment();

	UFUNCTION(BlueprintCallable, Category = "Equipment UI")
	void RefreshInventory();

	UFUNCTION(BlueprintCallable, Category = "Equipment UI")
	void RefreshStats();

	UFUNCTION(BlueprintCallable, Category = "Equipment UI")
	void HideInventoryContextMenu();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment UI")
	void OnEquipmentUpdated();
	virtual void OnEquipmentUpdated_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Equipment UI")
	void OnInventoryUpdated();
	virtual void OnInventoryUpdated_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Equipment UI")
	void OnStatsUpdated();
	virtual void OnStatsUpdated_Implementation();

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UEquipmentInventoryWidget> EquipmentInventoryWidget;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UEquipmentStatsWidget> EquipmentStatsWidget;

private:
	void StyleEquipmentLayout();

	UFUNCTION()
	void CloseEquipmentPanel();

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY()
	TObjectPtr<USecondaryAttributeComponent> SecondaryAttributeComponent;

	void BindComponents();

	UFUNCTION()
	void HandleEquipmentChanged(EEquipmentSlot ChangedSlot, UEquipmentItem* Item);

	UFUNCTION()
	void HandleInventoryChanged(int32 SlotIndex, UItemBase* Item);

	UFUNCTION()
	void HandlePrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue);

	UFUNCTION()
	void HandleSecondaryAttributeChanged(ESecondaryAttribute Attribute, float NewValue);
};
