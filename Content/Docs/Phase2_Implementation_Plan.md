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
- Implement drag and drop functionality with stack splitting
- Implement world item dropping (inventory → world)
- Create quick-use bar system (10 slots: 1-8 for skills, 9-10 for consumables)
- Implement hotkey support for consumable quick-use slots (9-0 keys)
- Create item usage system integration
- Test complete inventory flow (pickup → inventory → use → drop → pickup)

### Phase 2 Deliverables
- ✅ InventoryComponent class (C++ and Blueprint-ready)
- ✅ ItemPickupActor class (C++ and Blueprint-ready)
- ✅ InventoryWidget and InventorySlotWidget (UMG widgets)
- ✅ Drag and drop system (ItemDragDropOperation)
- ✅ Stack splitting functionality (Ctrl+drag or right-click drag)
- ✅ World item dropping (items spawn as ItemPickupActor in world)
- ✅ Quick-use bar system (10 slots: 1-8 for skills, 9-10 for consumables)
- ✅ Hotkey support for consumable quick-use slots (9-0 keys)
- ✅ Item usage integration with character
- ✅ Complete inventory UI with visual feedback
- ✅ Item tooltip system
- ✅ Sample item pickups in test level

### Success Metrics
- InventoryComponent compiles and attaches to player character
- Items can be added to and removed from inventory
- Item stacking works correctly
- Stack splitting works (Ctrl+drag or right-click drag)
- Items can be dropped from inventory to world (spawn as ItemPickupActor)
- Dropped items can be picked up again
- Item pickups spawn in world and can be collected
- Inventory UI displays correctly and responds to input
- Drag and drop between slots works
- Drag and drop to/from quick-use slots works
- Quick-use bar displays with 10 slots (9-10 functional for consumables)
- Hotkeys 9-0 activate consumable quick-use slots
- Items can be used from inventory and quick-use slots
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
1. **Enhance UseItem in InventoryComponent**
   - ✅ Basic UseItem implementation exists (calls ItemBase::Use())
   - Enhance item consumption logic
   - Add support for different item types (Consumable, Equipment, SkillItem, etc.)
   - Handle quantity management properly
   - Remove item when quantity reaches 0
   - Add proper error handling and logging

2. **Add Item Usage Validation**
   - ✅ Basic validation exists (CanUse() check)
   - Enhance validation for different item types
   - Check quantity > 0 before use
   - Add item type-specific requirements
   - Handle usage failures gracefully with user feedback
   - Add validation for equipment items (can't use, must equip)

3. **Connect to Input System**
   - ✅ OnInteract() implemented (for pickup)
   - Add quick-use item functionality (optional for Phase 2)
   - Connect inventory UI to UseItem (when UI is implemented)
   - Add right-click use item support
   - Prepare for hotkey support (Phase 3)

4. **Create Item Usage Feedback**
   - ✅ Logging exists (console debug)
   - Add visual feedback (particles, UI updates)
   - Add audio feedback (sound effects)
   - Connect consumables to character stats (health/mana/stamina)
   - Add usage success/failure messages

5. **Implement Item Type-Specific Behavior**
   - Consumable items: Reduce quantity, apply effects, remove when quantity = 0
   - Equipment items: Mark as "equipable" (equip system in Phase 3)
   - Skill items: Prepare for skill granting (Phase 3)
   - Misc items: Basic use with no consumption

6. **Test Item Usage**
   - Test consumable item usage
   - Test item quantity decrease
   - Test item removal when quantity = 0
   - Test multiple item types
   - Test edge cases (empty inventory, invalid slots, etc.)

#### Deliverables
- Enhanced UseItem method with proper item type handling
- Item consumption working correctly
- Input integration complete (OnInteract for pickup)
- Visual/audio feedback system ready
- Item usage tested with multiple item types
- Proper validation and error handling

---

### Day 21: Integration Testing (Inventory Core)

#### Tasks
1. **Test Complete Flow**
   - ✅ Spawn ItemPickupActor in test level
   - ✅ Test manual interaction (IA_Interact) for pickup
   - ✅ Verify item added to inventory correctly
   - ✅ Check inventory debug reports show items
   - ✅ Test item usage from inventory
   - ✅ Verify consumable items are consumed
   - ✅ Verify health potion heals player
   - ✅ Verify health potion cannot be used at max health
   - ✅ Test item removal when quantity = 0

2. **Test Edge Cases**
   - Full inventory pickup attempt (should fail gracefully)
   - Stacking with max stack size (should split to new slot)
   - Weight limit reached (should prevent pickup)
   - Multiple pickups simultaneously (should handle correctly)
   - Item usage with quantity = 0 (should fail gracefully)
   - Using health potion at max health (should be prevented)
   - Invalid slot access (should return null/fail)
   - Empty slot operations (should fail gracefully)

3. **Test Item Type Handling**
   - Consumable items (should consume on use)
   - Equipment items (should not consume, prepare for Phase 3)
   - Skill items (should not consume, prepare for Phase 3)
   - Misc items (should not consume)

4. **Debug and Fix Issues**
   - Check Output Log for errors and warnings
   - Verify event bindings are working
   - Check inventory debug reports for consistency
   - Verify item persistence (items not disappearing)
   - Fix any memory leaks or GC issues
   - Verify all logging is working correctly

5. **Performance Testing**
   - Test with full inventory (50 slots)
   - Test rapid item pickup/usage
   - Check for frame drops during inventory operations
   - Verify no memory leaks during extended play
   - Test inventory debug reporting performance

6. **Integration Verification**
   - Verify InventoryComponent ↔ ItemPickupActor integration
   - Verify InventoryComponent ↔ PlayerCharacter integration
   - Verify ItemDatabase ↔ InventoryComponent integration
   - Verify event system (OnItemUsed, OnInventoryChanged, etc.)
   - Verify health system integration with item usage

7. **Documentation and Cleanup**
   - Document any issues found and solutions
   - Update code comments if needed
   - Remove excessive debug logs (keep important ones)
   - Create test checklist for future reference
   - Prepare summary for Phase 2 completion

#### Deliverables
- ✅ Complete inventory flow working (pickup → inventory → use)
- ✅ All edge cases tested and handled correctly
- ✅ All bugs identified and fixed
- ✅ Performance verified (no significant issues)
- ✅ Integration between all systems verified
- ✅ Health system working with consumables
- ✅ Debug reporting functional
- ✅ Code cleaned and documented
- ✅ Ready for Phase 2 UI implementation (Days 22-23)

---

## Week 2 Breakdown

### Day 22-23: Inventory UI Foundation

#### Overview
Create the foundation for the inventory UI system. This includes creating C++ widget classes, designing the UI layout in Blueprint, implementing widget communication with the InventoryComponent, and adding the inventory toggle functionality.

#### Tasks

**Day 22: C++ Widget Classes and Structure**

1. **Create UI Widget Structure**
   - Create `Source/ActionRPG/Public/UI/Inventory/` folder
   - Create `Source/ActionRPG/Private/UI/Inventory/` folder
   - Create `Content/UI/Inventory/` folder in Content Browser
   - Create InventoryWidget class (C++) inheriting from UUserWidget
   - Create InventorySlotWidget class (C++) inheriting from UUserWidget

2. **Implement InventoryWidget Header (C++)**
   - Add widget references (UniformGridPanel, TextBlocks for weight/capacity)
   - Add TSubclassOf<UInventorySlotWidget> for slot widget class
   - Add TArray<UInventorySlotWidget*> to store slot widgets
   - Add TObjectPtr<UInventoryComponent> reference
   - Declare NativeConstruct/NativeDestruct overrides
   - Declare UpdateInventoryDisplay() method
   - Declare OnInventorySlotClicked() and OnInventorySlotRightClicked() methods
   - Declare InitializeSlots() and RefreshSlot() helper methods

3. **Implement InventoryWidget Implementation (C++)**
   - NativeConstruct: Get InventoryComponent from PlayerCharacter
   - NativeConstruct: Bind to InventoryComponent events (OnInventoryChanged, OnItemAdded, OnItemRemoved)
   - NativeConstruct: Initialize slot widgets array
   - NativeDestruct: Unbind from events
   - UpdateInventoryDisplay(): Refresh all slots and weight/capacity displays
   - InitializeSlots(): Create slot widgets and add to grid (10x5 = 50 slots)
   - RefreshSlot(): Update individual slot widget with item data
   - OnInventorySlotClicked(): Handle left-click (prepare for drag and drop in Day 24)
   - OnInventorySlotRightClicked(): Handle right-click (use item)

4. **Implement InventorySlotWidget Header (C++)**
   - Add widget references (Image for icon, TextBlock for quantity, Border for background)
   - Add slot index property (int32)
   - Add TObjectPtr<UItemBase> for current item
   - Declare SetSlotData() and ClearSlot() methods
   - Declare UpdateSlotVisuals() helper method
   - Declare NativeOnMouseButtonDown() override for click detection
   - Add hover detection (OnMouseEnter, OnMouseLeave) for future tooltip

5. **Implement InventorySlotWidget Implementation (C++)**
   - NativeConstruct: Initialize default empty state
   - SetSlotData(): Set slot index, item, and quantity; update visuals
   - ClearSlot(): Clear item data and reset to empty state
   - UpdateSlotVisuals(): Update icon, quantity text, and border based on item data
   - NativeOnMouseButtonDown(): Detect left/right click and call parent widget handlers

**Day 23: Blueprint Widgets and Integration**

6. **Create Blueprint Widget Classes**
   - Create WBP_InventoryWidget (Blueprint) based on InventoryWidget C++ class
   - Create WBP_InventorySlotWidget (Blueprint) based on InventorySlotWidget C++ class
   - Design inventory widget layout in UMG:
     - Background panel/border
     - UniformGridPanel (10 columns x 5 rows = 50 slots)
     - TextBlocks for "Weight: X / Y" and "Capacity: X / Y"
     - Close button
     - Title text ("Inventory")

7. **Design InventorySlotWidget Layout (UMG)**
   - Border widget as base (slot background)
   - Image widget for item icon (centered, 64x64 recommended)
   - TextBlock for quantity (bottom-right corner, bold font)
   - Optional: Border overlay for hover/selected states (initially hidden)

8. **Bind Widget References in Blueprint**
   - In WBP_InventoryWidget: Bind all widget references using BindWidget meta
   - Set InventoryGrid reference
   - Set WeightText and CapacityText references
   - Set CloseButton reference
   - Set SlotWidgetClass to WBP_InventorySlotWidget

9. **Implement Widget Communication**
   - In InventoryWidget::NativeConstruct: Get PlayerCharacter and InventoryComponent
   - Bind to OnInventoryChanged event: Call UpdateInventoryDisplay()
   - Bind to OnItemAdded event: Call RefreshSlot() for affected slot
   - Bind to OnItemRemoved event: Call RefreshSlot() for affected slot
   - Update weight/capacity text when inventory changes
   - Handle Close button: Hide inventory widget and restore game input

10. **Implement Inventory Toggle**
    - Complete OnOpenInventory() in PlayerController C++:
      - Get InventoryWidget from HUD or PlayerController
      - Toggle widget visibility
      - Set input mode: GameAndUI when open, GameOnly when closed
      - Optionally pause game (SetPause(true/false))
      - Show/hide mouse cursor
    - Create or update BP_ActionRPGHUD:
      - Add InventoryWidget reference as property
      - Create widget in BeginPlay
      - Initially hidden
    - Or add widget directly to PlayerController (simpler approach)

11. **Test Inventory UI**
    - Test widget creation and display
    - Test inventory toggle (I key or configured input)
    - Test slot widget creation (50 slots should appear)
    - Test weight/capacity display updates
    - Test slot updates when items added/removed
    - Test visual feedback (icons, quantities)

#### Code Structure Examples

**InventoryWidget.h Key Features:**
```cpp
// Widget References (BindWidget)
UPROPERTY(meta = (BindWidget))
class UUniformGridPanel* InventoryGrid;

UPROPERTY(meta = (BindWidget))
class UTextBlock* WeightText;

UPROPERTY(meta = (BindWidget))
class UTextBlock* CapacityText;

UPROPERTY(meta = (BindWidget))
class UButton* CloseButton;

// Slot Widget Class
UPROPERTY(EditDefaultsOnly, Category = "Inventory UI")
TSubclassOf<class UInventorySlotWidget> SlotWidgetClass;

// Runtime Data
UPROPERTY()
TArray<UInventorySlotWidget*> SlotWidgets;

UPROPERTY()
TObjectPtr<class UInventoryComponent> InventoryComponent;
```

**InventorySlotWidget.h Key Features:**
```cpp
// Widget References (BindWidget)
UPROPERTY(meta = (BindWidget))
class UImage* ItemIcon;

UPROPERTY(meta = (BindWidget))
class UTextBlock* QuantityText;

UPROPERTY(meta = (BindWidget))
class UBorder* SlotBorder;

// Runtime Data
UPROPERTY()
int32 SlotIndex;

UPROPERTY()
TObjectPtr<UItemBase> CurrentItem;
```

#### Deliverables
- ✅ InventoryWidget C++ class created and compiled
- ✅ InventorySlotWidget C++ class created and compiled
- ✅ WBP_InventoryWidget Blueprint created and designed
- ✅ WBP_InventorySlotWidget Blueprint created and designed
- ✅ UI layout functional (50 slots in grid)
- ✅ Widget references bound correctly
- ✅ Inventory toggle working (OnOpenInventory implemented)
- ✅ Widget communication with InventoryComponent working
- ✅ Weight/capacity display updating correctly
- ✅ Slot widgets updating when inventory changes
- ✅ Visual feedback working (icons, quantities display)
- ✅ Ready for drag and drop implementation (Day 24)

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
   - Prepare for stack splitting (basic structure, full implementation in Days 26-27)

#### Deliverables
- ItemDragDropOperation class created
- Drag detection working
- Drop logic implemented
- Visual feedback functional
- Special cases handled (move, swap, stack)
- Basic structure for stack splitting (full implementation in Days 26-27)

---

### Day 26-27: UI Polish & Advanced Features

#### Overview
Enhance the inventory system with advanced features including item tooltips, quick-use bar for consumables, world item dropping, stack splitting, and UI polish. The quick-use bar will support 10 slots: slots 1-8 for skills (Phase 3) and slots 9-10 for consumable items.

#### Tasks

**Day 26: Stack Splitting & World Item Dropping**

1. **Implement Stack Splitting**
   - Add split stack UI dialog/popup
   - Detect right-click drag or Ctrl+drag for split mode
   - Update ItemDragDropOperation to support split operations
   - Add `SplitStack(int32 SlotIndex, int32 SplitQuantity)` method to InventoryComponent
   - Validate split quantity (must be > 0 and < current quantity)
   - Create new item instance with split quantity
   - Update source slot quantity (reduce by split amount)
   - Handle split on drag: when dragging with split mode, only move split quantity
   - Visual feedback for split mode (different drag preview or cursor)
   - Update NativeOnDragDetected to detect Ctrl+drag or right-click drag
   - Set `bIsSplitOperation = true` in ItemDragDropOperation when splitting

2. **Implement World Item Dropping**
   - Add "Drop Item" functionality when dragging outside inventory UI
   - Detect drag drop outside inventory widget bounds
   - Create DropItemActor class or reuse ItemPickupActor for dropped items
   - On drop outside UI: Spawn ItemPickupActor at player location or cursor world position
   - Remove item from inventory when dropped to world
   - Handle quantity: drop entire stack or split quantity based on drag operation
   - Visual feedback: show drop preview/preview actor position
   - Add cancel mechanism: if drop is cancelled, item stays in inventory
   - Handle inventory full scenario when trying to pick up dropped item
   - Test dropped items persist in world and can be picked up again

3. **Enhance Drag and Drop for Split Operations**
   - Update NativeOnMouseButtonDown to detect Ctrl+LeftClick or RightClick drag
   - Show split quantity dialog when initiating split drag
   - Update drag operation to store split quantity vs full quantity
   - Update drop logic to handle split quantities correctly
   - Visual indicator when in split mode (different drag preview, cursor change)

**Day 27: Quick-Use Bar & UI Polish**

4. **Create Quick-Use Bar System**
   - Create QuickUseBarWidget class (C++ and Blueprint)
   - Design UI layout: 10 slots in horizontal bar
     - Slots 1-8: Skill slots (for Phase 3, initially empty/disabled)
     - Slots 9-10: Consumable item slots (active in Phase 2)
   - Create QuickUseSlotWidget class for individual slots
   - Visual distinction between skill slots and consumable slots
   - Display item icons and quantity for consumable slots
   - Show hotkey labels (1-8 for skills, 9-0 for consumables)
   - Position quick-use bar at bottom center of screen (always visible)
   - Make quick-use bar always on top (high Z-order)

5. **Implement Quick-Use Bar Functionality**
   - Add `QuickUseSlots` array to InventoryComponent (10 slots, TArray<FQuickUseSlot>)
   - Create FQuickUseSlot structure:
     ```cpp
     USTRUCT(BlueprintType)
     struct FQuickUseSlot
     {
         UPROPERTY(BlueprintReadWrite)
         TObjectPtr<UItemBase> Item;
         
         UPROPERTY(BlueprintReadWrite)
         int32 InventorySlotIndex; // Reference to inventory slot
         
         UPROPERTY(BlueprintReadWrite)
         EQuickUseSlotType SlotType; // Skill or Consumable
     };
     ```
   - Add methods to InventoryComponent:
     - `AssignItemToQuickUseSlot(int32 InventorySlotIndex, int32 QuickUseSlotIndex)`
     - `UseQuickUseSlot(int32 QuickUseSlotIndex)` - Use item from quick-use slot
     - `ClearQuickUseSlot(int32 QuickUseSlotIndex)`
     - `GetQuickUseSlot(int32 QuickUseSlotIndex)` - Get item in quick-use slot
   - Support drag and drop from inventory to quick-use slots (slots 9-10 only)
   - Support drag and drop from quick-use slots back to inventory
   - Update inventory slot display when item is assigned to quick-use slot (optional visual indicator)
   - Remove item from quick-use slot if removed from inventory

6. **Implement Hotkey Support for Quick-Use Bar**
   - Add Input Actions for consumable hotkeys:
     - `IA_QuickUseSlot9` (Hotkey: 9)
     - `IA_QuickUseSlot10` (Hotkey: 0)
   - Add Input Actions for skill hotkeys (prepare for Phase 3):
     - `IA_QuickUseSlot1` through `IA_QuickUseSlot8` (Hotkeys: 1-8)
   - Implement OnQuickUseSlot1-10 handlers in PlayerController
   - Connect hotkeys to InventoryComponent::UseQuickUseSlot()
   - Add visual feedback when hotkey pressed (slot highlight, cooldown indicator)
   - Handle quick-use slot empty (no action, optional error sound/message)
   - Handle item consumption from quick-use slot (reduce quantity, remove if empty)
   - Auto-remove from quick-use slot if item quantity reaches 0

7. **Add Item Tooltip**
   - Create ItemTooltipWidget class
   - Display item name, description, stats
   - Show on hover over inventory slot and quick-use slot
   - Position tooltip near cursor (avoid screen edges)
   - Fade in/out animation
   - Show item type, rarity, weight, stack info
   - Show hotkey assignment if item is in quick-use slot

8. **Add Item Details Panel** (Optional, can be simplified)
   - Side panel showing selected item details (when item is clicked)
   - Full item description
   - Item stats (damage, healing, etc.)
   - Item type and rarity display
   - Use button in details panel
   - Assign to quick-use slot button (for consumables)

9. **Add Filter/Sort Functionality** (Optional, can be deferred)
   - Filter by item type (All, Consumable, Equipment, etc.)
   - Filter by rarity
   - Sort by name, type, rarity, weight
   - Search bar for item names (optional)

10. **Polish UI Visuals**
    - Add inventory background image/color
    - Style slots with borders and backgrounds
    - Add hover effects for inventory slots
    - Add click animations
    - Add open/close animations for inventory
    - Style quick-use bar with background and borders
    - Add hover effects for quick-use slots
    - Add visual feedback for hotkey presses
    - Add cooldown indicators (for future skill cooldowns in Phase 3)

#### Code Structure Examples

**Quick-Use Slot Structure:**
```cpp
// Public/Components/Inventory/InventoryComponent.h
UENUM(BlueprintType)
enum class EQuickUseSlotType : uint8
{
    Skill,      // Slots 1-8 (for Phase 3)
    Consumable  // Slots 9-10 (for Phase 2)
};

USTRUCT(BlueprintType)
struct ACTIONRPG_API FQuickUseSlot
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Quick Use")
    TObjectPtr<UItemBase> Item;

    UPROPERTY(BlueprintReadWrite, Category = "Quick Use")
    int32 InventorySlotIndex; // -1 if not assigned

    UPROPERTY(BlueprintReadWrite, Category = "Quick Use")
    EQuickUseSlotType SlotType;

    FQuickUseSlot()
        : Item(nullptr), InventorySlotIndex(-1), SlotType(EQuickUseSlotType::Consumable)
    {}
};
```

**Quick-Use Methods in InventoryComponent:**
```cpp
// Public/Components/Inventory/InventoryComponent.h
UFUNCTION(BlueprintCallable, Category = "Inventory|Quick Use")
bool AssignItemToQuickUseSlot(int32 InventorySlotIndex, int32 QuickUseSlotIndex);

UFUNCTION(BlueprintCallable, Category = "Inventory|Quick Use")
bool UseQuickUseSlot(int32 QuickUseSlotIndex);

UFUNCTION(BlueprintCallable, Category = "Inventory|Quick Use")
void ClearQuickUseSlot(int32 QuickUseSlotIndex);

UFUNCTION(BlueprintCallable, Category = "Inventory|Quick Use")
FQuickUseSlot GetQuickUseSlot(int32 QuickUseSlotIndex) const;

UFUNCTION(BlueprintCallable, Category = "Inventory")
bool SplitStack(int32 SlotIndex, int32 SplitQuantity);
```

**World Item Drop Method:**
```cpp
// Public/Components/Inventory/InventoryComponent.h
UFUNCTION(BlueprintCallable, Category = "Inventory")
bool DropItemToWorld(int32 SlotIndex, int32 Quantity, const FVector& WorldLocation);
```

#### Deliverables
- ✅ Stack splitting functional (Ctrl+drag or right-click drag)
- ✅ World item dropping implemented (items spawn as ItemPickupActor)
- ✅ Quick-use bar created with 10 slots (1-8 for skills, 9-10 for consumables)
- ✅ Quick-use slots 9-10 functional for consumables
- ✅ Hotkeys 9-0 working for consumable quick-use slots
- ✅ Hotkeys 1-8 prepared for skills (Phase 3)
- ✅ Quick-use bar always visible at bottom of screen
- ✅ Drag and drop to/from quick-use slots working
- ✅ Item tooltip functional
- ✅ Item details panel working (optional)
- ✅ UI polished and visually appealing
- ✅ Filter/sort features added (optional, can be deferred)

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

    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    int32 SourceQuantity; // Full quantity in source slot

    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    bool bIsSplitOperation; // True if this is a stack split operation
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

#### Day 26-27: UI Polish & Advanced Features
- [ ] Stack splitting works (Ctrl+drag or right-click drag)
- [ ] Split quantity dialog works correctly
- [ ] World item dropping works (items spawn in world)
- [ ] Dropped items can be picked up again
- [ ] Quick-use bar displays correctly (10 slots)
- [ ] Quick-use slots 9-10 accept consumable items
- [ ] Quick-use slots 1-8 prepared for skills (Phase 3)
- [ ] Hotkeys 9-0 work for consumable quick-use slots
- [ ] Drag and drop to quick-use slots works
- [ ] Drag and drop from quick-use slots back to inventory works
- [ ] Quick-use items are consumed when hotkey pressed
- [ ] Quick-use slots update when items removed from inventory
- [ ] Tooltip displays on hover (inventory and quick-use slots)
- [ ] Item details panel works (optional)
- [ ] Filter/sort features work (optional)
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
- ✅ Stack splitting functional (Ctrl+drag or right-click drag)
- ✅ World item dropping implemented (items spawn in world)
- ✅ Quick-use bar with slots 9-10 for consumables functional
- ✅ Hotkeys 9-0 working for consumable quick-use slots
- ✅ Drag and drop to/from quick-use slots working

### Nice to Have (Can Defer)
- Item tooltip system (highly recommended but can be basic)
- Item details panel (can be simplified or deferred)
- Quick-use slots 1-8 for skills (prepared for Phase 3)
- Filter/sort functionality (optional enhancement)
- Advanced UI animations (basic animations sufficient)
- Split stack dialog/popup (basic implementation acceptable)

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

### Issue: Stack Splitting Not Working
**Symptoms:** Can't split stacks or split doesn't work correctly
**Solutions:**
- Verify Ctrl+drag or right-click drag detection is working
- Check SplitStack method in InventoryComponent
- Verify split quantity validation (must be > 0 and < current quantity)
- Check ItemDragDropOperation has bIsSplitOperation flag set
- Verify new item instance is created correctly
- Check source slot quantity is updated correctly

### Issue: World Item Drop Not Working
**Symptoms:** Items don't spawn in world or can't be dropped
**Solutions:**
- Verify DropItemToWorld method is implemented
- Check ItemPickupActor spawning logic
- Verify world location calculation (player position or cursor world position)
- Check item removal from inventory after drop
- Verify dropped items can be picked up again
- Check inventory full scenario when picking up dropped item

### Issue: Quick-Use Bar Not Working
**Symptoms:** Quick-use slots don't accept items or hotkeys don't work
**Solutions:**
- Verify QuickUseBarWidget is created and visible
- Check quick-use slots array is initialized (10 slots)
- Verify drag and drop to quick-use slots (only slots 9-10 for consumables)
- Check Input Actions for hotkeys 9-0 are set up correctly
- Verify OnQuickUseSlot9/10 handlers are bound in PlayerController
- Check UseQuickUseSlot method in InventoryComponent
- Verify quick-use slot updates when inventory changes

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
