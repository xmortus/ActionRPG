// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "UI/Inventory/ItemDragDropOperation.h"
#include "InventoryWidget.generated.h"

class UInventoryComponent;
class AActionRPGPlayerCharacter;

/**
 * Main inventory widget displaying inventory slots in a grid.
 * Manages communication with InventoryComponent and updates display.
 */
UCLASS()
class ACTIONRPG_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * Update the entire inventory display.
	 * Refreshes all slots and weight/capacity displays.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void UpdateInventoryDisplay();

	/**
	 * Handle slot clicked event (left click).
	 * @param SlotIndex The index of the clicked slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void OnInventorySlotClicked(int32 SlotIndex);

	/**
	 * Handle slot right-clicked event (right click - use item).
	 * @param SlotIndex The index of the right-clicked slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void OnInventorySlotRightClicked(int32 SlotIndex);

	/**
	 * Close the inventory widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void CloseInventory();

	/**
	 * Handle item drop between slots using drag operation (for drag and drop system with split support).
	 * @param DragOperation The drag operation containing source slot, item, and quantity info
	 * @param TargetSlotIndex The slot where the item is being dropped
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void HandleItemDrop(UItemDragDropOperation* DragOperation, int32 TargetSlotIndex);

	/**
	 * Handle item drop between slots (legacy version for backward compatibility).
	 * Creates a temporary drag operation and calls the main HandleItemDrop method.
	 * @param SourceSlotIndex The slot where the item is being dragged from
	 * @param TargetSlotIndex The slot where the item is being dropped
	 * @param Quantity The quantity being moved (-1 for entire stack)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI", meta = (DeprecatedFunction, DeprecationMessage = "Use HandleItemDrop with ItemDragDropOperation instead"))
	void HandleItemDropLegacy(int32 SourceSlotIndex, int32 TargetSlotIndex, int32 Quantity = -1);

	// Widget References (must match names in Blueprint)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> InventoryGrid;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeightText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CapacityText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;

	// Slot Widget Class (set in Blueprint)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory UI")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

protected:
	// Event handlers for InventoryComponent events
	UFUNCTION()
	void OnInventoryChanged(int32 SlotIndex, UItemBase* Item);

	UFUNCTION()
	void OnItemAdded(UItemBase* Item);

	UFUNCTION()
	void OnItemRemoved(UItemBase* Item, int32 Quantity);

	UFUNCTION()
	void OnCloseButtonClicked();

private:
	UPROPERTY()
	TArray<TObjectPtr<UInventorySlotWidget>> SlotWidgets;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	/**
	 * Initialize all slot widgets in the grid.
	 */
	void InitializeSlots();

	/**
	 * Refresh a specific slot widget.
	 * @param SlotIndex The index of the slot to refresh
	 */
	void RefreshSlot(int32 SlotIndex);

	/**
	 * Get the InventoryComponent from the player character.
	 * @return The InventoryComponent or nullptr if not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory UI")
	UInventoryComponent* GetInventoryComponent() const;
};
