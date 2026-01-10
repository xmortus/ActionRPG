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

	/**
	 * Handle drag over - accept all drag operations to enable world drops.
	 * Override NativeOnDragOver to accept drops anywhere on the widget (including outside slots).
	 */
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	/**
	 * Handle drop - check if drop is on a valid slot, otherwise handle as world drop.
	 * Override NativeOnDrop to accept drops anywhere and route to appropriate handler.
	 */
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	/**
	 * Handle drag cancellation - when item is dropped outside inventory widget, drop it to world.
	 * Override NativeOnDragCancelled to detect when drag is cancelled.
	 */
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
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
	/**
	 * Handle dragging item to world (when dropped outside inventory widget).
	 * Calculates world location from mouse position and drops item using DropItemToWorld.
	 * @param DragOperation The drag operation containing source slot and quantity info
	 * @param DragDropEvent The drag drop event containing mouse position info
	 */
	void HandleDragToWorld(UItemDragDropOperation* DragOperation, const FDragDropEvent& DragDropEvent);

	/**
	 * Find the slot widget at the given screen coordinates.
	 * @param ScreenPosition The screen position to check
	 * @return The slot index if found, -1 otherwise
	 */
	int32 FindSlotAtScreenPosition(const FVector2D& ScreenPosition) const;

	// Track active drag operation for world drop detection
	UPROPERTY()
	TObjectPtr<UItemDragDropOperation> ActiveDragOperation;

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
