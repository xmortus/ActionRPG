// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Inventory/InventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "UI/Inventory/ItemDragDropOperation.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeConstruct - Constructing inventory widget"));

	// Verify widget bindings
	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeConstruct - Checking widget bindings:"));
	UE_LOG(LogTemp, Log, TEXT("  - InventoryGrid: %s"), InventoryGrid ? TEXT("Found") : TEXT("NOT FOUND - Check name matches in Blueprint!"));
	UE_LOG(LogTemp, Log, TEXT("  - WeightText: %s"), WeightText ? TEXT("Found") : TEXT("NOT FOUND - Check name matches in Blueprint!"));
	UE_LOG(LogTemp, Log, TEXT("  - CapacityText: %s"), CapacityText ? TEXT("Found") : TEXT("NOT FOUND - Check name matches in Blueprint!"));
	UE_LOG(LogTemp, Log, TEXT("  - CloseButton: %s"), CloseButton ? TEXT("Found") : TEXT("NOT FOUND - Check name matches in Blueprint!"));
	
	if (!InventoryGrid)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::NativeConstruct - InventoryGrid widget binding failed! Make sure widget named 'InventoryGrid' exists in Blueprint"));
	}
	if (!WeightText)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::NativeConstruct - WeightText widget binding failed! Make sure widget named 'WeightText' exists in Blueprint"));
	}
	if (!CapacityText)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::NativeConstruct - CapacityText widget binding failed! Make sure widget named 'CapacityText' exists in Blueprint"));
	}
	if (!CloseButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::NativeConstruct - CloseButton widget binding failed! Make sure widget named 'CloseButton' exists in Blueprint"));
	}

	// Get InventoryComponent from player character
	InventoryComponent = GetInventoryComponent();
	
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::NativeConstruct - Failed to get InventoryComponent!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeConstruct - InventoryComponent found"));

	// Bind to InventoryComponent events
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::OnInventoryChanged);
		InventoryComponent->OnItemAdded.AddDynamic(this, &UInventoryWidget::OnItemAdded);
		InventoryComponent->OnItemRemoved.AddDynamic(this, &UInventoryWidget::OnItemRemoved);
		
		UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeConstruct - Bound to InventoryComponent events"));
	}

	// Bind close button
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnCloseButtonClicked);
		UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeConstruct - Bound close button"));
	}

	// Initialize slot widgets
	InitializeSlots();

	// Initial display update
	UpdateInventoryDisplay();

	// Log widget status
	// Note: GetDesiredSize() may return 0.0 for Canvas Panel widgets until after layout pass
	// This is normal and doesn't indicate a problem if the widget renders correctly
	FVector2D WidgetSize = GetDesiredSize();
	if (WidgetSize.X > 0 && WidgetSize.Y > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeConstruct - Widget size: %.1f x %.1f, Visibility: %d"), 
			WidgetSize.X, WidgetSize.Y, (int32)GetVisibility());
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::NativeConstruct - Widget size not yet calculated (%.1f x %.1f) - this is normal for Canvas Panel widgets. Visibility: %d"), 
			WidgetSize.X, WidgetSize.Y, (int32)GetVisibility());
	}

	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeConstruct - Widget constructed successfully"));
}

void UInventoryWidget::NativeDestruct()
{
	// Unbind from events
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UInventoryWidget::OnInventoryChanged);
		InventoryComponent->OnItemAdded.RemoveDynamic(this, &UInventoryWidget::OnItemAdded);
		InventoryComponent->OnItemRemoved.RemoveDynamic(this, &UInventoryWidget::OnItemRemoved);
	}

	// Clear slot widgets
	SlotWidgets.Empty();

	Super::NativeDestruct();
}

void UInventoryWidget::UpdateInventoryDisplay()
{
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::UpdateInventoryDisplay - InventoryComponent is null"));
		return;
	}

	UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::UpdateInventoryDisplay - Updating all slots"));

	// Refresh all slots
	const TArray<FInventorySlot>& InventorySlots = InventoryComponent->GetInventorySlots();
	
	for (int32 i = 0; i < SlotWidgets.Num() && i < InventorySlots.Num(); i++)
	{
		RefreshSlot(i);
	}

	// Update weight display
	if (WeightText)
	{
		float CurrentWeight = InventoryComponent->GetCurrentWeight();
		float MaxWeight = InventoryComponent->GetMaxWeight();
		FText WeightDisplayText = FText::Format(FText::FromString(TEXT("Weight: {0} / {1}")), 
			FText::AsNumber(FMath::RoundToInt(CurrentWeight)), 
			FText::AsNumber(FMath::RoundToInt(MaxWeight)));
		WeightText->SetText(WeightDisplayText);
	}

	// Update capacity display (slot-based, not item count)
	if (CapacityText)
	{
		int32 UsedSlots = InventoryComponent->GetUsedSlotCount();
		int32 MaxCapacity = InventoryComponent->GetMaxCapacity();
		int32 EmptySlots = InventoryComponent->GetEmptySlotCount();
		FText CapacityDisplayText = FText::Format(FText::FromString(TEXT("Capacity: {0} / {1} ({2} empty)")), 
			FText::AsNumber(UsedSlots), 
			FText::AsNumber(MaxCapacity),
			FText::AsNumber(EmptySlots));
		CapacityText->SetText(CapacityDisplayText);
	}

	UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::UpdateInventoryDisplay - Display updated"));
}

void UInventoryWidget::OnInventorySlotClicked(int32 SlotIndex)
{
	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::OnInventorySlotClicked - Slot %d clicked"), SlotIndex);
	
	// Left click handling - will be used for drag and drop in Day 24
	// For now, just log the click
}

void UInventoryWidget::OnInventorySlotRightClicked(int32 SlotIndex)
{
	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::OnInventorySlotRightClicked - Slot %d right-clicked (use item)"), SlotIndex);
	
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::OnInventorySlotRightClicked - InventoryComponent is null"));
		return;
	}

	// Use item at slot
	bool bUsed = InventoryComponent->UseItem(SlotIndex);
	
	if (bUsed)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryWidget::OnInventorySlotRightClicked - Item used successfully from slot %d"), SlotIndex);
		// Slot will be updated via OnInventoryChanged event
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::OnInventorySlotRightClicked - Failed to use item from slot %d"), SlotIndex);
	}
}

void UInventoryWidget::CloseInventory()
{
	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::CloseInventory - Closing inventory"));
	
	// Don't remove from parent here - let the PlayerController handle widget lifecycle
	// If this widget was added via AddToViewport(), RemoveFromParent() will cause warnings
	// Instead, we just hide the widget and let PlayerController's OnOpenInventory handle the toggle
	// Using Collapsed instead of Hidden to prevent the widget from taking up layout space
	SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryWidget::HandleItemDropLegacy(int32 SourceSlotIndex, int32 TargetSlotIndex, int32 Quantity)
{
	// Legacy method for backward compatibility
	// Create a temporary drag operation and call the main HandleItemDrop method
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::HandleItemDropLegacy - InventoryComponent is null"));
		return;
	}

	// Validate slot indices
	const TArray<FInventorySlot>& InventorySlots = InventoryComponent->GetInventorySlots();
	
	if (!InventorySlots.IsValidIndex(SourceSlotIndex) || !InventorySlots.IsValidIndex(TargetSlotIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::HandleItemDropLegacy - Invalid slot indices (Source: %d, Target: %d)"), 
			SourceSlotIndex, TargetSlotIndex);
		return;
	}

	// Get item from source slot
	const FInventorySlot& SourceSlot = InventorySlots[SourceSlotIndex];
	if (SourceSlot.bIsEmpty || !SourceSlot.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleItemDropLegacy - Source slot %d is empty"), SourceSlotIndex);
		return;
	}

	// Create temporary drag operation
	UItemDragDropOperation* TempDragOp = NewObject<UItemDragDropOperation>(this, UItemDragDropOperation::StaticClass());
	if (!TempDragOp)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::HandleItemDropLegacy - Failed to create temporary drag operation"));
		return;
	}

	TempDragOp->SourceSlotIndex = SourceSlotIndex;
	TempDragOp->Item = SourceSlot.Item;
	TempDragOp->SourceQuantity = SourceSlot.Quantity;
	
	// Set quantity (if -1, use full stack)
	if (Quantity == -1 || Quantity > SourceSlot.Quantity)
	{
		TempDragOp->Quantity = SourceSlot.Quantity;
		TempDragOp->bIsSplitOperation = false;
	}
	else
	{
		TempDragOp->Quantity = Quantity;
		TempDragOp->bIsSplitOperation = (Quantity < SourceSlot.Quantity);
	}

	// Call the main HandleItemDrop method
	HandleItemDrop(TempDragOp, TargetSlotIndex);
}

void UInventoryWidget::HandleItemDrop(UItemDragDropOperation* DragOperation, int32 TargetSlotIndex)
{
	if (!InventoryComponent || !DragOperation)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::HandleItemDrop - Invalid parameters"));
		return;
	}

	int32 SourceSlotIndex = DragOperation->SourceSlotIndex;

	// Validate slot indices
	const TArray<FInventorySlot>& InventorySlots = InventoryComponent->GetInventorySlots();
	
	if (!InventorySlots.IsValidIndex(SourceSlotIndex) || !InventorySlots.IsValidIndex(TargetSlotIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::HandleItemDrop - Invalid slot indices (Source: %d, Target: %d)"), 
			SourceSlotIndex, TargetSlotIndex);
		return;
	}

	// Handle same slot drop (do nothing)
	if (SourceSlotIndex == TargetSlotIndex)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleItemDrop - Drop on same slot, ignoring"));
		return;
	}

	// Handle split operation
	if (DragOperation->bIsSplitOperation)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleItemDrop - Split operation: Moving %d/%d from slot %d to slot %d"),
			DragOperation->Quantity, DragOperation->SourceQuantity, SourceSlotIndex, TargetSlotIndex);

		// Use SplitStackToSlot to split directly to target slot
		bool bSuccess = InventoryComponent->SplitStackToSlot(SourceSlotIndex, TargetSlotIndex, DragOperation->Quantity);
		
		if (bSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleItemDrop - Split stack successfully moved to target slot"));
			UpdateInventoryDisplay();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleItemDrop - Failed to split stack to target slot"));
			UpdateInventoryDisplay(); // Refresh to restore source slot
		}
	}
	else
	{
		// Normal operation: move entire stack
		bool bMoved = InventoryComponent->MoveItem(SourceSlotIndex, TargetSlotIndex);
		
		if (bMoved)
		{
			UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleItemDrop - Item moved successfully from slot %d to %d"), 
				SourceSlotIndex, TargetSlotIndex);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleItemDrop - Failed to move item from slot %d to %d"), 
				SourceSlotIndex, TargetSlotIndex);
		}
	}
}

void UInventoryWidget::OnInventoryChanged(int32 SlotIndex, UItemBase* Item)
{
	UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::OnInventoryChanged - Slot %d changed"), SlotIndex);
	
	// Refresh the affected slot
	RefreshSlot(SlotIndex);
	
	// Update weight/capacity displays
	if (WeightText)
	{
		float CurrentWeight = InventoryComponent->GetCurrentWeight();
		float MaxWeight = InventoryComponent->GetMaxWeight();
		FText WeightDisplayText = FText::Format(FText::FromString(TEXT("Weight: {0} / {1}")), 
			FText::AsNumber(FMath::RoundToInt(CurrentWeight)), 
			FText::AsNumber(FMath::RoundToInt(MaxWeight)));
		WeightText->SetText(WeightDisplayText);
	}

	if (CapacityText)
	{
		int32 UsedSlots = InventoryComponent->GetUsedSlotCount();
		int32 MaxCapacity = InventoryComponent->GetMaxCapacity();
		int32 EmptySlots = InventoryComponent->GetEmptySlotCount();
		FText CapacityDisplayText = FText::Format(FText::FromString(TEXT("Capacity: {0} / {1} ({2} empty)")), 
			FText::AsNumber(UsedSlots), 
			FText::AsNumber(MaxCapacity),
			FText::AsNumber(EmptySlots));
		CapacityText->SetText(CapacityDisplayText);
	}
}

void UInventoryWidget::OnItemAdded(UItemBase* Item)
{
	UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::OnItemAdded - Item added: %s"), 
		Item && Item->ItemData ? *Item->ItemData->ItemName.ToString() : TEXT("Unknown"));
	
	// Full refresh to ensure all slots are updated (stacking might affect multiple slots)
	UpdateInventoryDisplay();
}

void UInventoryWidget::OnItemRemoved(UItemBase* Item, int32 Quantity)
{
	UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::OnItemRemoved - Item removed: %s (Quantity: %d)"), 
		Item && Item->ItemData ? *Item->ItemData->ItemName.ToString() : TEXT("Unknown"), Quantity);
	
	// Full refresh to ensure all slots are updated
	UpdateInventoryDisplay();
}

void UInventoryWidget::OnCloseButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::OnCloseButtonClicked - Close button clicked"));
	CloseInventory();
}

void UInventoryWidget::InitializeSlots()
{
	if (!InventoryGrid)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::InitializeSlots - InventoryGrid not found! Make sure it's named 'InventoryGrid' in Blueprint"));
		return;
	}

	if (!SlotWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::InitializeSlots - SlotWidgetClass not set! Set it in Blueprint"));
		return;
	}

	// Get inventory capacity (use stored InventoryComponent or get it)
	UInventoryComponent* InvComp = InventoryComponent.Get();
	if (!InvComp)
	{
		InvComp = GetInventoryComponent();
	}
	if (!InvComp)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::InitializeSlots - InventoryComponent not found"));
		return;
	}

	// Get inventory capacity
	int32 MaxCapacity = InvComp->GetMaxCapacity();
	
	// Clear existing slot widgets
	InventoryGrid->ClearChildren();
	SlotWidgets.Empty();
	SlotWidgets.Reserve(MaxCapacity);

	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::InitializeSlots - Initializing %d slot widgets"), MaxCapacity);

	// Create slot widgets in a 10x5 grid (10 columns, 5 rows = 50 slots)
	const int32 Columns = 10;
	const int32 Rows = FMath::CeilToInt((float)MaxCapacity / (float)Columns);

	for (int32 Row = 0; Row < Rows; Row++)
	{
		for (int32 Col = 0; Col < Columns; Col++)
		{
			int32 SlotIndex = Row * Columns + Col;
			
			if (SlotIndex >= MaxCapacity)
			{
				break; // Don't create more slots than capacity
			}

			// Create slot widget
			// Use UUserWidget as base type for CreateWidget, then cast
			UUserWidget* UserWidget = CreateWidget<UUserWidget>(this, SlotWidgetClass);
			UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(UserWidget);
			if (!SlotWidget)
			{
				UE_LOG(LogTemp, Error, TEXT("InventoryWidget::InitializeSlots - Failed to create slot widget for index %d (SlotWidgetClass may not be a UInventorySlotWidget subclass)"), SlotIndex);
				continue;
			}

			// Bind slot events (before adding to grid)
			SlotWidget->OnSlotClicked.AddDynamic(this, &UInventoryWidget::OnInventorySlotClicked);
			SlotWidget->OnSlotRightClicked.AddDynamic(this, &UInventoryWidget::OnInventorySlotRightClicked);

			// Add to grid (this will trigger NativeConstruct)
			InventoryGrid->AddChildToUniformGrid(SlotWidget, Row, Col);
			
			// Set parent inventory widget reference AFTER adding to grid (after NativeConstruct)
			// This ensures the parent is set and won't be reset by NativeConstruct
			SlotWidget->SetParentInventoryWidget(this);
			UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::InitializeSlots - Set parent widget for slot %d"), SlotIndex);
			
			// Store reference (SlotWidgets is TArray<TObjectPtr<UInventorySlotWidget>>)
			SlotWidgets.Add(SlotWidget);

			// Initialize as empty slot - use SetSlotData to preserve SlotIndex (don't use ClearSlot which resets it)
			SlotWidget->SetSlotData(SlotIndex, nullptr, 0);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::InitializeSlots - Created %d slot widgets in grid"), SlotWidgets.Num());
}

void UInventoryWidget::RefreshSlot(int32 SlotIndex)
{
	if (!InventoryComponent)
	{
		return;
	}

	if (!SlotWidgets.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::RefreshSlot - Invalid slot index: %d"), SlotIndex);
		return;
	}

	// Get slot data from inventory
	const TArray<FInventorySlot>& InventorySlots = InventoryComponent->GetInventorySlots();
	
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::RefreshSlot - Invalid inventory slot index: %d"), SlotIndex);
		return;
	}

	const FInventorySlot& InventorySlot = InventorySlots[SlotIndex];
	UInventorySlotWidget* SlotWidget = SlotWidgets[SlotIndex];

	if (!SlotWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::RefreshSlot - Slot widget is null for index: %d"), SlotIndex);
		return;
	}

	// Update slot widget with inventory slot data
	// Always set slot data with the correct index, even for empty slots
	// This ensures SlotIndex is always valid for drag and drop operations
	if (InventorySlot.bIsEmpty || !InventorySlot.Item || InventorySlot.Quantity <= 0)
	{
		SlotWidget->SetSlotData(SlotIndex, nullptr, 0);
	}
	else
	{
		SlotWidget->SetSlotData(SlotIndex, InventorySlot.Item, InventorySlot.Quantity);
	}
}

UInventoryComponent* UInventoryWidget::GetInventoryComponent() const
{
	// Get PlayerController
	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::GetInventoryComponent - PlayerController is null"));
		return nullptr;
	}

	// Get PlayerCharacter
	APawn* Pawn = PlayerController->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::GetInventoryComponent - Pawn is null"));
		return nullptr;
	}

	AActionRPGPlayerCharacter* PlayerCharacter = Cast<AActionRPGPlayerCharacter>(Pawn);
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::GetInventoryComponent - Pawn is not AActionRPGPlayerCharacter"));
		return nullptr;
	}

	// Get InventoryComponent
	UInventoryComponent* InvComp = PlayerCharacter->InventoryComponent;
	if (!InvComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::GetInventoryComponent - InventoryComponent is null on PlayerCharacter"));
		return nullptr;
	}

	return InvComp;
}

bool UInventoryWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// Accept all item drag operations to enable world drops
	// This allows the widget to accept drops anywhere, including outside slots
	UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
	if (ItemDragOp)
	{
		// Store active drag operation for world drop detection
		ActiveDragOperation = ItemDragOp;
		UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::NativeOnDragOver - Accepting item drag operation"));
		return true; // Accept the drag operation
	}

	return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	// Check if this is an item drag operation
	UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
	if (!ItemDragOp)
	{
		return false;
	}

	// Clear active drag operation since drop was handled
	ActiveDragOperation = nullptr;

	// Get the local position of the drop
	FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	
	// Find which slot (if any) the drop is over
	int32 TargetSlotIndex = FindSlotAtScreenPosition(LocalPosition);

	if (TargetSlotIndex >= 0 && TargetSlotIndex < SlotWidgets.Num())
	{
		// Drop is on a valid slot - handle normally
		UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeOnDrop - Drop on slot %d, routing to HandleItemDrop"), TargetSlotIndex);
		HandleItemDrop(ItemDragOp, TargetSlotIndex);
		return true;
	}
	else
	{
		// Drop is outside any slot - handle as world drop
		UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeOnDrop - Drop outside slots (target slot: %d), handling as world drop"), TargetSlotIndex);
		HandleDragToWorld(ItemDragOp, InDragDropEvent);
		return true;
	}
}

FReply UInventoryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Get the local position of the click
	FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	
	// Check if click is on an inventory slot using the same logic as FindSlotAtScreenPosition
	int32 ClickedSlotIndex = FindSlotAtScreenPosition(LocalPosition);
	bool bClickOnSlot = (ClickedSlotIndex >= 0 && ClickedSlotIndex < SlotWidgets.Num());
	
	// Also check if click is on the CloseButton
	if (!bClickOnSlot && CloseButton && CloseButton->IsValidLowLevel())
	{
		FGeometry ButtonGeometry = CloseButton->GetCachedGeometry();
		FVector2D ScreenPosition = InMouseEvent.GetScreenSpacePosition();
		FVector2D ButtonScreenPos = ButtonGeometry.LocalToAbsolute(FVector2D::ZeroVector);
		FVector2D ButtonScreenSize = ButtonGeometry.GetLocalSize() * ButtonGeometry.Scale;
		
		if (ScreenPosition.X >= ButtonScreenPos.X && ScreenPosition.X <= ButtonScreenPos.X + ButtonScreenSize.X &&
			ScreenPosition.Y >= ButtonScreenPos.Y && ScreenPosition.Y <= ButtonScreenPos.Y + ButtonScreenSize.Y)
		{
			bClickOnSlot = true;
			UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::NativeOnMouseButtonDown - Click on CloseButton"));
		}
	}
	
	if (bClickOnSlot)
	{
		// Click is on an inventory slot or close button - let the child widget handle it
		// Return Unhandled so the child widget can receive the event (child widgets are in front and will receive it first)
		UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::NativeOnMouseButtonDown - Click on inventory slot/button %d, allowing child widget to handle"), ClickedSlotIndex);
		return FReply::Unhandled();
	}
	else
	{
		// Click is outside inventory slots and buttons - allow click to pass through to widgets behind (like quick-use bar)
		// This ensures the quick-use bar remains clickable when inventory is open
		UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::NativeOnMouseButtonDown - Click outside inventory slots/buttons (local pos: %.1f, %.1f), allowing click to pass through to quick-use bar"), 
			LocalPosition.X, LocalPosition.Y);
		return FReply::Unhandled();
	}
}

int32 UInventoryWidget::FindSlotAtScreenPosition(const FVector2D& LocalPosition) const
{
	// Calculate which slot based on grid layout (10 columns)
	// This is approximate - for exact slot detection, we'd need to check each slot's actual geometry
	if (!InventoryGrid || SlotWidgets.Num() == 0)
	{
		return -1;
	}

	const int32 Columns = 10;
	const int32 SlotSize = 64; // Approximate slot size in pixels (adjust based on your Blueprint)
	
	int32 Col = FMath::FloorToInt(LocalPosition.X / SlotSize);
	int32 Row = FMath::FloorToInt(LocalPosition.Y / SlotSize);
	int32 CalculatedIndex = Row * Columns + Col;
	
	// Validate the calculated index
	if (CalculatedIndex >= 0 && CalculatedIndex < SlotWidgets.Num())
	{
		return CalculatedIndex;
	}

	// Could not determine slot - return -1 to indicate world drop
	return -1;
}


void UInventoryWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeOnDragCancelled - Drag cancelled event received"));

	// Check if this is an item drag operation
	UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
	if (!ItemDragOp)
	{
		// Not an item drag operation, ignore
		UE_LOG(LogTemp, Verbose, TEXT("InventoryWidget::NativeOnDragCancelled - Not an item drag operation, ignoring"));
		return;
	}

	// Check if the drag was already handled by a drop
	if (ItemDragOp->bWasHandled)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeOnDragCancelled - Drag operation was already handled, ignoring"));
		ActiveDragOperation = nullptr;
		return;
	}

	// Handle world drop - item was dropped outside the inventory widget
	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::NativeOnDragCancelled - Handling world drop (drag was not handled)"));
	HandleDragToWorld(ItemDragOp, InDragDropEvent);
	ActiveDragOperation = nullptr;
}

void UInventoryWidget::HandleDragToWorld(UItemDragDropOperation* DragOperation, const FDragDropEvent& DragDropEvent)
{
	if (!DragOperation || !InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleDragToWorld - Invalid drag operation or inventory component"));
		return;
	}

	// Get the source slot index and quantity
	int32 SourceSlotIndex = DragOperation->SourceSlotIndex;
	int32 Quantity = DragOperation->Quantity;

	if (SourceSlotIndex < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleDragToWorld - Invalid source slot index: %d"), SourceSlotIndex);
		return;
	}

	// Get player controller for screen-to-world conversion
	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleDragToWorld - PlayerController is null"));
		return;
	}

	// Get player character for drop location calculation
	APawn* Pawn = PlayerController->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleDragToWorld - Pawn is null"));
		return;
	}

	AActionRPGPlayerCharacter* PlayerCharacter = Cast<AActionRPGPlayerCharacter>(Pawn);
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleDragToWorld - Pawn is not AActionRPGPlayerCharacter"));
		return;
	}

	// Calculate world drop location
	// For top-down gameplay, drop item in front of player character at a fixed distance
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector ForwardVector = PlayerCharacter->GetActorForwardVector();
	
	// Drop item 150 units in front of player character
	float DropDistance = 150.0f;
	FVector DropLocation = PlayerLocation + (ForwardVector * DropDistance);
	
	// Alternative: Use mouse position to determine drop location
	// This allows dropping where the mouse cursor is (for top-down camera)
	FVector WorldLocation;
	FVector WorldDirection;
	
	if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		// Calculate intersection point on the ground plane at player's height
		float PlaneZ = PlayerLocation.Z;
		
		// Calculate intersection with horizontal plane using ray-plane intersection
		if (FMath::Abs(WorldDirection.Z) > 0.001f) // Avoid division by zero
		{
			// Ray-plane intersection formula: T = (PlaneZ - RayOrigin.Z) / RayDirection.Z
			float T = (PlaneZ - WorldLocation.Z) / WorldDirection.Z;
			FVector MouseWorldLocation = WorldLocation + (WorldDirection * T);
			
			// Limit drop distance to reasonable range (max 500 units from player)
			FVector ToMouseLocation = MouseWorldLocation - PlayerLocation;
			float DistanceToMouse = ToMouseLocation.Size2D();
			
			if (DistanceToMouse <= 500.0f)
			{
				// Use mouse location if within range
				DropLocation = MouseWorldLocation;
			}
			else
			{
				// Clamp to max distance if too far
				FVector ClampedDirection = ToMouseLocation.GetSafeNormal2D();
				DropLocation = PlayerLocation + (ClampedDirection * 500.0f);
			}
		}
	}

	// Find actual ground level at drop location using line trace
	UWorld* World = GetWorld();
	if (World)
	{
		// Trace downward from above the drop location to find ground
		// Start from a high point (player location + 500 units) to ensure we're above terrain
		FVector TraceStart = DropLocation;
		TraceStart.Z = FMath::Max(PlayerLocation.Z + 500.0f, DropLocation.Z + 500.0f);
		
		// Trace down to well below the player (player Z - 1000 units) to find ground
		FVector TraceEnd = DropLocation;
		TraceEnd.Z = PlayerLocation.Z - 1000.0f;
		
		// Use world's LineTraceSingleByChannel - FHitResult and FCollisionQueryParams should be available through Engine/World.h
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(PlayerCharacter); // Ignore player character
		QueryParams.bTraceComplex = false; // Use simple collision for performance
		
		// Trace against world static objects (ground, terrain, etc.)
		if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
		{
			// Found ground - use hit location, but add a small offset above ground to prevent clipping
			DropLocation = HitResult.ImpactPoint;
			DropLocation.Z += 5.0f; // Small offset to place item slightly above ground
			UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleDragToWorld - Found ground at Z=%.2f (adjusted to %.2f) using line trace"), 
				HitResult.ImpactPoint.Z, DropLocation.Z);
		}
		else
		{
			// Try tracing against WorldDynamic as fallback (for platforms, etc.)
			if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldDynamic, QueryParams))
			{
				DropLocation = HitResult.ImpactPoint;
				DropLocation.Z += 5.0f; // Small offset above ground
				UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleDragToWorld - Found surface (WorldDynamic) at Z=%.2f (adjusted to %.2f)"), 
					HitResult.ImpactPoint.Z, DropLocation.Z);
			}
			else
			{
				// No ground found - use player's Z as fallback (but still try to place on ground plane)
				// For top-down games, player Z is usually at ground level
				DropLocation.Z = PlayerLocation.Z;
				UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleDragToWorld - No ground found via line trace, using player Z=%.2f as fallback"), DropLocation.Z);
			}
		}
	}
	else
	{
		// No world - use player's Z as fallback
		DropLocation.Z = PlayerLocation.Z;
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleDragToWorld - World is null, using player Z=%.2f as fallback"), DropLocation.Z);
	}

	// Log drop attempt
	UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleDragToWorld - Attempting to drop item from slot %d (Quantity: %d) to world at location (%.2f, %.2f, %.2f)"), 
		SourceSlotIndex, Quantity, DropLocation.X, DropLocation.Y, DropLocation.Z);
	
	// Drop item to world at calculated location
	bool bSuccess = InventoryComponent->DropItemToWorld(SourceSlotIndex, Quantity, DropLocation);
	
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleDragToWorld - Successfully dropped item from slot %d to world at location (%.2f, %.2f, %.2f)"), 
			SourceSlotIndex, DropLocation.X, DropLocation.Y, DropLocation.Z);
		
		// Update inventory display to reflect item removal
		UpdateInventoryDisplay();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::HandleDragToWorld - FAILED to drop item from slot %d to world. Check DropItemToWorld implementation and ItemPickupActor spawning."), SourceSlotIndex);
	}
}
