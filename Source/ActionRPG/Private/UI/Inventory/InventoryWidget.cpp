// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Inventory/InventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "GameFramework/PlayerController.h"

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
	
	// Remove from viewport (handled by PlayerController)
	RemoveFromParent();
}

void UInventoryWidget::HandleItemDrop(int32 SourceSlotIndex, int32 TargetSlotIndex, int32 Quantity)
{
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryWidget::HandleItemDrop - InventoryComponent is null"));
		return;
	}

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

	// Note: Quantity parameter is reserved for future partial stack split functionality (right-click drag)
	// Currently, -1 means "entire stack" and MoveItem handles moving the full stack
	// When partial stack splits are implemented, check Quantity != -1 and handle accordingly
	
	// Use InventoryComponent's MoveItem method which handles:
	// - Moving to empty slot
	// - Stacking same items
	// - Swapping different items
	bool bMoved = InventoryComponent->MoveItem(SourceSlotIndex, TargetSlotIndex);
	
	if (bMoved)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleItemDrop - Item moved successfully from slot %d to %d"), 
			SourceSlotIndex, TargetSlotIndex);
		// Slots will be updated via OnInventoryChanged event
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleItemDrop - Failed to move item from slot %d to %d"), 
			SourceSlotIndex, TargetSlotIndex);
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
