// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Progression/AttributeComponent.h"
#include "Progression/Core/PrimaryAttributeDataAsset.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Items/Equipment/EquipmentItem.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	EquipmentComponent = GetOwner() ? GetOwner()->FindComponentByClass<UEquipmentComponent>() : nullptr;
	if (EquipmentComponent)
	{
		EquipmentComponent->OnEquipmentChanged.AddDynamic(this, &UAttributeComponent::OnEquipmentChanged);
	}
	InitializeFromDataAsset();
}

void UAttributeComponent::InitializeFromDataAsset()
{
	BaseAttributes.Empty();

	if (AttributeDataAsset)
	{
		for (const TPair<EPrimaryAttribute, float>& Pair : AttributeDataAsset->BaseValues)
		{
			BaseAttributes.Add(Pair.Key, ClampAttributeValue(Pair.Value));
		}
	}

	RecalculateAttributes();
}

float UAttributeComponent::GetAttribute(EPrimaryAttribute Attribute) const
{
	if (const float* Value = Attributes.Find(Attribute))
	{
		return *Value;
	}

	return 0.0f;
}

void UAttributeComponent::SetAttribute(EPrimaryAttribute Attribute, float NewValue)
{
	const float ClampedValue = ClampAttributeValue(NewValue);
	BaseAttributes.Add(Attribute, ClampedValue);
	RecalculateAttributes();
}

void UAttributeComponent::ModifyAttribute(EPrimaryAttribute Attribute, float Delta)
{
	BaseAttributes.Add(Attribute, ClampAttributeValue(BaseAttributes.FindRef(Attribute) + Delta));
	RecalculateAttributes();
}

void UAttributeComponent::SetClassBonuses(const TMap<EPrimaryAttribute, float>& Bonuses)
{
	ClassBonuses = Bonuses;
	RecalculateAttributes();
}

void UAttributeComponent::AddAttributePoints(int32 Points)
{
	if (Points <= 0)
	{
		return;
	}

	UnspentAttributePoints += Points;
}

bool UAttributeComponent::AllocateAttributePoints(EPrimaryAttribute Attribute, int32 Points)
{
	if (Points <= 0 || UnspentAttributePoints < Points)
	{
		return false;
	}

	UnspentAttributePoints -= Points;
	AllocatedAttributes.FindOrAdd(Attribute) += Points;
	RecalculateAttributes();
	return true;
}

float UAttributeComponent::ClampAttributeValue(float Value) const
{
	const float MinValue = AttributeDataAsset ? AttributeDataAsset->MinValue : 0.0f;
	const float MaxValue = AttributeDataAsset ? AttributeDataAsset->MaxValue : 999.0f;
	return FMath::Clamp(Value, MinValue, MaxValue);
}

void UAttributeComponent::OnEquipmentChanged(EEquipmentSlot Slot, UEquipmentItem* Item)
{
	RecalculateAttributes();
}

void UAttributeComponent::ApplyEquipmentBonuses()
{
	if (!EquipmentComponent)
	{
		return;
	}

	const TMap<EEquipmentSlot, UEquipmentItem*> EquippedItems = EquipmentComponent->GetEquippedItems();
	for (const TPair<EEquipmentSlot, UEquipmentItem*>& Pair : EquippedItems)
	{
		const UEquipmentItem* EquippedItem = Pair.Value;
		if (!EquippedItem)
		{
			continue;
		}

		for (const TPair<EPrimaryAttribute, float>& Bonus : EquippedItem->GetPrimaryAttributeBonuses())
		{
			float& AttributeValue = Attributes.FindOrAdd(Bonus.Key);
			AttributeValue = ClampAttributeValue(AttributeValue + Bonus.Value);
		}
	}
}

void UAttributeComponent::RecalculateAttributes()
{
	TMap<EPrimaryAttribute, float> PreviousAttributes = Attributes;
	Attributes = BaseAttributes;

	ApplyEquipmentBonuses();

	for (const TPair<EPrimaryAttribute, float>& Bonus : ClassBonuses)
	{
		float& AttributeValue = Attributes.FindOrAdd(Bonus.Key);
		AttributeValue = ClampAttributeValue(AttributeValue + Bonus.Value);
	}

	for (const TPair<EPrimaryAttribute, float>& Bonus : AllocatedAttributes)
	{
		float& AttributeValue = Attributes.FindOrAdd(Bonus.Key);
		AttributeValue = ClampAttributeValue(AttributeValue + Bonus.Value);
	}

	for (const TPair<EPrimaryAttribute, float>& Pair : Attributes)
	{
		const float OldValue = PreviousAttributes.Contains(Pair.Key) ? PreviousAttributes[Pair.Key] : Pair.Value;
		if (!FMath::IsNearlyEqual(OldValue, Pair.Value))
		{
			OnPrimaryAttributeChanged.Broadcast(Pair.Key, Pair.Value, OldValue);
		}
	}
}
