// Copyright Epic Games, Inc. All Rights Reserved.

#include "Items/Equipment/EquipmentItem.h"
#include "Items/Core/ItemDataAsset.h"

EEquipmentSlot UEquipmentItem::GetEquipmentSlot() const
{
	return ItemData ? ItemData->EquipmentSlot : EEquipmentSlot::None;
}

const TMap<ESecondaryAttribute, float>& UEquipmentItem::GetEquipmentBonuses() const
{
	static const TMap<ESecondaryAttribute, float> EmptyBonuses;
	return ItemData ? ItemData->EquipmentBonuses : EmptyBonuses;
}

const TMap<EPrimaryAttribute, float>& UEquipmentItem::GetPrimaryAttributeBonuses() const
{
	static const TMap<EPrimaryAttribute, float> EmptyBonuses;
	return ItemData ? ItemData->EquipmentPrimaryBonuses : EmptyBonuses;
}

int32 UEquipmentItem::GetRequiredLevel() const
{
	return ItemData ? ItemData->EquipmentRequiredLevel : 1;
}

const TMap<EPrimaryAttribute, float>& UEquipmentItem::GetRequiredAttributes() const
{
	static const TMap<EPrimaryAttribute, float> EmptyRequirements;
	return ItemData ? ItemData->EquipmentRequiredAttributes : EmptyRequirements;
}
