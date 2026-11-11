// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Inventory/EquipmentComponent.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Equipment/WeaponItem.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/ClassComponent.h"

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UEquipmentComponent::EquipItem(UEquipmentItem* Item)
{
	const FText FailureReason = GetEquipFailureReason(Item);
	if (!FailureReason.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent::EquipItem - %s"), *FailureReason.ToString());
		return false;
	}

	const EEquipmentSlot Slot = Item->GetEquipmentSlot();
	if (UWeaponItem* Weapon = Cast<UWeaponItem>(Item); Weapon && !Weapon->BindToOwner(GetOwner())) return false;
	EquippedItems.Add(Slot, Item);
	OnEquipmentChanged.Broadcast(Slot, Item);
	OnEquipmentChangedNative.Broadcast(Slot, Item);
	UE_LOG(LogTemp, Log, TEXT("EquipmentComponent::EquipItem - Equipped item in slot %d"), static_cast<int32>(Slot));
	return true;
}

FText UEquipmentComponent::GetEquipFailureReason(const UEquipmentItem* Item, EEquipmentSlot TargetSlot) const
{
	if (!Item || !Item->ItemData)
	{
		return FText::FromString(TEXT("Invalid equipment item"));
	}

	if (Item->ItemData->Type != EItemType::Equipment)
	{
		return FText::FromString(TEXT("Item is not equipment"));
	}

	const EEquipmentSlot Slot = Item->GetEquipmentSlot();
	if (Slot == EEquipmentSlot::None)
	{
		return FText::FromString(TEXT("Item has no equipment slot"));
	}
	if (TargetSlot != EEquipmentSlot::None && Slot != TargetSlot)
	{
		const UEnum* SlotEnum = StaticEnum<EEquipmentSlot>();
		const FString SlotName = SlotEnum
			? SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(Slot)).ToString()
			: TEXT("another");
		return FText::FromString(FString::Printf(TEXT("Requires %s slot"), *SlotName));
	}
	if (IsSlotOccupied(Slot))
	{
		return FText::FromString(TEXT("Slot is occupied; unequip the current item first"));
	}
	if (const UWeaponItem* Weapon = Cast<UWeaponItem>(Item); Weapon && Weapon->IsBoundToOtherOwner(GetOwner()))
	{
		return FText::FromString(TEXT("Soulbound to another owner"));
	}

	const int32 RequiredLevel = FMath::Max(1, Item->GetRequiredLevel());
	if (RequiredLevel > 1)
	{
		const UClassComponent* Classes = GetOwner() ? GetOwner()->FindComponentByClass<UClassComponent>() : nullptr;
		if (!Classes || Classes->GetPlayerLevel() < RequiredLevel)
		{
			return FText::FromString(FString::Printf(TEXT("Requires player level %d (current %d)"),
				RequiredLevel, Classes ? Classes->GetPlayerLevel() : 1));
		}
	}

	if (const UAttributeComponent* AttributeComponent = GetOwner() ? GetOwner()->FindComponentByClass<UAttributeComponent>() : nullptr)
	{
		for (const TPair<EPrimaryAttribute, float>& Requirement : Item->GetRequiredAttributes())
		{
			const float CurrentValue = AttributeComponent->GetAttribute(Requirement.Key);
			if (CurrentValue < Requirement.Value)
			{
				const UEnum* AttributeEnum = StaticEnum<EPrimaryAttribute>();
				const FString AttributeName = AttributeEnum
					? AttributeEnum->GetDisplayNameTextByValue(static_cast<int64>(Requirement.Key)).ToString()
					: TEXT("Attribute");
				return FText::FromString(FString::Printf(TEXT("Requires %s %.0f (current %.0f)"),
					*AttributeName, Requirement.Value, CurrentValue));
			}
		}
	}
	else if (Item->GetRequiredAttributes().Num() > 0)
	{
		return FText::FromString(TEXT("Attribute information unavailable"));
	}
	return FText::GetEmpty();
}

UEquipmentItem* UEquipmentComponent::UnequipSlot(EEquipmentSlot Slot)
{
	if (TObjectPtr<UEquipmentItem>* Existing = EquippedItems.Find(Slot))
	{
		UEquipmentItem* RemovedItem = Existing->Get();
		EquippedItems.Remove(Slot);
		OnEquipmentChanged.Broadcast(Slot, nullptr);
		OnEquipmentChangedNative.Broadcast(Slot, nullptr);
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

void UEquipmentComponent::ReportCreditedEnemyDefeat()
{
	if (UWeaponItem* Weapon = Cast<UWeaponItem>(GetEquippedItem(EEquipmentSlot::MainHand)))
	{
		const int32 PreviousRank = Weapon->GetGrowthRank();
		const int32 PreviousDefeats = Weapon->GetDefeatsTowardGrowth();
		Weapon->RecordCreditedDefeat();
		if (Weapon->GetGrowthRank() != PreviousRank || Weapon->GetDefeatsTowardGrowth() != PreviousDefeats)
		{
			OnEquipmentChanged.Broadcast(EEquipmentSlot::MainHand, Weapon);
			OnEquipmentChangedNative.Broadcast(EEquipmentSlot::MainHand, Weapon);
		}
	}
}
