// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/QuickUse/QuickUseSlotWidget.h"
#include "UI/QuickUse/QuickUseBarWidget.h"
#include "UI/Inventory/ItemDragDropOperation.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Core/ItemTypes.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Engine/Texture2D.h"
#include "UObject/EnumProperty.h"

void UQuickUseSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("QuickUseSlotWidget::NativeConstruct - Constructing slot widget"));

	// Initialize to empty state
	// Note: SlotIndex will be set by SetSlotData() when the slot is added to the grid
	// We initialize it to -1 as a sentinel value, but it should be set to 0-9 immediately after construction
	SlotIndex = -1;
	InventorySlotIndex = -1;
	CurrentItem = nullptr;
	CurrentQuantity = 0;

	// Initialize visual feedback colors
	DefaultBorderColor = FLinearColor(0.4f, 0.4f, 0.45f, 1.0f);
	HoverBorderColor = FLinearColor(0.5f, 0.5f, 0.6f, 1.0f);
	ValidDropBorderColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); // Green for valid drop
	InvalidDropBorderColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // Red for invalid drop
	
	// Initialize drag state
	bIsDragOver = false;
	ParentQuickUseBar = nullptr;

	// Enable interaction so the widget can receive mouse events
	SetIsEnabled(true);
	
	// Log widget state
	UE_LOG(LogTemp, Log, TEXT("QuickUseSlotWidget::NativeConstruct - Widget enabled: YES, IsInteractable: %s"), 
		NativeIsInteractable() ? TEXT("YES") : TEXT("NO"));

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

void UQuickUseSlotWidget::SetSlotData(int32 InSlotIndex, UItemBase* Item, int32 InInventorySlotIndex, int32 Quantity)
{
	SlotIndex = InSlotIndex;
	CurrentItem = Item;
	InventorySlotIndex = InInventorySlotIndex;

	// Use provided quantity if valid, otherwise fall back to Item->Quantity
	if (Quantity >= 0)
	{
		CurrentQuantity = Quantity;
	}
	else if (Item && Item->ItemData)
	{
		CurrentQuantity = Item->Quantity;
	}
	else
	{
		CurrentQuantity = 0;
	}

	// Update hotkey text - get the bound key from the Input Mapping Context
	if (HotkeyText)
	{
		FText HotkeyLabel;
		if (ParentQuickUseBar)
		{
			// Get the actual bound key text from the Input Mapping Context
			HotkeyLabel = ParentQuickUseBar->GetHotkeyTextForSlot(SlotIndex);
		}
		
		// Fallback to slot number if parent widget not available or method failed
		if (HotkeyLabel.IsEmpty())
		{
			if (SlotIndex < 8)
			{
				// Slots 1-8: show 1-8 (for skills - Phase 3)
				HotkeyLabel = FText::AsNumber(SlotIndex + 1);
			}
			else
			{
				// Slots 9-10: show 9-0 (for consumables - Phase 2)
				if (SlotIndex == 8)
				{
					HotkeyLabel = FText::FromString(TEXT("9"));
				}
				else if (SlotIndex == 9)
				{
					HotkeyLabel = FText::FromString(TEXT("0"));
				}
			}
		}
		
		HotkeyText->SetText(HotkeyLabel);
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
	// IMPORTANT: Do NOT set SlotIndex to -1 here!
	// SlotIndex represents this slot's position in the quick-use bar (0-9), not the inventory slot index.
	// It should remain constant throughout the slot's lifetime.
	// Only clear the InventorySlotIndex, Item, and Quantity, which represent the slot's contents.
	
	int32 PreservedSlotIndex = SlotIndex; // Preserve the slot's position in the bar
	InventorySlotIndex = -1;
	CurrentItem = nullptr;
	CurrentQuantity = 0;

	UpdateSlotVisuals();

	UE_LOG(LogTemp, Verbose, TEXT("QuickUseSlotWidget::ClearSlot - Quick-use bar slot %d cleared (InventorySlotIndex = -1)"), PreservedSlotIndex);
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

	// Update Border (only set default color if not in drag over state)
	if (SlotBorder)
	{
		SlotBorder->SetVisibility(ESlateVisibility::Visible);
		if (!bIsDragOver)
		{
			SlotBorder->SetBrushColor(DefaultBorderColor);
		}
		// If bIsDragOver is true, the border color is set by NativeOnDragEnter/Leave
	}
}

void UQuickUseSlotWidget::SetParentQuickUseBar(UQuickUseBarWidget* InParentQuickUseBar)
{
	ParentQuickUseBar = InParentQuickUseBar;
}

bool UQuickUseSlotWidget::NativeIsInteractable() const
{
	// Always allow interaction so we can receive mouse events for clicking
	bool bIsInteractable = true;
	UE_LOG(LogTemp, VeryVerbose, TEXT("QuickUseSlotWidget::NativeIsInteractable - Slot %d: %s"), SlotIndex, bIsInteractable ? TEXT("YES") : TEXT("NO"));
	return bIsInteractable;
}

FReply UQuickUseSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Log, TEXT("QuickUseSlotWidget::NativeOnMouseButtonDown - CALLED (SlotIndex: %d, Button: %s)"), 
		SlotIndex, *InMouseEvent.GetEffectingButton().ToString());
	
	// Log click for debugging
	if (SlotIndex >= 0)
	{
		FString ButtonName = InMouseEvent.GetEffectingButton().ToString();
		FString ItemName = CurrentItem && CurrentItem->ItemData ? CurrentItem->ItemData->ItemName.ToString() : TEXT("Empty");
		FString ItemType = CurrentItem && CurrentItem->ItemData ? UEnum::GetValueAsString(CurrentItem->ItemData->Type) : TEXT("None");
		
		UE_LOG(LogTemp, Log, TEXT("========================================"));
		UE_LOG(LogTemp, Log, TEXT("QuickUseSlotWidget::NativeOnMouseButtonDown - CLICKED"));
		UE_LOG(LogTemp, Log, TEXT("  Slot Index: %d"), SlotIndex);
		UE_LOG(LogTemp, Log, TEXT("  Hotkey: %s (Slot 9 = '9', Slot 10 = '0')"), SlotIndex == 8 ? TEXT("9") : SlotIndex == 9 ? TEXT("0") : *FString::Printf(TEXT("%d"), SlotIndex + 1));
		UE_LOG(LogTemp, Log, TEXT("  Mouse Button: %s"), *ButtonName);
		UE_LOG(LogTemp, Log, TEXT("  Item: %s"), *ItemName);
		UE_LOG(LogTemp, Log, TEXT("  Item Type: %s"), *ItemType);
		UE_LOG(LogTemp, Log, TEXT("  Quantity: %d"), CurrentQuantity);
		UE_LOG(LogTemp, Log, TEXT("  Inventory Slot Index: %d"), InventorySlotIndex);
		UE_LOG(LogTemp, Log, TEXT("  Parent Quick-Use Bar: %s"), ParentQuickUseBar ? TEXT("Valid") : TEXT("NULL"));
		
		if (CurrentItem && CurrentItem->ItemData)
		{
			UE_LOG(LogTemp, Log, TEXT("  Item ID: %s"), *CurrentItem->ItemData->ItemID.ToString());
			UE_LOG(LogTemp, Log, TEXT("  Item Rarity: %s"), *UEnum::GetValueAsString(CurrentItem->ItemData->Rarity));
			UE_LOG(LogTemp, Log, TEXT("  Max Stack Size: %d"), CurrentItem->ItemData->MaxStackSize);
		}
		
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			UE_LOG(LogTemp, Log, TEXT("  Action: Left Click (Use item if available)"));
			
			// Use the item if it's assigned
			if (CurrentItem && CurrentQuantity > 0 && InventorySlotIndex >= 0 && ParentQuickUseBar)
			{
				UInventoryComponent* InventoryComponent = ParentQuickUseBar->GetInventoryComponent();
				if (InventoryComponent)
				{
					UE_LOG(LogTemp, Log, TEXT("  Attempting to use item from inventory slot %d"), InventorySlotIndex);
					bool bSuccess = InventoryComponent->UseQuickUseSlot(SlotIndex);
					UE_LOG(LogTemp, Log, TEXT("  UseQuickUseSlot result: %s"), bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("  Cannot use item: InventoryComponent is NULL"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("  Cannot use item: Slot is empty or not properly assigned"));
			}
		}
		else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			UE_LOG(LogTemp, Log, TEXT("  Action: Right Click (Clear slot if available)"));
			
			// Clear the slot if right-clicked
			if (CurrentItem && ParentQuickUseBar)
			{
				UInventoryComponent* InventoryComponent = ParentQuickUseBar->GetInventoryComponent();
				if (InventoryComponent)
				{
					UE_LOG(LogTemp, Log, TEXT("  Attempting to clear quick-use slot %d"), SlotIndex);
					InventoryComponent->ClearQuickUseSlot(SlotIndex);
					UE_LOG(LogTemp, Log, TEXT("  Quick-use slot cleared"));
				}
			}
		}
		
		UE_LOG(LogTemp, Log, TEXT("========================================"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickUseSlotWidget::NativeOnMouseButtonDown - SlotIndex is invalid (%d)"), SlotIndex);
	}
	
	// Always handle the click (even if slot is invalid) to prevent event propagation
	return FReply::Handled();
}

bool UQuickUseSlotWidget::CanAcceptItem(UItemBase* Item) const
{
	if (!Item || !Item->ItemData)
	{
		return false;
	}

	// For Phase 2, slots 9-10 (indices 8-9) only accept consumables
	// Slots 1-8 (indices 0-7) are for skills (Phase 3, currently not available)
	if (SlotIndex >= 8 && SlotIndex <= 9)
	{
		// Slots 9-10: only accept consumables
		return Item->ItemData->Type == EItemType::Consumable;
	}
	else if (SlotIndex >= 0 && SlotIndex <= 7)
	{
		// Slots 1-8: only accept skills (Phase 3 - not implemented yet)
		// For now, return false as skills are not available in Phase 2
		return false;
	}

	return false;
}

bool UQuickUseSlotWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// Check if this is an item drag operation from inventory
	UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
	if (!ItemDragOp || !ItemDragOp->Item)
	{
		// Not an item drag operation from inventory, reject
		return false;
	}

	// Check if item can be accepted by this slot
	bool bCanAccept = CanAcceptItem(ItemDragOp->Item);
	
	UE_LOG(LogTemp, Verbose, TEXT("QuickUseSlotWidget::NativeOnDragOver - Slot %d: %s accept item %s"),
		SlotIndex,
		bCanAccept ? TEXT("CAN") : TEXT("CANNOT"),
		ItemDragOp->Item && ItemDragOp->Item->ItemData ? *ItemDragOp->Item->ItemData->ItemName.ToString() : TEXT("Unknown"));

	return bCanAccept;
}

bool UQuickUseSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// Check if this is an item drag operation from inventory
	UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
	if (!ItemDragOp || !ItemDragOp->Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickUseSlotWidget::NativeOnDrop - Not an ItemDragDropOperation or item is null"));
		// Mark as handled even if invalid to prevent world drop
		if (ItemDragOp) ItemDragOp->bWasHandled = true;
		return false;
	}

	// Reset drag state
	bIsDragOver = false;
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(DefaultBorderColor);
	}

	// Verify SlotIndex is valid - if not, try to get it from parent quick-use bar
	if (SlotIndex < 0 && ParentQuickUseBar)
	{
		// Try to determine slot index from our position in the parent's slot widgets array
		// This is a fallback if the slot wasn't properly initialized
		UE_LOG(LogTemp, Warning, TEXT("QuickUseSlotWidget::NativeOnDrop - SlotIndex is invalid (%d), attempting to determine from parent"), SlotIndex);
		
		// This is a workaround - ideally SlotIndex should always be set correctly
		// For now, we'll let the parent handle it or use a different approach
	}

	// Check if SlotIndex is valid before checking item acceptance
	if (SlotIndex < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("QuickUseSlotWidget::NativeOnDrop - SlotIndex is invalid (%d), cannot accept drop. Slot widget may not be properly initialized."), SlotIndex);
		// Mark as handled to prevent world drop
		ItemDragOp->bWasHandled = true;
		return false;
	}

	// Check if item can be accepted
	if (!CanAcceptItem(ItemDragOp->Item))
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickUseSlotWidget::NativeOnDrop - Item %s cannot be assigned to quick-use slot %d (wrong type)"),
			ItemDragOp->Item && ItemDragOp->Item->ItemData ? *ItemDragOp->Item->ItemData->ItemName.ToString() : TEXT("Unknown"),
			SlotIndex);
		// Mark as handled even if invalid to prevent world drop
		ItemDragOp->bWasHandled = true;
		return false;
	}

	// Get inventory component from parent quick-use bar
	if (!ParentQuickUseBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickUseSlotWidget::NativeOnDrop - ParentQuickUseBar is null"));
		return false;
	}

	UInventoryComponent* InventoryComponent = ParentQuickUseBar->GetInventoryComponent();
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickUseSlotWidget::NativeOnDrop - InventoryComponent is null"));
		return false;
	}

	// Assign item to quick-use slot (use SourceSlotIndex from drag operation)
	int32 SourceInventorySlotIndex = ItemDragOp->SourceSlotIndex;
	
	UE_LOG(LogTemp, Log, TEXT("QuickUseSlotWidget::NativeOnDrop - Assigning item from inventory slot %d to quick-use slot %d"),
		SourceInventorySlotIndex, SlotIndex);

	bool bSuccess = InventoryComponent->AssignItemToQuickUseSlot(SourceInventorySlotIndex, SlotIndex);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("QuickUseSlotWidget::NativeOnDrop - Successfully assigned item to quick-use slot %d"), SlotIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickUseSlotWidget::NativeOnDrop - Failed to assign item to quick-use slot %d"), SlotIndex);
	}
	
	// Always mark as handled if we attempted to handle it (even if it failed)
	// This prevents the inventory widget from treating it as a world drop
	ItemDragOp->bWasHandled = true;

	return bSuccess;
}

void UQuickUseSlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
	if (!ItemDragOp || !ItemDragOp->Item)
	{
		return;
	}

	bIsDragOver = true;

	// Visual feedback: highlight slot based on whether drop is valid
	if (SlotBorder)
	{
		bool bCanAccept = CanAcceptItem(ItemDragOp->Item);
		FLinearColor BorderColor = bCanAccept ? ValidDropBorderColor : InvalidDropBorderColor;
		
		SlotBorder->SetBrushColor(BorderColor);
		
		UE_LOG(LogTemp, Verbose, TEXT("QuickUseSlotWidget::NativeOnDragEnter - Slot %d: %s drop (Item: %s)"),
			SlotIndex,
			bCanAccept ? TEXT("VALID") : TEXT("INVALID"),
			ItemDragOp->Item && ItemDragOp->Item->ItemData ? *ItemDragOp->Item->ItemData->ItemName.ToString() : TEXT("Unknown"));
	}
}

void UQuickUseSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	bIsDragOver = false;

	// Reset border color
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(DefaultBorderColor);
	}

	UE_LOG(LogTemp, Verbose, TEXT("QuickUseSlotWidget::NativeOnDragLeave - Drag left slot %d"), SlotIndex);
}
