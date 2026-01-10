// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/QuickUse/QuickUseBarWidget.h"
#include "Components/UniformGridPanel.h"
#include "UI/QuickUse/QuickUseSlotWidget.h"
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
			// Get quantity from inventory slot
			const TArray<FInventorySlot>& InventorySlots = InventoryComponent->GetInventorySlots();
			if (InventorySlots.IsValidIndex(QuickSlot.InventorySlotIndex))
			{
				const FInventorySlot& InvSlot = InventorySlots[QuickSlot.InventorySlotIndex];
				SlotWidget->SetSlotData(SlotIndex, QuickSlot.Item, QuickSlot.InventorySlotIndex);
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
