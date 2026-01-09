// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Items/Core/ItemBase.h"
#include "InventorySlotWidget.generated.h"

/**
 * Widget representing a single inventory slot.
 * Displays item icon, quantity, and handles click events.
 */
UCLASS()
class ACTIONRPG_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/**
	 * Set the slot data with item and quantity.
	 * @param SlotIndex The index of this slot in the inventory
	 * @param Item The item to display (nullptr for empty slot)
	 * @param Quantity The quantity of the item
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	void SetSlotData(int32 InSlotIndex, UItemBase* Item, int32 InQuantity);

	/**
	 * Clear the slot (set to empty state).
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
	void ClearSlot();

	/**
	 * Get the slot index.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Slot")
	int32 GetSlotIndex() const { return SlotIndex; }

	/**
	 * Get the current item in this slot.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory Slot")
	UItemBase* GetCurrentItem() const { return CurrentItem; }

	// Delegates for click events (public so parent widget can bind)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, int32, SlotIndex);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotRightClicked, int32, SlotIndex);

	UPROPERTY(BlueprintAssignable, Category = "Inventory Slot|Events", meta = (DisplayName = "On Slot Clicked"))
	FOnSlotClicked OnSlotClicked;

	UPROPERTY(BlueprintAssignable, Category = "Inventory Slot|Events", meta = (DisplayName = "On Slot Right Clicked"))
	FOnSlotRightClicked OnSlotRightClicked;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// Widget References (must match names in Blueprint)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> QuantityText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBorder;

private:
	UPROPERTY()
	int32 SlotIndex;

	UPROPERTY()
	TObjectPtr<UItemBase> CurrentItem;

	UPROPERTY()
	int32 CurrentQuantity;

	/**
	 * Update the visual appearance of the slot based on item data.
	 */
	void UpdateSlotVisuals();
};
