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
			// Left click - start drag detection for drag and drop
			// Only allow drag if slot has an item
			if (CurrentItem && CurrentQuantity > 0)
			{
				UE_LOG(LogTemp, Verbose, TEXT("InventorySlotWidget::NativeOnMouseButtonDown - Starting drag detection for slot %d"), SlotIndex);
				// Get the Slate widget for drag detection
				// NativeOnDragDetected will be called if drag actually starts
				TSharedPtr<SWidget> SlateWidget = GetCachedWidget();
				if (SlateWidget.IsValid())
				{
					return FReply::Handled().DetectDrag(SlateWidget.ToSharedRef(), EKeys::LeftMouseButton);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidget::NativeOnMouseButtonDown - Cached widget is invalid, cannot start drag"));
					// Fall back to just handling the click
					OnSlotClicked.Broadcast(SlotIndex);
					return FReply::Handled();
				}
			}
			else
			{
				// Empty slot or invalid item - just broadcast click
				OnSlotClicked.Broadcast(SlotIndex);
				UE_LOG(LogTemp, Verbose, TEXT("InventorySlotWidget::NativeOnMouseButtonDown - Left clicked empty slot %d"), SlotIndex);
				return FReply::Handled();
			}
		}
		else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			// Right click - use item (only if slot has item)
			if (CurrentItem && CurrentQuantity > 0)
			{
				OnSlotRightClicked.Broadcast(SlotIndex);
				UE_LOG(LogTemp, Log, TEXT("InventorySlotWidget::NativeOnMouseButtonDown - Right clicked slot %d (use item)"), SlotIndex);
				return FReply::Handled();
			}
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
