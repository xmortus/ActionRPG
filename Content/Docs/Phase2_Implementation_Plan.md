# Phase 2: Inventory System - Implementation Plan
**Duration:** Week 3-4 (14 days)  
**Based on:** Architecture & Design Plan v1.1  
**Last Updated:** 2025-01-07

---

## Table of Contents
1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Week 1 Breakdown](#week-1-breakdown)
4. [Week 2 Breakdown](#week-2-breakdown)
5. [Detailed Implementation Steps](#detailed-implementation-steps)
6. [Testing Checklist](#testing-checklist)
7. [Success Criteria](#success-criteria)
8. [Common Issues & Solutions](#common-issues--solutions)

---

## Overview

### Phase 2 Goals
- Implement InventoryComponent with item storage and management
- Create ItemPickupActor for world item pickups
- Build inventory UI system (InventoryWidget, InventorySlotWidget)
- Implement drag and drop functionality
- Create item usage system integration
- Test complete inventory flow (pickup → inventory → use)

### Phase 2 Deliverables
- ✅ InventoryComponent class (C++ and Blueprint-ready)
- ✅ ItemPickupActor class (C++ and Blueprint-ready)
- ✅ InventoryWidget and InventorySlotWidget (UMG widgets)
- ✅ Drag and drop system (ItemDragDropOperation)
- ✅ Item usage integration with character
- ✅ Complete inventory UI with visual feedback
- ✅ Sample item pickups in test level

### Success Metrics
- InventoryComponent compiles and attaches to player character
- Items can be added to and removed from inventory
- Item stacking works correctly
- Item pickups spawn in world and can be collected
- Inventory UI displays correctly and responds to input
- Drag and drop between slots works
- Items can be used from inventory
- Weight/capacity limits function correctly

---

## Prerequisites

### Required Knowledge
- Unreal Engine 5.7 component system
- UMG Widget system basics
- Drag and drop in UMG
- Actor spawning and overlap events
- Blueprint event communication

### Required Tools
- Unreal Engine 5.7 installed
- Visual Studio 2022 (or compatible IDE)
- Phase 1 complete (all Phase 1 deliverables verified)

### Pre-Phase Setup
- ✅ Phase 1 complete (verified in Phase1_Completion_Summary.md)
- ✅ ItemDatabase working and initialized
- ✅ ItemBase and ItemDataAsset classes functional
- ✅ PlayerCharacter class ready for component attachment
- ✅ Input system working (IA_Interact, IA_OpenInventory)
- ✅ Test Item Data Assets created (HealthPotion, etc.)
- ✅ Project compiles and runs in editor

### Phase 1 Foundations Available
- ✅ `UItemBase` - Base item class with Use() and CanUse()
- ✅ `UItemDataAsset` - Item data definitions
- ✅ `UItemDatabase` - Singleton for item lookup
- ✅ `AActionRPGPlayerCharacter` - Character ready for components
- ✅ `AActionRPGPlayerController` - Input handling ready
- ✅ Enhanced Input System with IA_Interact and IA_OpenInventory

---

## Week 1 Breakdown

### Day 15-16: Inventory Component Foundation

#### Tasks
1. **Create Folder Structure**
   - Create `Source/ActionRPG/Public/Components/Inventory/`
   - Create `Source/ActionRPG/Private/Components/Inventory/`
   - Create `Content/Blueprints/Components/Inventory/`

2. **Create InventoryComponent Header**
   - Inherit from `UActorComponent`
   - Add inventory slot structure (FInventorySlot)
   - Add item storage (TArray<FInventorySlot>)
   - Add capacity and weight properties
   - Add events/delegates for inventory changes

3. **Implement InventoryComponent Core Methods**
   - `AddItem(UItemBase* Item)` - Add item with stacking logic
   - `RemoveItem(int32 SlotIndex, int32 Quantity)` - Remove item from slot
   - `MoveItem(int32 FromSlot, int32 ToSlot)` - Move item between slots
   - `SwapItems(int32 SlotA, int32 SlotB)` - Swap two items
   - `UseItem(int32 SlotIndex)` - Use item at slot
   - `GetItemAt(int32 SlotIndex)` - Get item reference
   - `FindItemSlot(const FName& ItemID)` - Find slot containing item
   - `HasSpaceFor(UItemBase* Item)` - Check if inventory has space
   - `CalculateTotalWeight()` - Calculate current inventory weight
   - `GetTotalItemCount()` - Get total number of items (including stacks)

4. **Add Item Stacking Logic**
   - Check if item can stack with existing items
   - Merge stacks when possible
   - Split stacks when needed
   - Validate MaxStackSize from ItemDataAsset

5. **Attach to Player Character**
   - Add component in PlayerCharacter constructor or Blueprint
   - Test component attachment

#### Deliverables
- InventoryComponent class created
- Core methods implemented
- Item stacking logic functional
- Component attached to PlayerCharacter
- Blueprint class created

---

### Day 17-18: Item Pickup Actor

#### Tasks
1. **Create ItemPickupActor Header**
   - Inherit from `AActor`
   - Add ItemDataAsset reference
   - Add quantity property
   - Add visual representation (StaticMeshComponent or MeshComponent)
   - Add collision/sphere component for overlap detection

2. **Implement Pickup Logic**
   - `OnPlayerOverlap()` - Handle player overlap event
   - `CanPickup(AActionRPGPlayerCharacter* Player)` - Validation check
   - `PickupItem(AActionRPGPlayerCharacter* Player)` - Execute pickup
   - `SpawnPickupEffect()` - Visual/audio feedback
   - `DestroyPickup()` - Cleanup after pickup

3. **Add Visual Representation**
   - StaticMeshComponent for item mesh
   - Material instance for item glow/rarity color
   - Particle system for pickup feedback
   - Rotating animation (optional)

4. **Create Blueprint Class**
   - Create BP_ItemPickup
   - Configure mesh and materials
   - Set up collision settings
   - Test pickup in-game

5. **Integrate with Inventory Component**
   - OnPickup calls InventoryComponent::AddItem()
   - Validate inventory space before pickup
   - Handle full inventory scenarios

#### Deliverables
- ItemPickupActor class created
- Pickup logic implemented
- Visual representation working
- Blueprint class created and configured
- Integration with InventoryComponent tested

---

### Day 19-20: Item Usage System

#### Tasks
1. **Implement UseItem in InventoryComponent**
   - Call ItemBase::Use() method
   - Handle item consumption (remove if consumable)
   - Handle equipment items (equip/unequip)
   - Handle skill items (grant skills - Phase 3)
   - Update quantity after use
   - Remove item if quantity reaches 0

2. **Add Item Usage Validation**
   - Check CanUse() before using
   - Check quantity > 0
   - Check item type-specific requirements
   - Handle usage failures gracefully

3. **Connect to Input System**
   - Implement OnInteract() in PlayerController (for pickup)
   - Implement OnUseItem() handler (for quick-use)
   - Connect inventory UI to UseItem
   - Add hotkey support for quick-use slots (future)

4. **Create Item Usage Feedback**
   - Log item usage to console (debug)
   - Visual feedback (particles, UI updates)
   - Audio feedback (sound effects)
   - Health/mana/stamina changes (for consumables)

5. **Test Item Usage**
   - Test consumable item usage
   - Test item quantity decrease
   - Test item removal when quantity = 0
   - Test multiple item types

#### Deliverables
- UseItem method fully implemented
- Item consumption working
- Input integration complete
- Visual/audio feedback added
- Item usage tested with multiple item types

---

### Day 21: Integration Testing (Inventory Core)

#### Tasks
1. **Test Complete Flow**
   - Spawn ItemPickupActor in test level
   - Player overlaps pickup
   - Item added to inventory
   - Inventory UI shows new item
   - Player uses item from inventory
   - Item consumed correctly

2. **Test Edge Cases**
   - Full inventory pickup attempt
   - Stacking with max stack size
   - Weight limit reached
   - Multiple pickups simultaneously
   - Item usage with quantity = 0

3. **Fix Any Issues**
   - Debug pickup integration
   - Debug inventory display
   - Debug item usage
   - Performance optimization

#### Deliverables
- Complete inventory flow working
- Edge cases handled
- Bugs fixed
- Performance verified

---

## Week 2 Breakdown

### Day 22-23: Inventory UI Foundation

#### Tasks
1. **Create UI Widget Structure**
   - Create `Source/ActionRPG/Public/UI/Inventory/`
   - Create `Content/UI/Inventory/`
   - Create InventoryWidget class (C++)
   - Create InventorySlotWidget class (C++)

2. **Design Inventory Widget Layout**
   - Grid panel for inventory slots (default 10x5 = 50 slots)
   - Item icon display
   - Quantity text overlay
   - Weight/capacity display
   - Close button

3. **Create InventorySlotWidget**
   - Image widget for item icon
   - Text widget for quantity
   - Border widget for slot background
   - Highlight border for selected/hovered slots
   - Click and right-click handlers

4. **Implement Widget Communication**
   - InventoryWidget gets inventory reference from PlayerCharacter
   - Bind to InventoryComponent events (OnItemAdded, OnItemRemoved, etc.)
   - Update slot widgets when inventory changes
   - Refresh display on inventory change events

5. **Add Inventory Toggle**
   - Implement OnOpenInventory() in PlayerController
   - Toggle inventory widget visibility
   - Pause game when inventory open (optional)
   - Update input mode (GameAndUI vs GameOnly)

#### Deliverables
- InventoryWidget class created
- InventorySlotWidget class created
- UI layout designed and functional
- Inventory toggle working
- Widget communication implemented

---

### Day 24-25: Drag and Drop System

#### Tasks
1. **Create ItemDragDropOperation**
   - Inherit from `UDragDropOperation`
   - Store source slot index
   - Store item reference
   - Create drag preview widget
   - Visual feedback during drag

2. **Implement Drag Detection**
   - Override OnDragDetected in InventorySlotWidget
   - Create ItemDragDropOperation
   - Return drag operation to UMG system
   - Show drag preview

3. **Implement Drop Logic**
   - Override OnDrop in InventorySlotWidget
   - Validate drop target
   - Call InventoryComponent::MoveItem() or SwapItems()
   - Update UI after drop
   - Handle invalid drops (return item to source)

4. **Add Visual Feedback**
   - Highlight valid drop targets
   - Show invalid drop feedback (red highlight)
   - Drag preview follows mouse cursor
   - Slot highlight on hover during drag

5. **Handle Special Cases**
   - Drop on same slot (do nothing)
   - Drop on empty slot (move item)
   - Drop on item with same ID (stack if possible)
   - Drop on item with different ID (swap)
   - Partial stack splits (right-click drag for split)

#### Deliverables
- ItemDragDropOperation class created
- Drag detection working
- Drop logic implemented
- Visual feedback functional
- Special cases handled

---

### Day 26-27: UI Polish & Advanced Features

#### Tasks
1. **Add Item Tooltip**
   - Create ItemTooltipWidget class
   - Display item name, description, stats
   - Show on hover over inventory slot
   - Position tooltip near cursor
   - Fade in/out animation

2. **Add Item Details Panel**
   - Side panel showing selected item details
   - Full item description
   - Item stats (damage, healing, etc.)
   - Item type and rarity display
   - Use button in details panel

3. **Add Quick-Use Slots**
   - Horizontal bar of 10 quick-use slots
   - Items can be dragged to quick-use slots
   - Hotkey support (1-0 keys) for quick-use
   - Visual indicators for assigned items
   - Use items directly from quick-use slots

4. **Add Filter/Sort Functionality**
   - Filter by item type (All, Consumable, Equipment, etc.)
   - Filter by rarity
   - Sort by name, type, rarity, weight
   - Search bar for item names (optional)

5. **Polish UI Visuals**
   - Add inventory background image/color
   - Style slots with borders and backgrounds
   - Add hover effects
   - Add click animations
   - Add open/close animations

#### Deliverables
- Item tooltip functional
- Item details panel working
- Quick-use slots implemented
- Filter/sort features added
- UI polished and visually appealing

---

### Day 28: Final Integration & Testing

#### Tasks
1. **Complete Integration Testing**
   - Test all inventory features together
   - Test pickup → inventory → use flow
   - Test drag and drop in all scenarios
   - Test quick-use slots
   - Test UI polish features

2. **Performance Testing**
   - Verify no frame drops with full inventory
   - Test widget creation/destruction performance
   - Optimize slot widget updates
   - Cache references where possible

3. **Bug Fixes**
   - Fix any remaining bugs
   - Fix edge cases
   - Fix UI glitches
   - Fix drag and drop issues

4. **Code Cleanup**
   - Add code comments
   - Remove debug logs
   - Format code according to UE 5.7 standards
   - Verify naming conventions

5. **Documentation**
   - Document any deviations from plan
   - Create manual setup guide for Phase 2
   - Create progress report
   - Update architecture document if needed

#### Deliverables
- All systems integrated and working
- Performance optimized
- Bugs fixed
- Code cleaned and documented
- Phase 2 complete and verified

---

## Detailed Implementation Steps

### Step 1: Inventory Component Structure

#### FInventorySlot Structure
```cpp
// Public/Components/Inventory/InventoryComponent.h
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

#### InventoryComponent Header
```cpp
// Public/Components/Inventory/InventoryComponent.h
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    // Inventory Management
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(UItemBase* Item, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(int32 SlotIndex, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool MoveItem(int32 FromSlot, int32 ToSlot);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool SwapItems(int32 SlotA, int32 SlotB);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UseItem(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UItemBase* GetItemAt(int32 SlotIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 FindItemSlot(const FName& ItemID) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool HasSpaceFor(UItemBase* Item) const;

    // Getters
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetMaxCapacity() const { return MaxCapacity; }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    float GetMaxWeight() const { return MaxWeight; }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    float GetCurrentWeight() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetTotalItemCount() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FInventorySlot> GetInventorySlots() const { return InventorySlots; }

    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryChanged, int32, SlotIndex, UItemBase*, Item);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, UItemBase*, Item);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, UItemBase*, Item, int32, Quantity);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUsed, UItemBase*, Item);

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryChanged OnInventoryChanged;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnItemAdded OnItemAdded;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnItemRemoved OnItemRemoved;

    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnItemUsed OnItemUsed;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<FInventorySlot> InventorySlots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxCapacity = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    float MaxWeight = 100.0f;

private:
    bool TryStackItem(UItemBase* Item, int32 Quantity, int32& RemainingQuantity);
    int32 FindEmptySlot() const;
};
```

### Step 2: Item Pickup Actor

#### ItemPickupActor Header
```cpp
// Public/Items/Pickups/ItemPickupActor.h
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API AItemPickupActor : public AActor
{
    GENERATED_BODY()

public:
    AItemPickupActor();

    // Item Data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    TObjectPtr<UItemDataAsset> ItemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    int32 Quantity = 1;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> CollisionComponent;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                        bool bFromSweep, const FHitResult& SweepResult);

    void PickupItem(AActionRPGPlayerCharacter* Player);
    bool CanPickup(AActionRPGPlayerCharacter* Player) const;
    void SpawnPickupEffect();
};
```

### Step 3: Inventory Widget

#### InventoryWidget Header
```cpp
// Public/UI/Inventory/InventoryWidget.h
UCLASS()
class ACTIONRPG_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void UpdateInventoryDisplay();

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void OnInventorySlotClicked(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void OnInventorySlotRightClicked(int32 SlotIndex);

    // Widget References (bind in Blueprint)
    UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel* InventoryGrid;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* WeightText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* CapacityText;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI")
    TSubclassOf<class UInventorySlotWidget> SlotWidgetClass;

private:
    UPROPERTY()
    TArray<UInventorySlotWidget*> SlotWidgets;

    UPROPERTY()
    TObjectPtr<class UInventoryComponent> InventoryComponent;

    void InitializeSlots();
    void RefreshSlot(int32 SlotIndex);
};
```

#### InventorySlotWidget Header
```cpp
// Public/UI/Inventory/InventorySlotWidget.h
UCLASS()
class ACTIONRPG_API UInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
    void SetSlotData(int32 SlotIndex, UItemBase* Item, int32 Quantity);

    UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
    void ClearSlot();

    // Widget References (bind in Blueprint)
    UPROPERTY(meta = (BindWidget))
    class UImage* ItemIcon;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* QuantityText;

    UPROPERTY(meta = (BindWidget))
    class UBorder* SlotBorder;

    // Drag and Drop
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    int32 GetSlotIndex() const { return SlotIndex; }

private:
    UPROPERTY()
    int32 SlotIndex;

    UPROPERTY()
    TObjectPtr<UItemBase> CurrentItem;

    void UpdateSlotVisuals();
};
```

### Step 4: Drag and Drop Operation

#### ItemDragDropOperation Header
```cpp
// Public/UI/Inventory/ItemDragDropOperation.h
UCLASS()
class ACTIONRPG_API UItemDragDropOperation : public UDragDropOperation
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    int32 SourceSlotIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    TObjectPtr<UItemBase> Item;

    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    int32 Quantity;
};
```

---

## Testing Checklist

### Week 1 Testing

#### Day 15-16: Inventory Component
- [ ] InventoryComponent class compiles
- [ ] Component can be added to PlayerCharacter
- [ ] AddItem works correctly
- [ ] Item stacking works
- [ ] RemoveItem works correctly
- [ ] Weight calculation works
- [ ] Capacity limits enforced
- [ ] Events fire correctly

#### Day 17-18: Item Pickup
- [ ] ItemPickupActor class compiles
- [ ] Pickup spawns in world
- [ ] Overlap detection works
- [ ] Pickup adds item to inventory
- [ ] Visual feedback works
- [ ] Full inventory handled correctly
- [ ] Multiple pickups work

#### Day 19-20: Item Usage
- [ ] UseItem method works
- [ ] Consumable items consumed correctly
- [ ] Quantity decreases on use
- [ ] Item removed when quantity = 0
- [ ] Usage validation works
- [ ] Visual/audio feedback works

#### Day 21: Integration
- [ ] Complete flow works (pickup → inventory → use)
- [ ] Edge cases handled
- [ ] No crashes or errors

### Week 2 Testing

#### Day 22-23: Inventory UI
- [ ] InventoryWidget compiles
- [ ] InventorySlotWidget compiles
- [ ] UI displays correctly
- [ ] Slots update when inventory changes
- [ ] Inventory toggle works
- [ ] UI binds to inventory component

#### Day 24-25: Drag and Drop
- [ ] Drag detection works
- [ ] Drag preview displays
- [ ] Drop validation works
- [ ] MoveItem works via drag and drop
- [ ] SwapItems works via drag and drop
- [ ] Stacking works via drag and drop
- [ ] Visual feedback works

#### Day 26-27: UI Polish
- [ ] Tooltip displays on hover
- [ ] Item details panel works
- [ ] Quick-use slots work
- [ ] Filter/sort features work
- [ ] UI animations work
- [ ] Visual polish complete

#### Day 28: Final Testing
- [ ] All features work together
- [ ] Performance acceptable
- [ ] No bugs or crashes
- [ ] Code quality verified

---

## Success Criteria

### Must Have (Phase 2 Complete)
- ✅ InventoryComponent functional with all core methods
- ✅ Item stacking works correctly
- ✅ Weight/capacity limits enforced
- ✅ ItemPickupActor spawns and can be collected
- ✅ Inventory UI displays correctly
- ✅ Drag and drop works between slots
- ✅ Items can be used from inventory
- ✅ Complete flow tested (pickup → inventory → use)

### Nice to Have (Can Defer)
- Tooltip system
- Item details panel
- Quick-use slots
- Filter/sort functionality
- Advanced UI animations
- Split stack functionality (right-click drag)

---

## Common Issues & Solutions

### Issue: Inventory Component Not Attaching
**Symptoms:** Component doesn't appear on character
**Solutions:**
- Verify component is added in constructor or Blueprint
- Check component class name matches
- Verify BlueprintSpawnableComponent meta tag
- Recompile C++ code

### Issue: Items Not Adding to Inventory
**Symptoms:** AddItem returns false or items disappear
**Solutions:**
- Check inventory capacity/weight limits
- Verify ItemBase instance is valid
- Check stacking logic for existing items
- Add debug logs to trace flow

### Issue: Pickup Not Triggering
**Symptoms:** No overlap event fired
**Solutions:**
- Verify CollisionComponent settings (OverlapAllDynamic, etc.)
- Check CollisionComponent is registered
- Verify OnOverlapBegin is bound correctly
- Check actor collision settings

### Issue: UI Not Updating
**Symptoms:** Inventory display doesn't refresh
**Solutions:**
- Verify OnInventoryChanged event is bound
- Check RefreshSlot is called on changes
- Verify widget references are bound in Blueprint
- Check widget is visible and constructed

### Issue: Drag and Drop Not Working
**Symptoms:** Drag doesn't start or drop fails
**Solutions:**
- Verify NativeOnDragDetected is overridden correctly
- Check drag operation is created properly
- Verify OnDrop validation logic
- Check slot index is correct
- Verify MoveItem/SwapItems methods work

---

## Next Steps (Phase 3 Preview)

After Phase 2 completion, Phase 3 will focus on:
- SkillComponent and SkillManagerComponent implementation
- Skill bar UI system
- Skill activation from hotkeys
- Skill-granting items (SkillItem, SkillStone, BeastCore)

**Preparation for Phase 3:**
- Review Skill System Design section
- Prepare skill icons and effects
- Plan skill bar layout
- Review SkillBase and SkillDatabase from Phase 1

---

## Notes & Deviations

### Document Any:
- Changes from original plan
- Issues encountered
- Solutions found
- Decisions made
- Future considerations

---

**End of Phase 2 Implementation Plan**
