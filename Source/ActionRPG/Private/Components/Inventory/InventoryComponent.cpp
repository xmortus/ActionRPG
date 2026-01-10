// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Inventory/InventoryComponent.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Core/ItemTypes.h"
#include "Items/Pickups/ItemPickupActor.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Engine/World.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	// Initialize inventory slots
	InventorySlots.SetNum(MaxCapacity);

	// Initialize quick-use slots (10 slots: 1-8 for skills, 9-10 for consumables)
	QuickUseSlots.SetNum(10);
	for (int32 i = 0; i < 10; i++)
	{
		FQuickUseSlot& Slot = QuickUseSlots[i];
		Slot.Item = nullptr;
		Slot.InventorySlotIndex = -1;
		Slot.SlotType = (i < 8) ? EQuickUseSlotType::Skill : EQuickUseSlotType::Consumable;
	}
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

		// Clear quick-use slots referencing this inventory slot
		for (int32 i = 0; i < QuickUseSlots.Num(); i++)
		{
			if (QuickUseSlots[i].InventorySlotIndex == SlotIndex)
			{
				ClearQuickUseSlot(i);
			}
		}
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
		
		// Remove 1 quantity (RemoveItem will handle quantity = 0 and slot cleanup, including clearing quick-use slots)
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
	// Disabled by user request - uncomment the line below to re-enable
	// ReportInventoryContents();
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

bool UInventoryComponent::SplitStack(int32 SlotIndex, int32 SplitQuantity)
{
	// Validate slot index
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStack - Invalid slot index: %d"), SlotIndex);
		return false;
	}

	FInventorySlot& Slot = InventorySlots[SlotIndex];

	// Check if slot is empty
	if (Slot.bIsEmpty || !Slot.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStack - Slot %d is empty"), SlotIndex);
		return false;
	}

	// Validate split quantity
	if (SplitQuantity <= 0 || SplitQuantity >= Slot.Quantity)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStack - Invalid split quantity: %d (current: %d)"), 
			SplitQuantity, Slot.Quantity);
		return false;
	}

	// Check if item can stack (MaxStackSize > 1)
	if (!Slot.Item->ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStack - Item has no ItemData"));
		return false;
	}

	const UItemDataAsset* ItemData = Slot.Item->ItemData;
	if (ItemData->MaxStackSize <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStack - Item cannot stack (MaxStackSize: %d)"), 
			ItemData->MaxStackSize);
		return false;
	}

	// Find empty slot for split stack
	int32 EmptySlotIndex = FindEmptySlot();
	if (EmptySlotIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStack - No empty slot available"));
		return false;
	}

	// Create new item instance with split quantity
	UItemBase* NewItem = NewObject<UItemBase>(this, UItemBase::StaticClass());
	if (!NewItem)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::SplitStack - Failed to create new item instance"));
		return false;
	}

	// Initialize new item with same data
	NewItem->ItemData = Slot.Item->ItemData;
	NewItem->Quantity = SplitQuantity;

	// Update source slot (reduce quantity)
	Slot.Quantity -= SplitQuantity;
	if (Slot.Quantity <= 0)
	{
		Slot.Item = nullptr;
		Slot.bIsEmpty = true;
		Slot.Quantity = 0;
	}
	else
	{
		// Update source item quantity
		Slot.Item->Quantity = Slot.Quantity;
	}

	// Add split stack to empty slot
	FInventorySlot& NewSlot = InventorySlots[EmptySlotIndex];
	NewSlot.Item = NewItem;
	NewSlot.Quantity = SplitQuantity;
	NewSlot.bIsEmpty = false;

	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::SplitStack - Split %d from slot %d to slot %d"), 
		SplitQuantity, SlotIndex, EmptySlotIndex);

	// Fire events
	BroadcastInventoryChanged(SlotIndex, Slot.Item);
	BroadcastInventoryChanged(EmptySlotIndex, NewSlot.Item);

	return true;
}

bool UInventoryComponent::SplitStackToSlot(int32 SourceSlotIndex, int32 TargetSlotIndex, int32 SplitQuantity)
{
	// Validate slot indices
	if (!InventorySlots.IsValidIndex(SourceSlotIndex) || !InventorySlots.IsValidIndex(TargetSlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStackToSlot - Invalid slot indices (Source: %d, Target: %d)"), 
			SourceSlotIndex, TargetSlotIndex);
		return false;
	}

	// Can't split to same slot
	if (SourceSlotIndex == TargetSlotIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStackToSlot - Cannot split to same slot"));
		return false;
	}

	FInventorySlot& SourceSlot = InventorySlots[SourceSlotIndex];
	FInventorySlot& TargetSlot = InventorySlots[TargetSlotIndex];

	// Check if source slot is empty
	if (SourceSlot.bIsEmpty || !SourceSlot.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStackToSlot - Source slot %d is empty"), SourceSlotIndex);
		return false;
	}

	// Validate split quantity
	if (SplitQuantity <= 0 || SplitQuantity >= SourceSlot.Quantity)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStackToSlot - Invalid split quantity: %d (current: %d)"), 
			SplitQuantity, SourceSlot.Quantity);
		return false;
	}

	// Check if item can stack (MaxStackSize > 1)
	if (!SourceSlot.Item->ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStackToSlot - Item has no ItemData"));
		return false;
	}

	const UItemDataAsset* ItemData = SourceSlot.Item->ItemData;
	if (ItemData->MaxStackSize <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStackToSlot - Item cannot stack (MaxStackSize: %d)"), 
			ItemData->MaxStackSize);
		return false;
	}

	// Handle different target slot states
	if (TargetSlot.bIsEmpty)
	{
		// Target slot is empty - create split stack directly in target slot
		// Create new item instance with split quantity
		UItemBase* NewItem = NewObject<UItemBase>(this, UItemBase::StaticClass());
		if (!NewItem)
		{
			UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::SplitStackToSlot - Failed to create new item instance"));
			return false;
		}

		// Initialize new item with same data
		NewItem->ItemData = SourceSlot.Item->ItemData;
		NewItem->Quantity = SplitQuantity;

		// Update source slot (reduce quantity)
		SourceSlot.Quantity -= SplitQuantity;
		if (SourceSlot.Quantity <= 0)
		{
			SourceSlot.Item = nullptr;
			SourceSlot.bIsEmpty = true;
			SourceSlot.Quantity = 0;
		}
		else
		{
			// Update source item quantity
			SourceSlot.Item->Quantity = SourceSlot.Quantity;
		}

		// Place split stack in target slot
		TargetSlot.Item = NewItem;
		TargetSlot.Quantity = SplitQuantity;
		TargetSlot.bIsEmpty = false;

		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::SplitStackToSlot - Split %d from slot %d to slot %d"), 
			SplitQuantity, SourceSlotIndex, TargetSlotIndex);

		// Fire events
		BroadcastInventoryChanged(SourceSlotIndex, SourceSlot.Item);
		BroadcastInventoryChanged(TargetSlotIndex, NewItem);

		return true;
	}
	else if (TargetSlot.Item && TargetSlot.Item->ItemData && 
	         TargetSlot.Item->ItemData->ItemID == SourceSlot.Item->ItemData->ItemID)
	{
		// Same item: try to stack the split quantity
		int32 MaxStackSize = TargetSlot.Item->ItemData->MaxStackSize;
		int32 RemainingSpace = MaxStackSize - TargetSlot.Quantity;
		
		if (RemainingSpace <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStackToSlot - Target slot is full, cannot stack"));
			return false;
		}

		// Calculate how much can be stacked
		int32 StackAmount = FMath::Min(SplitQuantity, RemainingSpace);
		
		// Add to target slot
		TargetSlot.Quantity += StackAmount;
		TargetSlot.Item->Quantity = TargetSlot.Quantity;

		// Remove from source slot
		SourceSlot.Quantity -= StackAmount;
		if (SourceSlot.Quantity <= 0)
		{
			SourceSlot.Item = nullptr;
			SourceSlot.bIsEmpty = true;
			SourceSlot.Quantity = 0;
		}
		else
		{
			SourceSlot.Item->Quantity = SourceSlot.Quantity;
		}

		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::SplitStackToSlot - Stacked %d from split (remaining space: %d)"), 
			StackAmount, RemainingSpace);

		// Fire events
		BroadcastInventoryChanged(SourceSlotIndex, SourceSlot.Item);
		BroadcastInventoryChanged(TargetSlotIndex, TargetSlot.Item);

		return true;
	}
	else
	{
		// Different item: can't drop split stack on different item
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SplitStackToSlot - Cannot drop split stack on different item"));
		return false;
	}
}

bool UInventoryComponent::DropItemToWorld(int32 SlotIndex, int32 Quantity, const FVector& WorldLocation)
{
	// Validate slot index
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::DropItemToWorld - Invalid slot index: %d"), SlotIndex);
		return false;
	}

	FInventorySlot& Slot = InventorySlots[SlotIndex];

	// Check if slot is empty
	if (Slot.bIsEmpty || !Slot.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::DropItemToWorld - Slot %d is empty"), SlotIndex);
		return false;
	}

	// Validate quantity
	if (Quantity <= 0 || Quantity > Slot.Quantity)
	{
		Quantity = Slot.Quantity; // Drop entire stack if invalid quantity
	}

	// Get world context
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::DropItemToWorld - World is null"));
		return false;
	}

	// Get item data
	if (!Slot.Item->ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::DropItemToWorld - Item has no ItemData"));
		return false;
	}

	const UItemDataAsset* ItemData = Slot.Item->ItemData;

	// Determine which class to spawn: Blueprint class from DataAsset if specified, otherwise base class
	TSubclassOf<AItemPickupActor> PickupActorClass = ItemData->ItemPickupActorClass;
	if (!PickupActorClass)
	{
		// Fallback to base C++ class if no Blueprint class is specified
		PickupActorClass = AItemPickupActor::StaticClass();
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::DropItemToWorld - No Blueprint class specified in ItemDataAsset, using base AItemPickupActor class"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::DropItemToWorld - Using Blueprint class: %s"), *PickupActorClass->GetName());
	}

	// Spawn ItemPickupActor at drop location using the specified class (Blueprint or base class)
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AItemPickupActor* PickupActor = World->SpawnActor<AItemPickupActor>(PickupActorClass, WorldLocation, FRotator::ZeroRotator, SpawnParams);
	if (!PickupActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::DropItemToWorld - Failed to spawn ItemPickupActor (Class: %s)"), 
			PickupActorClass ? *PickupActorClass->GetName() : TEXT("NULL"));
		return false;
	}

	// Set item data and quantity BEFORE removing from inventory (to ensure actor is properly set up)
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::DropItemToWorld - Preparing to set item data: %s (Quantity: %d)"), 
		ItemData ? *ItemData->ItemName.ToString() : TEXT("NULL"), Quantity);
	
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::DropItemToWorld - ItemData is NULL! Cannot drop item."));
		PickupActor->Destroy();
		return false;
	}
	
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::DropItemToWorld - Calling SetItemData with ItemData: %s (Pointer: %p)"), 
		*ItemData->ItemName.ToString(), ItemData);
	PickupActor->SetItemData(const_cast<UItemDataAsset*>(ItemData));
	
	// Verify ItemData was set correctly
	UItemDataAsset* SetItemDataResult = PickupActor->GetItemData();
	if (!SetItemDataResult)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::DropItemToWorld - FAILED: SetItemData returned NULL! ItemData was not set on PickupActor."));
		PickupActor->Destroy();
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::DropItemToWorld - Verified: ItemData was set successfully: %s"), 
		*SetItemDataResult->ItemName.ToString());
	
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::DropItemToWorld - Calling SetQuantity with Quantity: %d"), Quantity);
	PickupActor->SetQuantity(Quantity);
	
	// Verify Quantity was set correctly
	int32 SetQuantityResult = PickupActor->GetQuantity();
	if (SetQuantityResult != Quantity)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::DropItemToWorld - Quantity mismatch: Expected %d, Got %d"), Quantity, SetQuantityResult);
	}
	
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::DropItemToWorld - SetItemData and SetQuantity calls completed successfully"));

	// Remove item from inventory AFTER ensuring actor is set up correctly
	UItemBase* RemovedItem = Slot.Item;
	if (Quantity >= Slot.Quantity)
	{
		// Remove entire stack
		Slot.Item = nullptr;
		Slot.Quantity = 0;
		Slot.bIsEmpty = true;
	}
	else
	{
		// Reduce quantity
		Slot.Quantity -= Quantity;
		Slot.Item->Quantity = Slot.Quantity;
	}

	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::DropItemToWorld - Dropped %d of %s at location (%.2f, %.2f, %.2f)"), 
		Quantity, *ItemData->ItemName.ToString(), WorldLocation.X, WorldLocation.Y, WorldLocation.Z);

	// Clear quick-use slots referencing this inventory slot
	for (int32 i = 0; i < QuickUseSlots.Num(); i++)
	{
		if (QuickUseSlots[i].InventorySlotIndex == SlotIndex)
		{
			ClearQuickUseSlot(i);
		}
	}

	// Fire events
	BroadcastInventoryChanged(SlotIndex, Slot.Item);
	OnItemRemoved.Broadcast(RemovedItem, Quantity);

	return true;
}

bool UInventoryComponent::AssignItemToQuickUseSlot(int32 InventorySlotIndex, int32 QuickUseSlotIndex)
{
	// Validate indices
	if (!InventorySlots.IsValidIndex(InventorySlotIndex) || !QuickUseSlots.IsValidIndex(QuickUseSlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AssignItemToQuickUseSlot - Invalid indices (Inventory: %d, QuickUse: %d)"), 
			InventorySlotIndex, QuickUseSlotIndex);
		return false;
	}

	FInventorySlot& InvSlot = InventorySlots[InventorySlotIndex];
	
	// Check if inventory slot is empty
	if (InvSlot.bIsEmpty || !InvSlot.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AssignItemToQuickUseSlot - Inventory slot %d is empty"), InventorySlotIndex);
		return false;
	}

	FQuickUseSlot& QuickSlot = QuickUseSlots[QuickUseSlotIndex];

	// For Phase 2, only slots 9-10 (indices 8-9) accept consumables
	if (QuickSlot.SlotType == EQuickUseSlotType::Skill)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AssignItemToQuickUseSlot - Skill slots not available in Phase 2 (slot %d)"), QuickUseSlotIndex);
		return false;
	}

	// Check if item is consumable (for Phase 2, slots 9-10 only accept consumables)
	if (!InvSlot.Item->ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AssignItemToQuickUseSlot - Item has no ItemData"));
		return false;
	}

	const UItemDataAsset* ItemData = InvSlot.Item->ItemData;
	if (ItemData->Type != EItemType::Consumable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AssignItemToQuickUseSlot - Only consumable items can be assigned to slots 9-10 (Item Type: %d)"), 
			(int32)ItemData->Type);
		return false;
	}

	// Before assigning to the new slot, check if this inventory slot is already assigned to any other quick-use slot
	// If so, remove it from the old slot first (prevents item from being in multiple quick-use slots)
	for (int32 i = 0; i < QuickUseSlots.Num(); i++)
	{
		if (i != QuickUseSlotIndex && QuickUseSlots[i].InventorySlotIndex == InventorySlotIndex)
		{
			// This inventory slot is already assigned to a different quick-use slot
			// Clear it from the old slot before assigning to the new one
			UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AssignItemToQuickUseSlot - Item from inventory slot %d is already assigned to quick-use slot %d, clearing old assignment"), 
				InventorySlotIndex, i);
			ClearQuickUseSlot(i);
		}
	}

	// Clear existing assignment in the target slot if any (in case target slot has a different item)
	if (QuickSlot.InventorySlotIndex != -1 && QuickSlot.InventorySlotIndex != InventorySlotIndex)
	{
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AssignItemToQuickUseSlot - Clearing existing item from quick-use slot %d (was inventory slot %d)"), 
			QuickUseSlotIndex, QuickSlot.InventorySlotIndex);
		ClearQuickUseSlot(QuickUseSlotIndex);
	}

	// Assign item to the new slot
	QuickSlot.Item = InvSlot.Item;
	QuickSlot.InventorySlotIndex = InventorySlotIndex;

	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AssignItemToQuickUseSlot - Assigned %s from slot %d to quick-use slot %d"), 
		*ItemData->ItemName.ToString(), InventorySlotIndex, QuickUseSlotIndex);

	// Fire event
	OnQuickUseSlotChanged.Broadcast(QuickUseSlotIndex, QuickSlot.Item);

	return true;
}

bool UInventoryComponent::UseQuickUseSlot(int32 QuickUseSlotIndex)
{
	// Validate index
	if (!QuickUseSlots.IsValidIndex(QuickUseSlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::UseQuickUseSlot - Invalid slot index: %d"), QuickUseSlotIndex);
		return false;
	}

	FQuickUseSlot& QuickSlot = QuickUseSlots[QuickUseSlotIndex];

	// Check if slot is empty
	if (!QuickSlot.Item || QuickSlot.InventorySlotIndex == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::UseQuickUseSlot - Quick-use slot %d is empty"), QuickUseSlotIndex);
		return false;
	}

	// Validate inventory slot index
	if (!InventorySlots.IsValidIndex(QuickSlot.InventorySlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::UseQuickUseSlot - Invalid inventory slot index: %d"), QuickSlot.InventorySlotIndex);
		ClearQuickUseSlot(QuickUseSlotIndex);
		return false;
	}

	// Check if inventory slot still has the item
	const FInventorySlot& InvSlot = InventorySlots[QuickSlot.InventorySlotIndex];
	if (InvSlot.bIsEmpty || !InvSlot.Item || InvSlot.Item != QuickSlot.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::UseQuickUseSlot - Item no longer in inventory slot %d"), QuickSlot.InventorySlotIndex);
		ClearQuickUseSlot(QuickUseSlotIndex);
		return false;
	}

	// Save inventory slot index before using item (since UseItem might clear the quick-use slot)
	int32 SavedInventorySlotIndex = QuickSlot.InventorySlotIndex;

	// Use item from inventory slot
	bool bSuccess = UseItem(SavedInventorySlotIndex);

	// After UseItem returns, the quick-use slot might have been cleared by RemoveItem
	// if the item was consumed (quantity reached 0). Check if the quick-use slot is still valid.
	if (!QuickUseSlots.IsValidIndex(QuickUseSlotIndex))
	{
		// Quick-use slot array was resized or invalidated (shouldn't happen, but safety check)
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::UseQuickUseSlot - Quick-use slot index became invalid after UseItem"));
		return bSuccess;
	}

	// Get quick-use slot again (it might have been cleared, so check if it's still valid)
	FQuickUseSlot& UpdatedQuickSlot = QuickUseSlots[QuickUseSlotIndex];

	// If item was consumed and quantity reached 0, RemoveItem already cleared the quick-use slot
	// Check if the slot was cleared (InventorySlotIndex == -1) or if the slot index changed
	if (UpdatedQuickSlot.InventorySlotIndex == -1 || UpdatedQuickSlot.InventorySlotIndex != SavedInventorySlotIndex)
	{
		// Quick-use slot was already cleared by RemoveItem (item was consumed completely)
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::UseQuickUseSlot - Quick-use slot %d was cleared (item consumed completely)"), QuickUseSlotIndex);
		return bSuccess;
	}

	// Item still exists in inventory (wasn't fully consumed), update quick-use slot
	if (bSuccess && InventorySlots.IsValidIndex(SavedInventorySlotIndex))
	{
		const FInventorySlot& UpdatedInvSlot = InventorySlots[SavedInventorySlotIndex];
		if (UpdatedInvSlot.bIsEmpty || !UpdatedInvSlot.Item)
		{
			// Slot became empty after use (shouldn't happen if RemoveItem cleared quick-use, but safety check)
			ClearQuickUseSlot(QuickUseSlotIndex);
		}
		else
		{
			// Update quick-use slot item reference (in case item instance changed or quantity updated)
			UpdatedQuickSlot.Item = UpdatedInvSlot.Item;
			OnQuickUseSlotChanged.Broadcast(QuickUseSlotIndex, UpdatedQuickSlot.Item);
		}
	}

	return bSuccess;
}

void UInventoryComponent::ClearQuickUseSlot(int32 QuickUseSlotIndex)
{
	// Validate index
	if (!QuickUseSlots.IsValidIndex(QuickUseSlotIndex))
	{
		return;
	}

	FQuickUseSlot& QuickSlot = QuickUseSlots[QuickUseSlotIndex];

	// Clear slot
	UItemBase* OldItem = QuickSlot.Item;
	QuickSlot.Item = nullptr;
	QuickSlot.InventorySlotIndex = -1;

	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::ClearQuickUseSlot - Cleared quick-use slot %d"), QuickUseSlotIndex);

	// Fire event
	OnQuickUseSlotChanged.Broadcast(QuickUseSlotIndex, nullptr);
}

FQuickUseSlot UInventoryComponent::GetQuickUseSlot(int32 QuickUseSlotIndex) const
{
	if (QuickUseSlots.IsValidIndex(QuickUseSlotIndex))
	{
		return QuickUseSlots[QuickUseSlotIndex];
	}

	return FQuickUseSlot();
}
