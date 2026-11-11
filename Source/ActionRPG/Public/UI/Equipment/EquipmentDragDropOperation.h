// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Items/Core/ItemTypes.h"
#include "EquipmentDragDropOperation.generated.h"

class UEquipmentItem;

/**
 * Drag and drop operation for equipped items.
 * Used when dragging from equipment slots to inventory.
 */
UCLASS()
class ACTIONRPG_API UEquipmentDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UEquipmentDragDropOperation();

	// Equipment slot the drag started from
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	EEquipmentSlot EquipmentSlot;

	// Equipped item being dragged (may be null for empty slot)
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	TObjectPtr<UEquipmentItem> Item;

	// Flag to track if this drag operation was handled by a drop
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	bool bWasHandled;
};
