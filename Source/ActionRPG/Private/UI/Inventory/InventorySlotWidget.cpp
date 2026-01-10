// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Inventory/InventorySlotWidget.h"
#include "UI/Inventory/ItemDragDropOperation.h"
#include "UI/Inventory/InventoryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Engine/Texture2D.h"
#include "Slate/SlateBrushAsset.h"
#include "Components/Widget.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize to empty state
	SlotIndex = -1;
	CurrentItem = nullptr;
	CurrentQuantity = 0;
	// Note: ParentInventoryWidget is set externally by InventoryWidget::InitializeSlots()
	// Do NOT reset it here as NativeConstruct may be called after SetParentInventoryWidget
	
	// Initialize visual feedback colors
	// Use a lighter default color so it's visible and distinct from dark gray background
	DefaultBorderColor = FLinearColor(0.4f, 0.4f, 0.45f, 1.0f);  // Light gray (was dark gray 0.1, 0.1, 0.1)
	HoverBorderColor = FLinearColor(0.5f, 0.5f, 0.6f, 1.0f);     // Brighter gray-blue
	ValidDropColor = FLinearColor(0.2f, 0.8f, 0.3f, 1.0f);       // Bright green (increased brightness)
	InvalidDropColor = FLinearColor(0.9f, 0.2f, 0.2f, 1.0f);     // Bright red (increased brightness)
	
	bIsDragOver = false;
	
	// Set initial border color
	if (SlotBorder)
	{
		// Use SetBrushColor to set the border color
		SlotBorder->SetBrushColor(DefaultBorderColor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidget::NativeConstruct - SlotBorder is null! Make sure 'SlotBorder' widget is bound in Blueprint WBP_InventorySlotWidget."));
	}
	
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
	SlotIndex = -1;
	CurrentItem = nullptr;
	CurrentQuantity = 0;
	
	UpdateSlotVisuals();

	UE_LOG(LogTemp, Verbose, TEXT("InventorySlotWidget::ClearSlot - Slot %d cleared"), SlotIndex);
}

void UInventorySlotWidget::SetParentInventoryWidget(UInventoryWidget* InventoryWidget)
{
	ParentInventoryWidget = InventoryWidget;
	UE_LOG(LogTemp, Verbose, TEXT("InventorySlotWidget::SetParentInventoryWidget - Slot %d parent widget set: %s"), 
		SlotIndex, InventoryWidget ? TEXT("Valid") : TEXT("NULL"));
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
		// For now, just ensure it's visible and has default color (unless drag is active)
		SlotBorder->SetVisibility(ESlateVisibility::Visible);
		
		// Only set default color if not currently being dragged over
		// This prevents resetting the drag feedback color
		if (!bIsDragOver)
		{
			SlotBorder->SetBrushColor(DefaultBorderColor);
		}
	}
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Only handle clicks if slot has an item or is in a valid state
	if (SlotIndex != -1)
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

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	// Only allow drag if slot has an item
	if (SlotIndex == -1 || !CurrentItem || CurrentQuantity <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidget::NativeOnDragDetected - Cannot drag empty slot or invalid item"));
		return;
	}

	// Create drag drop operation
	UItemDragDropOperation* DragOperation = NewObject<UItemDragDropOperation>(this, UItemDragDropOperation::StaticClass());
	if (!DragOperation)
	{
		UE_LOG(LogTemp, Error, TEXT("InventorySlotWidget::NativeOnDragDetected - Failed to create drag operation"));
		return;
	}

	// Set drag operation data
	DragOperation->SourceSlotIndex = SlotIndex;
	DragOperation->Item = CurrentItem;
	DragOperation->Quantity = CurrentQuantity;  // Default: drag entire stack
	DragOperation->SourceQuantity = CurrentQuantity;
	DragOperation->bIsSplitOperation = false;  // Left-click drag = full stack

	// Note: Drag preview is handled automatically by UE
	// If you want a custom preview, you can create a widget and set it as DefaultDragVisual
	// For now, using default preview

	OutOperation = DragOperation;

	UE_LOG(LogTemp, Log, TEXT("InventorySlotWidget::NativeOnDragDetected - Started drag from slot %d (Item: %s, Quantity: %d)"),
		SlotIndex,
		CurrentItem && CurrentItem->ItemData ? *CurrentItem->ItemData->ItemName.ToString() : TEXT("Unknown"),
		CurrentQuantity);
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
	if (!ItemDragOp)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidget::NativeOnDrop - Invalid drag operation"));
		return false;
	}

	// Get parent inventory widget (stored when slot was created)
	UInventoryWidget* InventoryWidget = ParentInventoryWidget;
	if (!InventoryWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("InventorySlotWidget::NativeOnDrop - ParentInventoryWidget is not set! Make sure SetParentInventoryWidget is called when creating slots."));
		return false;
	}

	int32 SourceSlot = ItemDragOp->SourceSlotIndex;
	int32 TargetSlot = SlotIndex;

	// Debug logging
	UE_LOG(LogTemp, Log, TEXT("InventorySlotWidget::NativeOnDrop - Drop detected: Source=%d, Target=%d, This SlotIndex=%d"), 
		SourceSlot, TargetSlot, SlotIndex);

	// Validate slots
	if (SourceSlot == -1 || TargetSlot == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidget::NativeOnDrop - Invalid slot indices (Source: %d, Target: %d). SlotIndex may have been reset by ClearSlot(). Check RefreshSlot implementation."), SourceSlot, TargetSlot);
		return false;
	}

	// Handle drop on same slot (do nothing)
	if (SourceSlot == TargetSlot)
	{
		UE_LOG(LogTemp, Log, TEXT("InventorySlotWidget::NativeOnDrop - Drop on same slot, ignoring"));
		return false;
	}

	// Request parent widget to handle the drop
	// The parent widget has access to InventoryComponent
	InventoryWidget->HandleItemDrop(SourceSlot, TargetSlot, ItemDragOp->Quantity);

	// Reset visual feedback
	bIsDragOver = false;
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(DefaultBorderColor);
	}

	return true;
}

void UInventorySlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
	if (!ItemDragOp)
	{
		UE_LOG(LogTemp, Verbose, TEXT("InventorySlotWidget::NativeOnDragEnter - Not an ItemDragDropOperation, ignoring"));
		return;
	}

	bIsDragOver = true;

	// Visual feedback: highlight slot based on whether drop is valid
	if (SlotBorder)
	{
		bool bCanDrop = CanDropItem(ItemDragOp);
		FLinearColor BorderColor = bCanDrop ? ValidDropColor : InvalidDropColor;
		
		// Set the brush color using SetBrushColor
		SlotBorder->SetBrushColor(BorderColor);
		
		// Log for debugging
		UE_LOG(LogTemp, Log, TEXT("InventorySlotWidget::NativeOnDragEnter - Slot %d: %s drop (Color: R=%.2f, G=%.2f, B=%.2f, SlotBorder valid: %s)"), 
			SlotIndex, 
			bCanDrop ? TEXT("VALID") : TEXT("INVALID"),
			BorderColor.R, BorderColor.G, BorderColor.B,
			SlotBorder ? TEXT("YES") : TEXT("NO"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidget::NativeOnDragEnter - SlotBorder is null! Make sure 'SlotBorder' widget is bound in Blueprint WBP_InventorySlotWidget."));
	}

	UE_LOG(LogTemp, Verbose, TEXT("InventorySlotWidget::NativeOnDragEnter - Drag entered slot %d"), SlotIndex);
}

void UInventorySlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	bIsDragOver = false;

	// Reset border color
	if (SlotBorder)
	{
		// Set the brush color back to default using SetBrushColor
		SlotBorder->SetBrushColor(DefaultBorderColor);
	}

	UE_LOG(LogTemp, Verbose, TEXT("InventorySlotWidget::NativeOnDragLeave - Drag left slot %d"), SlotIndex);
}

bool UInventorySlotWidget::CanDropItem(UItemDragDropOperation* DragOperation) const
{
	if (!DragOperation || !DragOperation->Item)
	{
		return false;
	}

	// Can always drop on empty slot
	if (SlotIndex == -1 || !CurrentItem || CurrentQuantity <= 0)
	{
		return true;
	}

	// Can drop if same item and can stack
	if (CurrentItem->ItemData && DragOperation->Item->ItemData)
	{
		if (CurrentItem->ItemData->ItemID == DragOperation->Item->ItemData->ItemID)
		{
			// Check if there's room in stack
			int32 MaxStack = CurrentItem->ItemData->MaxStackSize;
			return (CurrentQuantity + DragOperation->Quantity) <= MaxStack;
		}
	}

	// Can drop if different item (will swap)
	return true;
}
