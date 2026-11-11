// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Equipment/EquipmentInventorySlotWidget.h"
#include "UI/ActionRPGUITheme.h"
#include "UI/Inventory/ItemDragDropOperation.h"
#include "UI/Equipment/EquipmentDragDropOperation.h"
#include "UI/Equipment/EquipmentInventoryWidget.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/Pawn.h"

void UEquipmentInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ActionRPGUITheme::StyleBlueprintWidgetTree(WidgetTree, true);
	SlotBorder = WidgetTree ? Cast<UBorder>(WidgetTree->FindWidget(TEXT("Border"))) : nullptr;
	SetDropHighlight(ActionRPGUITheme::Slot());
}

void UEquipmentInventorySlotWidget::SetupSlot(int32 InSlotIndex, UItemBase* InItem, int32 InQuantity)
{
	SlotIndex = InSlotIndex;
	Item = InItem;
	Quantity = FMath::Max(0, InQuantity);

	UpdateSlotVisuals();
	if (Item && Item->ItemData)
	{
		SetToolTipText(Item->GetItemTooltip());
	}
	else
	{
		SetToolTipText(FText::GetEmpty());
	}
}

FReply UEquipmentInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && Item && Quantity > 0 && ParentInventoryWidget)
	{
		ParentInventoryWidget->ShowContextMenuForSlot(SlotIndex);
		return FReply::Handled();
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && ParentInventoryWidget)
	{
		ParentInventoryWidget->HideContextMenu();
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && Item && Quantity > 0)
	{
		TSharedPtr<SWidget> SlateWidget = GetCachedWidget();
		if (SlateWidget.IsValid())
		{
			return FReply::Handled().DetectDrag(SlateWidget.ToSharedRef(), EKeys::LeftMouseButton);
		}
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UEquipmentInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (!Item || Quantity <= 0)
	{
		return;
	}

	UItemDragDropOperation* DragOperation = NewObject<UItemDragDropOperation>(this, UItemDragDropOperation::StaticClass());
	if (!DragOperation)
	{
		return;
	}

	DragOperation->Item = Item;
	DragOperation->SourceSlotIndex = SlotIndex;
	const bool bSplit = InMouseEvent.IsControlDown() && Quantity > 1 && Item->ItemData && Item->ItemData->MaxStackSize > 1;
	DragOperation->Quantity = bSplit ? Quantity / 2 : Quantity;
	DragOperation->SourceQuantity = Quantity;
	DragOperation->bIsSplitOperation = bSplit;
	DragOperation->bWasHandled = false;

	OutOperation = DragOperation;
}

bool UEquipmentInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
	if (ItemDragOp)
	{
		if (ItemDragOp->bWasHandled)
		{
			return true;
		}

		APawn* OwningPawn = GetOwningPlayerPawn();
		if (!OwningPawn)
		{
			return false;
		}

		UInventoryComponent* InventoryComponent = OwningPawn->FindComponentByClass<UInventoryComponent>();
		if (!InventoryComponent)
		{
			return false;
		}

		const int32 SourceSlot = ItemDragOp->SourceSlotIndex;
		const int32 TargetSlot = SlotIndex;

		if (SourceSlot == TargetSlot || SourceSlot < 0 || TargetSlot < 0)
		{
			return false;
		}

		bool bSuccess = false;
		if (ItemDragOp->bIsSplitOperation)
		{
			bSuccess = InventoryComponent->SplitStackToSlot(SourceSlot, TargetSlot, ItemDragOp->Quantity);
		}
		else
		{
			bSuccess = InventoryComponent->MoveItem(SourceSlot, TargetSlot);
		}

		if (bSuccess)
		{
			ItemDragOp->bWasHandled = true;
			SetDropHighlight(ActionRPGUITheme::Slot());
			return true;
		}

		return false;
	}

	UEquipmentDragDropOperation* EquipmentDragOp = Cast<UEquipmentDragDropOperation>(InOperation);
	if (!EquipmentDragOp)
	{
		return false;
	}

	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		return false;
	}

	UInventoryComponent* InventoryComponent = OwningPawn->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		return false;
	}

	const bool bUnequipped = InventoryComponent->UnequipItemToSlot(EquipmentDragOp->EquipmentSlot, SlotIndex);
	if (bUnequipped)
	{
		EquipmentDragOp->bWasHandled = true;
		SetDropHighlight(ActionRPGUITheme::Slot());
		return true;
	}

	return false;
}

void UEquipmentInventorySlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	if (const UItemDragDropOperation* DragOp = Cast<UItemDragDropOperation>(InOperation))
	{
		bool bValid = DragOp->Item && DragOp->SourceSlotIndex != SlotIndex;
		if (bValid && DragOp->bIsSplitOperation)
		{
			bValid = DragOp->Quantity > 0 && (!Item ||
				(Item->ItemData && DragOp->Item->ItemData &&
				 Item->ItemData->ItemID == DragOp->Item->ItemData->ItemID &&
				 Quantity < Item->ItemData->MaxStackSize));
		}
		SetDropHighlight(bValid ? FLinearColor(0.2f, 0.8f, 0.3f, 1.0f) : FLinearColor(0.9f, 0.2f, 0.2f, 1.0f));
	}
	else if (Cast<UEquipmentDragDropOperation>(InOperation))
	{
		SetDropHighlight(FLinearColor(0.2f, 0.8f, 0.3f, 1.0f));
	}
}

void UEquipmentInventorySlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	SetDropHighlight(ActionRPGUITheme::Slot());
}

void UEquipmentInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	SetDropHighlight(ActionRPGUITheme::Slot());
	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->RefreshInventory();
	}
}

void UEquipmentInventorySlotWidget::SetDropHighlight(const FLinearColor& Color)
{
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(Color);
	}
}

void UEquipmentInventorySlotWidget::UpdateSlotVisuals()
{
	if (ItemIcon)
	{
		if (Item && Item->ItemData && Item->ItemData->ItemIcon)
		{
			ItemIcon->SetBrushFromTexture(Item->ItemData->ItemIcon);
			ItemIcon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ItemIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (QuantityText)
	{
		if (Quantity > 1 && Item && Item->ItemData)
		{
			QuantityText->SetText(FText::AsNumber(Quantity));
			QuantityText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
