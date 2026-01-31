// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Equipment/EquipmentInventoryWidget.h"
#include "Components/Inventory/InventoryComponent.h"
#include "GameFramework/Pawn.h"

void UEquipmentInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindInventoryComponent();
	OnInventoryUpdated();
}

void UEquipmentInventoryWidget::NativeDestruct()
{
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
