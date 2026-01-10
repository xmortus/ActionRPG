// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Inventory/InventoryComponent.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Core/ItemTypes.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Engine/World.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	// Initialize inventory slots
	InventorySlots.SetNum(MaxCapacity);
}

void UInventoryComponent::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::BeginPlay - CALLED! This will initialize/reset slots."));
	
	// Check if we have any items before clearing
	int32 ItemsBeforeBeginPlay = 0;
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (InventorySlots[i].Item && InventorySlots[i].Quantity > 0)
		{
			ItemsBeforeBeginPlay++;
			UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::BeginPlay - WARNING: Slot %d has item before BeginPlay! Item=%s, Quantity=%d"),
				i,
				InventorySlots[i].Item->ItemData ? *InventorySlots[i].Item->ItemData->ItemName.ToString() : TEXT("NULL ItemData"),
				InventorySlots[i].Quantity);
		}
	}
	
	if (ItemsBeforeBeginPlay > 0)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent::BeginPlay - ERROR: %d items will be lost because BeginPlay is clearing slots!"), ItemsBeforeBeginPlay);
	}

	Super::BeginPlay();

	// Ensure all slots are initialized as empty
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (!InventorySlots[i].Item)
		{
			InventorySlots[i].bIsEmpty = true;
			InventorySlots[i].Quantity = 0;
		}
		else
		{
			// If item exists, update bIsEmpty based on quantity
			InventorySlots[i].bIsEmpty = (InventorySlots[i].Quantity <= 0);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent: Initialized with %d slots, Max Weight: %.2f"), MaxCapacity, MaxWeight);
	UE_LOG(LogTemp, Log, TEXT("  This InventoryComponent is UNIQUE to this player/actor."));
	UE_LOG(LogTemp, Log, TEXT("  Items stored here are separate from other players' inventories."));
	UE_LOG(LogTemp, Log, TEXT("  Debug reporting will trigger on inventory changes."));
}

void UInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool UInventoryComponent::AddItem(UItemBase* Item, int32 Quantity)
{
	UE_LOG(LogTemp, Log, TEXT("InventoryComponent::AddItem - Called with Item: %s, Quantity: %d"),
		Item && Item->ItemData ? *Item->ItemData->ItemName.ToString() : TEXT("NULL"), Quantity);

	if (!Item || !Item->ItemData || Quantity <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::AddItem - Invalid item or quantity"));
		return false;
	}

	// Check if inventory has space (weight and capacity) - use the Quantity parameter
	if (!HasSpaceFor(Item, Quantity))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::AddItem - No space for item: %s (Quantity: %d)"), 
			*Item->ItemData->ItemName.ToString(), Quantity);
		return false;
	}

	int32 RemainingQuantity = Quantity;
	FName ItemID = Item->ItemData->ItemID;

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent::AddItem - Attempting to stack items (RemainingQuantity: %d)"), RemainingQuantity);

	// Try to stack with existing items first
	bool bStacked = TryStackItem(Item, RemainingQuantity, RemainingQuantity);
	
	UE_LOG(LogTemp, Log, TEXT("InventoryComponent::AddItem - TryStackItem returned: %s, RemainingQuantity: %d"),
		bStacked ? TEXT("TRUE") : TEXT("FALSE"), RemainingQuantity);

	// Add remaining quantity to new slots
	while (RemainingQuantity > 0)
	{
		int32 EmptySlot = FindEmptySlot();
		if (EmptySlot == INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::AddItem - No empty slots available (RemainingQuantity: %d)"), RemainingQuantity);
			return false; // Couldn't add all items
		}

		// Calculate how many we can add to this slot (respecting MaxStackSize)
		int32 StackSize = FMath::Min(RemainingQuantity, Item->ItemData->MaxStackSize);

		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::AddItem - Creating new item instance for slot %d (StackSize: %d)"), EmptySlot, StackSize);

		// Create new item instance for this slot
		// Use component as outer - component owns inventory items
		// This ensures items are not garbage collected while component exists
		UItemBase* NewItem = NewObject<UItemBase>(this, UItemBase::StaticClass());
		if (!NewItem)
		{
			UE_LOG(LogTemp, Error, TEXT("InventoryComponent::AddItem - Failed to create new item instance"));
			return false;
		}

		NewItem->ItemData = Item->ItemData;
		NewItem->Quantity = StackSize;

		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::AddItem - Item instance created: %s (Quantity: %d, ItemData: %s, Outer: Component)"),
			NewItem ? TEXT("Valid") : TEXT("nullptr"),
			NewItem ? NewItem->Quantity : 0,
			NewItem && NewItem->ItemData ? *NewItem->ItemData->ItemName.ToString() : TEXT("nullptr"));

		// Add to slot - CRITICAL: Set Item pointer FIRST, then Quantity, then bIsEmpty
		// This ensures the TObjectPtr maintains a reference to prevent GC
		InventorySlots[EmptySlot].Item = NewItem;
		InventorySlots[EmptySlot].Quantity = StackSize;
		InventorySlots[EmptySlot].bIsEmpty = false;

		// Verify the slot was set correctly
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::AddItem - Slot %d updated: Item=%s (Ptr: %p), Quantity=%d, bIsEmpty=%s"),
			EmptySlot,
			InventorySlots[EmptySlot].Item ? TEXT("Valid") : TEXT("NULL"),
			InventorySlots[EmptySlot].Item.Get(),
			InventorySlots[EmptySlot].Quantity,
			InventorySlots[EmptySlot].bIsEmpty ? TEXT("TRUE") : TEXT("FALSE"));
		
		// Double-check immediately after setting
		if (!InventorySlots[EmptySlot].Item)
		{
			UE_LOG(LogTemp, Error, TEXT("InventoryComponent::AddItem - CRITICAL: Item pointer is NULL immediately after setting!"));
		}

		RemainingQuantity -= StackSize;

		// Broadcast events
		BroadcastInventoryChanged(EmptySlot, NewItem);
		OnItemAdded.Broadcast(NewItem);

		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::AddItem - Successfully added %d of %s to slot %d (RemainingQuantity: %d)"), 
			StackSize, *Item->ItemData->ItemName.ToString(), EmptySlot, RemainingQuantity);
	}

	// Verify the item was actually added
	if (RemainingQuantity == 0)
	{
		// Double-check that at least one item was added
		bool bFoundItem = false;
		for (int32 i = 0; i < InventorySlots.Num(); i++)
		{
			if (!InventorySlots[i].bIsEmpty && InventorySlots[i].Item && InventorySlots[i].Item->ItemData)
			{
				if (InventorySlots[i].Item->ItemData->ItemID == ItemID)
				{
					bFoundItem = true;
					UE_LOG(LogTemp, Log, TEXT("InventoryComponent::AddItem - Verification: Found item in slot %d (Quantity: %d)"),
						i, InventorySlots[i].Quantity);
					break;
				}
			}
		}

		if (!bFoundItem)
		{
			UE_LOG(LogTemp, Error, TEXT("InventoryComponent::AddItem - ERROR: Item was not found in inventory after adding!"));
		}
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
	UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Attempting to use item from slot %d"), SlotIndex);

	// Validate slot index
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - Invalid slot index: %d (Max: %d)"), 
			SlotIndex, InventorySlots.Num() - 1);
		return false;
	}

	FInventorySlot& Slot = InventorySlots[SlotIndex];
	
	// Check if slot is empty
	if (Slot.bIsEmpty || !Slot.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - Slot %d is empty"), SlotIndex);
		return false;
	}

	// Validate item data exists
	if (!Slot.Item->ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent::UseItem - Slot %d has item with NULL ItemData"), SlotIndex);
		return false;
	}

	// Check quantity > 0
	if (Slot.Quantity <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - Slot %d has quantity 0 (Item: %s)"), 
			SlotIndex, *Slot.Item->ItemData->ItemName.ToString());
		return false;
	}

	// Get item ID for type-specific validation
	FName ItemID = Slot.Item->ItemData->ItemID;

	// Special validation for health potions - check if player health is at max
	if (ItemID == FName("HealthPotion") || ItemID == FName("healthpotion"))
	{
		// Get the owner (should be the player character)
		if (AActor* Owner = GetOwner())
		{
			if (AActionRPGPlayerCharacter* PlayerCharacter = Cast<AActionRPGPlayerCharacter>(Owner))
			{
				if (PlayerCharacter->IsHealthAtMax())
				{
					UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - Cannot use health potion: Health already at max (%.1f/%.1f)"), 
						PlayerCharacter->CurrentHealth, PlayerCharacter->MaxHealth);
					return false;
				}
			}
		}
	}

	// Check if item can be used (item-specific validation)
	if (!Slot.Item->CanUse())
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - Item cannot be used: %s (Slot: %d)"), 
			*Slot.Item->ItemData->ItemName.ToString(), SlotIndex);
		return false;
	}

	// Get item type for type-specific handling
	EItemType ItemType = Slot.Item->ItemData->Type;
	FText ItemName = Slot.Item->ItemData->ItemName;

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Using item: %s (Type: %d, Quantity: %d)"), 
		*ItemName.ToString(), (int32)ItemType, Slot.Quantity);

	// Handle different item types
	bool bShouldConsume = false;
	
	switch (ItemType)
	{
	case EItemType::Consumable:
		// Consumables are used and consumed
		bShouldConsume = true;
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Consumable item, will consume 1 quantity"));
		break;

	case EItemType::Equipment:
		// Equipment items are not consumed, they should be equipped (Phase 3)
		// For now, just use them without consuming
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Equipment item, not consumed (equip system in Phase 3)"));
		break;

	case EItemType::SkillItem:
	case EItemType::SkillStone:
	case EItemType::BeastCore:
		// Skill-related items are not consumed, they grant skills (Phase 3)
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Skill item, not consumed (skill system in Phase 3)"));
		break;

	case EItemType::Misc:
	default:
		// Misc items can be used but not consumed
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Misc item, not consumed"));
		break;
	}

	// Use the item (calls ItemBase::Use() which broadcasts OnItemUsed)
	Slot.Item->Use();

	// Broadcast inventory event
	OnItemUsed.Broadcast(Slot.Item);

	// Consume item if needed (for consumables)
	if (bShouldConsume)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Consuming 1 quantity of %s"), *ItemName.ToString());
		
		// Remove 1 quantity (RemoveItem will handle quantity = 0 and slot cleanup)
		bool bRemoved = RemoveItem(SlotIndex, 1);
		
		if (!bRemoved)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - Failed to remove item after use"));
			// Item was used but not removed - this is an error state
			// The item effect was applied, but inventory state may be inconsistent
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Item consumed successfully"));
		}
	}

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Successfully used item: %s"), *ItemName.ToString());
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

bool UInventoryComponent::HasSpaceFor(const UItemBase* Item, int32 Quantity) const
{
	if (!Item || !Item->ItemData || Quantity <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::HasSpaceFor - Invalid item or quantity"));
		return false;
	}

	// Check weight limit - use the Quantity parameter, not Item->Quantity
	float ItemWeight = Item->ItemData->Weight * Quantity;
	float CurrentWeight = GetCurrentWeight();
	if (CurrentWeight + ItemWeight > MaxWeight)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::HasSpaceFor - Weight limit would be exceeded (Current: %.2f + Item: %.2f = %.2f > Max: %.2f)"),
			CurrentWeight, ItemWeight, CurrentWeight + ItemWeight, MaxWeight);
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
	int32 RemainingQuantity = Quantity;
	
	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (!Slot.bIsEmpty && Slot.Item && Slot.Item->ItemData)
		{
			if (Slot.Item->ItemData->ItemID == ItemID && Slot.Quantity < Slot.Item->ItemData->MaxStackSize)
			{
				int32 AvailableSpace = Slot.Item->ItemData->MaxStackSize - Slot.Quantity;
				RemainingQuantity -= FMath::Min(RemainingQuantity, AvailableSpace);
				
				if (RemainingQuantity <= 0)
				{
					return true; // Can stack all items with existing
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent::HasSpaceFor - No space available (Need %d slots for %d items, Empty slots: %d)"),
		Quantity, Quantity, EmptySlots);
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

int32 UInventoryComponent::GetUsedSlotCount() const
{
	return MaxCapacity - GetEmptySlotCount();
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
					// Also update the Item's Quantity to match Slot.Quantity for consistency
					if (Slot.Item)
					{
						Slot.Item->Quantity = Slot.Quantity;
					}
					RemainingQuantity -= StackAmount;

					UE_LOG(LogTemp, Log, TEXT("InventoryComponent::TryStackItem - Stacked %d of %s in slot %d (New Slot.Quantity: %d)"), 
						StackAmount, *Item->ItemData->ItemName.ToString(), i, Slot.Quantity);
					UE_LOG(LogTemp, Log, TEXT("InventoryComponent::TryStackItem - Slot state: Item=%s, Quantity=%d, bIsEmpty=%s"),
						Slot.Item ? TEXT("Valid") : TEXT("NULL"),
						Slot.Quantity,
						Slot.bIsEmpty ? TEXT("TRUE") : TEXT("FALSE"));

					// Broadcast event
					BroadcastInventoryChanged(i, Slot.Item);
					OnItemAdded.Broadcast(Slot.Item);

					UE_LOG(LogTemp, Log, TEXT("InventoryComponent::TryStackItem - After BroadcastInventoryChanged: bIsEmpty=%s"),
						Slot.bIsEmpty ? TEXT("TRUE") : TEXT("FALSE"));
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
			UE_LOG(LogTemp, Verbose, TEXT("InventoryComponent::FindEmptySlot - Found empty slot: %d"), i);
			return i;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::FindEmptySlot - No empty slots found (Total slots: %d)"), InventorySlots.Num());
	return INDEX_NONE;
}

void UInventoryComponent::UpdateSlotEmptyStatus(int32 SlotIndex)
{
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	FInventorySlot& Slot = InventorySlots[SlotIndex];
	bool bWasEmpty = Slot.bIsEmpty;
	Slot.bIsEmpty = (Slot.Item == nullptr || Slot.Quantity <= 0);
	
	if (bWasEmpty != Slot.bIsEmpty)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UpdateSlotEmptyStatus - Slot %d: bIsEmpty changed from %s to %s (Item=%s, Quantity=%d)"),
			SlotIndex,
			bWasEmpty ? TEXT("TRUE") : TEXT("FALSE"),
			Slot.bIsEmpty ? TEXT("TRUE") : TEXT("FALSE"),
			Slot.Item ? TEXT("Valid") : TEXT("NULL"),
			Slot.Quantity);
	}
}

void UInventoryComponent::BroadcastInventoryChanged(int32 SlotIndex, UItemBase* Item)
{
	OnInventoryChanged.Broadcast(SlotIndex, Item);
	UpdateSlotEmptyStatus(SlotIndex);
	
	// Report inventory contents on change (for debugging)
	ReportInventoryContents();
}


void UInventoryComponent::ReportInventoryContents() const
{
	UE_LOG(LogTemp, Warning, TEXT("========================================"));
	UE_LOG(LogTemp, Warning, TEXT("=== INVENTORY DEBUG REPORT ==="));
	UE_LOG(LogTemp, Warning, TEXT("========================================"));
	
	float CurrentWeight = GetCurrentWeight();
	int32 TotalItems = GetTotalItemCount();
	int32 EmptySlots = GetEmptySlotCount();
	int32 UsedSlots = MaxCapacity - EmptySlots;

	UE_LOG(LogTemp, Warning, TEXT("Capacity: %d/%d slots used (%d empty)"), UsedSlots, MaxCapacity, EmptySlots);
	UE_LOG(LogTemp, Warning, TEXT("Weight: %.2f/%.2f"), CurrentWeight, MaxWeight);
	UE_LOG(LogTemp, Warning, TEXT("Total Item Count: %d"), TotalItems);
	UE_LOG(LogTemp, Warning, TEXT("---"));

	int32 ItemCount = 0;
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		const FInventorySlot& Slot = InventorySlots[i];
		
		// Log ALL slots for debugging
		UE_LOG(LogTemp, Warning, TEXT("Slot %d: Item=%s, Quantity=%d, bIsEmpty=%s, ItemData=%s"),
			i,
			Slot.Item ? TEXT("Valid") : TEXT("NULL"),
			Slot.Quantity,
			Slot.bIsEmpty ? TEXT("TRUE") : TEXT("FALSE"),
			Slot.Item && Slot.Item->ItemData ? TEXT("Valid") : TEXT("NULL"));
		
		if (!Slot.bIsEmpty && Slot.Item && Slot.Item->ItemData)
		{
			ItemCount++;
			FString ItemName = Slot.Item->ItemData->ItemName.ToString();
			FString ItemID = Slot.Item->ItemData->ItemID.ToString();
			float SlotWeight = Slot.Item->ItemData->Weight * Slot.Quantity;
			EItemType ItemType = Slot.Item->ItemData->Type;
			EItemRarity ItemRarity = Slot.Item->ItemData->Rarity;

			UE_LOG(LogTemp, Warning, TEXT("  -> %s (ID: %s)"), *ItemName, *ItemID);
			UE_LOG(LogTemp, Warning, TEXT("     Quantity: %d | Weight: %.2f | Type: %d | Rarity: %d"),
				Slot.Quantity, SlotWeight, (int32)ItemType, (int32)ItemRarity);
		}
		else if (Slot.Item != nullptr)
		{
			// Log slots that have an item pointer but are marked as empty or have invalid data
			UE_LOG(LogTemp, Warning, TEXT("  -> INCONSISTENT STATE - Item exists but slot marked empty or invalid"));
		}
	}

	if (ItemCount == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is empty"));
	}

	UE_LOG(LogTemp, Warning, TEXT("========================================"));
}
