# Phase 2 Manual Steps - Day 28
**After Code Implementation**  
**Date:** 2025-01-07  
**Phase:** Phase 2 - Inventory System  
**Day:** 28 - Final Integration & Testing

---

## Overview

After completing all C++ code implementation and Blueprint setup for Phase 2, you need to perform comprehensive manual verification steps in the Unreal Editor to ensure all systems are properly configured and working correctly. These steps are required before Phase 2 can be considered complete.

---

## Prerequisites

- ✅ All C++ code compiled successfully
- ✅ All Blueprint classes created
- ✅ All Widget Blueprints created
- ✅ Unreal Editor is open
- ✅ Project is loaded in the editor
- ✅ No compilation errors in Output Log
- ✅ Visual Studio build completed successfully

**Before Starting:**
1. Close Visual Studio (if open)
2. Return to Unreal Editor
3. Wait for Hot Reload to complete (if automatic)
4. If Hot Reload failed, restart Unreal Editor

---

## Step 1: Verify All Blueprint Classes

### 1.1 Verify PlayerCharacter Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Characters/` folder

2. **Open PlayerCharacter Blueprint**
   - Double-click `BP_ActionRPGPlayerCharacter`
   - Blueprint Editor should open

3. **Verify InventoryComponent**
   - On the left side, find the `Components` panel
   - Look for `InventoryComponent` in the Components list
   - If not visible, go to: `Window` → `Components`

4. **Check InventoryComponent Properties**
   - Select `InventoryComponent` in Components list
   - In `Details` panel, you should see:
     - **Inventory** category:
       - `Max Capacity` (int32, default: 50)
       - `Max Weight` (float, default: 100.0)
       - `Inventory Slots` (Array, runtime only - will be empty in editor)
   - Verify values are set correctly

5. **Compile Blueprint**
   - Click `Compile` button (top toolbar)
   - Should compile without errors
   - Look for green checkmark or success message

6. **Save Blueprint**
   - Click `Save` button (top toolbar)
   - Or press `Ctrl + S`

7. **Close Blueprint Editor**
   - Close the Blueprint Editor window

---

## Step 2: Verify Widget Blueprints

### 2.1 Verify InventoryWidget

1. **Navigate to Content Browser**
   - Go to `Content/UI/Inventory/` folder

2. **Open InventoryWidget Blueprint**
   - Double-click `WBP_InventoryWidget`
   - UMG Widget Editor should open

3. **Verify Widget Layout (Designer Tab)**
   - In the `Designer` tab, verify:
     - `InventoryGrid` (UniformGridPanel) exists
     - Grid is configured: 10 columns x 5 rows = 50 slots
     - `WeightText` (TextBlock) exists and is visible
     - `CapacityText` (TextBlock) exists and is visible
     - `CloseButton` (Button) exists and is visible
     - Background panel/border exists

4. **Verify Widget Bindings (Graph Tab)**
   - Switch to `Graph` tab
   - Verify no compilation errors in Output Log
   - All widget references should be bound (check for warnings)

5. **Verify SlotWidgetClass Property**
   - In `Details` panel (Class Defaults), find:
     - `Slot Widget Class` property
     - Should be set to `WBP_InventorySlotWidget`
     - If not set, assign it now

6. **Compile Widget**
   - Click `Compile` button (top toolbar)
   - Should compile without errors

7. **Save Widget**
   - Click `Save` button (top toolbar)
   - Or press `Ctrl + S`

8. **Close Widget Editor**

### 2.2 Verify InventorySlotWidget

1. **Open InventorySlotWidget Blueprint**
   - In `Content/UI/Inventory/` folder
   - Double-click `WBP_InventorySlotWidget`
   - UMG Widget Editor should open

2. **Verify Widget Layout (Designer Tab)**
   - In the `Designer` tab, verify:
     - `SlotBorder` (Border) exists as base widget
     - `ItemIcon` (Image) exists and is centered
     - `QuantityText` (TextBlock) exists (bottom-right corner recommended)
     - Widget sizing is appropriate (64x64 or similar)

3. **Verify Widget Bindings (Graph Tab)**
   - Switch to `Graph` tab
   - Verify no compilation errors
   - All widget references should be bound

4. **Compile Widget**
   - Click `Compile` button
   - Should compile without errors

5. **Save Widget**
   - Click `Save` button
   - Close Widget Editor

### 2.3 Verify QuickUseBarWidget

1. **Navigate to Content Browser**
   - Go to `Content/UI/QuickUse/` folder

2. **Open QuickUseBarWidget Blueprint**
   - Double-click `WBP_QuickUseBarWidget`
   - UMG Widget Editor should open

3. **Verify Widget Layout (Designer Tab)**
   - In the `Designer` tab, verify:
     - 10 slots are visible and properly laid out
     - Slots 9-10 are visually distinct (consumable slots)
     - Slots 1-8 are prepared for skills (can be disabled/visually distinct)
     - Widget is set to always visible
     - Widget has appropriate background/border

4. **Verify Widget Settings**
   - In `Details` panel (Class Defaults), check:
     - Widget should be set to always visible
     - Z-order should be high (200+) to stay on top of other widgets

5. **Compile Widget**
   - Click `Compile` button
   - Should compile without errors

6. **Save Widget**
   - Click `Save` button
   - Close Widget Editor

### 2.4 Verify InventoryContextMenuWidget

1. **Navigate to Content Browser**
   - Go to `Content/UI/Inventory/` folder

2. **Open ContextMenuWidget Blueprint**
   - Double-click `WBP_InventoryContextMenuWidget`
   - UMG Widget Editor should open

3. **Verify Widget Layout (Designer Tab)**
   - In the `Designer` tab, verify:
     - `UseButton` (Button) exists
     - `DropButton` (Button) exists
     - `SplitButton` (Button) exists
     - `EquipButton` (Button) exists
     - Optional: Text blocks for button labels
     - Widget layout (Vertical Box or similar)

4. **Verify Widget Bindings (Graph Tab)**
   - Switch to `Graph` tab
   - Verify no compilation errors
   - All button references should be bound

5. **Verify Positioning Setup**
   - Widget should use Canvas Panel for positioning
   - Or use Render Transform for dynamic positioning
   - This is needed for positioning at cursor location

6. **Compile Widget**
   - Click `Compile` button
   - Should compile without errors

7. **Save Widget**
   - Click `Save` button
   - Close Widget Editor

---

## Step 3: Verify PlayerController Setup

### 3.1 Open PlayerController Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder (or appropriate folder)
   - Look for `BP_ActionRPGPlayerController`

2. **Open PlayerController Blueprint**
   - Double-click `BP_ActionRPGPlayerController`
   - Blueprint Editor should open

### 3.2 Verify Widget Class Properties

1. **Check Class Defaults**
   - Click `Class Defaults` button (top toolbar)
   - Or select `Class Defaults` in the viewport dropdown

2. **Verify InventoryWidgetClass**
   - In `Details` panel, find:
     - `Inventory Widget Class` property
     - Should be set to `WBP_InventoryWidget`
     - If not set, assign it now

3. **Verify QuickUseBarWidgetClass** (if managed by PlayerController)
   - In `Details` panel, find:
     - `Quick Use Bar Widget Class` property
     - Should be set to `WBP_QuickUseBarWidget`
     - If not set, assign it now

### 3.3 Verify Input Action Bindings

1. **Open Event Graph**
   - Click `Event Graph` tab (or `Graph` button)

2. **Verify Input Action Bindings**
   - Look for event nodes:
     - `OnOpenInventory` (bound to `IA_OpenInventory`)
     - `OnQuickUseSlot9` (bound to `IA_QuickUseSlot9`)
     - `OnQuickUseSlot10` (bound to `IA_QuickUseSlot10`)
     - `OnInteract` (bound to `IA_Interact`)

3. **Check Event Implementations**
   - Verify each event has proper implementation
   - `OnOpenInventory` should toggle inventory widget
   - `OnQuickUseSlot9/10` should call inventory component methods

4. **Compile Blueprint**
   - Click `Compile` button
   - Should compile without errors
   - Look for any warnings about missing bindings

5. **Save Blueprint**
   - Click `Save` button
   - Close Blueprint Editor

---

## Step 4: Verify HUD Setup

### 4.1 Open HUD Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/UI/` folder
   - Look for `BP_ActionRPGHUD`

2. **Open HUD Blueprint**
   - Double-click `BP_ActionRPGHUD`
   - Blueprint Editor should open

### 4.2 Verify Quick-Use Bar Setup (if managed by HUD)

1. **Check Class Defaults**
   - Click `Class Defaults` button
   - In `Details` panel, verify:
     - `Quick Use Bar Widget Class` is set to `WBP_QuickUseBarWidget`

2. **Open Event Graph**
   - Click `Event Graph` tab

3. **Verify BeginPlay Implementation**
   - Look for `Event BeginPlay` node
   - Should create QuickUseBarWidget
   - Should add widget to viewport with high Z-order (200+)
   - Widget should be set to always visible

4. **Compile Blueprint**
   - Click `Compile` button
   - Should compile without errors

5. **Save Blueprint**
   - Click `Save` button
   - Close Blueprint Editor

---

## Step 5: Verify Input Actions

### 5.1 Open Input Mapping Context

1. **Navigate to Content Browser**
   - Go to `Content/Input/` folder

2. **Open Input Mapping Context**
   - Double-click `IM_ActionRPG` (Input Mapping Context)
   - Input Mapping Context Editor should open

### 5.2 Verify Input Action Mappings

1. **Verify Required Input Actions Exist**
   - Check that all required Input Actions are in the list:
     - `IA_OpenInventory`
     - `IA_QuickUseSlot9`
     - `IA_QuickUseSlot10`
     - `IA_Interact`
     - `IA_QuickUseSlot1` through `IA_QuickUseSlot8` (for Phase 3)

2. **Verify Key Mappings**
   - For each Input Action, verify key mappings:
     - `IA_OpenInventory` → Tab key (or configured key)
     - `IA_QuickUseSlot9` → 9 key
     - `IA_QuickUseSlot10` → 0 key
     - `IA_Interact` → E key
     - `IA_QuickUseSlot1-8` → 1-8 keys (for Phase 3)

3. **Check Mapping Context Priority**
   - Verify mapping context has appropriate priority
   - Should be active during gameplay

4. **Save Input Mapping Context**
   - Click `Save` button
   - Close Editor

---

## Step 6: Verify Item Pickup Setup

### 6.1 Open ItemPickup Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Items/Pickups/` folder (or appropriate folder)
   - Look for `BP_ItemPickup`

2. **Open ItemPickup Blueprint**
   - Double-click `BP_ItemPickup`
   - Blueprint Editor should open

### 6.2 Verify ItemPickup Properties

1. **Check Class Defaults**
   - Click `Class Defaults` button
   - In `Details` panel, verify:
     - `Item Data` property (can be set per-instance in level)
     - `Quantity` property (default: 1)

2. **Verify Components**
   - In `Components` panel, verify:
     - `MeshComponent` (StaticMeshComponent) exists
     - `CollisionComponent` (SphereComponent) exists

3. **Check MeshComponent Settings**
   - Select `MeshComponent` in Components list
   - In `Details` panel:
     - Verify a mesh is assigned (or can be assigned per-instance)
     - Check material settings

4. **Check CollisionComponent Settings**
   - Select `CollisionComponent` in Components list
   - In `Details` panel, verify:
     - `Collision Enabled`: Query Only
     - `Object Type`: WorldDynamic
     - `Collision Responses`: Appropriate settings for overlap detection
     - `Sphere Radius`: Appropriate size (e.g., 100 units)

5. **Compile Blueprint**
   - Click `Compile` button
   - Should compile without errors

6. **Save Blueprint**
   - Click `Save` button
   - Close Blueprint Editor

---

## Step 7: Verify Test Level Setup

### 7.1 Open Test Level

1. **Open Level**
   - Open your test level (e.g., `NewMap` or `Lvl_TopDown`)
   - Level Editor should open

### 7.2 Verify Level Settings

1. **Check Player Start**
   - In viewport, verify `Player Start` actor exists
   - If not, add one:
     - Place Actors → Basic → Player Start
     - Position appropriately

2. **Check GameMode**
   - Go to `Edit` → `World Settings`
   - Or press `Shift + F1` to open World Settings panel
   - In `Game Mode` section:
     - `GameMode Override` should be set to `BP_ActionRPGGameMode`
     - If not set, assign it now

### 7.3 Place Test Item Pickups

1. **Add Item Pickups to Level**
   - In Content Browser, navigate to `Content/Blueprints/Items/Pickups/`
   - Drag `BP_ItemPickup` from Content Browser to level viewport
   - Place multiple pickups for testing

2. **Configure Each Pickup**
   - Select each `BP_ItemPickup` in level
   - In `Details` panel:
     - Set `Item Data` to a test item (e.g., Health Potion Data Asset)
     - Set `Quantity` (e.g., 1, 5, 10 for variety)
     - Adjust `MeshComponent` mesh if needed
     - Position pickups at different locations

3. **Add Variety**
   - Place pickups with different items (if available)
   - Place pickups with different quantities
   - Space them out for easy testing

4. **Save Level**
   - Click `Save` button (or `Ctrl + S`)
   - Level is now ready for testing

---

## Step 8: In-Game Manual Testing

### 8.1 Launch Game

1. **Start Play Mode**
   - Click `Play` button in toolbar (or press `P`)
   - Or use `Play in Editor` (PIE)
   - Game should launch

2. **Open Output Log**
   - Go to `Window` → `Developer Tools` → `Output Log`
   - Or press `Ctrl + Shift + L`
   - Keep this open to monitor for errors

### 8.2 Test Inventory Toggle

1. **Open Inventory**
   - Press `Tab` key (or configured key for `IA_OpenInventory`)
   - Inventory widget should appear
   - Verify all 50 slots are visible
   - Verify weight and capacity text displays correctly

2. **Close Inventory**
   - Press `Tab` key again
   - Inventory widget should close
   - Game should resume

3. **Repeat Toggle**
   - Open and close inventory multiple times
   - Verify no errors in Output Log
   - Verify widget appears/disappears smoothly

### 8.3 Test Item Pickup

1. **Approach Item Pickup**
   - Walk to an `ItemPickupActor` in the level
   - Get close enough to trigger overlap

2. **Pick Up Item**
   - Press `E` key (or configured key for `IA_Interact`)
   - Item should be added to inventory
   - Pickup actor should disappear
   - Check Output Log for any errors

3. **Verify Item in Inventory**
   - Open inventory (Tab key)
   - Verify item appears in a slot
   - Verify item icon displays correctly
   - Verify quantity displays correctly (if stackable)

4. **Test Multiple Pickups**
   - Pick up multiple different items
   - Verify all items appear in inventory
   - Verify stacking works (if same item type)

### 8.4 Test Item Usage

1. **Use Item from Inventory**
   - Open inventory
   - Right-click on an item slot (or use context menu)
   - Select "Use" from context menu
   - Item should be used (if consumable)
   - Quantity should decrease
   - Check Output Log for usage messages

2. **Test Item Consumption**
   - Use a consumable item (e.g., Health Potion)
   - Verify item effect applies (e.g., health increases)
   - Verify quantity decreases
   - If quantity reaches 0, item should be removed from inventory

3. **Test Usage Validation**
   - Try using item at max health/mana
   - Usage should be prevented (if validation is implemented)
   - Check for appropriate error message

### 8.5 Test Drag and Drop

1. **Drag Item to Empty Slot**
   - Open inventory
   - Click and drag an item to an empty slot
   - Item should move to new slot
   - Original slot should become empty

2. **Swap Two Items**
   - Drag an item onto another item
   - Items should swap positions
   - Both items should remain in inventory

3. **Stack Same Items**
   - Drag an item onto another item of the same type
   - Items should stack (if stackable and space available)
   - Quantity should increase
   - Verify MaxStackSize is respected

4. **Test Drag Cancellation**
   - Start dragging an item
   - Release mouse outside inventory widget
   - Item should return to original slot
   - No changes should occur

### 8.6 Test Stack Splitting

1. **Split Stack via Ctrl+Drag**
   - Open inventory
   - Hold `Ctrl` key
   - Click and drag a stack (quantity > 1)
   - Release on target slot
   - Stack should split (half quantity moves)
   - Original slot quantity should decrease
   - Target slot should have split quantity

2. **Split Stack via Context Menu**
   - Right-click on a stack (quantity > 1)
   - Select "Split" from context menu
   - Stack should split in half
   - Split stack should appear in next available slot (or target slot if dragged)

3. **Test Split Validation**
   - Try splitting stack of quantity 1
   - Split should be prevented
   - Check for appropriate error message

### 8.7 Test Quick-Use Bar

1. **Assign Item to Quick-Use Slot**
   - Open inventory
   - Drag an item to quick-use slot 9 or 10 (bottom of screen)
   - Item should appear in quick-use slot
   - Item icon and quantity should display

2. **Use Item from Quick-Use Slot**
   - Press `9` key (for slot 9) or `0` key (for slot 10)
   - Item should be used
   - Quantity should decrease
   - If quantity reaches 0, slot should clear

3. **Test Quick-Use Slot Updates**
   - Remove item from inventory
   - Quick-use slot should clear automatically
   - Verify slot updates correctly

4. **Test Drag from Quick-Use Slot**
   - Drag item from quick-use slot back to inventory
   - Item should move to inventory
   - Quick-use slot should clear

### 8.8 Test Context Menu

1. **Open Context Menu**
   - Right-click on an inventory slot with an item
   - Context menu should appear at cursor location
   - Menu should show: Use, Drop, Split, Equip buttons

2. **Test Button Visibility**
   - Right-click on consumable item
   - "Equip" button should be hidden
   - "Split" button should be visible (if quantity > 1)
   - Right-click on equipment item
   - "Equip" button should be visible
   - Right-click on stack of quantity 1
   - "Split" button should be hidden

3. **Test Context Menu Actions**
   - Click "Use" button - item should be used
   - Click "Drop" button - item should spawn in world in front of character
   - Click "Split" button - stack should split
   - Click "Equip" button - should prepare for equipment system (Phase 3)

4. **Test Context Menu Closing**
   - Open context menu
   - Click outside menu (within inventory widget)
   - Menu should close
   - Right-click another slot
   - Menu should close and reopen at new location

### 8.9 Test World Item Dropping

1. **Drop Item to World**
   - Open inventory
   - Right-click on an item
   - Select "Drop" from context menu
   - Item should spawn as `ItemPickupActor` in front of character
   - Item should be removed from inventory

2. **Pick Up Dropped Item**
   - Walk to dropped item
   - Press `E` to pick up
   - Item should be added back to inventory
   - Pickup actor should disappear

3. **Test Drop with Full Inventory**
   - Fill inventory to capacity (50 slots)
   - Drop an item
   - Try to pick up dropped item
   - Pickup should fail (inventory full)
   - Appropriate error message should appear

### 8.10 Test Edge Cases

1. **Test Full Inventory**
   - Add items until inventory is full (50 slots)
   - Try to pick up another item
   - Pickup should fail gracefully
   - Check for appropriate error message

2. **Test Weight Limits**
   - Add heavy items until weight limit is reached
   - Try to pick up another heavy item
   - Pickup should fail gracefully
   - Check for appropriate error message

3. **Test Rapid Operations**
   - Rapidly open/close inventory
   - Rapidly pick up items
   - Rapidly use items
   - Verify no crashes or errors
   - Verify all operations complete correctly

4. **Monitor Output Log**
   - Throughout all testing, monitor Output Log
   - Check for any errors or warnings
   - Note any issues for fixing

### 8.11 Stop Play Mode

1. **End Testing**
   - Press `Esc` key or click `Stop` button
   - Return to editor
   - Review any errors or warnings in Output Log

---

## Step 9: Final Verification

### 9.1 Check Output Log

1. **Review Output Log**
   - Open Output Log (`Window` → `Developer Tools` → `Output Log`)
   - Look for any errors (red text)
   - Look for any warnings (yellow text)
   - Note any issues that need fixing

2. **Check for Common Issues**
   - Widget binding warnings
   - Missing reference warnings
   - Compilation errors
   - Runtime errors

### 9.2 Verify All Systems

1. **Inventory System**
   - ✅ Items can be added to inventory
   - ✅ Items can be removed from inventory
   - ✅ Item stacking works
   - ✅ Weight/capacity limits enforced

2. **UI System**
   - ✅ Inventory widget displays correctly
   - ✅ Slot widgets update correctly
   - ✅ Quick-use bar displays correctly
   - ✅ Context menu works correctly

3. **Input System**
   - ✅ Inventory toggle works
   - ✅ Quick-use hotkeys work
   - ✅ Interaction key works

4. **Item System**
   - ✅ Item pickup works
   - ✅ Item usage works
   - ✅ Item dropping works

### 9.3 Document Issues

1. **Create Issue List**
   - List all bugs found during testing
   - Prioritize issues (critical, high, medium, low)
   - Note steps to reproduce
   - Note expected vs actual behavior

2. **Fix Critical Issues**
   - Fix any crashes or data loss issues
   - Fix any major functionality issues
   - Re-test after fixes

---

## Troubleshooting

### Issue: Widget Not Appearing

**Symptoms:** Inventory widget doesn't appear when pressing Tab key

**Solutions:**
1. Check `InventoryWidgetClass` is set in PlayerController
2. Check Input Action `IA_OpenInventory` is bound correctly
3. Check `OnOpenInventory` event is implemented in PlayerController
4. Verify widget is added to viewport in code
5. Check Output Log for errors

### Issue: Items Not Picking Up

**Symptoms:** Pressing E near ItemPickupActor doesn't pick up item

**Solutions:**
1. Check `CollisionComponent` settings (Overlap enabled)
2. Check `OnOverlapBegin` is bound correctly
3. Check InventoryComponent is attached to PlayerCharacter
4. Check inventory has space
5. Check Output Log for errors

### Issue: Drag and Drop Not Working

**Symptoms:** Can't drag items between slots

**Solutions:**
1. Check `NativeOnDragDetected` is implemented
2. Check `ItemDragDropOperation` is created correctly
3. Check `NativeOnDrop` is implemented
4. Verify slot indices are correct
5. Check Output Log for errors

### Issue: Quick-Use Bar Not Working

**Symptoms:** Hotkeys don't use items or slots don't accept items

**Solutions:**
1. Check Input Actions are mapped correctly (9 and 0 keys)
2. Check `OnQuickUseSlot9/10` events are bound
3. Check QuickUseBarWidget is visible and added to viewport
4. Check InventoryComponent quick-use methods are called
5. Check Output Log for errors

### Issue: Context Menu Not Appearing

**Symptoms:** Right-click doesn't show context menu

**Solutions:**
1. Check `OnInventorySlotRightClicked` is implemented
2. Check `ContextMenuWidgetClass` is set in InventoryWidget
3. Check context menu widget is created and added to viewport
4. Check positioning logic (Blueprint implementation)
5. Check Output Log for errors

---

## Verification Checklist

Before considering Phase 2 complete, verify:

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

## Next Steps

Once Day 28 verification is complete:

1. **Fix Any Issues Found**
   - Address all bugs and issues
   - Re-test after fixes

2. **Complete Code Cleanup**
   - Add code comments
   - Remove debug logs
   - Format code

3. **Create Phase 2 Completion Summary**
   - Document all completed features
   - Document any deviations from plan
   - Prepare for Phase 3

4. **Prepare for Phase 3**
   - Review Skill System Design
   - Prepare skill assets
   - Plan Phase 3 implementation

---

**Status:** Complete these manual steps before considering Phase 2 complete

---

**End of Phase 2 Manual Steps - Day 28**
