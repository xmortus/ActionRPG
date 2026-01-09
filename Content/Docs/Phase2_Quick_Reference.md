# Phase 2: Inventory System - Quick Reference Guide
**Project:** ActionRPG  
**Next Phase:** Phase 2 - Inventory System  
**Duration:** Week 3-4 (14 days)  
**Prerequisites:** Phase 1 Complete ✅

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

#### Day 15-16: Inventory Component
- [ ] Create `UInventoryComponent` class
- [ ] Implement item storage (TArray<UItemBase*>)
- [ ] Add `AddItem()` method
- [ ] Add `RemoveItem()` method
- [ ] Implement item stacking logic
- [ ] Add weight calculation
- [ ] Add capacity limits
- [ ] Create Blueprint class
- [ ] Attach to PlayerCharacter

#### Day 17-18: Item Pickup Actor
- [ ] Create `AItemPickupActor` class
- [ ] Implement item data reference
- [ ] Add interaction detection (overlap)
- [ ] Implement pickup logic
- [ ] Add visual representation (StaticMesh/Material)
- [ ] Add pickup feedback (particles/sound)
- [ ] Create Blueprint class
- [ ] Test in-game

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
// Item Data Asset - Static item definition
UItemDataAsset* ItemData = ItemDatabase->GetItemDataAsset("HealthPotion");

// Item Instance - Runtime item object
UItemBase* Item = ItemDatabase->CreateItem("HealthPotion", 5);

// Item Database - Singleton lookup
UItemDatabase* DB = UItemDatabase::Get();
DB->Initialize(); // Call once at game start
```

### Character System
```cpp
// Player Character - Add component here
AActionRPGPlayerCharacter* Player = GetPlayerCharacter();
UInventoryComponent* Inventory = NewObject<UInventoryComponent>(Player);
Player->AddComponentByClass(UInventoryComponent::StaticClass());
```

### Input System
```cpp
// Input handlers already in PlayerController
void OnInteract(); // Ready for item pickup
void OnOpenInventory(); // Ready for UI toggle
```

---

## Quick Start: Phase 2 Day 1

### Step 1: Create Inventory Component Header
```cpp
// Public/Components/InventoryComponent.h
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();
    
    // Add item to inventory
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(UItemBase* Item);
    
    // Remove item from inventory
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(UItemBase* Item, int32 Quantity = 1);
    
    // Get all items
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<UItemBase*> GetItems() const { return Items; }
    
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<UItemBase*> Items;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxCapacity = 50;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    float MaxWeight = 100.0f;
};
```

### Step 2: Implement AddItem Logic
- Check capacity
- Check weight
- Handle stacking (if same item type and stackable)
- Add to array
- Broadcast events

### Step 3: Attach to Player Character
- Add component in PlayerCharacter constructor
- Or add via Blueprint
- Test AddItem in Blueprint

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

### Phase 1 Documentation
- `Phase1_Implementation_Plan.md` - Master plan
- `Phase1_Completion_Summary.md` - Phase 1 summary
- `Architecture_Design_Plan.md` - Full architecture

### UE 5.7 Documentation
- [Inventory System Best Practices](https://docs.unrealengine.com/5.7/en-US/)
- [Widget System](https://docs.unrealengine.com/5.7/en-US/umg-ui-designer-for-unreal-engine/)
- [Drag and Drop in UMG](https://docs.unrealengine.com/5.7/en-US/drag-and-drop-in-umg/)

### Phase 2 Files to Create
- `Source/ActionRPG/Public/Components/InventoryComponent.h`
- `Source/ActionRPG/Public/Items/ItemPickupActor.h`
- `Source/ActionRPG/Public/UI/Inventory/InventoryWidget.h`
- `Source/ActionRPG/Public/UI/Inventory/InventorySlotWidget.h`

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
