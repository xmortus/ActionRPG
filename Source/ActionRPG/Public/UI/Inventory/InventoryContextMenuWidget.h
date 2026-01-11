// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Items/Core/ItemTypes.h"
#include "InventoryContextMenuWidget.generated.h"

class UInventoryWidget;

/**
 * Context menu widget for inventory slots.
 * Displays Use, Drop, Split, and Equip options when right-clicking an inventory slot.
 */
UCLASS()
class ACTIONRPG_API UInventoryContextMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * Initialize the context menu with slot information.
	 * @param InSlotIndex The inventory slot index
	 * @param InItem The item in the slot (can be nullptr for empty slots)
	 * @param InQuantity The quantity of the item
	 * @param InParentWidget The parent InventoryWidget (for callbacks)
	 */
	UFUNCTION(BlueprintCallable, Category = "Context Menu")
	void InitializeMenu(int32 InSlotIndex, class UItemBase* InItem, int32 InQuantity, UInventoryWidget* InParentWidget);

	/**
	 * Show the context menu at the specified screen position.
	 * @param ScreenPosition The screen position to show the menu at
	 */
	UFUNCTION(BlueprintCallable, Category = "Context Menu")
	void ShowMenuAtPosition(FVector2D ScreenPosition);

	/**
	 * Hide the context menu.
	 */
	UFUNCTION(BlueprintCallable, Category = "Context Menu")
	void HideMenu();

	/**
	 * Get the target screen position (for Blueprint positioning).
	 * @return The screen position where the menu should be displayed
	 */
	UFUNCTION(BlueprintPure, Category = "Context Menu")
	FVector2D GetTargetScreenPosition() const { return TargetScreenPosition; }

	// Button References (must match names in Blueprint)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UseButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DropButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SplitButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EquipButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UseButtonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DropButtonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SplitButtonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EquipButtonText;

protected:
	// Button click handlers
	UFUNCTION()
	void OnUseButtonClicked();

	UFUNCTION()
	void OnDropButtonClicked();

	UFUNCTION()
	void OnSplitButtonClicked();

	UFUNCTION()
	void OnEquipButtonClicked();

private:
	UPROPERTY()
	int32 SlotIndex;

	UPROPERTY()
	TObjectPtr<class UItemBase> Item;

	UPROPERTY()
	int32 Quantity;

	UPROPERTY()
	TObjectPtr<UInventoryWidget> ParentInventoryWidget;

	// Screen position where the menu should be displayed
	UPROPERTY()
	FVector2D TargetScreenPosition;

	/**
	 * Update button visibility based on item type.
	 * Equip button is hidden for Consumables, SkillItems, and SkillStones.
	 */
	void UpdateButtonVisibility();
};
