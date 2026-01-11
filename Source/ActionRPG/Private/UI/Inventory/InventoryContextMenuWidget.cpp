// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Inventory/InventoryContextMenuWidget.h"
#include "UI/Inventory/InventoryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Core/ItemTypes.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"

void UInventoryContextMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

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
		UseButton->OnClicked.RemoveAll(this);
	}

	if (DropButton)
	{
		DropButton->OnClicked.RemoveAll(this);
	}

	if (SplitButton)
	{
		SplitButton->OnClicked.RemoveAll(this);
	}

	if (EquipButton)
	{
		EquipButton->OnClicked.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UInventoryContextMenuWidget::InitializeMenu(int32 InSlotIndex, UItemBase* InItem, int32 InQuantity, UInventoryWidget* InParentWidget)
{
	SlotIndex = InSlotIndex;
	Item = InItem;
	Quantity = InQuantity;
	ParentInventoryWidget = InParentWidget;

	// Update button visibility based on item type
	UpdateButtonVisibility();
}

void UInventoryContextMenuWidget::ShowMenuAtPosition(FVector2D ScreenPosition)
{
	if (!IsInViewport())
	{
		AddToViewport(100); // High Z-order to appear above inventory
	}

	// Store the target position (Blueprint can access this via GetTargetScreenPosition if needed)
	TargetScreenPosition = ScreenPosition;
	
	SetVisibility(ESlateVisibility::Visible);
	
	UE_LOG(LogTemp, Log, TEXT("InventoryContextMenuWidget::ShowMenuAtPosition - Menu shown at screen position (%.1f, %.1f)"), ScreenPosition.X, ScreenPosition.Y);
	UE_LOG(LogTemp, Warning, TEXT("NOTE: Positioning must be handled in Blueprint. In your context menu widget Blueprint:"));
	UE_LOG(LogTemp, Warning, TEXT("1. Use Canvas Panel as root widget"));
	UE_LOG(LogTemp, Warning, TEXT("2. Set Anchor to Top-Left (0,0)"));
	UE_LOG(LogTemp, Warning, TEXT("3. Bind Position Offset to GetMousePositionOnViewport (or use Render Transform)"));
	UE_LOG(LogTemp, Warning, TEXT("Alternatively, handle positioning in Blueprint Event Graph using widget layout."));
}

void UInventoryContextMenuWidget::HideMenu()
{
	SetVisibility(ESlateVisibility::Collapsed);
	RemoveFromParent();
	
	UE_LOG(LogTemp, Verbose, TEXT("InventoryContextMenuWidget::HideMenu - Hiding context menu"));
}

// Note: NativeOnMouseButtonDown won't detect clicks outside the widget
// Click-outside detection is handled in InventoryWidget::NativeOnMouseButtonDown

void UInventoryContextMenuWidget::OnUseButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("InventoryContextMenuWidget::OnUseButtonClicked - Use button clicked for slot %d"), SlotIndex);
	
	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->UseItemFromContextMenu(SlotIndex);
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
	
	HideMenu();
}

void UInventoryContextMenuWidget::OnSplitButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("InventoryContextMenuWidget::OnSplitButtonClicked - Split button clicked for slot %d"), SlotIndex);
	
	if (ParentInventoryWidget)
	{
		ParentInventoryWidget->SplitItemFromContextMenu(SlotIndex);
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
	
	HideMenu();
}

void UInventoryContextMenuWidget::UpdateButtonVisibility()
{
	// Default: show all buttons
	bool bShowUse = true;
	bool bShowDrop = true;
	bool bShowSplit = (Quantity > 1); // Only show split if quantity > 1
	bool bShowEquip = true;

	// Hide Equip button for Consumables, SkillItems, and SkillStones
	if (Item && Item->ItemData)
	{
		EItemType ItemType = Item->ItemData->Type;
		if (ItemType == EItemType::Consumable || 
			ItemType == EItemType::SkillItem || 
			ItemType == EItemType::SkillStone)
		{
			bShowEquip = false;
		}
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
	}

	UE_LOG(LogTemp, Verbose, TEXT("InventoryContextMenuWidget::UpdateButtonVisibility - Use: %d, Drop: %d, Split: %d, Equip: %d"), 
		bShowUse, bShowDrop, bShowSplit, bShowEquip);
}
