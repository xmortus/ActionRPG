// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Inventory/ItemDragDropOperation.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"

UItemDragDropOperation::UItemDragDropOperation()
{
	SourceSlotIndex = -1;
	Item = nullptr;
	Quantity = 0;
	SourceQuantity = 0;
	bIsSplitOperation = false;
	
	UE_LOG(LogTemp, Verbose, TEXT("ItemDragDropOperation::UItemDragDropOperation - Created drag operation"));
}
