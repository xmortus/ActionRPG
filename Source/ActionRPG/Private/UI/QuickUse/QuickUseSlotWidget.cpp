// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/QuickUse/QuickUseSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Engine/Texture2D.h"

void UQuickUseSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize to empty state
	SlotIndex = -1;
	InventorySlotIndex = -1;
	CurrentItem = nullptr;
	CurrentQuantity = 0;

	// Initialize visual feedback colors
	DefaultBorderColor = FLinearColor(0.4f, 0.4f, 0.45f, 1.0f);
	HoverBorderColor = FLinearColor(0.5f, 0.5f, 0.6f, 1.0f);

	// Set initial border color
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(DefaultBorderColor);
	}

	// Set hotkey text based on slot index (will be updated in SetSlotData)
	if (HotkeyText)
	{
		// Default: show slot number
		HotkeyText->SetText(FText::AsNumber(SlotIndex + 1));
	}

	UpdateSlotVisuals();
}

void UQuickUseSlotWidget::SetSlotData(int32 InSlotIndex, UItemBase* Item, int32 InInventorySlotIndex)
{
	SlotIndex = InSlotIndex;
	CurrentItem = Item;
	InventorySlotIndex = InInventorySlotIndex;

	if (Item && Item->ItemData)
	{
		CurrentQuantity = Item->Quantity;
	}
	else
	{
		CurrentQuantity = 0;
	}

	// Update hotkey text (1-8 for skills, 9-0 for consumables)
	if (HotkeyText)
	{
		FString HotkeyLabel;
		if (SlotIndex < 8)
		{
			// Slots 1-8: show 1-8 (for skills - Phase 3)
			HotkeyLabel = FString::Printf(TEXT("%d"), SlotIndex + 1);
		}
		else
		{
			// Slots 9-10: show 9-0 (for consumables - Phase 2)
			if (SlotIndex == 8)
			{
				HotkeyLabel = TEXT("9");
			}
			else if (SlotIndex == 9)
			{
				HotkeyLabel = TEXT("0");
			}
		}
		HotkeyText->SetText(FText::FromString(HotkeyLabel));
	}

	UpdateSlotVisuals();

	UE_LOG(LogTemp, Verbose, TEXT("QuickUseSlotWidget::SetSlotData - Slot %d: Item=%s, InventorySlot=%d, Quantity=%d"),
		SlotIndex,
		CurrentItem && CurrentItem->ItemData ? *CurrentItem->ItemData->ItemName.ToString() : TEXT("Empty"),
		InventorySlotIndex,
		CurrentQuantity);
}

void UQuickUseSlotWidget::ClearSlot()
{
	SlotIndex = -1;
	InventorySlotIndex = -1;
	CurrentItem = nullptr;
	CurrentQuantity = 0;

	UpdateSlotVisuals();

	UE_LOG(LogTemp, Verbose, TEXT("QuickUseSlotWidget::ClearSlot - Slot %d cleared"), SlotIndex);
}

void UQuickUseSlotWidget::UpdateSlotVisuals()
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
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Update Border
	if (SlotBorder)
	{
		SlotBorder->SetVisibility(ESlateVisibility::Visible);
		SlotBorder->SetBrushColor(DefaultBorderColor);
	}
}
