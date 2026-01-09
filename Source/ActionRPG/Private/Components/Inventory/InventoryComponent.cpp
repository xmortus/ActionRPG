// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Inventory/InventoryComponent.h"
#include "Items/Core/ItemDataAsset.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	// Initialize inventory slots
	InventorySlots.SetNum(MaxCapacity);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Ensure all slots are initialized as empty
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (!InventorySlots[i].Item)
		{
			InventorySlots[i].bIsEmpty = true;
			InventorySlots[i].Quantity = 0;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent: Initialized with %d slots, Max Weight: %.2f"), MaxCapacity, MaxWeight);
}

bool UInventoryComponent::AddItem(UItemBase* Item, int32 Quantity)
{
	if (!Item || !Item->ItemData || Quantity <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::AddItem - Invalid item or quantity"));
		return false;
	}

	// Check if inventory has space (weight and capacity)
	if (!HasSpaceFor(Item))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::AddItem - No space for item: %s"), *Item->ItemData->ItemName.ToString());
		return false;
	}

	int32 RemainingQuantity = Quantity;
	FName ItemID = Item->ItemData->ItemID;

	// Try to stack with existing items first
	if (!TryStackItem(Item, RemainingQuantity, RemainingQuantity))
	{
		// If stacking failed or partial, we still need to handle remaining quantity
	}

	// Add remaining quantity to new slots
	while (RemainingQuantity > 0)
	{
		int32 EmptySlot = FindEmptySlot();
		if (EmptySlot == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::AddItem - No empty slots available"));
			return false; // Couldn't add all items
		}

		// Calculate how many we can add to this slot (respecting MaxStackSize)
		int32 StackSize = FMath::Min(RemainingQuantity, Item->ItemData->MaxStackSize);

		// Create new item instance for this slot
		UItemBase* NewItem = NewObject<UItemBase>(this);
		if (!NewItem)
		{
			UE_LOG(LogTemp, Error, TEXT("InventoryComponent::AddItem - Failed to create new item instance"));
			return false;
		}

		NewItem->ItemData = Item->ItemData;
		NewItem->Quantity = StackSize;

		// Add to slot
		InventorySlots[EmptySlot].Item = NewItem;
		InventorySlots[EmptySlot].Quantity = StackSize;
		InventorySlots[EmptySlot].bIsEmpty = false;

		RemainingQuantity -= StackSize;

		// Broadcast events
		BroadcastInventoryChanged(EmptySlot, NewItem);
		OnItemAdded.Broadcast(NewItem);

		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::AddItem - Added %d of %s to slot %d"), 
			StackSize, *Item->ItemData->ItemName.ToString(), EmptySlot);
	}

	return true;
}

bool UInventoryComponent::RemoveItem(int32 SlotIndex, int32 Quantity)
{
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::RemoveItem - Invalid slot index: %d"), SlotIndex);
		return false;
	}

	FInventorySlot& Slot = InventorySlots[SlotIndex];
	if (Slot.bIsEmpty || !Slot.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::RemoveItem - Slot %d is empty"), SlotIndex);
		return false;
	}

	int32 RemoveQuantity = FMath::Min(Quantity, Slot.Quantity);
	UItemBase* Item = Slot.Item;

	Slot.Quantity -= RemoveQuantity;

	if (Slot.Quantity <= 0)
	{
		// Slot is now empty
		Slot.Item = nullptr;
		Slot.Quantity = 0;
		Slot.bIsEmpty = true;
	}

	// Broadcast events
	BroadcastInventoryChanged(SlotIndex, Slot.Item);
	OnItemRemoved.Broadcast(Item, RemoveQuantity);

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent::RemoveItem - Removed %d of %s from slot %d"), 
		RemoveQuantity, Item ? *Item->ItemData->ItemName.ToString() : TEXT("NULL"), SlotIndex);

	return true;
}

bool UInventoryComponent::MoveItem(int32 FromSlot, int32 ToSlot)
{
	if (!InventorySlots.IsValidIndex(FromSlot) || !InventorySlots.IsValidIndex(ToSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::MoveItem - Invalid slot indices: %d -> %d"), FromSlot, ToSlot);
		return false;
	}

	if (FromSlot == ToSlot)
	{
		return true; // Nothing to do
	}

	FInventorySlot& FromSlotRef = InventorySlots[FromSlot];
	FInventorySlot& ToSlotRef = InventorySlots[ToSlot];

	if (FromSlotRef.bIsEmpty || !FromSlotRef.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::MoveItem - Source slot %d is empty"), FromSlot);
		return false;
	}

	// If destination is empty, just move the item
	if (ToSlotRef.bIsEmpty)
	{
		ToSlotRef = FromSlotRef;
		FromSlotRef = FInventorySlot(); // Clear source slot
		
		BroadcastInventoryChanged(FromSlot, nullptr);
		BroadcastInventoryChanged(ToSlot, ToSlotRef.Item);
		
		return true;
	}

	// If destination has same item, try to stack
	if (ToSlotRef.Item && ToSlotRef.Item->ItemData && FromSlotRef.Item && FromSlotRef.Item->ItemData)
	{
		if (ToSlotRef.Item->ItemData->ItemID == FromSlotRef.Item->ItemData->ItemID)
		{
			int32 MaxStack = ToSlotRef.Item->ItemData->MaxStackSize;
			int32 AvailableSpace = MaxStack - ToSlotRef.Quantity;

			if (AvailableSpace > 0)
			{
				int32 StackAmount = FMath::Min(AvailableSpace, FromSlotRef.Quantity);
				ToSlotRef.Quantity += StackAmount;
				FromSlotRef.Quantity -= StackAmount;

				if (FromSlotRef.Quantity <= 0)
				{
					FromSlotRef = FInventorySlot(); // Clear source slot
				}

				BroadcastInventoryChanged(FromSlot, FromSlotRef.Item);
				BroadcastInventoryChanged(ToSlot, ToSlotRef.Item);
				
				return true;
			}
		}
	}

	// Can't stack, so swap items
	return SwapItems(FromSlot, ToSlot);
}

bool UInventoryComponent::SwapItems(int32 SlotA, int32 SlotB)
{
	if (!InventorySlots.IsValidIndex(SlotA) || !InventorySlots.IsValidIndex(SlotB))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::SwapItems - Invalid slot indices: %d <-> %d"), SlotA, SlotB);
		return false;
	}

	if (SlotA == SlotB)
	{
		return true; // Nothing to do
	}

	FInventorySlot Temp = InventorySlots[SlotA];
	InventorySlots[SlotA] = InventorySlots[SlotB];
	InventorySlots[SlotB] = Temp;

	// Broadcast events
	BroadcastInventoryChanged(SlotA, InventorySlots[SlotA].Item);
	BroadcastInventoryChanged(SlotB, InventorySlots[SlotB].Item);

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent::SwapItems - Swapped slots %d <-> %d"), SlotA, SlotB);

	return true;
}

bool UInventoryComponent::UseItem(int32 SlotIndex)
{
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - Invalid slot index: %d"), SlotIndex);
		return false;
	}

	FInventorySlot& Slot = InventorySlots[SlotIndex];
	if (Slot.bIsEmpty || !Slot.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - Slot %d is empty"), SlotIndex);
		return false;
	}

	// Check if item can be used
	if (!Slot.Item->CanUse())
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - Item cannot be used: %s"), 
			Slot.Item->ItemData ? *Slot.Item->ItemData->ItemName.ToString() : TEXT("NULL"));
		return false;
	}

	// Use the item
	Slot.Item->Use();

	// Broadcast event
	OnItemUsed.Broadcast(Slot.Item);

	// Check if item should be consumed (consumables typically reduce quantity)
	// For now, we'll remove 1 quantity for consumables
	// This can be customized per item type later
	if (Slot.Item->ItemData && Slot.Item->ItemData->Type == EItemType::Consumable)
	{
		RemoveItem(SlotIndex, 1);
	}

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Used item: %s"), 
		Slot.Item->ItemData ? *Slot.Item->ItemData->ItemName.ToString() : TEXT("NULL"));

	return true;
}

UItemBase* UInventoryComponent::GetItemAt(int32 SlotIndex) const
{
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		return nullptr;
	}

	return InventorySlots[SlotIndex].Item;
}

int32 UInventoryComponent::FindItemSlot(const FName& ItemID) const
{
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (!InventorySlots[i].bIsEmpty && InventorySlots[i].Item && InventorySlots[i].Item->ItemData)
		{
			if (InventorySlots[i].Item->ItemData->ItemID == ItemID)
			{
				return i;
			}
		}
	}

	return INDEX_NONE;
}

bool UInventoryComponent::HasSpaceFor(UItemBase* Item) const
{
	if (!Item || !Item->ItemData)
	{
		return false;
	}

	// Check weight limit
	float ItemWeight = Item->ItemData->Weight * Item->Quantity;
	if (GetCurrentWeight() + ItemWeight > MaxWeight)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::HasSpaceFor - Weight limit would be exceeded"));
		return false;
	}

	// Check capacity (need at least one empty slot or existing stack with space)
	int32 EmptySlots = GetEmptySlotCount();
	if (EmptySlots > 0)
	{
		return true; // Have empty slots
	}

	// Check if we can stack with existing items
	FName ItemID = Item->ItemData->ItemID;
	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (!Slot.bIsEmpty && Slot.Item && Slot.Item->ItemData)
		{
			if (Slot.Item->ItemData->ItemID == ItemID && Slot.Quantity < Slot.Item->ItemData->MaxStackSize)
			{
				return true; // Can stack with existing
			}
		}
	}

	return false; // No space
}

float UInventoryComponent::GetCurrentWeight() const
{
	float TotalWeight = 0.0f;

	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (!Slot.bIsEmpty && Slot.Item && Slot.Item->ItemData)
		{
			TotalWeight += Slot.Item->ItemData->Weight * Slot.Quantity;
		}
	}

	return TotalWeight;
}

int32 UInventoryComponent::GetTotalItemCount() const
{
	int32 TotalCount = 0;

	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (!Slot.bIsEmpty)
		{
			TotalCount += Slot.Quantity;
		}
	}

	return TotalCount;
}

int32 UInventoryComponent::GetEmptySlotCount() const
{
	int32 EmptyCount = 0;

	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.bIsEmpty)
		{
			EmptyCount++;
		}
	}

	return EmptyCount;
}

bool UInventoryComponent::TryStackItem(UItemBase* Item, int32 Quantity, int32& RemainingQuantity)
{
	if (!Item || !Item->ItemData || Quantity <= 0)
	{
		return false;
	}

	FName ItemID = Item->ItemData->ItemID;
	RemainingQuantity = Quantity;

	// Try to find existing stacks with space
	for (int32 i = 0; i < InventorySlots.Num() && RemainingQuantity > 0; i++)
	{
		FInventorySlot& Slot = InventorySlots[i];
		if (!Slot.bIsEmpty && Slot.Item && Slot.Item->ItemData)
		{
			if (Slot.Item->ItemData->ItemID == ItemID)
			{
				int32 MaxStack = Slot.Item->ItemData->MaxStackSize;
				int32 AvailableSpace = MaxStack - Slot.Quantity;

				if (AvailableSpace > 0)
				{
					int32 StackAmount = FMath::Min(AvailableSpace, RemainingQuantity);
					Slot.Quantity += StackAmount;
					RemainingQuantity -= StackAmount;

					// Broadcast event
					BroadcastInventoryChanged(i, Slot.Item);
					OnItemAdded.Broadcast(Slot.Item);

					UE_LOG(LogTemp, Log, TEXT("InventoryComponent::TryStackItem - Stacked %d of %s in slot %d"), 
						StackAmount, *Item->ItemData->ItemName.ToString(), i);
				}
			}
		}
	}

	return RemainingQuantity < Quantity; // Return true if we stacked at least some items
}

int32 UInventoryComponent::FindEmptySlot() const
{
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (InventorySlots[i].bIsEmpty)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

void UInventoryComponent::UpdateSlotEmptyStatus(int32 SlotIndex)
{
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	FInventorySlot& Slot = InventorySlots[SlotIndex];
	Slot.bIsEmpty = (Slot.Item == nullptr || Slot.Quantity <= 0);
}

void UInventoryComponent::BroadcastInventoryChanged(int32 SlotIndex, UItemBase* Item)
{
	OnInventoryChanged.Broadcast(SlotIndex, Item);
	UpdateSlotEmptyStatus(SlotIndex);
}
