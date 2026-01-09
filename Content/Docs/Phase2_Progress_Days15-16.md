# Phase 2 Progress Report - Days 15-16
**Date:** 2025-01-07  
**Status:** Ready for Implementation  
**Phase:** Phase 2 - Inventory System  
**Days:** 15-16 - Inventory Component Foundation

---

## Summary

Days 15-16 focus on creating the core InventoryComponent that will manage all item storage and operations for the player character. This component is the foundation of the entire inventory system and must be implemented correctly before proceeding to item pickups and UI.

---

## Day 15-16: Inventory Component Foundation

### Tasks Overview

1. **Create Folder Structure**
   - Set up Components folder structure in Source code
   - Create Inventory subfolder
   - Prepare Content folder structure

2. **Create InventoryComponent Class**
   - Define FInventorySlot structure
   - Create InventoryComponent header
   - Implement core inventory management methods
   - Add item stacking logic
   - Implement weight and capacity limits

3. **Attach to Player Character**
   - Integrate component with PlayerCharacter
   - Test component attachment
   - Verify component initialization

---

## ✅ Completed Tasks

### Folder Structure Created

#### Source Code Structure
```
Source/ActionRPG/
├── Public/
│   └── Components/
│       └── Inventory/
│           └── InventoryComponent.h
└── Private/
    └── Components/
        └── Inventory/
            └── InventoryComponent.cpp
```

#### Content Structure
```
Content/
└── Blueprints/
    └── Components/
        └── Inventory/
            └── (BP_InventoryComponent will be created here)
```

---

## Code Implementation

### 1. FInventorySlot Structure

**Location:** `Source/ActionRPG/Public/Components/Inventory/InventoryComponent.h`

**Purpose:** Represents a single inventory slot that can contain an item and quantity.

**Properties:**
- `Item` - Reference to UItemBase instance
- `Quantity` - Number of items in this slot
- `bIsEmpty` - Quick check if slot is empty

**Status:** ✅ Defined in header

---

### 2. InventoryComponent Class

**Location:** `Source/ActionRPG/Public/Components/Inventory/InventoryComponent.h`  
**Implementation:** `Source/ActionRPG/Private/Components/Inventory/InventoryComponent.cpp`

**Inheritance:** `UActorComponent`

**Status:** ✅ Class structure defined

#### Core Properties

- **InventorySlots** (TArray<FInventorySlot>)
  - Array of inventory slots (default 50 slots)
  - Each slot can hold one item type with quantity
  
- **MaxCapacity** (int32)
  - Maximum number of item stacks (default: 50)
  - Configurable in Blueprint
  
- **MaxWeight** (float)
  - Maximum inventory weight (default: 100.0f)
  - Prevents over-encumbrance
  - Configurable in Blueprint

#### Core Methods (To Be Implemented)

**Item Management:**
- ✅ `AddItem(UItemBase* Item, int32 Quantity = 1)` - Add item with stacking
- ✅ `RemoveItem(int32 SlotIndex, int32 Quantity = 1)` - Remove item from slot
- ✅ `MoveItem(int32 FromSlot, int32 ToSlot)` - Move item between slots
- ✅ `SwapItems(int32 SlotA, int32 SlotB)` - Swap two items
- ✅ `UseItem(int32 SlotIndex)` - Use item at slot

**Query Methods:**
- ✅ `GetItemAt(int32 SlotIndex)` - Get item reference at slot
- ✅ `FindItemSlot(const FName& ItemID)` - Find slot containing item by ID
- ✅ `HasSpaceFor(UItemBase* Item)` - Check if inventory has space
- ✅ `GetCurrentWeight()` - Calculate total inventory weight
- ✅ `GetTotalItemCount()` - Get total number of items

**Getter Methods:**
- ✅ `GetMaxCapacity()` - Get max capacity
- ✅ `GetMaxWeight()` - Get max weight
- ✅ `GetInventorySlots()` - Get all slots

**Helper Methods (Private):**
- ✅ `TryStackItem()` - Attempt to stack item with existing stacks
- ✅ `FindEmptySlot()` - Find first empty slot index

#### Events/Delegates

**Status:** ✅ Events defined

- `OnInventoryChanged` - Fired when any slot changes
  - Parameters: SlotIndex, Item
  
- `OnItemAdded` - Fired when item is added
  - Parameters: Item
  
- `OnItemRemoved` - Fired when item is removed
  - Parameters: Item, Quantity
  
- `OnItemUsed` - Fired when item is used
  - Parameters: Item

---

### 3. Item Stacking Logic

**Status:** ✅ Logic defined (implementation required)

**Key Features:**
- Checks if item can stack (same ItemID)
- Validates MaxStackSize from ItemDataAsset
- Merges stacks when possible
- Splits stacks when needed
- Handles partial stacking (add to existing, create new for remainder)

**Stacking Rules:**
1. Items with same ItemID can stack
2. Stack size cannot exceed ItemDataAsset.MaxStackSize
3. If adding quantity exceeds max stack, creates new stack in next slot
4. Empty slots are used first, then existing stacks

---

### 4. Weight Calculation

**Status:** ✅ Formula defined (implementation required)

**Weight Calculation:**
- `TotalWeight = Sum(Item.Weight * Item.Quantity)` for all items
- Validates weight before adding items
- Returns false from AddItem if weight limit would be exceeded

**Weight Limits:**
- Checked in `HasSpaceFor()` method
- Checked in `AddItem()` before adding
- Weight limit can be modified in Blueprint

---

### 5. Capacity Limits

**Status:** ✅ Logic defined (implementation required)

**Capacity Rules:**
- Maximum number of unique item stacks (not total items)
- Empty slots count toward capacity
- When capacity is full, cannot add new item types (only stack existing)
- Capacity can be modified in Blueprint

---

## Integration with Player Character

### Attachment Method

**Option 1: C++ Constructor (Recommended)**
- Add component in `AActionRPGPlayerCharacter` constructor
- Automatic initialization on character spawn

**Option 2: Blueprint Setup**
- Add component in Blueprint editor
- Manual configuration in Blueprint

**Status:** Integration plan defined (requires implementation)

---

## Code Quality Standards

### ✅ Follows UE 5.7 Standards

- Uses `TObjectPtr<>` for all object references
- Proper use of `UPROPERTY()` macros
- Blueprint-ready with `BlueprintType` and `Blueprintable`
- Component spawnable with `BlueprintSpawnableComponent` meta tag
- Proper event/delegate declarations
- Const correctness where appropriate
- Proper category organization in UPROPERTY

### ✅ Naming Conventions

- Class: `UInventoryComponent` (U prefix for UObject, Component suffix)
- Struct: `FInventorySlot` (F prefix for struct)
- Variables: PascalCase (UE 5.7 standard)
- Functions: PascalCase with descriptive names
- Categories: Properly organized ("Inventory", "Inventory|Debug")

---

## Testing Requirements

### Unit Testing (C++ Level)

- [ ] Component compiles without errors
- [ ] Component can be created in Blueprint
- [ ] AddItem works with single item
- [ ] AddItem works with stacking
- [ ] RemoveItem works correctly
- [ ] Weight calculation is accurate
- [ ] Capacity limits enforced
- [ ] Events fire correctly

### Integration Testing

- [ ] Component attaches to PlayerCharacter
- [ ] Component initializes correctly
- [ ] Can add items from ItemDatabase
- [ ] Can use items from inventory
- [ ] Events are received by listeners

---

## Known Dependencies

### Phase 1 Foundations Required

- ✅ `UItemBase` - Base item class (already created)
- ✅ `UItemDataAsset` - Item data definitions (already created)
- ✅ `UItemDatabase` - Item lookup singleton (already created)
- ✅ `AActionRPGPlayerCharacter` - Character class (already created)

### Future Dependencies (Not Yet Required)

- ItemPickupActor (Day 17-18)
- InventoryWidget (Day 22-23)
- Item usage system (Day 19-20)

---

## Implementation Notes

### Stacking Algorithm

**Priority Order:**
1. Find existing stack of same ItemID with space available
2. Fill existing stack (partial or full)
3. Create new stack in empty slot
4. Create new stack in next available slot if all full

**Validation:**
- Check MaxStackSize before stacking
- Check MaxCapacity before creating new stack
- Check MaxWeight before adding any items

### Weight Calculation

**Formula:**
```cpp
float TotalWeight = 0.0f;
for (const FInventorySlot& Slot : InventorySlots)
{
    if (!Slot.bIsEmpty && Slot.Item && Slot.Item->ItemData)
    {
        TotalWeight += Slot.Item->ItemData->Weight * Slot.Quantity;
    }
}
```

### Event Broadcasting

**When to Fire Events:**
- `OnInventoryChanged` - Any time a slot is modified
- `OnItemAdded` - When item is successfully added
- `OnItemRemoved` - When item is removed (any quantity)
- `OnItemUsed` - When UseItem() is called successfully

---

## Next Steps (Day 17-18)

After Days 15-16 are complete:

1. **ItemPickupActor Creation**
   - Create pickup actor class
   - Implement overlap detection
   - Integrate with InventoryComponent

2. **Visual Representation**
   - Add mesh component to pickups
   - Configure materials and effects
   - Test pickup in-world

---

## Files Created

### Headers (Public/)
- `Components/Inventory/InventoryComponent.h`

### Implementations (Private/)
- `Components/Inventory/InventoryComponent.cpp`

### Blueprint Classes (To Be Created Manually)
- `Content/Blueprints/Components/Inventory/BP_InventoryComponent`

---

## Manual Steps Required

⚠️ **IMPORTANT:** After C++ code is implemented, manual steps in Unreal Editor are required.

**See:** `Phase2_Manual_Steps_Days15-16.md` for detailed step-by-step instructions.

### Quick Summary of Manual Steps:
1. Compile C++ code in Visual Studio
2. Create Blueprint class from InventoryComponent
3. Add component to PlayerCharacter Blueprint
4. Configure component properties (capacity, weight)
5. Test component in Play mode

---

## Code Example Structure

### FInventorySlot Structure
```cpp
USTRUCT(BlueprintType)
struct ACTIONRPG_API FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    TObjectPtr<UItemBase> Item;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory")
    int32 Quantity;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    bool bIsEmpty;

    FInventorySlot()
        : Item(nullptr), Quantity(0), bIsEmpty(true)
    {}
};
```

### Component Attachment (Option 1: C++)
```cpp
// In ActionRPGPlayerCharacter.h
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
TObjectPtr<class UInventoryComponent> InventoryComponent;

// In ActionRPGPlayerCharacter.cpp constructor
InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
```

---

## Troubleshooting Checklist

If issues arise during implementation:

- [ ] Verify folder structure matches exactly
- [ ] Check all includes are correct
- [ ] Verify ItemBase and ItemDataAsset are accessible
- [ ] Check UPROPERTY specifiers are correct
- [ ] Verify BlueprintSpawnableComponent meta tag
- [ ] Test compilation before Blueprint creation
- [ ] Verify component can be found in Blueprint parent class picker

---

## Status Summary

**Days 15-16 Status:** ✅ **Plan Documented - Ready for Implementation**

**Next Actions:**
1. Implement InventoryComponent C++ code
2. Compile and verify no errors
3. Follow manual steps guide
4. Test component functionality
5. Proceed to Day 17-18 (ItemPickupActor)

---

**End of Phase 2 Progress Report - Days 15-16**
