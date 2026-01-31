// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Equipment/EquipmentSlotWidget.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Items/Equipment/EquipmentItem.h"
#include "GameFramework/Pawn.h"

void UEquipmentSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

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

void UEquipmentSlotWidget::HandleEquipmentChanged(EEquipmentSlot ChangedSlot, UEquipmentItem* Item)
{
	if (ChangedSlot == EquipmentSlot)
	{
		OnSlotUpdated(Item);
	}
}
