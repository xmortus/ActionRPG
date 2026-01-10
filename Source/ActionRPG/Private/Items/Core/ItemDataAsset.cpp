// Copyright Epic Games, Inc. All Rights Reserved.

#include "Items/Core/ItemDataAsset.h"

UItemDataAsset::UItemDataAsset()
{
	// Set default values
	ItemType = FPrimaryAssetType("Item");
	ItemID = NAME_None;
	ItemName = FText::GetEmpty();
	ItemDescription = FText::GetEmpty();
	ItemIcon = nullptr;
	ItemPickupActorClass = nullptr;
	Type = EItemType::Misc;
	Rarity = EItemRarity::Common;
	MaxStackSize = 1;
	Weight = 0.0f;
	Value = 0;
}

FPrimaryAssetId UItemDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ItemType, ItemID);
}

