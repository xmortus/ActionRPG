# Inventory System Architecture
**Last Updated:** 2025-01-07  
**UE Version:** 5.7

---

## Overview

The inventory system consists of several key components working together to manage item storage, retrieval, and operations.

---

## Core Components

### 1. ItemDatabase (Singleton)

**Purpose:** Stores ItemDataAssets (templates/definitions) for all items in the game.

**Key Points:**
- **Singleton Pattern:** One instance shared by all players
- **Stores Templates Only:** Does NOT store actual inventory items
- **Initialization:** Automatically loads all ItemDataAssets via Asset Manager on first access
- **Location:** `Source/ActionRPG/Public/Data/ItemDatabase.h`

**Functions:**
- `Get()` - Singleton accessor
- `GetItemDataAsset(ItemID)` - Lookup item data by ID
- `CreateItem(ItemID, Quantity)` - Create item instance from template
- `GetAllItemDataAssets()` - Get all registered items
- `GetItemsByType(Type)` - Filter by item type
- `GetItemsByRarity(Rarity)` - Filter by rarity

**Important:** The ItemDatabase is a **template library**, not a storage system. It provides the data needed to create item instances, but actual items are stored in InventoryComponent.

---

### 2. InventoryComponent (Per-Player)

**Purpose:** Manages a single player's inventory (item storage, stacking, limits).

**Key Points:**
- **Per-Player Instance:** Each player character has their own InventoryComponent
- **Stores Actual Items:** Contains UItemBase instances (actual inventory items)
- **Unique Per Player:** Items stored here are separate from other players
- **Location:** `Source/ActionRPG/Public/Components/Inventory/InventoryComponent.h`

**Properties:**
- `InventorySlots` - Array of FInventorySlot structures
- `MaxCapacity` - Maximum number of slots (default: 50)
- `MaxWeight` - Maximum weight limit (default: 100.0)

**Key Functions:**
- `AddItem(Item, Quantity)` - Add items to inventory
- `RemoveItem(SlotIndex, Quantity)` - Remove items from inventory
- `GetItemAt(SlotIndex)` - Get item at specific slot
- `HasSpaceFor(Item, Quantity)` - Check if inventory has space
- `GetCurrentWeight()` - Get current total weight
- `GetTotalItemCount()` - Get total number of items
- `ReportInventoryContents()` - Debug function to log inventory state

**Events:**
- `OnInventoryChanged` - Fired when any slot changes
- `OnItemAdded` - Fired when item is added
- `OnItemRemoved` - Fired when item is removed
- `OnItemUsed` - Fired when item is used

---

### 3. ItemPickupActor

**Purpose:** Represents an item pickup in the world that can be collected by players.

**Key Points:**
- **World Actor:** Spawned in the level
- **Overlap Detection:** Uses SphereComponent for collision detection
- **Auto-Destroy:** Destroys itself after successful pickup
- **Location:** `Source/ActionRPG/Public/Items/Pickups/ItemPickupActor.h`

**Properties:**
- `ItemData` - Reference to ItemDataAsset (template)
- `Quantity` - Number of items in this pickup
- `CollisionComponent` - SphereComponent for overlap detection
- `MeshComponent` - Visual representation

**Functions:**
- `CanPickup(Player)` - Validates if player can pick up item
- `PickupItem(Player)` - Handles the pickup process
- `OnOverlapBegin()` - Called when player overlaps pickup

---

### 4. ItemBase

**Purpose:** Represents an actual item instance in the inventory.

**Key Points:**
- **Runtime Object:** Created at runtime from ItemDataAsset
- **Instance Data:** Contains quantity and references ItemDataAsset
- **Location:** `Source/ActionRPG/Public/Items/Core/ItemBase.h`

**Properties:**
- `ItemData` - Reference to ItemDataAsset (template)
- `Quantity` - Number of this item

**Functions:**
- `Use()` - Use the item
- `CanUse()` - Check if item can be used
- `GetItemID()` - Get item ID
- `GetItemName()` - Get item name
- `GetItemType()` - Get item type

---

## Data Flow

### Item Pickup Flow:

1. **Player overlaps ItemPickupActor**
   - `OnOverlapBegin()` is called
   - Casts to `AActionRPGPlayerCharacter`

2. **Validation (CanPickup)**
   - Checks if ItemData is set
   - Gets player's InventoryComponent
   - Creates temporary item from ItemDatabase
   - Checks if inventory has space (`HasSpaceFor()`)

3. **Pickup (PickupItem)**
   - Creates item template from ItemDatabase
   - Calls `InventoryComponent::AddItem(Item, Quantity)`
   - `AddItem` creates new item instances and stores in slots
   - Destroys pickup actor

4. **Storage (AddItem)**
   - Tries to stack with existing items first
   - Creates new slots for remaining quantity
   - Creates item instances with proper outer object (GetWorld())
   - Stores in InventorySlots array
   - Broadcasts events

---

## Memory Management

### Item Creation:

**Temporary Items (for validation):**
- Created with `GetTransientPackage()` as outer
- Automatically garbage collected when not referenced
- Used only for `HasSpaceFor()` checks

**Inventory Items:**
- Created with `GetWorld()` or `GetOwner()` as outer
- Stored in `TObjectPtr<UItemBase>` in InventorySlots
- Protected from GC by TObjectPtr reference
- Persist until removed from inventory

### UE 5.7 Best Practices:

- ✅ Use `TObjectPtr` for all UObject pointers
- ✅ Let GC handle cleanup (no `ConditionalBeginDestroy()` needed)
- ✅ Use proper outer objects for `NewObject()`
- ✅ Items in inventory are protected by TObjectPtr references

---

## Debug Features

### Automatic Debug Reporting:

The InventoryComponent automatically logs inventory contents every 5 seconds:
- Shows all slots (empty and filled)
- Displays capacity, weight, and item counts
- Logs inconsistent slot states
- Can be called manually via `ReportInventoryContents()` Blueprint function

### Enhanced Logging:

All components have comprehensive logging:
- ItemDatabase: Logs initialization and item registration
- InventoryComponent: Logs all operations with detailed state
- ItemPickupActor: Logs overlap events and pickup process
- ItemBase: Logs item usage

---

## Blueprint Integration

### Exposed Functions:

**InventoryComponent:**
- All management functions are `BlueprintCallable`
- Getter functions are `BlueprintPure` (no side effects)
- Events are `BlueprintAssignable`

**ItemPickupActor:**
- `PickupItem()` - `BlueprintCallable`
- `CanPickup()` - `BlueprintCallable`
- `DebugCollisionSettings()` - `BlueprintCallable`

**ItemBase:**
- All functions are `BlueprintCallable`
- Properties are `BlueprintReadWrite`

---

## Common Patterns

### Adding Items to Inventory:

```cpp
// Get player's inventory component
UInventoryComponent* Inventory = Player->InventoryComponent;

// Create item from database
UItemBase* Item = ItemDatabase::Get()->CreateItem(ItemID, 1);

// Add to inventory
Inventory->AddItem(Item, Quantity);
```

### Checking Inventory Space:

```cpp
// Create temporary item for validation
UItemBase* TempItem = ItemDatabase::Get()->CreateItem(ItemID, 1);

// Check if inventory has space
bool bHasSpace = Inventory->HasSpaceFor(TempItem, Quantity);
```

### Getting Inventory Items:

```cpp
// Get item at specific slot
UItemBase* Item = Inventory->GetItemAt(SlotIndex);

// Find slot by item ID
int32 SlotIndex = Inventory->FindItemSlot(ItemID);

// Get all slots
const TArray<FInventorySlot>& Slots = Inventory->GetInventorySlots();
```

---

## Troubleshooting

### Items Not Persisting:

1. Check if `BeginPlay` is being called multiple times
2. Verify items are created with proper outer object
3. Check debug reports to see slot states
4. Look for "INCONSISTENT STATE" messages

### Items Not Showing in Details Panel:

1. Details Panel may not auto-refresh during PIE
2. Reselect the actor/component to refresh
3. Check debug reports to verify items are actually stored
4. Verify UPROPERTY flags are correct

### Memory Issues:

1. Verify `TObjectPtr` is used for all UObject pointers
2. Check that items have proper outer objects
3. Don't use `ConditionalBeginDestroy()` - let GC handle it
4. Ensure items in inventory are referenced by TObjectPtr

---

## UE 5.7 Compliance

All code follows UE 5.7 standards:

- ✅ `TObjectPtr` for UObject pointers
- ✅ `BlueprintPure` for getter functions
- ✅ Proper UPROPERTY/UFUNCTION flags
- ✅ GC-safe memory management
- ✅ Const correctness
- ✅ Modern C++ practices

---

**End of Inventory System Architecture Documentation**
