// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Equipment/EquipmentWidget.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Items/Equipment/EquipmentItem.h"
#include "GameFramework/Pawn.h"

void UEquipmentWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindComponents();
	RefreshEquipment();
	RefreshInventory();
	RefreshStats();
}

void UEquipmentWidget::NativeDestruct()
{
	if (EquipmentComponent)
	{
		EquipmentComponent->OnEquipmentChanged.RemoveDynamic(this, &UEquipmentWidget::HandleEquipmentChanged);
	}

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UEquipmentWidget::HandleInventoryChanged);
	}

	if (AttributeComponent)
	{
		AttributeComponent->OnPrimaryAttributeChanged.RemoveDynamic(this, &UEquipmentWidget::HandlePrimaryAttributeChanged);
	}

	if (SecondaryAttributeComponent)
	{
		SecondaryAttributeComponent->OnSecondaryAttributeChanged.RemoveDynamic(this, &UEquipmentWidget::HandleSecondaryAttributeChanged);
	}

	Super::NativeDestruct();
}

void UEquipmentWidget::BindComponents()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentWidget - No owning pawn found."));
		return;
	}

	EquipmentComponent = OwningPawn->FindComponentByClass<UEquipmentComponent>();
	InventoryComponent = OwningPawn->FindComponentByClass<UInventoryComponent>();
	AttributeComponent = OwningPawn->FindComponentByClass<UAttributeComponent>();
	SecondaryAttributeComponent = OwningPawn->FindComponentByClass<USecondaryAttributeComponent>();

	if (EquipmentComponent)
	{
		EquipmentComponent->OnEquipmentChanged.AddUniqueDynamic(this, &UEquipmentWidget::HandleEquipmentChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentWidget - EquipmentComponent not found."));
	}

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddUniqueDynamic(this, &UEquipmentWidget::HandleInventoryChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentWidget - InventoryComponent not found."));
	}

	if (AttributeComponent)
	{
		AttributeComponent->OnPrimaryAttributeChanged.AddUniqueDynamic(this, &UEquipmentWidget::HandlePrimaryAttributeChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentWidget - AttributeComponent not found."));
	}

	if (SecondaryAttributeComponent)
	{
		SecondaryAttributeComponent->OnSecondaryAttributeChanged.AddUniqueDynamic(this, &UEquipmentWidget::HandleSecondaryAttributeChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentWidget - SecondaryAttributeComponent not found."));
	}
}

void UEquipmentWidget::RefreshEquipment()
{
	OnEquipmentUpdated();
}

void UEquipmentWidget::RefreshInventory()
{
	OnInventoryUpdated();
}

void UEquipmentWidget::RefreshStats()
{
	OnStatsUpdated();
}

void UEquipmentWidget::HandleEquipmentChanged(EEquipmentSlot ChangedSlot, UEquipmentItem* Item)
{
	RefreshEquipment();
	RefreshStats();
}

void UEquipmentWidget::HandleInventoryChanged(int32 SlotIndex, UItemBase* Item)
{
	RefreshInventory();
}

void UEquipmentWidget::HandlePrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue)
{
	RefreshStats();
}

void UEquipmentWidget::HandleSecondaryAttributeChanged(ESecondaryAttribute Attribute, float NewValue)
{
	RefreshStats();
}
