// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UI/Inventory/InventorySlotWidget.h"
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
	UInventoryComponent* GetInventoryComponent() const;
};
