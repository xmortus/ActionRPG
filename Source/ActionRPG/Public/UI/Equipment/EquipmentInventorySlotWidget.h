// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentInventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class UItemBase;
class UBorder;
class UEquipmentInventoryWidget;

/**
 * Widget representing a single equipment inventory slot.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UEquipmentInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Equipment UI")
	void SetupSlot(int32 InSlotIndex, UItemBase* InItem, int32 InQuantity);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	int32 GetSlotIndex() const { return SlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	UItemBase* GetItem() const { return Item; }

	void SetParentInventoryWidget(UEquipmentInventoryWidget* InParent) { ParentInventoryWidget = InParent; }

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> ItemIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuantityText;

private:
	UPROPERTY()
	int32 SlotIndex = -1;

	UPROPERTY()
	TObjectPtr<UItemBase> Item;

	UPROPERTY()
	TObjectPtr<UEquipmentInventoryWidget> ParentInventoryWidget;

	UPROPERTY()
	TObjectPtr<UBorder> SlotBorder;

	UPROPERTY()
	int32 Quantity = 0;

	void UpdateSlotVisuals();
	void SetDropHighlight(const FLinearColor& Color);
};
