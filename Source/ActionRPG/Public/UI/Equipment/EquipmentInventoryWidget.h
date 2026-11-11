// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentInventoryWidget.generated.h"

class UUniformGridPanel;
class UInventoryComponent;
class UEquipmentInventorySlotWidget;
class UItemBase;
class UInventoryContextMenuWidget;
class UItemDragDropOperation;

/**
 * Inventory grid panel used in the equipment UI.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UEquipmentInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UFUNCTION(BlueprintCallable, Category = "Equipment UI")
	void RefreshInventory();

	void ShowContextMenuForSlot(int32 SlotIndex);
	void HideContextMenu();
	void UseItemFromContextMenu(int32 SlotIndex);
	void DropItemFromContextMenu(int32 SlotIndex);
	void SplitItemFromContextMenu(int32 SlotIndex);
	void EquipItemFromContextMenu(int32 SlotIndex);

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment UI")
	void OnInventoryUpdated();
	virtual void OnInventoryUpdated_Implementation();

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUniformGridPanel> InventoryGrid;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment UI")
	TSubclassOf<UEquipmentInventorySlotWidget> SlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment UI")
	TSubclassOf<UInventoryContextMenuWidget> ContextMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment UI")
	int32 Columns = 6;

private:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<UInventoryContextMenuWidget> ContextMenuWidget;

	UPROPERTY()
	TArray<TObjectPtr<UEquipmentInventorySlotWidget>> SlotWidgets;

	void BindInventoryComponent();
	bool DropItemToWorld(int32 SlotIndex, int32 Quantity, bool bAtCursor);

	UFUNCTION()
	void HandleInventoryChanged(int32 SlotIndex, UItemBase* Item);
};
