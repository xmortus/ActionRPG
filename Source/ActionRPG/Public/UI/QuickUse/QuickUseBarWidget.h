// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Inventory/InventoryComponent.h"
#include "QuickUseBarWidget.generated.h"

class UUniformGridPanel;
class UQuickUseSlotWidget;
class UInventoryComponent;

/**
 * Quick-use bar widget displaying 10 slots (1-8 for skills, 9-10 for consumables).
 * Always visible at the bottom of the screen.
 */
UCLASS()
class ACTIONRPG_API UQuickUseBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Quick Use Bar")
	void UpdateQuickUseBar();

	UFUNCTION(BlueprintCallable, Category = "Quick Use Bar")
	void OnQuickUseSlotChanged(int32 QuickUseSlotIndex, UItemBase* Item);

	/**
	 * Get the InventoryComponent from the player character.
	 * @return The InventoryComponent or nullptr if not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quick Use Bar")
	UInventoryComponent* GetInventoryComponent() const;

	/**
	 * Get the bound key text for a quick-use slot from the Input Mapping Context.
	 * @param SlotIndex The slot index (0-9)
	 * @return The key text (e.g., "1", "2", "9", "0") or empty text if not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quick Use Bar")
	FText GetHotkeyTextForSlot(int32 SlotIndex) const;

	// Widget References (bind in Blueprint)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> QuickUseGrid;

	// Slot Widget Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quick Use Bar")
	TSubclassOf<class UQuickUseSlotWidget> SlotWidgetClass;

protected:
	// Drag and Drop
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	// Event handlers for InventoryComponent events
	UFUNCTION()
	void OnQuickUseSlotChangedInternal(int32 QuickUseSlotIndex, UItemBase* Item);
	
	UFUNCTION()
	void OnInventoryChangedInternal(int32 SlotIndex, UItemBase* Item);

private:
	UPROPERTY()
	TArray<TObjectPtr<UQuickUseSlotWidget>> SlotWidgets;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	void InitializeSlots();
	void RefreshSlot(int32 SlotIndex);
};
