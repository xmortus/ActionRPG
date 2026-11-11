// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Equipment/EquipmentDragDropOperation.h"
#include "Items/Equipment/EquipmentItem.h"

UEquipmentDragDropOperation::UEquipmentDragDropOperation()
{
	EquipmentSlot = EEquipmentSlot::None;
	Item = nullptr;
	bWasHandled = false;
}
