// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Items/Core/ItemBase.h"
#include "ItemDragDropOperation.generated.h"

/**
 * Drag and drop operation for inventory items.
 * Handles dragging items between inventory slots.
 */
UCLASS()
class ACTIONRPG_API UItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UItemDragDropOperation();

	// Source slot index where the drag started
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	int32 SourceSlotIndex;

	// Item being dragged
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	TObjectPtr<UItemBase> Item;

	// Quantity being dragged (for partial stack splits)
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	int32 Quantity;

	// Full quantity in source slot (for reference)
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	int32 SourceQuantity;

	// Is this a partial stack split (Ctrl+drag)?
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	bool bIsSplitOperation;
};
