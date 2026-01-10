// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/QuickUse/QuickUseBarWidget.h"
#include "Components/UniformGridPanel.h"
#include "UI/QuickUse/QuickUseSlotWidget.h"
#include "UI/Inventory/ItemDragDropOperation.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "GameFramework/PlayerController.h"

void UQuickUseBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("QuickUseBarWidget::NativeConstruct - Constructing quick-use bar widget"));

	// Get InventoryComponent from player character
	InventoryComponent = GetInventoryComponent();
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuickUseBarWidget::NativeConstruct - InventoryComponent not found"));
		return;
	}

	// Bind to quick-use slot changed event
	InventoryComponent->OnQuickUseSlotChanged.AddDynamic(this, &UQuickUseBarWidget::OnQuickUseSlotChangedInternal);
	
	// Bind to inventory changed event to update quick-use slots when item quantities change
	InventoryComponent->OnInventoryChanged.AddDynamic(this, &UQuickUseBarWidget::OnInventoryChangedInternal);

	// Initialize slot widgets
	InitializeSlots();

	// Update display
	UpdateQuickUseBar();

	UE_LOG(LogTemp, Log, TEXT("QuickUseBarWidget::NativeConstruct - Quick-use bar constructed successfully"));
}

void UQuickUseBarWidget::NativeDestruct()
{
	// Unbind from events
	if (InventoryComponent)
	{
		InventoryComponent->OnQuickUseSlotChanged.RemoveAll(this);
		InventoryComponent->OnInventoryChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UQuickUseBarWidget::UpdateQuickUseBar()
{
	if (!InventoryComponent)
	{
		return;
	}

	// Refresh all slots
	for (int32 i = 0; i < SlotWidgets.Num(); i++)
	{
		RefreshSlot(i);
	}
}

void UQuickUseBarWidget::OnQuickUseSlotChanged(int32 QuickUseSlotIndex, UItemBase* Item)
{
	// Refresh the affected slot
	RefreshSlot(QuickUseSlotIndex);
}

void UQuickUseBarWidget::OnQuickUseSlotChangedInternal(int32 QuickUseSlotIndex, UItemBase* Item)
{
	OnQuickUseSlotChanged(QuickUseSlotIndex, Item);
}

void UQuickUseBarWidget::OnInventoryChangedInternal(int32 SlotIndex, UItemBase* Item)
{
	// When an inventory slot changes, refresh any quick-use slots that reference it
	if (!InventoryComponent)
	{
		return;
	}
	
	// Check all quick-use slots to see if any reference this inventory slot
	// There are 10 quick-use slots (0-9)
	for (int32 i = 0; i < 10; i++)
	{
		FQuickUseSlot QuickSlot = InventoryComponent->GetQuickUseSlot(i);
		if (QuickSlot.InventorySlotIndex == SlotIndex)
		{
			// This quick-use slot references the changed inventory slot, refresh it
			RefreshSlot(i);
		}
	}
}

void UQuickUseBarWidget::InitializeSlots()
{
	if (!QuickUseGrid)
	{
		UE_LOG(LogTemp, Error, TEXT("QuickUseBarWidget::InitializeSlots - QuickUseGrid not found! Make sure it's named 'QuickUseGrid' in Blueprint"));
		return;
	}

	if (!SlotWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("QuickUseBarWidget::InitializeSlots - SlotWidgetClass not set! Set it in Blueprint"));
		return;
	}

	// Clear existing slot widgets
	QuickUseGrid->ClearChildren();
	SlotWidgets.Empty();
	SlotWidgets.Reserve(10);

	UE_LOG(LogTemp, Log, TEXT("QuickUseBarWidget::InitializeSlots - Initializing 10 quick-use slot widgets"));

	// Create slot widgets in a horizontal bar (10 slots in 1 row)
	const int32 Columns = 10;
	const int32 Rows = 1;

	for (int32 Row = 0; Row < Rows; Row++)
	{
		for (int32 Col = 0; Col < Columns; Col++)
		{
			int32 SlotIndex = Row * Columns + Col;

			// Create slot widget
			UUserWidget* UserWidget = CreateWidget<UUserWidget>(this, SlotWidgetClass);
			UQuickUseSlotWidget* SlotWidget = Cast<UQuickUseSlotWidget>(UserWidget);
			if (!SlotWidget)
			{
				UE_LOG(LogTemp, Error, TEXT("QuickUseBarWidget::InitializeSlots - Failed to create slot widget for index %d"), SlotIndex);
				continue;
			}

			// Add to grid
			QuickUseGrid->AddChildToUniformGrid(SlotWidget, Row, Col);

			// Store reference
			SlotWidgets.Add(SlotWidget);

			// Set parent reference for drag and drop operations
			SlotWidget->SetParentQuickUseBar(this);

			// Initialize as empty slot
			SlotWidget->SetSlotData(SlotIndex, nullptr, -1);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("QuickUseBarWidget::InitializeSlots - Created %d slot widgets in grid"), SlotWidgets.Num());
}

void UQuickUseBarWidget::RefreshSlot(int32 SlotIndex)
{
	if (!InventoryComponent)
	{
		return;
	}

	if (!SlotWidgets.IsValidIndex(SlotIndex))
	{
		return;
	}

	// Get quick-use slot data
	FQuickUseSlot QuickSlot = InventoryComponent->GetQuickUseSlot(SlotIndex);

	// Update slot widget
	UQuickUseSlotWidget* SlotWidget = SlotWidgets[SlotIndex];
	if (SlotWidget)
	{
		if (QuickSlot.Item && QuickSlot.InventorySlotIndex != -1)
		{
			// Get quantity from inventory slot (always use current quantity from inventory, not from item reference)
			const TArray<FInventorySlot>& InventorySlots = InventoryComponent->GetInventorySlots();
			if (InventorySlots.IsValidIndex(QuickSlot.InventorySlotIndex))
			{
				const FInventorySlot& InvSlot = InventorySlots[QuickSlot.InventorySlotIndex];
				// Use current quantity from inventory slot, not from Item->Quantity (which may be stale)
				int32 CurrentQuantity = InvSlot.Quantity;
				SlotWidget->SetSlotData(SlotIndex, QuickSlot.Item, QuickSlot.InventorySlotIndex, CurrentQuantity);
			}
			else
			{
				SlotWidget->ClearSlot();
			}
		}
		else
		{
			SlotWidget->ClearSlot();
		}
	}
}

UInventoryComponent* UQuickUseBarWidget::GetInventoryComponent() const
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AActionRPGPlayerCharacter* PlayerCharacter = Cast<AActionRPGPlayerCharacter>(PC->GetPawn()))
		{
			return PlayerCharacter->InventoryComponent;
		}
	}

	return nullptr;
}

bool UQuickUseBarWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	// Check if this is an item drag operation from inventory
	UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
	if (!ItemDragOp)
	{
		// Not an item drag operation, let parent handle it
		return false;
	}

	// If the drag operation was already handled by a child slot widget, we're done
	if (ItemDragOp->bWasHandled)
	{
		UE_LOG(LogTemp, Log, TEXT("QuickUseBarWidget::NativeOnDrop - Drag operation already handled by child slot"));
		return true;
	}

	// Get the local position of the drop
	FVector2D LocalPosition = InGeometry.AbsoluteToLocal(InDragDropEvent.GetScreenSpacePosition());
	FVector2D ScreenPosition = InDragDropEvent.GetScreenSpacePosition();
	
	// Check if the drop is over any of our slot widgets by checking their actual geometry
	// Calculate which slot (if any) the drop is over (10 slots in 1 row)
	const int32 Columns = 10;
	const int32 SlotSize = 64; // Approximate slot size in pixels (adjust based on your Blueprint)
	
	int32 Col = FMath::FloorToInt(LocalPosition.X / SlotSize);
	int32 TargetSlotIndex = FMath::Clamp(Col, 0, Columns - 1);
	
	// Validate the slot index and manually forward the drop to the correct slot widget
	// This ensures the slot widget's SlotIndex is properly set when it handles the drop
	if (TargetSlotIndex >= 0 && TargetSlotIndex < SlotWidgets.Num())
	{
		UQuickUseSlotWidget* TargetSlotWidget = SlotWidgets[TargetSlotIndex];
		if (TargetSlotWidget)
		{
			// Verify the slot widget has been properly initialized with its slot index
			// RefreshSlot should have been called to set the slot data, but we verify here
			if (TargetSlotWidget->GetSlotIndex() != TargetSlotIndex)
			{
				// Slot widget has incorrect SlotIndex - fix it now by calling SetSlotData directly
				UE_LOG(LogTemp, Warning, TEXT("QuickUseBarWidget::NativeOnDrop - Slot widget %d has incorrect SlotIndex (%d), fixing it"), 
					TargetSlotIndex, TargetSlotWidget->GetSlotIndex());
				
				// Get current slot data from inventory component to preserve it
				UInventoryComponent* InvComp = InventoryComponent ? InventoryComponent.Get() : GetInventoryComponent();
				if (InvComp)
				{
					FQuickUseSlot QuickSlot = InvComp->GetQuickUseSlot(TargetSlotIndex);
					if (QuickSlot.Item && QuickSlot.InventorySlotIndex != -1)
					{
						const TArray<FInventorySlot>& InventorySlots = InvComp->GetInventorySlots();
						if (InventorySlots.IsValidIndex(QuickSlot.InventorySlotIndex))
						{
							const FInventorySlot& InvSlot = InventorySlots[QuickSlot.InventorySlotIndex];
							int32 CurrentQuantity = InvSlot.Quantity;
							TargetSlotWidget->SetSlotData(TargetSlotIndex, QuickSlot.Item, QuickSlot.InventorySlotIndex, CurrentQuantity);
						}
						else
						{
							// Slot is empty - set with empty data but correct SlotIndex
							TargetSlotWidget->SetSlotData(TargetSlotIndex, nullptr, -1);
						}
					}
					else
					{
						// Slot is empty - set with empty data but correct SlotIndex
						TargetSlotWidget->SetSlotData(TargetSlotIndex, nullptr, -1);
					}
				}
				else
				{
					// InventoryComponent is null - set with empty data but correct SlotIndex
					UE_LOG(LogTemp, Warning, TEXT("QuickUseBarWidget::NativeOnDrop - InventoryComponent is null, setting slot with empty data"));
					TargetSlotWidget->SetSlotData(TargetSlotIndex, nullptr, -1);
				}
				
				// Verify it's now correct
				if (TargetSlotWidget->GetSlotIndex() != TargetSlotIndex)
				{
					UE_LOG(LogTemp, Error, TEXT("QuickUseBarWidget::NativeOnDrop - Failed to fix SlotIndex for slot widget %d (still %d)"), 
						TargetSlotIndex, TargetSlotWidget->GetSlotIndex());
				}
			}
			
			// Drop is over the quick-use bar area - ensure slot widget has correct SlotIndex
			// Then let UMG's natural event routing handle the drop (child widgets receive events before parents)
			// The slot widget's NativeOnDrop will be called automatically by UMG
			UE_LOG(LogTemp, Log, TEXT("QuickUseBarWidget::NativeOnDrop - Drop over quick-use slot %d (slot widget SlotIndex: %d), marking as handled"), 
				TargetSlotIndex, TargetSlotWidget->GetSlotIndex());
			
			// Mark as handled to prevent inventory widget from treating as world drop
			// The child slot widget will handle the actual drop via UMG's event routing
			ItemDragOp->bWasHandled = true;
			return true;
		}
		
		// Drop is over the quick-use bar area but slot widget is null or invalid
		UE_LOG(LogTemp, Warning, TEXT("QuickUseBarWidget::NativeOnDrop - Drop over quick-use bar slot %d but slot widget is null or invalid"), TargetSlotIndex);
		
		// Still mark as handled to prevent inventory widget from treating as world drop
		ItemDragOp->bWasHandled = true;
		return true;
	}

	// Drop is not over the quick-use bar, let parent handle it
	// Don't mark as handled here - let other widgets (like inventory) handle it
	UE_LOG(LogTemp, Verbose, TEXT("QuickUseBarWidget::NativeOnDrop - Drop not over quick-use bar, letting parent handle"));
	return false;
}
