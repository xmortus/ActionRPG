# Phase 2 Manual Steps - Complete
**After Code Implementation**  
**Date:** 2025-01-07  
**Phase:** Phase 2 - Inventory System  
**Days:** 15-28 - Complete Manual Steps Guide

---

## Overview

This document consolidates all manual steps required in the Unreal Editor for Phase 2 of the Inventory System implementation. These steps must be performed after completing the C++ code implementation for each day.

**Note:** This is a consolidated guide. For detailed day-specific instructions, refer to the individual day sections below.

---

## Table of Contents

1. [Days 15-16: Inventory Component Foundation](#days-15-16-inventory-component-foundation)
2. [Days 17-18: Item Pickup Actor](#days-17-18-item-pickup-actor)
3. [Days 19-20: Item Usage System](#days-19-20-item-usage-system)
4. [Day 21: Integration Testing](#day-21-integration-testing)
5. [Days 22-23: Inventory UI Foundation](#days-22-23-inventory-ui-foundation)
6. [Days 24-25: Drag and Drop System](#days-24-25-drag-and-drop-system)
7. [Days 26-27: Stack Splitting, World Item Dropping, Quick-Use Bar & UI Polish](#days-26-27-stack-splitting-world-item-dropping-quick-use-bar--ui-polish)
8. [Day 28: Final Integration & Testing](#day-28-final-integration--testing)

---

## Days 15-16: Inventory Component Foundation

### Prerequisites

- ✅ All C++ code compiled successfully
- ✅ Unreal Editor is open
- ✅ Project is loaded in the editor
- ✅ No compilation errors in Output Log
- ✅ Visual Studio build completed successfully

### Step 1: Verify C++ Code Compiled

1. **Open Output Log**
   - Window → `Developer Tools` → `Output Log`
   - Or press `Ctrl + Shift + L`

2. **Check for Errors**
   - Look for any compilation errors
   - If errors exist, fix them in Visual Studio first

3. **Verify Hot Reload**
   - Look for message: `LogHotReload: Hot reload completed successfully`
   - If hot reload failed, close and reopen Unreal Editor

### Step 2: Create Folder Structure

1. **Navigate to Content Browser**
   - Go to `Content/` folder

2. **Create Blueprints/Components Folder**
   - Right-click in Content Browser
   - Select `New Folder`
   - Name it: `Components`
   - Location: `Content/Blueprints/Components/`

3. **Create Inventory Subfolder**
   - Right-click on `Components` folder
   - Select `New Folder`
   - Name it: `Inventory`
   - Location: `Content/Blueprints/Components/Inventory/`

### Step 3: Create Blueprint Class from InventoryComponent

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Components/Inventory/` folder

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - In the "Pick Parent Class" dialog:
     - Search for: `InventoryComponent`
     - Select `Inventory Component`
     - Click `Select`

3. **Name the Blueprint**
   - Name it: `BP_InventoryComponent`
   - Save it in `Content/Blueprints/Components/Inventory/`

4. **Open and Configure Blueprint**
   - Double-click `BP_InventoryComponent`
   - Check `Class Defaults` tab
   - Verify properties are visible:
     - `Inventory Slots` (Array)
     - `Max Capacity` (int32, default: 50)
     - `Max Weight` (float, default: 100.0)
     - Event delegates (On Inventory Changed, On Item Added, etc.)

5. **Compile and Save**
   - Click `Compile` button
   - Click `Save` button
   - Close Blueprint Editor

### Step 4: Attach Component to Player Character

1. **Open Player Character Blueprint**
   - Navigate to `Content/Blueprints/Characters/`
   - Double-click `BP_ActionRPGPlayerCharacter`

2. **Add Inventory Component**
   - In Components panel, click `+ Add` button
   - Search for: `Inventory` or `BP_InventoryComponent`
   - Select: `Inventory Component` or `BP Inventory Component`

3. **Configure Component Properties**
   - Select `InventoryComponent` in Components list
   - In Details panel, set default values (optional):
     - `Max Capacity`: 50
     - `Max Weight`: 100.0

4. **Compile and Save**
   - Click `Compile` button
   - Click `Save` button
   - Close Blueprint Editor

### Step 5: Test Component Attachment

1. **Test in Play Mode**
   - Open test level
   - Press `Play` button
   - Check Output Log for any errors
   - Component should initialize automatically

2. **Verify Component Integration**
   - Open PlayerCharacter Blueprint
   - In Event Graph, search for: `Get Inventory Component`
   - Should find the node (component is properly integrated)

### Verification Checklist

- [ ] InventoryComponent C++ code compiles without errors
- [ ] Blueprint class `BP_InventoryComponent` created successfully
- [ ] Component added to `BP_ActionRPGPlayerCharacter`
- [ ] Component properties visible in Details panel
- [ ] Component methods accessible in Blueprint Event Graph
- [ ] Blueprint compiles without errors
- [ ] No errors in Output Log during Play mode
- [ ] Component initializes correctly in game

---

## Days 17-18: Item Pickup Actor

### Prerequisites

- ✅ Days 15-16 complete (InventoryComponent functional)
- ✅ InventoryComponent attached to PlayerCharacter
- ✅ ItemDatabase working and initialized
- ✅ Test Item Data Assets created

### Step 1: Verify C++ Code Compiled

1. **Check Compilation Status**
   - Open Output Log
   - Check for any compilation errors
   - Verify Hot Reload completed

2. **Verify ItemPickupActor Class**
   - Try to create a Blueprint Class
   - Search for `ItemPickupActor`
   - If it appears, C++ code is ready

### Step 2: Create Folder Structure

1. **Create Content Folders**
   - Navigate to `Content/Blueprints/Items/`
   - Create `Pickups` subfolder if needed
   - Final structure: `Content/Blueprints/Items/Pickups/`

### Step 3: Create Blueprint Class from ItemPickupActor

1. **Create Blueprint**
   - Navigate to `Content/Blueprints/Items/Pickups/`
   - Right-click → `Blueprint Class`
   - Search for `ItemPickupActor`
   - Select and click `Select`

2. **Name the Blueprint**
   - Name: `BP_ItemPickup`
   - Save in `Content/Blueprints/Items/Pickups/`

### Step 4: Configure Blueprint Components

1. **Open Blueprint Editor**
   - Double-click `BP_ItemPickup`

2. **Configure CollisionComponent**
   - Select `CollisionComponent` in Components panel
   - In Details panel:
     - **Collision Enabled:** `Query Only`
     - **Object Type:** `WorldDynamic`
     - **Pawn Response:** `Overlap`
     - **Sphere Radius:** `50.0` (adjust as needed)

3. **Configure MeshComponent**
   - Select `MeshComponent` in Components panel
   - In Details panel:
     - Assign a Static Mesh (e.g., Cube, Sphere from Engine/BasicShapes)
     - **Collision Enabled:** `No Collision`
     - **Mobility:** `Movable`

4. **Configure Item Data (Class Defaults)**
   - Select Blueprint Root
   - In Details panel → `Pickup` category:
     - **Item Data:** Set to test Item Data Asset (or leave None for per-instance)
     - **Quantity:** `1` (default)

5. **Compile and Save**
   - Click `Compile` button
   - Click `Save` button

### Step 5: Add Pickup to Test Level

1. **Open Test Level**
   - Open your test level

2. **Place Item Pickup Actors**
   - Drag `BP_ItemPickup` from Content Browser to level
   - Place multiple pickups for testing

3. **Configure Each Instance**
   - Select each pickup in level
   - In Details Panel → `Pickup` category:
     - Set `Item Data` to test Item Data Asset
     - Set `Quantity` if needed

4. **Save Level**

### Step 6: Test Pickup in Game

1. **Play in Editor**
   - Click `Play` button
   - Navigate to pickup
   - Press `E` key (Interact) when near pickup

2. **Verify Pickup Behavior**
   - Pickup should disappear when interacted
   - Item should be added to inventory
   - Check console for pickup messages

3. **Test Edge Cases**
   - Test full inventory scenario
   - Test stacking
   - Test weight limits

### Verification Checklist

- [ ] ItemPickupActor C++ class compiles without errors
- [ ] Blueprint class `BP_ItemPickup` created
- [ ] Components configured correctly
- [ ] Collision settings correct
- [ ] Item Data property can be set
- [ ] Pickup spawns in world correctly
- [ ] Player overlap detected correctly
- [ ] Item added to inventory on pickup
- [ ] Pickup destroyed after successful pickup
- [ ] Full inventory scenario handled correctly

---

## Days 19-20: Item Usage System

### Prerequisites

- ✅ Days 15-18 complete
- ✅ Items can be picked up and added to inventory
- ✅ PlayerCharacter has stats system (Health, Mana, Stamina)

### Step 1: Verify C++ Code Compiled

1. **Check Compilation Status**
   - Open Output Log
   - Check for any compilation errors
   - Verify UseItem function is accessible

### Step 2: Test Basic Item Usage

1. **Test UseItem via Blueprint**
   - Create test Blueprint or use PlayerCharacter Blueprint
   - Get InventoryComponent reference
   - Call `UseItem` function with slot index
   - Test in PIE

2. **Verify Console Logs**
   - Check Output Log for usage messages
   - Verify item quantity decreases
   - Verify item is removed when quantity = 0

### Step 3: Configure Item Effects (Consumables)

1. **Set Up Character Stats**
   - Open `BP_ActionRPGPlayerCharacter`
   - Verify stats variables exist (Health, MaxHealth, etc.)

2. **Connect Item Usage to Stats**
   - Bind to `OnItemUsed` event
   - Check item type and apply effects
   - For Health Potion: Add to current Health, clamp to MaxHealth

3. **Add Visual/Audio Feedback** (Optional)
   - Sound effects on item use
   - Particle effects on item use
   - UI feedback (floating text, health bar update)

### Step 4: Test Item Type Handling

1. **Test Consumable Items**
   - Use consumable item
   - Verify item quantity decreases
   - Verify item is removed when quantity = 0
   - Verify effects are applied

2. **Test Equipment Items**
   - Use equipment item
   - Verify item is NOT consumed
   - Verify quantity stays the same

3. **Test Other Item Types**
   - Test Skill Items (not consumed)
   - Test Misc Items (not consumed)

### Verification Checklist

- [ ] UseItem function works correctly
- [ ] Consumable items are consumed properly
- [ ] Item quantity decreases on use
- [ ] Item is removed when quantity = 0
- [ ] OnItemUsed event fires correctly
- [ ] Different item types handled correctly
- [ ] Error handling works (empty slots, invalid slots, etc.)

---

## Day 21: Integration Testing

### Prerequisites

- ✅ Days 15-20 complete
- ✅ All systems functional
- ✅ Test level created with Player Start

### Step 1: Prepare Test Environment

1. **Open Test Level**
   - Open your test level
   - Ensure Player Start exists
   - Ensure GameMode is set

2. **Place Item Pickups**
   - Place 3-5 pickups around player start
   - Configure each with different items

3. **Open Output Log**
   - Keep Output Log open for monitoring

### Step 2: Test Complete Flow

1. **Test Item Pickup**
   - Start Play Mode
   - Approach item pickup
   - Press Interact key (E)
   - Verify item added to inventory
   - Verify pickup destroyed

2. **Test Item Usage**
   - Use Health Potion from inventory
   - Verify health increases
   - Verify item consumed
   - Verify item removed when quantity = 0

3. **Test Complete Flow Multiple Times**
   - Pickup → Use flow
   - Verify consistency

### Step 3: Test Edge Cases

1. **Test Full Inventory**
   - Fill inventory to capacity
   - Attempt pickup when full
   - Should fail gracefully

2. **Test Item Stacking**
   - Pick up same item multiple times
   - Verify stacking works correctly

3. **Test Weight Limits**
   - Add heavy items until weight limit reached
   - Attempt pickup when over weight
   - Should fail gracefully

4. **Test Health Potion at Max Health**
   - Heal to max health
   - Attempt to use health potion
   - Should fail gracefully
   - Item should NOT be consumed

### Step 4: Debug and Fix Issues

1. **Check Output Log for Errors**
   - Review all log messages
   - Look for errors or warnings
   - Fix any issues found

2. **Verify Event Bindings**
   - Check OnItemUsed binding
   - Test event firing
   - Verify inventory debug reports

### Verification Checklist

- [ ] Complete flow works (pickup → inventory → use)
- [ ] Health potion heals player correctly
- [ ] Health potion cannot be used at max health
- [ ] Items are consumed correctly
- [ ] Edge cases handled gracefully
- [ ] No crashes or critical errors
- [ ] Performance is acceptable

---

## Days 22-23: Inventory UI Foundation

### Prerequisites

- ✅ Days 15-21 complete
- ✅ InventoryComponent functional and tested
- ✅ Test Item Data Assets created with icons

### Step 1: Create Folder Structure

1. **Create Source Folders**
   - `Source/ActionRPG/Public/UI/Inventory/`
   - `Source/ActionRPG/Private/UI/Inventory/`

2. **Create Content Folders**
   - `Content/UI/Inventory/`

### Step 2: Create Blueprint Widget Classes

#### 2.1 Create WBP_InventorySlotWidget

1. **Create Widget Blueprint**
   - Navigate to `Content/UI/Inventory/`
   - Right-click → `User Interface` → `Widget Blueprint`
   - Name: `WBP_InventorySlotWidget`
   - Set Parent Class to `Inventory Slot Widget`

2. **Design Slot Widget Layout**
   - Root: Canvas Panel
   - Add `Border` widget (Name: `SlotBorder`)
   - Add `Image` widget (Name: `ItemIcon`)
   - Add `Text Block` widget (Name: `QuantityText`)
   - **Critical:** All widgets must be direct children of Canvas Panel root

3. **Verify Widget Names**
   - Names must match C++ BindWidget names exactly:
     - `ItemIcon`
     - `QuantityText`
     - `SlotBorder`

4. **Compile and Save**

#### 2.2 Create WBP_InventoryWidget

1. **Create Widget Blueprint**
   - Right-click in `Content/UI/Inventory/`
   - Select `User Interface` → `Widget Blueprint`
   - Name: `WBP_InventoryWidget`
   - Set Parent Class to `Inventory Widget`

2. **Design Inventory Widget Layout**
   - Root: Canvas Panel (set size via anchors/offsets)
   - Add `Uniform Grid Panel` (Name: `InventoryGrid`)
   - Add `Text Block` (Name: `WeightText`)
   - Add `Text Block` (Name: `CapacityText`)
   - Add `Button` (Name: `CloseButton`)

3. **Verify Widget Names**
   - Names must match C++ BindWidget names exactly:
     - `InventoryGrid`
     - `WeightText`
     - `CapacityText`
     - `CloseButton`

4. **Set Slot Widget Class**
   - In Class Defaults, set `Slot Widget Class` to `WBP_InventorySlotWidget`

5. **Compile and Save**

### Step 3: Set Up Widget in PlayerController

1. **Open BP_ActionRPGPlayerController**
   - Navigate to `Content/Blueprints/Core/`
   - Open `BP_ActionRPGPlayerController`

2. **Set Inventory Widget Class**
   - In Class Defaults
   - Set `Inventory Widget Class` to `WBP_InventoryWidget`

3. **Compile and Save**

### Step 4: Test Inventory UI

1. **Test Widget Creation**
   - Start Play Mode
   - Press inventory key (Tab or I)
   - Inventory widget should appear
   - Widget should show 50 empty slots in grid

2. **Test Slot Display**
   - Pick up some items
   - Open inventory
   - Verify slots update with items
   - Verify icons and quantities display

3. **Test Item Usage from UI**
   - Right-click on item in inventory
   - Item should be used
   - Slot should update after use

### Verification Checklist

- [ ] InventorySlotWidget C++ class compiles without errors
- [ ] InventoryWidget C++ class compiles without errors
- [ ] WBP_InventorySlotWidget Blueprint created and compiles
- [ ] WBP_InventoryWidget Blueprint created and compiles
- [ ] All widget names match C++ BindWidget names exactly
- [ ] Inventory toggle works (key press opens/closes widget)
- [ ] Widget displays 50 slots in 10x5 grid
- [ ] Items in inventory display in slots (icons and quantities)
- [ ] Weight and capacity text updates when items added/removed
- [ ] Right-click on item uses item correctly
- [ ] Close button closes inventory

---

## Days 24-25: Drag and Drop System

### Prerequisites

- ✅ Days 22-23 complete
- ✅ Inventory UI displays correctly
- ✅ Inventory populated with test items

### Step 1: Create ItemDragDropOperation Class

**Note:** C++ implementation should already be complete. Verify class compiles.

### Step 2: Test Drag and Drop

1. **Test Drag from Slot**
   - Open inventory
   - Left-click and hold on item slot
   - Drag the mouse cursor
   - Item should follow cursor (drag preview appears)

2. **Test Drop on Empty Slot**
   - Drag item to empty slot
   - Release mouse button
   - Item should move to new slot
   - Original slot should become empty

3. **Test Drop on Same Item (Stacking)**
   - Drag one stack onto another of same item
   - Stacks should merge if within MaxStackSize

4. **Test Drop on Different Item (Swap)**
   - Drag one item onto another different item
   - Items should swap positions

5. **Test Visual Feedback**
   - Start dragging an item
   - Hover over valid drop target (green border)
   - Hover over invalid drop target (red border)

6. **Test Drag Cancel**
   - Start dragging an item
   - Press `Escape` or drag outside inventory
   - Item should return to original slot

### Verification Checklist

- [ ] ItemDragDropOperation class compiles without errors
- [ ] Drag detection works (click and hold starts drag)
- [ ] Drag preview appears and follows cursor
- [ ] Drop on empty slot moves item correctly
- [ ] Drop on same item stacks correctly
- [ ] Drop on different item swaps correctly
- [ ] Visual feedback works (green/red borders on hover)
- [ ] Drag cancel works (Escape key)
- [ ] All slots update correctly after drop

---

## Days 26-27: Stack Splitting, World Item Dropping, Quick-Use Bar & UI Polish

### Prerequisites

- ✅ Days 24-25 complete
- ✅ Drag and drop system working
- ✅ Test level has items to test all features

### Step 1: Test Stack Splitting

1. **Test Ctrl+Drag Split Operation**
   - Open inventory
   - Hold `Ctrl` key
   - Click and drag a stackable item (quantity > 1)
   - Release on target slot
   - Stack should split (half quantity moves)
   - Original slot quantity should decrease
   - Target slot should have split quantity

2. **Verify Split Goes to Exact Target Slot**
   - Ctrl+drag from slot A to slot B specifically
   - Verify split stack appears in slot B (not next available slot)

3. **Test Split to Existing Stack**
   - Ctrl+drag split to slot with same item
   - Should stack if space available (respects MaxStackSize)

### Step 2: Test World Item Dropping

1. **Test Drag-to-World Feature**
   - Open inventory
   - Drag item from inventory slot
   - Drag outside inventory widget (over game world)
   - Release mouse button
   - Item should spawn as ItemPickupActor in world
   - Item should be removed from inventory

2. **Test Drop Location**
   - Verify item spawns at mouse cursor location (projected to ground)
   - Verify item is accessible for pickup

3. **Test Context Menu Drop**
   - Right-click on item
   - Select "Drop" from context menu
   - Item should spawn in front of character
   - Item should be removed from inventory

### Step 3: Create Input Actions for Quick-Use Bar

1. **Create Input Actions**
   - Navigate to `Content/Input/`
   - Create `IA_QuickUseSlot9` (Input Action)
   - Create `IA_QuickUseSlot10` (Input Action)
   - Set `Value Type` to `Digital (bool)`

2. **Configure Input Mapping Context**
   - Open `IM_ActionRPG`
   - Add mapping: `IA_QuickUseSlot9` → Keyboard `9`
   - Add mapping: `IA_QuickUseSlot10` → Keyboard `0`
   - Save Input Mapping Context

3. **Assign Input Actions to PlayerController**
   - Open `BP_ActionRPGPlayerController`
   - In Class Defaults, assign:
     - `Quick Use Slot 9 Action` → `IA_QuickUseSlot9`
     - `Quick Use Slot 10 Action` → `IA_QuickUseSlot10`
   - Compile and Save

### Step 4: Create Quick-Use Bar Blueprint Widgets

1. **Create WBP_QuickUseSlotWidget**
   - Navigate to `Content/UI/QuickUse/`
   - Create Widget Blueprint
   - Parent Class: `Quick Use Slot Widget`
   - Name: `WBP_QuickUseSlotWidget`
   - Add widgets: `SlotBorder`, `ItemIcon`, `QuantityText`, `HotkeyText`
   - Compile and Save

2. **Create WBP_QuickUseBarWidget**
   - Create Widget Blueprint
   - Parent Class: `Quick Use Bar Widget`
   - Name: `WBP_QuickUseBarWidget`
   - Add `Uniform Grid Panel` (Name: `QuickUseGrid`)
   - Set `Slot Widget Class` to `WBP_QuickUseSlotWidget`
   - Compile and Save

3. **Add Quick-Use Bar to HUD**
   - Open `BP_ActionRPGHUD`
   - In Event Graph → BeginPlay:
     - Create Widget (Class: `WBP_QuickUseBarWidget`)
     - Add to Viewport (ZOrder: 200+)
   - Compile and Save

### Step 5: Create Context Menu Widget

1. **Create WBP_InventoryContextMenuWidget**
   - Navigate to `Content/UI/Inventory/`
   - Create Widget Blueprint
   - Parent Class: `Inventory Context Menu Widget`
   - Name: `WBP_InventoryContextMenuWidget`
   - Add buttons: `UseButton`, `DropButton`, `SplitButton`, `EquipButton`
   - **Critical:** Button names must match C++ exactly
   - Compile and Save

2. **Assign Context Menu Widget Class**
   - Open `WBP_InventoryWidget`
   - In Class Defaults, set `Context Menu Widget Class` to `WBP_InventoryContextMenuWidget`
   - Compile and Save

### Step 6: Test Quick-Use Bar Functionality

1. **Test Hotkey Functionality**
   - Assign consumable item to quick-use slot 9 or 10
   - Press `9` key (for slot 9) or `0` key (for slot 10)
   - Item should be used
   - Quantity should decrease

2. **Test Slot Validation**
   - Try assigning non-consumable item to slots 9-10
   - Should fail (only consumables allowed)

### Step 7: Test Context Menu Functionality

1. **Test Right-Click Context Menu**
   - Right-click on inventory slot with item
   - Context menu should appear
   - Menu should show: Use, Drop, Split, Equip buttons

2. **Test Button Visibility Rules**
   - Right-click on consumable: "Equip" button should be hidden
   - Right-click on equipment: "Equip" button should be visible
   - Right-click on stack (quantity > 1): "Split" button should be visible
   - Right-click on single item (quantity = 1): "Split" button should be hidden

3. **Test Context Menu Actions**
   - Click "Use" button - item should be used
   - Click "Drop" button - item should spawn in world in front of character
   - Click "Split" button - stack should split
   - Click "Equip" button - should prepare for equipment system

4. **Test Context Menu Closing**
   - Click outside menu - menu should close
   - Right-click another slot - menu should close and reopen

### Verification Checklist

- [ ] Stack splitting works with Ctrl+drag
- [ ] Split stack goes to exact target slot
- [ ] World item drop works (drag-to-world and context menu)
- [ ] Input Actions created and mapped correctly
- [ ] Hotkeys 9-0 activate quick-use slots correctly
- [ ] Quick-Use Bar Widget created and visible
- [ ] Context Menu Widget created
- [ ] Context menu appears on right-click
- [ ] Button visibility rules work correctly
- [ ] All context menu actions work correctly

---

## Day 28: Final Integration & Testing

### Prerequisites

- ✅ All C++ code compiled successfully
- ✅ All Blueprint classes created
- ✅ All Widget Blueprints created

### Step 1: Verify All Blueprint Classes

1. **Verify PlayerCharacter Blueprint**
   - Open `BP_ActionRPGPlayerCharacter`
   - Verify `InventoryComponent` exists in Components panel
   - Check properties are visible
   - Compile and Save

2. **Verify Widget Blueprints**
   - Open `WBP_InventoryWidget` - verify layout and bindings
   - Open `WBP_InventorySlotWidget` - verify layout and bindings
   - Open `WBP_QuickUseBarWidget` - verify layout
   - Open `WBP_InventoryContextMenuWidget` - verify buttons
   - All should compile without errors

### Step 2: Verify PlayerController Setup

1. **Open BP_ActionRPGPlayerController**
   - Verify `Inventory Widget Class` is set to `WBP_InventoryWidget`
   - Verify Input Actions are assigned:
     - `Quick Use Slot 9 Action` → `IA_QuickUseSlot9`
     - `Quick Use Slot 10 Action` → `IA_QuickUseSlot10`
   - Compile and Save

### Step 3: Verify HUD Setup

1. **Open BP_ActionRPGHUD**
   - Verify Quick-Use Bar is created in BeginPlay
   - Verify widget is added to viewport with high Z-order (200+)
   - Compile and Save

### Step 4: Verify Input Actions

1. **Open IM_ActionRPG**
   - Verify all required Input Actions are mapped:
     - `IA_OpenInventory` → Tab key
     - `IA_QuickUseSlot9` → 9 key
     - `IA_QuickUseSlot10` → 0 key
     - `IA_Interact` → E key
   - Save Input Mapping Context

### Step 5: Verify Item Pickup Setup

1. **Open BP_ItemPickup**
   - Verify `ItemData` property exists
   - Verify `Quantity` property exists
   - Verify Components: `MeshComponent`, `CollisionComponent`
   - Compile and Save

### Step 6: Verify Test Level Setup

1. **Open Test Level**
   - Verify Player Start exists
   - Verify GameMode is set to `BP_ActionRPGGameMode`
   - Place test `BP_ItemPickup` actors in level
   - Configure each with different items
   - Save level

### Step 7: In-Game Manual Testing

1. **Test Inventory Toggle**
   - Press Tab key - inventory should open/close
   - Verify all 50 slots are visible
   - Verify weight and capacity text displays

2. **Test Item Pickup**
   - Walk to ItemPickupActor, press E
   - Verify item appears in inventory
   - Verify pickup destroyed

3. **Test Item Usage**
   - Right-click on item or use context menu
   - Verify item is used
   - Verify quantity decreases

4. **Test Drag and Drop**
   - Drag item between slots
   - Verify items move/swap/stack correctly

5. **Test Stack Splitting**
   - Ctrl+drag stackable item
   - Verify stack splits correctly
   - Verify split goes to exact target slot

6. **Test Quick-Use Bar**
   - Assign item to quick-use slot
   - Press 9 or 0 key
   - Verify item is used

7. **Test Context Menu**
   - Right-click on item
   - Verify menu appears
   - Test all menu actions (Use, Drop, Split, Equip)
   - Verify button visibility rules

8. **Test World Item Dropping**
   - Drag item outside inventory or use context menu Drop
   - Verify item spawns in world
   - Verify item can be picked up again

9. **Test Edge Cases**
   - Full inventory
   - Weight limits
   - Rapid operations
   - Monitor Output Log for errors

### Final Verification Checklist

- [ ] All Blueprint classes compile without errors
- [ ] All Widget Blueprints compile without errors
- [ ] InventoryComponent is attached to PlayerCharacter
- [ ] InventoryWidget displays correctly
- [ ] QuickUseBarWidget displays correctly
- [ ] ContextMenuWidget displays correctly
- [ ] Input Actions are mapped correctly
- [ ] Item pickup works
- [ ] Item usage works
- [ ] Drag and drop works
- [ ] Stack splitting works
- [ ] Quick-use slots work
- [ ] Context menu works
- [ ] World item dropping works
- [ ] No errors in Output Log
- [ ] All features tested and working
- [ ] Performance is acceptable

---

## Troubleshooting

### Common Issues

**Issue: Widget Not Appearing**
- Check widget class is set in PlayerController
- Check widget names match C++ BindWidget names exactly
- Check widget is compiled
- Check C++ code compiled successfully

**Issue: Items Not Picking Up**
- Check CollisionComponent settings (Overlap enabled)
- Check InventoryComponent is attached to PlayerCharacter
- Check inventory has space
- Check Output Log for errors

**Issue: Drag and Drop Not Working**
- Check NativeOnDragDetected is implemented
- Check ItemDragDropOperation is created correctly
- Check NativeOnDrop is implemented
- Check Output Log for errors

**Issue: Quick-Use Bar Not Working**
- Check Input Actions are mapped correctly
- Check OnQuickUseSlot9/10 events are bound
- Check QuickUseBarWidget is visible and added to viewport
- Check Output Log for errors

**Issue: Context Menu Not Appearing**
- Check OnInventorySlotRightClicked is implemented
- Check ContextMenuWidgetClass is set in InventoryWidget
- Check context menu widget is created and added to viewport
- Check positioning logic (Blueprint implementation)
- Check Output Log for errors

---

## Quick Reference

### Key File Paths
- **PlayerCharacter:** `Content/Blueprints/Characters/BP_ActionRPGPlayerCharacter`
- **PlayerController:** `Content/Blueprints/Core/BP_ActionRPGPlayerController`
- **HUD:** `Content/UI/BP_ActionRPGHUD`
- **InventoryWidget:** `Content/UI/Inventory/WBP_InventoryWidget`
- **InventorySlotWidget:** `Content/UI/Inventory/WBP_InventorySlotWidget`
- **QuickUseBarWidget:** `Content/UI/QuickUse/WBP_QuickUseBarWidget`
- **ContextMenuWidget:** `Content/UI/Inventory/WBP_InventoryContextMenuWidget`
- **ItemPickup:** `Content/Blueprints/Items/Pickups/BP_ItemPickup`
- **Input Mapping Context:** `Content/Input/IM_ActionRPG`

### Key Widget Names (Must Match Exactly)
- `InventoryGrid` - UniformGridPanel for slots
- `WeightText` - TextBlock for weight display
- `CapacityText` - TextBlock for capacity display
- `CloseButton` - Button to close inventory
- `ItemIcon` - Image for item icon (in slot widget)
- `QuantityText` - TextBlock for quantity (in slot widget)
- `SlotBorder` - Border for slot background (in slot widget)
- `UseButton`, `DropButton`, `SplitButton`, `EquipButton` - Context menu buttons

### Key Input Actions
- `IA_OpenInventory` → Tab key
- `IA_QuickUseSlot9` → 9 key
- `IA_QuickUseSlot10` → 0 key
- `IA_Interact` → E key

---

**Status:** Complete these manual steps before considering Phase 2 complete

---

**End of Phase 2 Manual Steps - Complete**
