// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Inventory/InventoryContextMenuWidget.h"
#include "UI/ActionRPGUITheme.h"
#include "UI/Inventory/InventoryWidget.h"
#include "UI/Equipment/EquipmentInventoryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Core/ItemTypes.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "GameFramework/Pawn.h"

void UInventoryContextMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ActionRPGUITheme::StyleBlueprintWidgetTree(WidgetTree, true);

	// Bind button click handlers
	if (UseButton)
	{
		UseButton->OnClicked.AddDynamic(this, &UInventoryContextMenuWidget::OnUseButtonClicked);
	}

	if (DropButton)
	{
		DropButton->OnClicked.AddDynamic(this, &UInventoryContextMenuWidget::OnDropButtonClicked);
	}

	if (SplitButton)
	{
		SplitButton->OnClicked.AddDynamic(this, &UInventoryContextMenuWidget::OnSplitButtonClicked);
	}

	if (EquipButton)
	{
		EquipButton->OnClicked.AddDynamic(this, &UInventoryContextMenuWidget::OnEquipButtonClicked);
	}

	// Initially hide the menu
	SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryContextMenuWidget::NativeDestruct()
{
	// Unbind button click handlers
	if (UseButton)
	{
		UseButton->OnClicked.RemoveDynamic(this, &UInventoryContextMenuWidget::OnUseButtonClicked);
	}

	if (DropButton)
	{
		DropButton->OnClicked.RemoveDynamic(this, &UInventoryContextMenuWidget::OnDropButtonClicked);
	}

	if (SplitButton)
	{
		SplitButton->OnClicked.RemoveDynamic(this, &UInventoryContextMenuWidget::OnSplitButtonClicked);
	}

	if (EquipButton)
	{
		EquipButton->OnClicked.RemoveDynamic(this, &UInventoryContextMenuWidget::OnEquipButtonClicked);
	}

	Super::NativeDestruct();
}

void UInventoryContextMenuWidget::InitializeMenu(int32 InSlotIndex, UItemBase* InItem, int32 InQuantity, UInventoryWidget* InParentWidget)
{
	SlotIndex = InSlotIndex;
	Item = InItem;
	Quantity = InQuantity;
	ParentInventoryWidget = InParentWidget;
	ParentEquipmentInventoryWidget = nullptr;

	// Update button visibility based on item type
	UpdateButtonVisibility();
}

void UInventoryContextMenuWidget::InitializeEquipmentMenu(int32 InSlotIndex, UItemBase* InItem, int32 InQuantity, UEquipmentInventoryWidget* InParentWidget)
{
	SlotIndex = InSlotIndex;
	Item = InItem;
	Quantity = InQuantity;
	ParentInventoryWidget = nullptr;
	ParentEquipmentInventoryWidget = InParentWidget;
	UpdateButtonVisibility();
}

void UInventoryContextMenuWidget::ShowMenuAtPosition(FVector2D ScreenPosition)
{
	if (!IsInViewport())
	{
		AddToViewport(220); // Above inventory and equipment panels.
	}

	// Store the target position (Blueprint can access this via GetTargetScreenPosition)
	TargetScreenPosition = ScreenPosition;
	
	SetVisibility(ESlateVisibility::Visible);
	
	// Call Blueprint event to update position (Blueprint must implement OnPositionUpdated to position the widget)
	OnPositionUpdated();
	
	UE_LOG(LogTemp, Verbose, TEXT("InventoryContextMenuWidget::ShowMenuAtPosition - Menu shown at screen position (%.1f, %.1f)"), ScreenPosition.X, ScreenPosition.Y);
}

void UInventoryContextMenuWidget::HideMenu()
{
	SetVisibility(ESlateVisibility::Collapsed);
	// Note: Widgets added via AddToViewport() should use visibility toggling instead of RemoveFromParent()
	// This avoids "no UMG parent" warnings in UE 5.7. The widget stays in viewport and is reused.
	
	UE_LOG(LogTemp, Verbose, TEXT("InventoryContextMenuWidget::HideMenu - Hiding context menu"));
}

FReply UInventoryContextMenuWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Convert screen position to local widget space
	FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	FVector2D WidgetSize = InGeometry.GetLocalSize();
	
	// Check if click is outside the widget bounds
	bool bClickOutside = (LocalPosition.X < 0.0f || LocalPosition.X > WidgetSize.X ||
	                      LocalPosition.Y < 0.0f || LocalPosition.Y > WidgetSize.Y);
	
	if (bClickOutside)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryContextMenuWidget::NativeOnMouseButtonDown - Click outside menu bounds, closing menu"));
		HideMenu();
		return FReply::Handled();
	}
	
	// Click is inside the widget - let child widgets handle it
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventoryContextMenuWidget::OnUseButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("InventoryContextMenuWidget::OnUseButtonClicked - Use button clicked for slot %d"), SlotIndex);
	
	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->UseItemFromContextMenu(SlotIndex);
	}
	else if (ParentEquipmentInventoryWidget)
	{
		ParentEquipmentInventoryWidget->UseItemFromContextMenu(SlotIndex);
	}
	
	HideMenu();
}

void UInventoryContextMenuWidget::OnDropButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("InventoryContextMenuWidget::OnDropButtonClicked - Drop button clicked for slot %d"), SlotIndex);
	
	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->DropItemFromContextMenu(SlotIndex);
	}
	else if (ParentEquipmentInventoryWidget)
	{
		ParentEquipmentInventoryWidget->DropItemFromContextMenu(SlotIndex);
	}
	
	HideMenu();
}

void UInventoryContextMenuWidget::OnSplitButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("InventoryContextMenuWidget::OnSplitButtonClicked - Split button clicked for slot %d"), SlotIndex);
	
	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->SplitItemFromContextMenu(SlotIndex);
	}
	else if (ParentEquipmentInventoryWidget)
	{
		ParentEquipmentInventoryWidget->SplitItemFromContextMenu(SlotIndex);
	}
	
	HideMenu();
}

void UInventoryContextMenuWidget::OnEquipButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("InventoryContextMenuWidget::OnEquipButtonClicked - Equip button clicked for slot %d"), SlotIndex);
	
	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->EquipItemFromContextMenu(SlotIndex);
	}
	else if (ParentEquipmentInventoryWidget)
	{
		ParentEquipmentInventoryWidget->EquipItemFromContextMenu(SlotIndex);
	}
	
	HideMenu();
}

void UInventoryContextMenuWidget::UpdateButtonVisibility()
{
	// Default: show all buttons
	bool bShowUse = true;
	bool bShowDrop = true;
	bool bShowSplit = (Quantity > 1); // Only show split if quantity > 1
	bool bShowEquip = false;

	if (Item && Item->ItemData)
	{
		bShowEquip = Item->ItemData->Type == EItemType::Equipment;
		bShowSplit = bShowSplit && Item->ItemData->MaxStackSize > 1;
	}
	else
	{
		// No item - hide all buttons
		bShowUse = false;
		bShowDrop = false;
		bShowSplit = false;
		bShowEquip = false;
	}

	// Update button visibility
	if (UseButton)
	{
		UseButton->SetVisibility(bShowUse ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (DropButton)
	{
		DropButton->SetVisibility(bShowDrop ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (SplitButton)
	{
		SplitButton->SetVisibility(bShowSplit ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (EquipButton)
	{
		EquipButton->SetVisibility(bShowEquip ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		if (bShowEquip)
		{
			const APawn* Pawn = GetOwningPlayerPawn();
			const UEquipmentComponent* Equipment = Pawn ? Pawn->FindComponentByClass<UEquipmentComponent>() : nullptr;
			const FText Failure = Equipment
				? Equipment->GetEquipFailureReason(Cast<UEquipmentItem>(Item))
				: FText::FromString(TEXT("Equipment unavailable"));
			EquipButton->SetIsEnabled(Failure.IsEmpty());
			EquipButton->SetToolTipText(Failure);
			if (EquipButtonText && Item && Item->ItemData)
			{
				const int32 RequiredLevel = FMath::Max(1, Item->ItemData->EquipmentRequiredLevel);
				EquipButtonText->SetText(RequiredLevel > 1
					? FText::FromString(FString::Printf(TEXT("Equip (Lv %d)"), RequiredLevel))
					: FText::FromString(TEXT("Equip")));
			}
		}
	}

	UE_LOG(LogTemp, Verbose, TEXT("InventoryContextMenuWidget::UpdateButtonVisibility - Use: %d, Drop: %d, Split: %d, Equip: %d"), 
		bShowUse, bShowDrop, bShowSplit, bShowEquip);
}
