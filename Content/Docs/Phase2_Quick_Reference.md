# Phase 2: Inventory System - Quick Reference Guide
**Project:** ActionRPG  
**Next Phase:** Phase 2 - Inventory System  
**Duration:** Week 3-4 (14 days)  
**Prerequisites:** Phase 1 Complete ✅  
**Last Updated:** 2025-01-07 (UE 5.7 Compliance Update)

---

## Phase 2 Overview

### Goals
- Implement Inventory Component
- Create Item Pickup Actor
- Develop Inventory UI System
- Implement Drag and Drop functionality
- Integrate Item usage with character

### Key Features
1. **InventoryComponent**
   - Item storage (TArray of ItemBase)
   - Add/Remove items
   - Item stacking
   - Weight management
   - Capacity limits

2. **ItemPickupActor**
   - World representation of items
   - Interaction detection
   - Auto-pickup on proximity
   - Visual feedback

3. **Inventory UI**
   - Grid-based inventory display
   - Item icons and tooltips
   - Drag and drop between slots
   - Quick-use slots
   - Item comparison view

---

## Phase 1 Foundations Ready for Phase 2

### Available Systems
✅ **Item System**
- `UItemDataAsset` - Item definitions
- `UItemBase` - Item instances
- `UItemDatabase` - Item lookup (singleton)
- Item types and rarities defined

✅ **Character System**
- `AActionRPGPlayerCharacter` - Player character
- Ready for component attachment
- Input system integrated

✅ **Input System**
- `IA_Interact` - For item pickup
- `IA_OpenInventory` - For UI toggle
- `OnInteract()` handler ready (placeholder)

✅ **Camera System**
- Top-down view configured
- Ready for UI overlay

---

## Phase 2 Implementation Checklist

### Week 3: Inventory Foundation

#### Day 15-16: Inventory Component ✅ COMPLETE
- [x] Create `UInventoryComponent` class
- [x] Implement item storage (TArray<FInventorySlot>)
- [x] Add `AddItem()` method
- [x] Add `RemoveItem()` method
- [x] Implement item stacking logic
- [x] Add weight calculation
- [x] Add capacity limits
- [x] Create Blueprint class
- [x] Attach to PlayerCharacter
- [x] UE 5.7 compliance updates
- [x] Debug reporting feature

#### Day 17-18: Item Pickup Actor ✅ COMPLETE
- [x] Create `AItemPickupActor` class
- [x] Implement item data reference
- [x] Add interaction detection (overlap)
- [x] Implement pickup logic
- [x] Add visual representation (StaticMesh/Material)
- [x] Add pickup feedback (particles/sound - placeholder)
- [x] Create Blueprint class
- [x] Test in-game
- [x] UE 5.7 compliance updates

#### Day 19-20: Item Usage
- [ ] Implement `UseItem()` in InventoryComponent
- [ ] Connect to `UItemBase::Use()`
- [ ] Handle consumable items
- [ ] Handle equipment items
- [ ] Handle skill items
- [ ] Add item quantity management
- [ ] Test item usage

#### Day 21: Integration Testing
- [ ] Test item pickup → inventory
- [ ] Test item usage from inventory
- [ ] Test item stacking
- [ ] Test weight/capacity limits
- [ ] Fix any issues

### Week 4: Inventory UI

#### Day 22-23: Inventory Widget Foundation
- [ ] Create `UInventoryWidget` class
- [ ] Design inventory grid layout
- [ ] Implement slot system (UUserWidget slots)
- [ ] Display item icons
- [ ] Display item quantities
- [ ] Add tooltip system
- [ ] Create Blueprint widget

#### Day 24-25: Drag and Drop
- [ ] Implement drag functionality
- [ ] Implement drop functionality
- [ ] Handle slot swapping
- [ ] Handle stacking on drop
- [ ] Add visual feedback (drag preview)
- [ ] Add drop validation
- [ ] Test drag and drop

#### Day 26-27: UI Polish & Features
- [ ] Add item details panel
- [ ] Add item comparison view
- [ ] Add quick-use slots
- [ ] Add filter/sort functionality
- [ ] Add inventory search
- [ ] Polish UI visuals
- [ ] Add animations

#### Day 28: Final Integration & Testing
- [ ] Integrate all UI features
- [ ] Test complete inventory flow
- [ ] Fix any bugs
- [ ] Performance optimization
- [ ] Documentation

---

## Key Classes Reference (Phase 1)

### Item System
```cpp
// Item Database - Singleton lookup (auto-initializes)
UItemDatabase* DB = UItemDatabase::Get();

// Item Data Asset - Static item definition (template)
UItemDataAsset* ItemData = DB->GetItemDataAsset("HealthPotion");

// Item Instance - Runtime item object (temporary, for validation)
UItemBase* TempItem = DB->CreateItem("HealthPotion", 1);

// Note: CreateItem() creates temporary items with GetTransientPackage() as outer
// Actual inventory items are created by InventoryComponent::AddItem()
```

### Character System
```cpp
// Player Character - Component created automatically in constructor
AActionRPGPlayerCharacter* Player = GetPlayerCharacter();
UInventoryComponent* Inventory = Player->InventoryComponent; // Already exists

// Access inventory functions
Inventory->AddItem(Item, Quantity);
bool bHasSpace = Inventory->HasSpaceFor(Item, Quantity);
float CurrentWeight = Inventory->GetCurrentWeight();
const TArray<FInventorySlot>& Slots = Inventory->GetInventorySlots();
```

### Input System
```cpp
// Input handlers already in PlayerController
void OnInteract(); // Ready for item pickup
void OnOpenInventory(); // Ready for UI toggle
```

---

## Current Implementation

### Inventory Component Structure
```cpp
// Uses slot-based system (not simple array)
struct FInventorySlot
{
    TObjectPtr<UItemBase> Item;  // Item instance
    int32 Quantity;              // Quantity in slot
    bool bIsEmpty;               // Empty status
};

// Component has array of slots
TArray<FInventorySlot> InventorySlots; // 50 slots by default
```

### Key Functions (UE 5.7 Compliant)
```cpp
// Add item (with quantity parameter)
bool AddItem(UItemBase* Item, int32 Quantity = 1);

// Check space (uses quantity parameter, not Item->Quantity)
bool HasSpaceFor(UItemBase* Item, int32 Quantity = 1) const;

// Get slots (returns const reference for performance)
const TArray<FInventorySlot>& GetInventorySlots() const;

// All getters are BlueprintPure
float GetCurrentWeight() const;
int32 GetTotalItemCount() const;
```

### Item Pickup Flow
1. Player overlaps ItemPickupActor
2. CanPickup() validates (checks space, creates temp item)
3. PickupItem() creates item template
4. AddItem() creates actual inventory items
5. Pickup actor destroyed

---

## Common Patterns

### Item Stacking
```cpp
// Check if item can stack
if (ExistingItem->ItemData->ItemID == NewItem->ItemData->ItemID)
{
    int32 StackSpace = ExistingItem->ItemData->MaxStackSize - ExistingItem->Quantity;
    if (StackSpace > 0)
    {
        // Add to existing stack
    }
}
```

### Weight Calculation
```cpp
float CalculateTotalWeight() const
{
    float Total = 0.0f;
    for (UItemBase* Item : Items)
    {
        Total += Item->ItemData->Weight * Item->Quantity;
    }
    return Total;
}
```

### Item Usage
```cpp
void UseItem(UItemBase* Item)
{
    if (Item && Item->CanUse())
    {
        Item->Use();
        if (Item->Quantity <= 0)
        {
            RemoveItem(Item, 1);
        }
    }
}
```

---

## UI Widget Structure

### Inventory Widget Hierarchy
```
InventoryWidget (Canvas)
├── GridPanel (Inventory Slots)
│   ├── InventorySlot_0
│   ├── InventorySlot_1
│   └── ... (50 slots)
├── QuickUseBar (Horizontal Box)
│   ├── QuickSlot_0
│   ├── QuickSlot_1
│   └── ... (10 slots)
├── ItemDetailsPanel (Vertical Box)
│   ├── ItemName (Text)
│   ├── ItemDescription (Text)
│   └── ItemStats (Text)
└── WeightBar (Progress Bar)
```

---

## Testing Checklist

### Inventory Component
- [ ] Add single item
- [ ] Add stackable item (test stacking)
- [ ] Remove item
- [ ] Remove partial stack
- [ ] Test capacity limit
- [ ] Test weight limit
- [ ] Get all items

### Item Pickup
- [ ] Pickup from world
- [ ] Pickup adds to inventory
- [ ] Pickup destroys actor
- [ ] Visual feedback works
- [ ] Multiple items in world

### Item Usage
- [ ] Use consumable item
- [ ] Use equipment item
- [ ] Use skill item
- [ ] Item quantity decreases
- [ ] Item removed when quantity = 0

### UI System
- [ ] Inventory opens/closes
- [ ] Items display correctly
- [ ] Drag and drop works
- [ ] Slot swapping works
- [ ] Tooltips show
- [ ] Quick-use slots work

---

## Resources

### Documentation
- `Inventory_System_Architecture.md` - Complete architecture documentation
- `ItemPickup_Inventory_Issue_Debug.md` - Debugging guide with current fixes
- `ItemPickup_Player_Blueprint_Setup.md` - Blueprint setup guide
- `ItemPickup_Collision_Troubleshooting.md` - Collision troubleshooting
- `ItemDatabase_Troubleshooting.md` - ItemDatabase issues
- `Phase2_InventoryComponent_Settings_Guide.md` - Settings reference
- `UE_5.7_Compliance_Updates.md` - All UE 5.7 changes

### Phase 1 Documentation
- `Phase1_Implementation_Plan.md` - Master plan
- `Phase1_Completion_Summary.md` - Phase 1 summary
- `Architecture_Design_Plan.md` - Full architecture

### Current Files (Completed)
- ✅ `Source/ActionRPG/Public/Components/Inventory/InventoryComponent.h`
- ✅ `Source/ActionRPG/Private/Components/Inventory/InventoryComponent.cpp`
- ✅ `Source/ActionRPG/Public/Items/Pickups/ItemPickupActor.h`
- ✅ `Source/ActionRPG/Private/Items/Pickups/ItemPickupActor.cpp`

### Phase 2 Files Still To Create
- [ ] `Source/ActionRPG/Public/UI/Inventory/InventoryWidget.h`
- [ ] `Source/ActionRPG/Public/UI/Inventory/InventorySlotWidget.h`

---

## Notes

- Start with InventoryComponent (core functionality)
- Test thoroughly before moving to UI
- UI can be developed in parallel after core is done
- Keep Item System foundation in mind (already built)
- Use Blueprint for UI, C++ for logic

---

**Ready to begin Phase 2!** 🚀

---

**End of Phase 2 Quick Reference**
