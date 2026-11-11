// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Equipment/EquipmentSlotWidget.h"
#include "UI/ActionRPGUITheme.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "UI/Equipment/EquipmentDragDropOperation.h"
#include "UI/Inventory/ItemDragDropOperation.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Items/Core/ItemDataAsset.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/Pawn.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UEquipmentSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ActionRPGUITheme::StyleBlueprintWidgetTree(WidgetTree, true);
	DropHighlightBorder = WidgetTree ? Cast<UBorder>(WidgetTree->FindWidget(TEXT("Border_146"))) : nullptr;
	if (DropHighlightBorder) NormalBorderColor = DropHighlightBorder->GetBrushColor();

	BindEquipmentComponent();
	OnSlotUpdated(GetEquippedItem());
}

void UEquipmentSlotWidget::NativeDestruct()
{
	if (EquipmentComponent)
	{
		EquipmentComponent->OnEquipmentChanged.RemoveDynamic(this, &UEquipmentSlotWidget::HandleEquipmentChanged);
	}

	Super::NativeDestruct();
}

void UEquipmentSlotWidget::BindEquipmentComponent()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentSlotWidget - No owning pawn found."));
		return;
	}

	EquipmentComponent = OwningPawn->FindComponentByClass<UEquipmentComponent>();
	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentSlotWidget - EquipmentComponent not found."));
		return;
	}

	EquipmentComponent->OnEquipmentChanged.AddUniqueDynamic(this, &UEquipmentSlotWidget::HandleEquipmentChanged);
}

UEquipmentItem* UEquipmentSlotWidget::GetEquippedItem() const
{
	return EquipmentComponent ? EquipmentComponent->GetEquippedItem(EquipmentSlot) : nullptr;
}

FReply UEquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (GetEquippedItem())
		{
			TSharedPtr<SWidget> SlateWidget = GetCachedWidget();
			if (SlateWidget.IsValid())
			{
				return FReply::Handled().DetectDrag(SlateWidget.ToSharedRef(), EKeys::LeftMouseButton);
			}
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UEquipmentSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UEquipmentItem* EquippedItem = GetEquippedItem();
	if (!EquippedItem)
	{
		return;
	}

	UEquipmentDragDropOperation* DragOperation = NewObject<UEquipmentDragDropOperation>(this, UEquipmentDragDropOperation::StaticClass());
	if (!DragOperation)
	{
		return;
	}

	DragOperation->EquipmentSlot = EquipmentSlot;
	DragOperation->Item = EquippedItem;
	DragOperation->bWasHandled = false;

	OutOperation = DragOperation;
}

bool UEquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	if (DropHighlightBorder) DropHighlightBorder->SetBrushColor(NormalBorderColor);

	UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
	const UEquipmentItem* DraggedItem = ItemDragOp ? Cast<UEquipmentItem>(ItemDragOp->Item) : nullptr;
	if (!DraggedItem || !EquipmentComponent || !EquipmentComponent->GetEquipFailureReason(DraggedItem, EquipmentSlot).IsEmpty())
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

	const bool bEquipped = InventoryComponent->EquipItemFromSlot(ItemDragOp->SourceSlotIndex);
	if (bEquipped)
	{
		ItemDragOp->bWasHandled = true;
		return true;
	}

	return false;
}

void UEquipmentSlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	const UItemDragDropOperation* Drag = Cast<UItemDragDropOperation>(InOperation);
	const UEquipmentItem* DraggedItem = Drag ? Cast<UEquipmentItem>(Drag->Item) : nullptr;
	const FText Failure = EquipmentComponent
		? EquipmentComponent->GetEquipFailureReason(DraggedItem, EquipmentSlot)
		: FText::FromString(TEXT("Equipment unavailable"));
	if (DropHighlightBorder)
	{
		DropHighlightBorder->SetBrushColor(Failure.IsEmpty()
			? FLinearColor(0.2f, 0.8f, 0.3f, 1.0f)
			: FLinearColor(0.9f, 0.2f, 0.2f, 1.0f));
	}
	SetToolTipText(Failure);
}

void UEquipmentSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	if (DropHighlightBorder) DropHighlightBorder->SetBrushColor(NormalBorderColor);
	SetToolTipText(FText::GetEmpty());
}

void UEquipmentSlotWidget::HandleEquipmentChanged(EEquipmentSlot ChangedSlot, UEquipmentItem* Item)
{
	if (ChangedSlot == EquipmentSlot)
	{
		OnSlotUpdated(Item);
	}
}

void UEquipmentSlotWidget::OnSlotUpdated_Implementation(UEquipmentItem* Item)
{
	if (!SlotIcon)
	{
		return;
	}

	if (Item && Item->ItemData && Item->ItemData->ItemIcon)
	{
		SlotIcon->SetBrushFromTexture(Item->ItemData->ItemIcon);
		SlotIcon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SlotIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}
