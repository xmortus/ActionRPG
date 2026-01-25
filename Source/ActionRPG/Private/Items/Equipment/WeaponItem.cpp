// Copyright Epic Games, Inc. All Rights Reserved.

#include "Items/Equipment/WeaponItem.h"
#include "Items/Core/ItemDataAsset.h"

float UWeaponItem::GetWeaponDamage() const
{
	return ItemData ? ItemData->WeaponDamage : 0.0f;
}

EDamageType UWeaponItem::GetWeaponDamageType() const
{
	return ItemData ? ItemData->WeaponDamageType : EDamageType::Physical;
}
