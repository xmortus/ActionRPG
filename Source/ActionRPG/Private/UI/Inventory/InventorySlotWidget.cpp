// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Engine/Texture2D.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize to empty state
	SlotIndex = INDEX_NONE;
	CurrentItem = nullptr;
	CurrentQuantity = 0;
	UpdateSlotVisuals();
}

void UInventorySlotWidget::SetSlotData(int32 InSlotIndex, UItemBase* Item, int32 InQuantity)
{
	SlotIndex = InSlotIndex;
	CurrentItem = Item;
	CurrentQuantity = FMath::Max(0, InQuantity);
	
	UpdateSlotVisuals();

	UE_LOG(LogTemp, Verbose, TEXT("InventorySlotWidget::SetSlotData - Slot %d: Item=%s, Quantity=%d"),
		SlotIndex,
		CurrentItem && CurrentItem->ItemData ? *CurrentItem->ItemData->ItemName.ToString() : TEXT("Empty"),
		CurrentQuantity);
}

void UInventorySlotWidget::ClearSlot()
{
	SlotIndex = INDEX_NONE;
	CurrentItem = nullptr;
	CurrentQuantity = 0;
	
	UpdateSlotVisuals();

	UE_LOG(LogTemp, Verbose, TEXT("InventorySlotWidget::ClearSlot - Slot %d cleared"), SlotIndex);
}

void UInventorySlotWidget::UpdateSlotVisuals()
{
	// Update Item Icon
	if (ItemIcon)
	{
		if (CurrentItem && CurrentItem->ItemData && CurrentItem->ItemData->ItemIcon)
		{
			ItemIcon->SetBrushFromTexture(CurrentItem->ItemData->ItemIcon);
			ItemIcon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ItemIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// Update Quantity Text
	if (QuantityText)
	{
		if (CurrentQuantity > 1 && CurrentItem && CurrentItem->ItemData)
		{
			QuantityText->SetText(FText::AsNumber(CurrentQuantity));
			QuantityText->SetVisibility(ESlateVisibility::Visible);
		}
		else if (CurrentQuantity == 1)
		{
			// Show quantity even if 1 for consistency (optional - can hide if desired)
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Update Border (optional - can be used for hover/selected states)
	if (SlotBorder)
	{
		// Border appearance can be customized based on item rarity or state
		// For now, just ensure it's visible
		SlotBorder->SetVisibility(ESlateVisibility::Visible);
	}
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Only handle clicks if slot has an item or is in a valid state
	if (SlotIndex != INDEX_NONE)
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			// Left click - prepare for drag and drop (Day 24)
			OnSlotClicked.Broadcast(SlotIndex);
			UE_LOG(LogTemp, Log, TEXT("InventorySlotWidget::NativeOnMouseButtonDown - Left clicked slot %d"), SlotIndex);
			return FReply::Handled();
		}
		else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			// Right click - use item
			OnSlotRightClicked.Broadcast(SlotIndex);
			UE_LOG(LogTemp, Log, TEXT("InventorySlotWidget::NativeOnMouseButtonDown - Right clicked slot %d"), SlotIndex);
			return FReply::Handled();
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
