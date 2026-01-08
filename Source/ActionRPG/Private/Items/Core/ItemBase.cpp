// Copyright Epic Games, Inc. All Rights Reserved.

#include "Items/Core/ItemBase.h"

UItemBase::UItemBase()
{
	ItemData = nullptr;
	Quantity = 1;
}

void UItemBase::Use()
{
	if (!CanUse())
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemBase::Use - Item cannot be used"));
		return;
	}

	// Broadcast event
	OnItemUsed.Broadcast(this);

	UE_LOG(LogTemp, Log, TEXT("ItemBase::Use - Item used: %s"), ItemData ? *ItemData->ItemName.ToString() : TEXT("NULL"));
}

bool UItemBase::CanUse() const
{
	if (!ItemData)
	{
		return false;
	}

	// Base validation - can be overridden in derived classes
	return Quantity > 0;
}

FName UItemBase::GetItemID() const
{
	return ItemData ? ItemData->ItemID : NAME_None;
}

FText UItemBase::GetItemName() const
{
	return ItemData ? ItemData->ItemName : FText::GetEmpty();
}

EItemType UItemBase::GetItemType() const
{
	return ItemData ? ItemData->Type : EItemType::Misc;
}

