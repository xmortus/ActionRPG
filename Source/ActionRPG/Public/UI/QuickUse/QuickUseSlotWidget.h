// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Items/Core/ItemBase.h"
#include "QuickUseSlotWidget.generated.h"

// Forward declarations
class UQuickUseBarWidget;

/**
 * Widget representing a single quick-use slot.
 * Displays item icon, quantity, and hotkey label.
 */
UCLASS()
class ACTIONRPG_API UQuickUseSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/**
	 * Set the slot data with item and quantity.
	 * @param SlotIndex The index of this slot in the quick-use bar (0-9)
	 * @param Item The item to display (nullptr for empty slot)
	 * @param InventorySlotIndex The inventory slot index (-1 if not assigned)
	 * @param Quantity The current quantity from inventory slot (if -1, will use Item->Quantity)
	 */
	UFUNCTION(BlueprintCallable, Category = "Quick Use Slot")
	void SetSlotData(int32 InSlotIndex, UItemBase* Item, int32 InInventorySlotIndex, int32 Quantity = -1);

	/**
	 * Clear the slot (set to empty state).
	 */
	UFUNCTION(BlueprintCallable, Category = "Quick Use Slot")
	void ClearSlot();

	/**
	 * Get the slot index.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quick Use Slot")
	int32 GetSlotIndex() const { return SlotIndex; }

	/**
	 * Get the inventory slot index (-1 if not assigned).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quick Use Slot")
	int32 GetInventorySlotIndex() const { return InventorySlotIndex; }

	/**
	 * Get the current item in this slot.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quick Use Slot")
	UItemBase* GetCurrentItem() const { return CurrentItem; }

	/**
	 * Set the parent quick-use bar widget (for drag and drop operations).
	 */
	UFUNCTION(BlueprintCallable, Category = "Quick Use Slot")
	void SetParentQuickUseBar(class UQuickUseBarWidget* InParentQuickUseBar);

	// Widget References (must match names in Blueprint)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> QuantityText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HotkeyText; // Hotkey label (1-8 for skills, 9-0 for consumables)

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBorder;

protected:
	// Mouse input
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual bool NativeIsInteractable() const override;
	
	// Drag and Drop
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	UPROPERTY()
	int32 SlotIndex;

	UPROPERTY()
	int32 InventorySlotIndex;

	UPROPERTY()
	TObjectPtr<UItemBase> CurrentItem;

	UPROPERTY()
	int32 CurrentQuantity;

	// Visual feedback colors
	FLinearColor DefaultBorderColor;
	FLinearColor HoverBorderColor;
	FLinearColor ValidDropBorderColor; // Green when valid item is dragged over
	FLinearColor InvalidDropBorderColor; // Red when invalid item is dragged over
	
	// Parent widget reference (for accessing inventory component)
	UPROPERTY()
	TObjectPtr<class UQuickUseBarWidget> ParentQuickUseBar;
	
	// Drag state tracking
	bool bIsDragOver;
	
	/**
	 * Update the visual appearance of the slot based on item data.
	 */
	void UpdateSlotVisuals();
	
	/**
	 * Check if an item can be assigned to this quick-use slot.
	 * @param Item The item to check
	 * @return True if the item type is valid for this slot
	 */
	bool CanAcceptItem(class UItemBase* Item) const;
};
