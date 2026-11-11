// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Inventory/InventoryComponent.h"
#include "Items/Equipment/WeaponItem.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Core/ItemTypes.h"
#include "Crafting/CraftingRecipeDataAsset.h"
#include "Items/Pickups/ItemPickupActor.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Progression/TrialComponent.h"
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
	UE_LOG(LogTemp, Verbose, TEXT("InventoryComponent::BeginPlay - normalizing slot state"));
	
	// Check if we have any items before clearing
	int32 ItemsBeforeBeginPlay = 0;
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (InventorySlots[i].Item && InventorySlots[i].Quantity > 0)
		{
			ItemsBeforeBeginPlay++;
			UE_LOG(LogTemp, Verbose, TEXT("InventoryComponent::BeginPlay - preserving slot %d item=%s quantity=%d"),
				i,
				InventorySlots[i].Item->ItemData ? *InventorySlots[i].Item->ItemData->ItemName.ToString() : TEXT("NULL ItemData"),
				InventorySlots[i].Quantity);
		}
	}
	
	if (ItemsBeforeBeginPlay > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::BeginPlay - preserving %d preloaded item slots"), ItemsBeforeBeginPlay);
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
	if (const UWeaponItem* Weapon = Cast<UWeaponItem>(Item); Weapon && Weapon->IsBoundToOtherOwner(GetOwner()))
	{
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
		UClass* ItemClass = Item->GetClass();
		UItemBase* NewItem = NewObject<UItemBase>(this, ItemClass);
		if (!NewItem)
		{
			UE_LOG(LogTemp, Error, TEXT("InventoryComponent::AddItem - Failed to create new item instance"));
			return false;
		}

		NewItem->ItemData = Item->ItemData;
		NewItem->Quantity = StackSize;
		if (UWeaponItem* Weapon = Cast<UWeaponItem>(NewItem); Weapon && !Weapon->BindToOwner(GetOwner()))
		{
			return false;
		}

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
		ReconcileQuickUseBindings();
		
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
				UItemBase* SourceItem = FromSlotRef.Item;
				ToSlotRef.Quantity += StackAmount;
				FromSlotRef.Quantity -= StackAmount;
				ToSlotRef.Item->Quantity = ToSlotRef.Quantity;

				if (FromSlotRef.Quantity <= 0)
				{
					FromSlotRef = FInventorySlot(); // Clear source slot
				}
				else
				{
					FromSlotRef.Item->Quantity = FromSlotRef.Quantity;
				}

				BroadcastInventoryChanged(FromSlot, FromSlotRef.Item);
				BroadcastInventoryChanged(ToSlot, ToSlotRef.Item);
				if (FromSlotRef.bIsEmpty)
				{
					bool bDestinationBound = false;
					for (const FQuickUseSlot& QuickSlot : QuickUseSlots)
					{
						if (QuickSlot.Item == ToSlotRef.Item) bDestinationBound = true;
					}
					for (int32 QuickIndex = 0; QuickIndex < QuickUseSlots.Num(); ++QuickIndex)
					{
						FQuickUseSlot& QuickSlot = QuickUseSlots[QuickIndex];
						if (QuickSlot.Item != SourceItem) continue;
						if (bDestinationBound)
						{
							ClearQuickUseSlot(QuickIndex);
						}
						else
						{
							QuickSlot.Item = ToSlotRef.Item;
							QuickSlot.InventorySlotIndex = ToSlot;
							OnQuickUseSlotChanged.Broadcast(QuickIndex, QuickSlot.Item);
							bDestinationBound = true;
						}
					}
				}
				ReconcileQuickUseBindings();
				
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
	ReconcileQuickUseBindings();

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
	if (!Slot.Item->CanUse())
	{
		return false;
	}
	FName ItemID = Slot.Item->ItemData->ItemID;

	const bool bIsHealthPotion = (ItemID == FName("HealthPotion") || ItemID == FName("healthpotion"));
	const bool bIsManaPotion = (ItemID == FName("ManaPotion") || ItemID == FName("manapotion"));
	const bool bIsStaminaPotion = (ItemID == FName("StaminaPotion") || ItemID == FName("staminapotion"));

	if (bIsHealthPotion || bIsManaPotion || bIsStaminaPotion)
	{
		USecondaryAttributeComponent* SecondaryAttributeComponent = nullptr;
		if (AActor* Owner = GetOwner())
		{
			SecondaryAttributeComponent = Owner->FindComponentByClass<USecondaryAttributeComponent>();
		}

		if (!SecondaryAttributeComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - Potion failed: SecondaryAttributeComponent missing"));
			return false;
		}

		const ESecondaryAttribute TargetAttribute = bIsHealthPotion ? ESecondaryAttribute::MaxHealth
			: (bIsManaPotion ? ESecondaryAttribute::MaxMana : ESecondaryAttribute::MaxStamina);
		const TCHAR* AttributeLabel = bIsHealthPotion ? TEXT("Health") : (bIsManaPotion ? TEXT("Mana") : TEXT("Stamina"));

		const float MaxValue = SecondaryAttributeComponent->GetSecondaryAttribute(TargetAttribute);
		const float CurrentValue = SecondaryAttributeComponent->GetCurrentSecondaryAttribute(TargetAttribute);

		if (MaxValue <= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - %s potion failed: Max value not set"), AttributeLabel);
			return false;
		}

		if (CurrentValue >= MaxValue)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - %s already at max (%.1f/%.1f)"), AttributeLabel, CurrentValue, MaxValue);
			return false;
		}

		const float RestoreAmount = Slot.Item->ItemData->ConsumableRestoreAmount;
		if (RestoreAmount <= 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UseItem - %s potion failed: Restore amount not set"), AttributeLabel);
			return false;
		}

		if (!SecondaryAttributeComponent->RestoreResource(TargetAttribute, RestoreAmount))
		{
			return false;
		}
		const float NewValue = SecondaryAttributeComponent->GetCurrentSecondaryAttribute(TargetAttribute);
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Restored %.1f %s (%.1f -> %.1f/%.1f)"),
			RestoreAmount, AttributeLabel, CurrentValue, NewValue, MaxValue);
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
		// Skill-related items are used and consumed on successful use
		bShouldConsume = true;
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Skill item, will consume 1 quantity on use"));
		break;

	case EItemType::Misc:
	default:
		// Misc items can be used but not consumed
		UE_LOG(LogTemp, Log, TEXT("InventoryComponent::UseItem - Misc item, not consumed"));
		break;
	}

	// Do not consume or broadcast a use when a skill grant fails after preflight.
	if (!Slot.Item->TryUse())
	{
		return false;
	}

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

int32 UInventoryComponent::CountItemData(const UItemDataAsset* ItemData) const
{
	if (!ItemData)
	{
		return 0;
	}

	int32 Count = 0;
	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (!Slot.bIsEmpty && Slot.Item && Slot.Item->ItemData == ItemData && Slot.Quantity > 0)
		{
			Count += Slot.Quantity;
		}
	}
	return Count;
}

bool UInventoryComponent::BuildCraftPlan(const UCraftingRecipeDataAsset* Recipe, TArray<int32>& Quantities,
	TArray<int32>& Consumed, TArray<int32>& Added) const
{
	Quantities.Reset();
	Consumed.Reset();
	Added.Reset();
	if (!Recipe || Recipe->Ingredients.IsEmpty() || !Recipe->OutputItem || Recipe->OutputQuantity <= 0 ||
		Recipe->OutputItem->MaxStackSize <= 0 || !FMath::IsFinite(Recipe->OutputItem->Weight) || Recipe->OutputItem->Weight < 0.0f)
	{
		return false;
	}

	const int32 NumSlots = InventorySlots.Num();
	Quantities.Init(0, NumSlots);
	Consumed.Init(0, NumSlots);
	Added.Init(0, NumSlots);
	for (int32 Index = 0; Index < NumSlots; ++Index)
	{
		const FInventorySlot& Slot = InventorySlots[Index];
		if (!Slot.bIsEmpty && Slot.Item && Slot.Item->ItemData && Slot.Quantity > 0)
		{
			Quantities[Index] = Slot.Quantity;
		}
	}

	for (const FCraftingIngredient& Ingredient : Recipe->Ingredients)
	{
		if (!Ingredient.Item || Ingredient.Quantity <= 0)
		{
			return false;
		}
		int32 Remaining = Ingredient.Quantity;
		for (int32 Index = 0; Index < NumSlots && Remaining > 0; ++Index)
		{
			const FInventorySlot& Slot = InventorySlots[Index];
			if (Quantities[Index] > 0 && Slot.Item && Slot.Item->ItemData == Ingredient.Item)
			{
				const int32 Taken = FMath::Min(Remaining, Quantities[Index]);
				Quantities[Index] -= Taken;
				Consumed[Index] += Taken;
				Remaining -= Taken;
			}
		}
		if (Remaining > 0)
		{
			return false;
		}
	}

	double WeightAfterCraft = static_cast<double>(Recipe->OutputItem->Weight) * Recipe->OutputQuantity;
	for (int32 Index = 0; Index < NumSlots; ++Index)
	{
		if (Quantities[Index] > 0)
		{
			const UItemDataAsset* Data = InventorySlots[Index].Item->ItemData;
			if (!FMath::IsFinite(Data->Weight) || Data->Weight < 0.0f)
			{
				return false;
			}
			WeightAfterCraft += static_cast<double>(Data->Weight) * Quantities[Index];
		}
	}
	if (!FMath::IsFinite(WeightAfterCraft) || WeightAfterCraft > MaxWeight)
	{
		return false;
	}

	int32 RemainingOutput = Recipe->OutputQuantity;
	const int32 MaxStack = Recipe->OutputItem->MaxStackSize;
	for (int32 Index = 0; Index < NumSlots && RemainingOutput > 0; ++Index)
	{
		const FInventorySlot& Slot = InventorySlots[Index];
		if (Quantities[Index] > 0 && Slot.Item && Slot.Item->ItemData == Recipe->OutputItem)
		{
			const int32 ToAdd = FMath::Min(RemainingOutput, FMath::Max(0, MaxStack - Quantities[Index]));
			Quantities[Index] += ToAdd;
			Added[Index] += ToAdd;
			RemainingOutput -= ToAdd;
		}
	}
	for (int32 Index = 0; Index < NumSlots && RemainingOutput > 0; ++Index)
	{
		if (Quantities[Index] == 0)
		{
			const int32 ToAdd = FMath::Min(RemainingOutput, MaxStack);
			Quantities[Index] = ToAdd;
			Added[Index] = ToAdd;
			RemainingOutput -= ToAdd;
		}
	}
	return RemainingOutput == 0;
}

bool UInventoryComponent::CanCraftRecipe(const UCraftingRecipeDataAsset* Recipe) const
{
	TArray<int32> Quantities, Consumed, Added;
	return BuildCraftPlan(Recipe, Quantities, Consumed, Added);
}

bool UInventoryComponent::CraftRecipe(const UCraftingRecipeDataAsset* Recipe)
{
	TArray<int32> Quantities, Consumed, Added;
	if (!BuildCraftPlan(Recipe, Quantities, Consumed, Added))
	{
		return false;
	}

	TArray<UItemBase*> NewItems;
	NewItems.Init(nullptr, InventorySlots.Num());
	UClass* OutputClass = Recipe->OutputItem->ItemClass ? Recipe->OutputItem->ItemClass.Get() : UItemBase::StaticClass();
	for (int32 Index = 0; Index < InventorySlots.Num(); ++Index)
	{
		if (Added[Index] > 0 && (InventorySlots[Index].bIsEmpty || Consumed[Index] == InventorySlots[Index].Quantity))
		{
			UItemBase* NewItem = NewObject<UItemBase>(this, OutputClass);
			if (!NewItem)
			{
				return false;
			}
			NewItem->ItemData = Recipe->OutputItem;
			NewItem->Quantity = Quantities[Index];
			NewItems[Index] = NewItem;
		}
	}

	TArray<UItemBase*> RemovedItems;
	RemovedItems.Init(nullptr, InventorySlots.Num());
	for (int32 Index = 0; Index < InventorySlots.Num(); ++Index)
	{
		if (Consumed[Index] == 0 && Added[Index] == 0)
		{
			continue;
		}
		FInventorySlot& Slot = InventorySlots[Index];
		RemovedItems[Index] = Slot.Item;
		const bool bOldItemGone = Consumed[Index] > 0 && Consumed[Index] == Slot.Quantity;
		if (Quantities[Index] == 0 || bOldItemGone)
		{
			for (int32 QuickIndex = 0; QuickIndex < QuickUseSlots.Num(); ++QuickIndex)
			{
				if (QuickUseSlots[QuickIndex].InventorySlotIndex == Index)
				{
					ClearQuickUseSlot(QuickIndex);
				}
			}
		}
		if (NewItems[Index])
		{
			Slot.Item = NewItems[Index];
		}
		else if (Quantities[Index] == 0)
		{
			Slot.Item = nullptr;
		}
		Slot.Quantity = Quantities[Index];
		Slot.bIsEmpty = Quantities[Index] == 0;
		if (Slot.Item)
		{
			Slot.Item->Quantity = Slot.Quantity;
		}
	}

	for (int32 Index = 0; Index < InventorySlots.Num(); ++Index)
	{
		if (Consumed[Index] > 0 || Added[Index] > 0)
		{
			BroadcastInventoryChanged(Index, InventorySlots[Index].Item);
			if (Consumed[Index] > 0)
			{
				OnItemRemoved.Broadcast(RemovedItems[Index], Consumed[Index]);
			}
			if (Added[Index] > 0)
			{
				OnItemAdded.Broadcast(InventorySlots[Index].Item);
			}
		}
	}
	if (!Recipe->RecipeID.IsNone())
	{
		if (AActor* OwnerActor = GetOwner())
		{
			if (UTrialComponent* Trials = OwnerActor->FindComponentByClass<UTrialComponent>())
			{
				Trials->ReportRecipeCrafted(Recipe->RecipeID);
			}
		}
	}
	return true;
}

bool UInventoryComponent::HasSpaceFor(const UItemBase* Item, int32 Quantity) const
{
	if (!Item || !Item->ItemData || Quantity <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::HasSpaceFor - Invalid item or quantity"));
		return false;
	}
	const int32 MaxStackSize = Item->ItemData->MaxStackSize;
	if (MaxStackSize <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::HasSpaceFor - Invalid maximum stack size"));
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

	// Count every unit that can fit, including partial matching stacks and all empty slots.
	const FName ItemID = Item->ItemData->ItemID;
	int64 AvailableUnits = 0;
	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.bIsEmpty)
		{
			AvailableUnits += MaxStackSize;
		}
		else if (Slot.Item && Slot.Item->ItemData && Slot.Item->ItemData->ItemID == ItemID)
		{
			AvailableUnits += FMath::Max(0, Slot.Item->ItemData->MaxStackSize - Slot.Quantity);
		}
		if (AvailableUnits >= Quantity) return true;
	}

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent::HasSpaceFor - Capacity insufficient (Need %d units, have %lld)"),
		Quantity, AvailableUnits);
	return false;
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
	if (ItemData->bSoulboundGrowth)
	{
		UE_LOG(LogTemp, Log, TEXT("Soulbound item cannot be split"));
		return false;
	}
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
	UClass* ItemClass = Slot.Item->GetClass();
	UItemBase* NewItem = NewObject<UItemBase>(this, ItemClass);
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
		UClass* ItemClass = SourceSlot.Item->GetClass();
		UItemBase* NewItem = NewObject<UItemBase>(this, ItemClass);
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
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::DropItemToWorld - Invalid quantity %d for slot with %d items"),
			Quantity, Slot.Quantity);
		return false;
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
	if (ItemData->bSoulboundGrowth)
	{
		UE_LOG(LogTemp, Log, TEXT("Soulbound item cannot be dropped"));
		return false;
	}

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
		PickupActor->Destroy();
		return false;
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

	// A partial drop leaves the same item instance in this slot, so its hotkey remains valid.
	if (Slot.bIsEmpty)
	{
		for (int32 i = 0; i < QuickUseSlots.Num(); i++)
		{
			if (QuickUseSlots[i].InventorySlotIndex == SlotIndex)
			{
				ClearQuickUseSlot(i);
			}
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

void UInventoryComponent::ReconcileQuickUseBindings()
{
	for (int32 QuickIndex = 0; QuickIndex < QuickUseSlots.Num(); ++QuickIndex)
	{
		FQuickUseSlot& QuickSlot = QuickUseSlots[QuickIndex];
		if (!QuickSlot.Item) continue;
		int32 NewInventoryIndex = INDEX_NONE;
		for (int32 InventoryIndex = 0; InventoryIndex < InventorySlots.Num(); ++InventoryIndex)
		{
			const FInventorySlot& InventorySlot = InventorySlots[InventoryIndex];
			if (!InventorySlot.bIsEmpty && InventorySlot.Item == QuickSlot.Item)
			{
				NewInventoryIndex = InventoryIndex;
				break;
			}
		}
		if (NewInventoryIndex == INDEX_NONE)
		{
			ClearQuickUseSlot(QuickIndex);
		}
		else if (QuickSlot.InventorySlotIndex != NewInventoryIndex)
		{
			QuickSlot.InventorySlotIndex = NewInventoryIndex;
			OnQuickUseSlotChanged.Broadcast(QuickIndex, QuickSlot.Item);
		}
	}
}

bool UInventoryComponent::EquipItemFromSlot(int32 SlotIndex)
{
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::EquipItemFromSlot - Invalid slot index: %d"), SlotIndex);
		return false;
	}

	FInventorySlot& Slot = InventorySlots[SlotIndex];
	if (Slot.bIsEmpty || !Slot.Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::EquipItemFromSlot - Slot %d is empty"), SlotIndex);
		return false;
	}

	UEquipmentItem* EquipmentItem = Cast<UEquipmentItem>(Slot.Item);
	if (!EquipmentItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::EquipItemFromSlot - Item is not equipment"));
		return false;
	}

	UEquipmentComponent* EquipmentComponent = GetOwner() ? GetOwner()->FindComponentByClass<UEquipmentComponent>() : nullptr;
	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::EquipItemFromSlot - EquipmentComponent missing"));
		return false;
	}

	if (!EquipmentComponent->EquipItem(EquipmentItem))
	{
		return false;
	}

	return RemoveItem(SlotIndex, 1);
}

bool UInventoryComponent::UnequipItemToInventory(EEquipmentSlot Slot)
{
	UEquipmentComponent* EquipmentComponent = GetOwner() ? GetOwner()->FindComponentByClass<UEquipmentComponent>() : nullptr;
	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UnequipItemToInventory - EquipmentComponent missing"));
		return false;
	}
	UEquipmentItem* EquippedItem = EquipmentComponent->GetEquippedItem(Slot);
	if (!EquippedItem)
	{
		return false;
	}
	if (!HasSpaceFor(EquippedItem, 1))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UnequipItemToInventory - No inventory capacity for equipped item"));
		return false;
	}
	const bool bPreserveSoulboundInstance = EquippedItem->ItemData && EquippedItem->ItemData->bSoulboundGrowth;
	const int32 TargetSlotIndex = bPreserveSoulboundInstance ? FindEmptySlot() : INDEX_NONE;
	if (bPreserveSoulboundInstance && TargetSlotIndex == INDEX_NONE)
	{
		return false;
	}

	UEquipmentItem* UnequippedItem = EquipmentComponent->UnequipSlot(Slot);
	if (!UnequippedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UnequipItemToInventory - No item in slot"));
		return false;
	}

	if (bPreserveSoulboundInstance)
	{
		FInventorySlot& TargetSlot = InventorySlots[TargetSlotIndex];
		TargetSlot.Item = UnequippedItem;
		TargetSlot.Quantity = 1;
		TargetSlot.bIsEmpty = false;
		BroadcastInventoryChanged(TargetSlotIndex, UnequippedItem);
		OnItemAdded.Broadcast(UnequippedItem);
		return true;
	}
	return AddItem(UnequippedItem, 1);
}

bool UInventoryComponent::UnequipItemToSlot(EEquipmentSlot Slot, int32 TargetSlotIndex)
{
	if (!InventorySlots.IsValidIndex(TargetSlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UnequipItemToSlot - Invalid target slot index: %d"), TargetSlotIndex);
		return false;
	}

	UEquipmentComponent* EquipmentComponent = GetOwner() ? GetOwner()->FindComponentByClass<UEquipmentComponent>() : nullptr;
	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UnequipItemToSlot - EquipmentComponent missing"));
		return false;
	}

	FInventorySlot& TargetSlot = InventorySlots[TargetSlotIndex];
	if (!TargetSlot.bIsEmpty)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UnequipItemToSlot - Target slot %d is not empty"), TargetSlotIndex);
		return false;
	}

	UEquipmentItem* UnequippedItem = EquipmentComponent->UnequipSlot(Slot);
	if (!UnequippedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent::UnequipItemToSlot - No item in slot"));
		return false;
	}

	TargetSlot.Item = UnequippedItem;
	TargetSlot.Quantity = 1;
	TargetSlot.bIsEmpty = false;

	BroadcastInventoryChanged(TargetSlotIndex, TargetSlot.Item);
	OnItemAdded.Broadcast(TargetSlot.Item);

	return true;
}
