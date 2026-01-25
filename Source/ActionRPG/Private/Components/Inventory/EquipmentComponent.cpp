// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Inventory/EquipmentComponent.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Items/Core/ItemDataAsset.h"
#include "Components/Progression/AttributeComponent.h"

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UEquipmentComponent::EquipItem(UEquipmentItem* Item)
{
	if (!Item || !Item->ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent::EquipItem - Invalid item"));
		return false;
	}

	if (Item->ItemData->Type != EItemType::Equipment)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent::EquipItem - Item is not Equipment type"));
		return false;
	}

	const EEquipmentSlot Slot = Item->GetEquipmentSlot();
	if (Slot == EEquipmentSlot::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent::EquipItem - Item has no equipment slot"));
		return false;
	}

	if (Item->GetRequiredLevel() > 1)
	{
		UE_LOG(LogTemp, Log, TEXT("EquipmentComponent::EquipItem - Required level check not yet enforced"));
	}

	if (const UAttributeComponent* AttributeComponent = GetOwner() ? GetOwner()->FindComponentByClass<UAttributeComponent>() : nullptr)
	{
		for (const TPair<EPrimaryAttribute, float>& Requirement : Item->GetRequiredAttributes())
		{
			const float CurrentValue = AttributeComponent->GetAttribute(Requirement.Key);
			if (CurrentValue < Requirement.Value)
			{
				UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent::EquipItem - Attribute requirement not met"));
				return false;
			}
		}
	}
	else if (Item->GetRequiredAttributes().Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent::EquipItem - AttributeComponent missing for requirements"));
		return false;
	}

	EquippedItems.Add(Slot, Item);
	OnEquipmentChanged.Broadcast(Slot, Item);
	UE_LOG(LogTemp, Log, TEXT("EquipmentComponent::EquipItem - Equipped item in slot %d"), static_cast<int32>(Slot));
	return true;
}

UEquipmentItem* UEquipmentComponent::UnequipSlot(EEquipmentSlot Slot)
{
	if (TObjectPtr<UEquipmentItem>* Existing = EquippedItems.Find(Slot))
	{
		UEquipmentItem* RemovedItem = Existing->Get();
		EquippedItems.Remove(Slot);
		OnEquipmentChanged.Broadcast(Slot, nullptr);
		UE_LOG(LogTemp, Log, TEXT("EquipmentComponent::UnequipSlot - Unequipped slot %d"), static_cast<int32>(Slot));
		return RemovedItem;
	}

	return nullptr;
}

bool UEquipmentComponent::IsSlotOccupied(EEquipmentSlot Slot) const
{
	const TObjectPtr<UEquipmentItem>* Existing = EquippedItems.Find(Slot);
	return Existing && Existing->Get() != nullptr;
}

UEquipmentItem* UEquipmentComponent::GetEquippedItem(EEquipmentSlot Slot) const
{
	if (const TObjectPtr<UEquipmentItem>* Existing = EquippedItems.Find(Slot))
	{
		return Existing->Get();
	}

	return nullptr;
}

TMap<EEquipmentSlot, UEquipmentItem*> UEquipmentComponent::GetEquippedItems() const
{
	TMap<EEquipmentSlot, UEquipmentItem*> Result;
	for (const TPair<EEquipmentSlot, TObjectPtr<UEquipmentItem>>& Pair : EquippedItems)
	{
		Result.Add(Pair.Key, Pair.Value.Get());
	}

	return Result;
}
