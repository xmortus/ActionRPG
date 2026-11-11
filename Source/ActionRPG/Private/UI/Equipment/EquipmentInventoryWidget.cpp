// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Equipment/EquipmentInventoryWidget.h"
#include "UI/ActionRPGUITheme.h"
#include "Components/Inventory/InventoryComponent.h"
#include "UI/Equipment/EquipmentInventorySlotWidget.h"
#include "UI/Equipment/EquipmentDragDropOperation.h"
#include "UI/Inventory/InventoryContextMenuWidget.h"
#include "UI/Inventory/ItemDragDropOperation.h"
#include "Components/UniformGridPanel.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Items/Core/ItemBase.h"

void UEquipmentInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ActionRPGUITheme::StyleBlueprintWidgetTree(WidgetTree);

	BindInventoryComponent();
	OnInventoryUpdated();
}

void UEquipmentInventoryWidget::NativeDestruct()
{
	HideContextMenu();
	if (ContextMenuWidget)
	{
		ContextMenuWidget->RemoveFromParent();
		ContextMenuWidget = nullptr;
	}
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UEquipmentInventoryWidget::HandleInventoryChanged);
	}

	Super::NativeDestruct();
}

void UEquipmentInventoryWidget::BindInventoryComponent()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentInventoryWidget - No owning pawn found."));
		return;
	}

	InventoryComponent = OwningPawn->FindComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentInventoryWidget - InventoryComponent not found."));
		return;
	}

	InventoryComponent->OnInventoryChanged.AddUniqueDynamic(this, &UEquipmentInventoryWidget::HandleInventoryChanged);
}

void UEquipmentInventoryWidget::HandleInventoryChanged(int32 SlotIndex, UItemBase* Item)
{
	OnInventoryUpdated();
}

void UEquipmentInventoryWidget::RefreshInventory()
{
	OnInventoryUpdated();
}

void UEquipmentInventoryWidget::OnInventoryUpdated_Implementation()
{
	if (!InventoryGrid)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentInventoryWidget - InventoryGrid is null. Check BindWidget name."));
		return;
	}

	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentInventoryWidget - InventoryComponent missing."));
		return;
	}

	if (!SlotWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentInventoryWidget - SlotWidgetClass not set."));
		return;
	}

	const TArray<FInventorySlot>& Slots = InventoryComponent->GetInventorySlots();
	if (SlotWidgets.Num() != Slots.Num())
	{
		InventoryGrid->ClearChildren();
		SlotWidgets.Reset();
		const int32 SafeColumns = FMath::Max(1, Columns);
		for (int32 Index = 0; Index < Slots.Num(); ++Index)
		{
			UEquipmentInventorySlotWidget* SlotWidget = CreateWidget<UEquipmentInventorySlotWidget>(GetOwningPlayer(), SlotWidgetClass);
			if (!SlotWidget)
			{
				UE_LOG(LogTemp, Error, TEXT("EquipmentInventoryWidget - Could not create slot %d"), Index);
				return;
			}
			SlotWidget->SetParentInventoryWidget(this);
			SlotWidgets.Add(SlotWidget);
			InventoryGrid->AddChildToUniformGrid(SlotWidget, Index / SafeColumns, Index % SafeColumns);
		}
	}

	for (int32 Index = 0; Index < Slots.Num(); ++Index)
	{
		const FInventorySlot& SlotData = Slots[Index];
		SlotWidgets[Index]->SetupSlot(Index, SlotData.Item, SlotData.Quantity);
	}
}

void UEquipmentInventoryWidget::ShowContextMenuForSlot(int32 SlotIndex)
{
	APlayerController* PC = GetOwningPlayer();
	if (!InventoryComponent || !ContextMenuWidgetClass || !PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentInventoryWidget - Context menu class or inventory is missing"));
		return;
	}
	const TArray<FInventorySlot>& Slots = InventoryComponent->GetInventorySlots();
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].bIsEmpty || !Slots[SlotIndex].Item)
	{
		return;
	}
	if (!ContextMenuWidget)
	{
		ContextMenuWidget = CreateWidget<UInventoryContextMenuWidget>(PC, ContextMenuWidgetClass);
	}
	if (ContextMenuWidget)
	{
		FVector2D ViewportPosition;
		if (!PC->GetMousePosition(ViewportPosition.X, ViewportPosition.Y))
		{
			UE_LOG(LogTemp, Warning, TEXT("EquipmentInventoryWidget - Could not locate mouse in the game viewport"));
			return;
		}
		ContextMenuWidget->InitializeEquipmentMenu(SlotIndex, Slots[SlotIndex].Item, Slots[SlotIndex].Quantity, this);
		ContextMenuWidget->ShowMenuAtPosition(ViewportPosition);
	}
}

void UEquipmentInventoryWidget::HideContextMenu()
{
	if (ContextMenuWidget)
	{
		ContextMenuWidget->HideMenu();
	}
}

void UEquipmentInventoryWidget::UseItemFromContextMenu(int32 SlotIndex)
{
	if (InventoryComponent)
	{
		InventoryComponent->UseItem(SlotIndex);
	}
}

void UEquipmentInventoryWidget::DropItemFromContextMenu(int32 SlotIndex)
{
	if (!InventoryComponent)
	{
		return;
	}
	const TArray<FInventorySlot>& Slots = InventoryComponent->GetInventorySlots();
	if (Slots.IsValidIndex(SlotIndex) && !Slots[SlotIndex].bIsEmpty)
	{
		DropItemToWorld(SlotIndex, Slots[SlotIndex].Quantity, false);
	}
}

void UEquipmentInventoryWidget::SplitItemFromContextMenu(int32 SlotIndex)
{
	if (!InventoryComponent)
	{
		return;
	}
	const TArray<FInventorySlot>& Slots = InventoryComponent->GetInventorySlots();
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].bIsEmpty || Slots[SlotIndex].Quantity <= 1)
	{
		return;
	}
	for (int32 TargetIndex = 0; TargetIndex < Slots.Num(); ++TargetIndex)
	{
		if (Slots[TargetIndex].bIsEmpty)
		{
			InventoryComponent->SplitStackToSlot(SlotIndex, TargetIndex, Slots[SlotIndex].Quantity / 2);
			return;
		}
	}
}

void UEquipmentInventoryWidget::EquipItemFromContextMenu(int32 SlotIndex)
{
	if (InventoryComponent)
	{
		InventoryComponent->EquipItemFromSlot(SlotIndex);
	}
}

bool UEquipmentInventoryWidget::DropItemToWorld(int32 SlotIndex, int32 Quantity, bool bAtCursor)
{
	APlayerController* PC = GetOwningPlayer();
	APawn* Pawn = GetOwningPlayerPawn();
	UWorld* World = GetWorld();
	if (!InventoryComponent || !PC || !Pawn || !World || Quantity <= 0)
	{
		return false;
	}
	const FVector PlayerLocation = Pawn->GetActorLocation();
	FVector DropLocation = PlayerLocation + Pawn->GetActorForwardVector() * 150.0f;
	if (bAtCursor)
	{
		FVector RayOrigin;
		FVector RayDirection;
		if (PC->DeprojectMousePositionToWorld(RayOrigin, RayDirection) && FMath::Abs(RayDirection.Z) > KINDA_SMALL_NUMBER)
		{
			const float DistanceAlongRay = (PlayerLocation.Z - RayOrigin.Z) / RayDirection.Z;
			if (DistanceAlongRay > 0.0f)
			{
				const FVector CursorLocation = RayOrigin + RayDirection * DistanceAlongRay;
				DropLocation = PlayerLocation + (CursorLocation - PlayerLocation).GetClampedToMaxSize2D(500.0f);
			}
		}
	}
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Pawn);
	FHitResult Hit;
	const FVector TraceStart(DropLocation.X, DropLocation.Y, PlayerLocation.Z + 500.0f);
	const FVector TraceEnd(DropLocation.X, DropLocation.Y, PlayerLocation.Z - 1000.0f);
	if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params) ||
		World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldDynamic, Params))
	{
		DropLocation = Hit.ImpactPoint + FVector(0.0f, 0.0f, 5.0f);
	}
	return InventoryComponent->DropItemToWorld(SlotIndex, Quantity, DropLocation);
}

bool UEquipmentInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UEquipmentDragDropOperation* EquipmentDragOp = Cast<UEquipmentDragDropOperation>(InOperation);
	if (UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation))
	{
		if (ItemDragOp->bWasHandled)
		{
			return true;
		}
		if (DropItemToWorld(ItemDragOp->SourceSlotIndex, ItemDragOp->Quantity, true))
		{
			ItemDragOp->bWasHandled = true;
			return true;
		}
		return false;
	}
	if (!EquipmentDragOp)
	{
		return false;
	}

	if (!InventoryComponent)
	{
		return false;
	}

	const bool bUnequipped = InventoryComponent->UnequipItemToInventory(EquipmentDragOp->EquipmentSlot);
	if (bUnequipped)
	{
		EquipmentDragOp->bWasHandled = true;
		return true;
	}

	return false;
}
