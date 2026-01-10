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

	// Widget References (bind in Blueprint)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> QuickUseGrid;

	// Slot Widget Class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quick Use Bar")
	TSubclassOf<class UQuickUseSlotWidget> SlotWidgetClass;

protected:
	// Event handlers for InventoryComponent events
	UFUNCTION()
	void OnQuickUseSlotChangedInternal(int32 QuickUseSlotIndex, UItemBase* Item);

private:
	UPROPERTY()
	TArray<TObjectPtr<UQuickUseSlotWidget>> SlotWidgets;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	void InitializeSlots();
	void RefreshSlot(int32 SlotIndex);

	/**
	 * Get the InventoryComponent from the player character.
	 * @return The InventoryComponent or nullptr if not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quick Use Bar")
	UInventoryComponent* GetInventoryComponent() const;
};
