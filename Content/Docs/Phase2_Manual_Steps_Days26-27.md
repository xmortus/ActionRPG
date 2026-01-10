# Phase 2 Manual Steps - Days 26-27
**UI Polish & Advanced Features**  
**Date:** 2025-01-07  
**Phase:** Phase 2 - Inventory System  
**Days:** 26-27 - Stack Splitting, World Item Dropping, Quick-Use Bar & UI Polish

---

## Overview

Days 26-27 focus on setting up the advanced inventory features in the Unreal Editor. The C++ code for stack splitting, world item dropping, and quick-use bar system has already been implemented. You will create Input Actions for hotkeys, set up the quick-use bar Blueprint widgets, configure the Input Mapping Context, and polish the UI visuals.

**Note:** All C++ implementation is complete. This guide covers only the Unreal Editor setup steps.

---

## Prerequisites

- ✅ All C++ code compiled successfully
- ✅ Days 24-25 complete (Drag and drop system working)
- ✅ InventoryWidget and InventorySlotWidget working correctly
- ✅ Stack splitting C++ implementation complete
- ✅ World item dropping C++ implementation complete
- ✅ Quick-use bar C++ implementation complete
- ✅ Hotkey handlers implemented in PlayerController
- ✅ QuickUseBarWidget and QuickUseSlotWidget C++ classes created
- ✅ Unreal Editor is open
- ✅ Project is loaded in the editor
- ✅ No compilation errors in Output Log

**Before Starting:**
1. Ensure all C++ code has been compiled successfully
2. Have Unreal Editor ready for Blueprint/UMG editing
3. Ensure test level has items to test all features
4. Have inventory populated with test items (stackable items for splitting tests)
5. Have test consumable items for quick-use bar (e.g., Health Potion)

---

## Day 26: Testing Stack Splitting & World Item Dropping

Since the C++ implementation for stack splitting and world item dropping is already complete, Day 26 focuses on testing these features in the editor and game.

### Step 1: Test Stack Splitting

#### 1.1 Verify Stack Splitting Functionality

**Prerequisites:**
- Have test level loaded with player character
- Have inventory populated with stackable items (items with quantity > 1)
- Know which inventory slot contains stackable items
- Have empty slots available in inventory for testing

1. **Prepare Test Environment**

   1. **Open Test Level**
      - Go to `File` → `Open Level` (or use Content Browser)
      - Navigate to your test level (e.g., `Content/Maps/TestLevel` or similar)
      - Double-click to open the level
      - Wait for level to fully load

   2. **Verify Player Character Exists**
      - Check that `PlayerStart` actor exists in the level
      - Verify `BP_ActionRPGPlayerCharacter` is set as the default pawn in GameMode
      - If needed, open `World Settings` (Window → World Settings)
      - Set `GameMode Override` to your GameMode Blueprint

   3. **Prepare Inventory with Stackable Items**
      - Start Play Mode (`Play` button or `Alt + P`)
      - Open inventory (press inventory hotkey, typically `I` key)
      - Check which slots have items with quantity > 1 (stackable items)
      - Note the slot indices (e.g., slot 0 has 5x Health Potion, slot 2 has 3x Mana Potion)
      - Identify empty slots for testing (e.g., slots 5, 10, 15 are empty)
      - Stop Play Mode (`Esc` or `Stop` button)
      - If you don't have stackable items, you'll need to add them first (via Blueprint or C++)

   4. **Identify Stackable Items**
      - Items with `MaxStackSize > 1` can be stacked
      - Common stackable items: Health Potions, Mana Potions, Arrows, etc.
      - Non-stackable items: Equipment, unique items (MaxStackSize = 1)
      - Verify item's `MaxStackSize` in ItemDataAsset if unsure

2. **Test Ctrl+Drag Split Operation**

   1. **Open Inventory**
      - Press `Play` button to start Play Mode
      - Press inventory hotkey (typically `I` key) to open inventory
      - Inventory widget should appear on screen
      - Verify inventory is visible and slots are displayed

   2. **Locate Stackable Item**
      - Look through inventory slots for an item with quantity > 1
      - Example: Health Potion with quantity 10 in slot 3
      - Verify the item icon and quantity text are visible
      - Note the slot index where the stackable item is located

   3. **Identify Target Slot for Drop**
      - Find an empty slot where you want to drop the split stack
      - Example: Slot 10 is empty and you want to drop split stack there
      - Note the target slot index for verification
      - Ensure target slot is visually empty (no item icon, no quantity)

   4. **Perform Ctrl+Drag Split**
      - Move mouse cursor over the stackable item slot (e.g., slot 3 with 10x Health Potion)
      - Press and hold `Ctrl` key on keyboard
      - While holding `Ctrl`, press and hold `Left Mouse Button` on the stackable item slot
      - While holding both `Ctrl` and `Left Mouse Button`, drag mouse cursor to the target empty slot (e.g., slot 10)
      - Release `Left Mouse Button` while still over the target slot
      - Release `Ctrl` key
      - The drag operation should show visual feedback (item icon following cursor)

   5. **Verify Split Operation Result**
      - **Check Target Slot:** Look at the target slot (e.g., slot 10) where you dropped the item
      - Verify: Item icon appears in target slot
      - Verify: Quantity text shows half the original stack (e.g., if original was 10, split should be 5)
      - Verify: Item is the same type as original (Health Potion icon matches)
      - **Check Source Slot:** Look at the original slot (e.g., slot 3) where you started dragging
      - Verify: Quantity is reduced by split amount (e.g., 10 - 5 = 5 remaining)
      - Verify: Item icon still visible in source slot
      - Verify: Quantity text updated to show remaining quantity

   6. **Verify Exact Slot Placement**
      - Confirm split stack is in the exact slot where you dropped it (e.g., slot 10)
      - Verify split stack is NOT in the next available slot (e.g., NOT in slot 5)
      - This confirms `SplitStackToSlot` method is working correctly
      - If split stack appears in wrong slot, check Output Log for errors

   7. **Test Multiple Split Operations**
      - Repeat Ctrl+drag split with different source and target slots
      - Test splitting from slot 3 to slot 15
      - Test splitting from slot 5 to slot 20
      - Verify each split goes to the exact target slot specified
      - Verify source slots update correctly after each split

3. **Test Normal Drag (Full Stack Move)**

   1. **Find Another Stackable Item**
      - Locate a different stackable item in inventory (or use a different slot)
      - Example: Mana Potion with quantity 8 in slot 7
      - Verify item has quantity > 1 for testing

   2. **Identify Target Empty Slot**
      - Find an empty slot for moving the entire stack
      - Example: Slot 12 is empty
      - Note the target slot index

   3. **Perform Normal Drag (No Ctrl)**
      - Move mouse cursor over the item slot (e.g., slot 7 with 8x Mana Potion)
      - Press and hold `Left Mouse Button` (do NOT hold Ctrl)
      - Drag mouse cursor to the target empty slot (e.g., slot 12)
      - Release `Left Mouse Button` while over the target slot

   4. **Verify Full Stack Move**
      - **Check Target Slot:** Look at target slot (e.g., slot 12)
      - Verify: Entire stack moved (quantity 8 in slot 12)
      - Verify: Item icon matches original item
      - Verify: Quantity text shows full original quantity
      - **Check Source Slot:** Look at original slot (e.g., slot 7)
      - Verify: Slot is now empty (no item icon, no quantity text)
      - Verify: Slot shows empty/default appearance

   5. **Compare with Split Operation**
      - Note the difference: Normal drag moves entire stack, Ctrl+drag splits stack
      - Verify normal drag doesn't use `SplitStackToSlot` (check Output Log)
      - Verify normal drag uses `MoveItem` method instead

4. **Test Split to Existing Stack (Stacking)**

   1. **Prepare Two Slots with Same Item**
      - Have one slot with a stackable item (e.g., 10x Health Potion in slot 3)
      - Have another slot with the same item type (e.g., 5x Health Potion in slot 8)
      - Verify both items are the same type (same ItemID)
      - Check `MaxStackSize` to ensure there's room to stack

   2. **Perform Ctrl+Drag Split to Same Item Slot**
      - Move mouse cursor over the larger stack (e.g., slot 3 with 10x Health Potion)
      - Press and hold `Ctrl` key
      - While holding `Ctrl`, press and hold `Left Mouse Button`
      - Drag to the slot with the same item (e.g., slot 8 with 5x Health Potion)
      - Release `Left Mouse Button` while over slot 8
      - Release `Ctrl` key

   3. **Verify Stacking Result**
      - **Check Target Slot:** Look at slot 8 (target slot)
      - Verify: Quantity increased by split amount (e.g., 5 + 5 = 10, if MaxStackSize allows)
      - Verify: Quantity does not exceed `MaxStackSize` (e.g., if MaxStackSize is 10, stacking stops at 10)
      - Verify: Item icon unchanged (still same item)
      - **Check Source Slot:** Look at slot 3 (source slot)
      - Verify: Quantity reduced by split amount (e.g., 10 - 5 = 5 remaining)
      - Verify: Item still present in source slot (if quantity > 0)

   4. **Test Stacking with MaxStackSize Limit**
      - If target slot is near MaxStackSize (e.g., 8/10), split should only stack up to MaxStackSize
      - Example: Slot has 8x Health Potion, MaxStackSize is 10, you split 5x
      - Result: Slot should have 10x (max), source should have 3x remaining (not all 5x stacked)
      - Verify: Only 2x stacked (8 + 2 = 10), remaining 3x should stay in source or fail

   5. **Test Full Stack (Cannot Stack More)**
      - Try splitting to a slot that's already at MaxStackSize (e.g., 10/10)
      - Verify: Operation fails gracefully (doesn't stack)
      - Verify: Source slot quantity unchanged
      - Verify: Target slot quantity unchanged
      - Check Output Log for warning message

5. **Verify Split Goes to Exact Target Slot**

   1. **Prepare Test Setup**
      - Have a stackable item in slot 3 (e.g., 20x Health Potion)
      - Note which slots are empty (e.g., slots 5, 10, 15 are empty)
      - Plan to split to a specific slot (e.g., slot 15)

   2. **Perform Split to Specific Slot**
      - Ctrl+drag from slot 3 to slot 15 specifically
      - Make sure you drop on slot 15, not slot 5 or slot 10
      - Verify visual feedback shows you're dragging to slot 15

   3. **Verify Exact Placement**
      - Check slot 15: Verify split stack (10x) appears here
      - Check slot 5: Verify this slot is still empty (not used)
      - Check slot 10: Verify this slot is still empty (not used)
      - Check slot 3: Verify remaining quantity (10x) is still here

   4. **Verify SplitStackToSlot Was Used**
      - Open `Output Log` (Window → Developer Tools → Output Log)
      - Look for log message: `UInventoryComponent::SplitStackToSlot - Split 10 from slot 3 to slot 15`
      - Verify message shows correct source slot (3) and target slot (15)
      - Verify message shows correct split quantity (10)
      - If you see `SplitStack` instead, this indicates wrong method was called (bug)

   5. **Test Different Target Slots**
      - Try splitting to slot 5 (verify it goes to slot 5, not slot 10)
      - Try splitting to slot 20 (verify it goes to slot 20, not slot 21)
      - Try splitting to the last slot (e.g., slot 49) to test edge case
      - Verify each split goes to the exact slot specified

6. **Test Edge Cases**

   1. **Test Split with Quantity = 1**
      - Try to split a stack with quantity 1 (can't split)
      - Verify: Operation doesn't work (nothing happens)
      - Verify: Item stays in original slot
      - Check Output Log for warning about invalid split quantity

   2. **Test Split with Ctrl+Drag to Same Slot**
      - Try Ctrl+dragging from slot 3 to slot 3 (same slot)
      - Verify: Operation fails gracefully (nothing happens)
      - Verify: Item quantity unchanged
      - Check Output Log for warning about same slot

   3. **Test Split Non-Stackable Item**
      - Try Ctrl+dragging a non-stackable item (MaxStackSize = 1)
      - Verify: Normal drag behavior (not split)
      - Verify: Item moves to target slot (full move)
      - Verify: No split operation attempted

   4. **Test Split with Inventory Nearly Full**
      - Fill inventory almost to capacity (e.g., 48/50 slots filled)
      - Try to split to an empty slot
      - Verify: Split works correctly even with few empty slots
      - Verify: Split goes to target slot, not next available

   5. **Test Split from Empty Slot (Invalid)**
      - Try Ctrl+dragging from an empty slot
      - Verify: Drag doesn't start (nothing happens)
      - Verify: No errors occur

7. **Check Output Log for Validation**

   1. **Open Output Log**
      - Go to: `Window` → `Developer Tools` → `Output Log`
      - Or press `Ctrl + Shift + L`
      - Output Log window should appear

   2. **Filter Log Messages**
      - Click on `Filters` dropdown in Output Log toolbar
      - Select `Log Temp` to filter for game log messages
      - Or leave filters off to see all messages

   3. **Clear Previous Logs (Optional)**
      - Click `Clear` button in Output Log toolbar
      - Or press `Ctrl + K` to clear log
      - This helps see only new messages from current test session

   4. **Perform Split Operation**
      - With Output Log open, perform a Ctrl+drag split operation
      - Watch the log messages appear in real-time

   5. **Verify Success Messages**
      - Look for log message: `InventorySlotWidget::NativeOnDragDetected - Started SPLIT drag...`
      - Look for log message: `InventoryWidget::HandleItemDrop - Split operation: Moving X/Y from slot A to slot B`
      - Look for log message: `UInventoryComponent::SplitStackToSlot - Split X from slot A to slot B`
      - Verify message shows:
        - Correct split quantity (e.g., 5 if original was 10)
        - Correct source slot index
        - Correct target slot index
      - Example: `Split 5 from slot 3 to slot 15`

   6. **Check for Error Messages**
      - Look for red error messages
      - Common errors:
        - `Invalid slot indices` - Slot numbers out of range
        - `Source slot is empty` - Trying to split from empty slot
        - `Invalid split quantity` - Quantity is 0 or >= source quantity
        - `Item cannot stack` - MaxStackSize <= 1
        - `Failed to create new item instance` - Memory issue (shouldn't happen)

   7. **Check for Warning Messages**
      - Look for yellow warning messages
      - Common warnings (some are expected):
        - `Slot is empty` - Normal if testing edge cases
        - `Invalid split quantity` - Normal if testing invalid inputs
        - `Cannot drop split stack on different item` - Normal if dropping on different item type
        - `Target slot is full` - Normal if stacking to full stack

   8. **Verify No Unexpected Errors**
      - Ensure no crashes occur during split operations
      - Ensure no memory leaks or GC warnings
      - Ensure no null pointer exceptions
      - If errors occur, check the specific error message and troubleshoot accordingly

8. **Compare Normal Drag vs Split Drag Log Messages**

   1. **Test Normal Drag Operation**
      - Perform a normal drag (no Ctrl) from slot A to slot B
      - Check Output Log for messages
      - Look for: `Started NORMAL drag...` (not SPLIT)
      - Look for: `HandleItemDrop - Item moved successfully...` (not split operation)
      - Verify: No `SplitStackToSlot` message appears
      - Verify: `MoveItem` method is used instead

   2. **Test Split Drag Operation**
      - Perform a Ctrl+drag from slot C to slot D
      - Check Output Log for messages
      - Look for: `Started SPLIT drag...` (indicates split mode)
      - Look for: `HandleItemDrop - Split operation: Moving...`
      - Look for: `SplitStackToSlot - Split...`
      - Verify: Split operation message appears

   3. **Compare the Differences**
      - Normal drag: Uses `MoveItem` method, no split quantity calculation
      - Split drag: Uses `SplitStackToSlot` method, calculates split quantity (half stack)
      - Normal drag: `bIsSplitOperation = false` in drag operation
      - Split drag: `bIsSplitOperation = true` in drag operation

9. **Visual Feedback Verification**

   1. **Check Drag Preview**
      - When dragging (normal or Ctrl+drag), verify item icon follows cursor
      - Verify drag preview is visible and matches the item being dragged
      - Verify drag preview shows correct item icon

   2. **Check Target Slot Highlighting**
      - When dragging over a valid drop target (empty slot or same item), verify slot border changes color
      - Valid drop: Green or light border color
      - Invalid drop: Red or dark border color
      - Verify color changes as you move cursor over different slots

   3. **Check Split Quantity Visual**
      - During Ctrl+drag, verify drag preview shows correct quantity (half stack)
      - Example: If dragging 10x Health Potion with Ctrl, preview should show 5x
      - Compare with normal drag preview (shows full quantity)

10. **Performance Testing (Optional)**

    1. **Test Rapid Splits**
       - Perform multiple split operations quickly in succession
       - Verify: No lag or performance issues
       - Verify: All operations complete correctly
       - Verify: No memory leaks or GC warnings

    2. **Test Large Stack Splits**
       - Split very large stacks (e.g., 100x items)
       - Verify: Split operation completes quickly
       - Verify: No performance degradation

    3. **Test Many Split Operations**
       - Perform many split operations (10+ times)
       - Verify: Memory usage stays stable
       - Verify: No accumulation of errors or warnings

### Step 2: Test World Item Dropping (Drag to World)

#### 2.1 Verify Drag-to-World Functionality

**Note:** World item dropping is now fully implemented! You can drag items from the inventory and drop them outside the inventory widget to drop them to the world. The item will be dropped at the mouse cursor location (projected to the ground plane at the player's height). If the mouse location is too far from the player (more than 500 units), the item will be dropped at the maximum distance in that direction.

**Prerequisites:**
- Have test items in inventory
- Have a test level loaded with player character
- Know which inventory slot has an item to test with

1. **Create Blueprint Test Function for World Drop**

   1. **Open PlayerCharacter Blueprint**
      - Navigate to `Content/Blueprints/Characters/` folder
      - Double-click `BP_ActionRPGPlayerCharacter` to open Blueprint Editor

   2. **Open Event Graph**
      - Click `Event Graph` tab (or `Graph` button in top toolbar)
      - Event Graph should be visible

   3. **Create Custom Event for Testing**
      - Right-click in Event Graph
      - Search for: `Custom Event`
      - Select `Add Custom Event`
      - Name the event: `TestDropItemToWorld` (or `TestDropItem`)

   4. **Get Inventory Component**
      - Right-click in Event Graph (away from existing nodes)
      - Search for: `Get Inventory Component`
      - Select `Get Inventory Component` node
      - This node should appear with an output pin for `Inventory Component`

   5. **Call Drop Item To World**
      - Right-click in Event Graph
      - Search for: `Drop Item To World`
      - Select `Drop Item To World` node
      - This node should have the following input pins:
        - `Inventory Component` (object reference)
        - `Slot Index` (integer)
        - `Quantity` (integer)
        - `World Location` (vector)

   6. **Connect Inventory Component**
      - Connect the `Return Value` output from `Get Inventory Component` node
      - To the `Inventory Component` input pin on `Drop Item To World` node
      - Or, if the node auto-connects, verify the connection is correct

   7. **Set Slot Index**
      - Click on `Slot Index` input pin on `Drop Item To World` node
      - In the details panel (or inline), set to a slot number with an item (e.g., `0` for first slot, `1` for second slot, etc.)
      - Or create a variable: Right-click → `Get` → Create variable (integer), set default value, and connect to `Slot Index`

   8. **Set Quantity**
      - Click on `Quantity` input pin
      - Set to a value (e.g., `1` to drop 1 item, or use full stack quantity)
      - To drop entire stack, use the slot's quantity (you may need to get item quantity first)

   9. **Calculate World Location**
      - Right-click in Event Graph
      - Search for: `Get Actor Location`
      - Select `Get Actor Location` node (returns player character's location)
      - Right-click in Event Graph
      - Search for: `Get Actor Forward Vector`
      - Select `Get Actor Forward Vector` node (returns direction player is facing)
      - Right-click in Event Graph
      - Search for: `Vector * Float` or `Multiply (Vector)`
      - Select `Multiply (Vector)` node
      - Connect `Return Value` from `Get Actor Forward Vector` to `A` input of `Multiply`
      - Set `B` input to `150.0` (or desired distance in front of player)
      - Right-click in Event Graph
      - Search for: `Vector + Vector` or `Add (Vector)`
      - Select `Add (Vector)` node
      - Connect `Return Value` from `Get Actor Location` to `A` input of `Add`
      - Connect `Return Value` from `Multiply` to `B` input of `Add`
      - Connect `Return Value` from `Add` to `World Location` input of `Drop Item To World` node

   10. **Add Input Binding (Optional - for easier testing)**
       - Open `Event Graph` (if not already open)
       - Right-click in Event Graph
       - Search for: `InputAction` or your inventory open action
       - Add `Event BeginPlay` if not already present
       - Add a key binding for testing (e.g., press `T` key to test drop)
       - Or connect to existing input action for testing

   11. **Compile Blueprint**
       - Click `Compile` button (top toolbar, or press `F7`)
       - Wait for compilation to complete
       - Look for green checkmark or success message
       - Check for any errors in Output Log

   12. **Save Blueprint**
       - Click `Save` button (top toolbar, or press `Ctrl + S`)
       - Close Blueprint Editor (optional, can keep open for debugging)

2. **Test in Play Mode**

   1. **Start Play Mode**
      - Press `Play` button (top toolbar, or press `Alt + P`)
      - Game should start in viewport

   2. **Verify Player Character Spawned**
      - Check that player character appears in the level
      - Verify player can move around (test basic movement)

   3. **Trigger Test Drop Function**
      - If you bound the test function to a key (e.g., `T`), press that key
      - Or open the inventory first (`I` key) to see current items
      - Note which slot has an item (e.g., slot 0, slot 1, etc.)
      - Press the key bound to your test function
      - Or use Blueprint debugger: Set breakpoint in Event Graph and step through

   4. **Verify Item Spawned in World**
      - Look in the viewport where the item should have spawned
      - The item should appear approximately 150 units in front of the player character
      - Verify the item pickup actor is visible (should have the item's mesh/icon)
      - Check that the item appears at ground level or reasonable height (not floating or underground)

   5. **Verify Item Can Be Picked Up**
      - Move player character close to the dropped item
      - Walk into the item pickup actor
      - Verify: Item is picked up and added back to inventory
      - Check inventory to confirm item was added

   6. **Verify Item Removed from Inventory**
      - Open inventory before dropping item (note item quantity)
      - Drop the item
      - Open inventory again immediately
      - Verify: Item quantity is reduced or item is completely removed from the slot
      - If dropped entire stack: Slot should be empty
      - If dropped partial quantity: Slot should show reduced quantity

   7. **Test Different Quantities**
      - Test dropping 1 item from a stack (quantity should reduce by 1)
      - Test dropping entire stack (slot should become empty)
      - Test dropping from different slots
      - Test dropping different item types (consumable, equipment, etc.)

   8. **Test Multiple Drops**
      - Drop multiple items in sequence
      - Verify: Each item spawns correctly
      - Verify: Items can be picked up individually
      - Verify: Inventory correctly tracks which items were dropped

3. **Check Output Log for Validation**

   1. **Open Output Log**
      - Go to: `Window` → `Developer Tools` → `Output Log`
      - Or press `Ctrl + Shift + L`

   2. **Filter Log Messages**
      - Click on `Filters` dropdown in Output Log
      - Select `Log Temp` (or clear filters to see all messages)
      - Look for messages related to drop operations

   3. **Verify Success Messages**
      - Look for: `UInventoryComponent::DropItemToWorld - Dropped...` messages
      - Message should show:
        - Quantity dropped
        - Item name
        - World location coordinates (X, Y, Z)
      - Example: `Dropped 1 of Health Potion at location (1200.5, 3400.2, 125.0)`

   4. **Check for Errors**
      - Look for any red error messages
      - Common errors to watch for:
        - `Invalid slot index` - Slot number is out of range or empty
        - `World is null` - Context issue (shouldn't happen in play mode)
        - `Item data is null` - Item in slot doesn't have valid ItemDataAsset
        - `Failed to spawn ItemPickupActor` - Actor spawning issue

   5. **Verify No Warnings**
      - Check for yellow warning messages
      - Common warnings:
        - `Slot is empty` - Trying to drop from empty slot
        - `Invalid quantity` - Quantity is 0 or greater than slot quantity
        - These warnings are expected if testing edge cases

   6. **Check ItemPickupActor Spawn Messages**
      - Look for any messages from ItemPickupActor
      - Verify pickup actor initialized correctly
      - Check if item data was set correctly on pickup actor

4. **Test Edge Cases (Optional but Recommended)**

   1. **Test Empty Slot Drop**
      - Try dropping from an empty slot
      - Verify: Operation fails gracefully
      - Check Output Log for warning message
      - Verify: No crash or error state

   2. **Test Invalid Quantity**
      - Try dropping quantity 0
      - Try dropping quantity greater than slot quantity
      - Verify: Operation handles invalid quantities correctly
      - Verify: Uses full stack if quantity is invalid (as per implementation)

   3. **Test Drop Location Validation**
      - Try dropping item very far from player
      - Try dropping item at invalid location (e.g., inside geometry)
      - Verify: Item spawns correctly (UE's collision handling should adjust)
      - Check if item is accessible for pickup

   4. **Test Inventory Full After Drop**
      - Fill inventory to capacity
      - Drop an item
      - Try to pick up the dropped item
      - Verify: Item pickup fails gracefully if inventory is full
      - Verify: Item stays in world for later pickup

**Note:** Drag-to-world functionality is now fully implemented! When you drag an item from the inventory and drop it outside the inventory widget (on the game world), the item will be automatically dropped at the mouse cursor location. See the testing steps below.

#### 2.2 Test Drag-to-World Feature (New!)

**Note:** This feature is now fully implemented! You can drag items from the inventory and drop them outside the inventory widget to drop them to the world.

1. **Open Inventory and Test Drag-to-World**
   - Press `Play` button to start Play Mode
   - Press inventory hotkey (typically `I` key) to open inventory
   - Inventory widget should appear on screen

2. **Locate Item to Drop**
   - Find an item in inventory that you want to drop
   - Verify the item is visible in a slot (e.g., Health Potion in slot 3)

3. **Perform Drag-to-World**
   - Move mouse cursor over the item slot (e.g., slot 3)
   - Press and hold `Left Mouse Button` on the item
   - While holding `Left Mouse Button`, drag the item OUTSIDE the inventory widget (over the game world)
   - Release `Left Mouse Button` while the mouse cursor is over the game world (not over any inventory slot)

4. **Verify Item Dropped to World**
   - Look in the viewport where you dropped the item
   - The item should appear at the mouse cursor location (projected to the ground plane)
   - Verify: `ItemPickupActor` spawns in world at the drop location
   - Verify: Item appears in world and can be picked up again
   - Verify: Item is removed from inventory (or quantity reduced if partial drop)

5. **Test Different Drop Locations**
   - Try dragging items to different locations in the world
   - Verify: Items spawn at the correct locations (at mouse cursor position)
   - Verify: Items are always at ground level (same Z as player character)
   - Verify: Items don't spawn underground or floating

6. **Test Drop Distance Limits**
   - Try dragging items very far from the player character
   - Verify: Items are dropped at maximum 500 units from player (if mouse is too far)
   - Verify: Items are still accessible and visible
   - Verify: Items don't spawn out of bounds or in invalid locations

7. **Test Partial Stack Drop**
   - Try dragging a stackable item with quantity > 1
   - Drop it outside the inventory widget
   - Verify: Entire stack is dropped (normal drag drops full stack)
   - Verify: Slot becomes empty after drop

8. **Test Ctrl+Drag-to-World** (Split Stack Drop)
   - Hold `Ctrl` + Left-click and drag a stackable item (quantity > 1)
   - Drop it outside the inventory widget
   - Verify: Half the stack is dropped to world
   - Verify: Remaining quantity stays in source slot
   - Verify: Split quantity is correct (half of original stack)

9. **Check Output Log**
   - Open `Output Log` (Window → Developer Tools → Output Log)
   - Look for log messages: `ItemDragDropOperation::OnDragCancelled - Drag cancelled, handling world drop`
   - Look for log messages: `InventoryWidget::HandleDragToWorld - Successfully dropped item...`
   - Look for log messages: `UInventoryComponent::DropItemToWorld - Dropped...`
   - Verify no errors occur during drop operations

**Note:** The drag-to-world feature automatically handles screen-to-world coordinate conversion. The item is dropped at the mouse cursor location projected onto the ground plane at the player's height. If the mouse location is more than 500 units away from the player, the item is dropped at the maximum distance (500 units) in that direction.

---

## Day 27: Quick-Use Bar Setup & UI Polish

### Step 1: Create Input Actions for Quick-Use Bar Hotkeys

#### 1.1 Create Quick-Use Slot 9 Input Action

1. **Navigate to Content Browser**
   - Go to `Content/Input/` folder
   - If folder doesn't exist, create it (Right-click → `New Folder` → Name: `Input`)

2. **Create Input Action**
   - Right-click in Content Browser
   - Select `Input` → `Input Action`
   - Name: `IA_QuickUseSlot9`

3. **Configure Input Action**
   - Open `IA_QuickUseSlot9`
   - Set `Value Type` to `Digital (bool)`
   - Leave other settings as default

4. **Save Asset**
   - Press `Ctrl + S` to save

#### 1.2 Create Quick-Use Slot 10 Input Action

1. **Create Second Input Action**
   - Right-click in Content Browser
   - Select `Input` → `Input Action`
   - Name: `IA_QuickUseSlot10`

2. **Configure Input Action**
   - Open `IA_QuickUseSlot10`
   - Set `Value Type` to `Digital (bool)`
   - Leave other settings as default

3. **Save Asset**
   - Press `Ctrl + S` to save

### Step 2: Configure Input Mapping Context

#### 2.1 Open Input Mapping Context

1. **Locate Input Mapping Context**
   - Navigate to `Content/Input/` folder
   - Open `IM_ActionRPG` (or your main input mapping context)
   - If it doesn't exist, create it:
     - Right-click → `Input` → `Input Mapping Context`
     - Name: `IM_ActionRPG`

#### 2.2 Add Quick-Use Slot 9 Mapping

1. **Add Mapping**
   - In `IM_ActionRPG`, click `+` button in `Mappings` array
   - Set `Action` to `IA_QuickUseSlot9` (drag from Content Browser or select from dropdown)
   - Click `+` under `Key` to add key binding
   - Select `Keyboard` → `Nine` (keyboard key `9`)

2. **Verify Mapping**
   - Mapping should show: `IA_QuickUseSlot9` → `Nine`

#### 2.3 Add Quick-Use Slot 10 Mapping

1. **Add Second Mapping**
   - Click `+` button in `Mappings` array again
   - Set `Action` to `IA_QuickUseSlot10`
   - Click `+` under `Key` to add key binding
   - Select `Keyboard` → `Zero` (keyboard key `0`)

2. **Verify Mapping**
   - Mapping should show: `IA_QuickUseSlot10` → `Zero`

#### 2.4 Save Input Mapping Context

1. **Save Asset**
   - Press `Ctrl + S` to save
   - Close the asset editor

### Step 3: Assign Input Actions to PlayerController Blueprint

#### 3.1 Open PlayerController Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder (or wherever your PlayerController Blueprint is located)

2. **Open PlayerController Blueprint**
   - Open `BP_ActionRPGPlayerController` (or your PlayerController Blueprint)

#### 3.2 Assign Input Actions

1. **Open Class Defaults**
   - Click `Class Defaults` button (top toolbar)
   - Or select `Class Defaults` in the viewport dropdown

2. **Find Input Category**
   - In `Details` panel, scroll to find `Input` category
   - You should see:
     - `Quick Use Slot 9 Action` (TObjectPtr<UInputAction>)
     - `Quick Use Slot 10 Action` (TObjectPtr<UInputAction>)

3. **Assign Input Actions**
   - Set `Quick Use Slot 9 Action` to `IA_QuickUseSlot9` (drag from Content Browser or select from dropdown)
   - Set `Quick Use Slot 10 Action` to `IA_QuickUseSlot10`

4. **Compile Blueprint**
   - Click `Compile` button (top toolbar)
   - Should compile without errors
   - Look for green checkmark

5. **Save Blueprint**
   - Click `Save` button (top toolbar)
   - Or press `Ctrl + S`

6. **Close Blueprint Editor**

### Step 4: Create Quick-Use Bar Blueprint Widget

#### 4.1 Create Quick-Use Bar Widget Folder

1. **Navigate to Content Browser**
   - Go to `Content/UI/` folder
   - If folder doesn't exist, create it

2. **Create QuickUse Subfolder**
   - Right-click on `UI` folder
   - Select `New Folder`
   - Name: `QuickUse`

#### 4.2 Create Quick-Use Slot Widget Blueprint

1. **Create Widget Blueprint**
   - Navigate to `Content/UI/QuickUse/` folder
   - Right-click in Content Browser
   - Select `User Interface` → `Widget Blueprint`
   - In "Pick Parent Class" dialog:
     - Search for: `QuickUseSlotWidget`
     - Select `Quick Use Slot Widget` (your C++ class)
     - Click `Select`

2. **Name the Blueprint**
   - Name: `WBP_QuickUseSlotWidget`
   - Save in `Content/UI/QuickUse/`

3. **Design Slot Widget**
   - Double-click `WBP_QuickUseSlotWidget` to open Widget Designer
   - Root should be a `Canvas Panel` or `Border`

4. **Add Widget Elements**
   - Add `Border` widget (for slot background)
     - Name: `SlotBorder` (must match C++ property name)
     - Set size (e.g., 64x64)
     - Style with background color/border
   - Add `Image` widget (for item icon)
     - Name: `ItemIcon` (must match C++ property name)
     - Position inside Border
     - Size: Fill border or slightly smaller
   - Add `Text Block` widget (for quantity)
     - Name: `QuantityText` (must match C++ property name)
     - Position: Bottom-right corner
     - Font size: 12-14
     - Color: White or contrasting color
   - Add `Text Block` widget (for hotkey label)
     - Name: `HotkeyText` (must match C++ property name)
     - Position: Top-left or top-center
     - Font size: 10-12
     - Color: Light gray or white
     - This will show "9" or "0" for consumable slots

5. **Bind Widget References**
   - Verify widget names match C++ property names exactly:
     - `SlotBorder`
     - `ItemIcon`
     - `QuantityText`
     - `HotkeyText`

6. **Style the Widget**
   - Set default border color (gray)
   - Add hover effect (optional)
   - Ensure empty state is clearly visible

7. **Compile and Save**
   - Click `Compile` button
   - Click `Save` button
   - Close Widget Designer

#### 4.3 Create Quick-Use Bar Widget Blueprint

1. **Create Widget Blueprint**
   - In `Content/UI/QuickUse/` folder
   - Right-click in Content Browser
   - Select `User Interface` → `Widget Blueprint`
   - In "Pick Parent Class" dialog:
     - Search for: `QuickUseBarWidget`
     - Select `Quick Use Bar Widget` (your C++ class)
     - Click `Select`

2. **Name the Blueprint**
   - Name: `WBP_QuickUseBarWidget`
   - Save in `Content/UI/QuickUse/`

3. **Design Quick-Use Bar Layout**
   - Double-click `WBP_QuickUseBarWidget` to open Widget Designer
   - Root should be a `Canvas Panel` or `Horizontal Box`

4. **Add Uniform Grid Panel**
   - Add `Uniform Grid Panel` widget
     - Name: `QuickUseGrid` (must match C++ property name)
     - Set to horizontal layout (10 columns, 1 row)
     - Size: Width ~640px (10 slots × 64px), Height ~64px
     - Position: Center of screen (for now, will be positioned in HUD)

5. **Bind Widget References**
   - Select root widget (Canvas Panel)
   - In `Details` panel, verify `QuickUseGrid` is bound to the Uniform Grid Panel

6. **Set Slot Widget Class**
   - Select root widget
   - In `Details` panel, find `Slot Widget Class` property
   - Set to `WBP_QuickUseSlotWidget` (select from dropdown)

7. **Style the Bar** (Optional)
   - Add background panel behind the grid
   - Add border or frame around the bar
   - Style to match your game's UI theme

8. **Compile and Save**
   - Click `Compile` button
   - Click `Save` button
   - Close Widget Designer

### Step 5: Add Quick-Use Bar to HUD

#### 5.1 Open or Create HUD Blueprint

1. **Locate HUD Blueprint**
   - Navigate to `Content/Blueprints/UI/` or `Content/UI/` folder
   - Open `BP_ActionRPGHUD` (or create if doesn't exist)
   - To create: Right-click → `Blueprint Class` → Parent: `HUD`

#### 5.2 Add Quick-Use Bar to HUD

1. **Open HUD Blueprint**
   - Double-click `BP_ActionRPGHUD` to open Blueprint Editor

2. **Add Quick-Use Bar in Event Graph**
   - Open `Event Graph` tab
   - Find or create `Event BeginPlay` node

3. **Create Widget**
   - Right-click in graph
   - Search for: `Create Widget`
   - Select `Create Widget` node
   - Set `Class` to `WBP_QuickUseBarWidget` (drag from Content Browser)

4. **Add to Viewport**
   - Connect `Create Widget` output to `Add to Viewport` node
   - Right-click → Search: `Add to Viewport`
   - Connect `Return Value` from `Create Widget` to `Target` of `Add to Viewport`
   - Set `ZOrder` to appropriate value (e.g., 100 to keep it above other widgets)

5. **Position Widget** (Optional)
   - If you want to position it manually:
     - Use `Set Position in Viewport` node
     - Calculate bottom-center position:
       - X: (Viewport Size X / 2) - (Widget Width / 2)
       - Y: Viewport Size Y - Widget Height - 20 (20px margin from bottom)

6. **Alternative: Use Anchor Points**
   - In `WBP_QuickUseBarWidget`, set root widget anchor to bottom-center
   - Use offset to position from anchor point
   - This method is more responsive to screen size changes

7. **Compile and Save**
   - Click `Compile` button
   - Click `Save` button
   - Close Blueprint Editor

### Step 6: Test Quick-Use Bar Functionality

#### 6.1 Test Hotkey Functionality

1. **Open Test Level**
   - Open your test level
   - Make sure player character has consumable items in inventory

2. **Assign Item to Quick-Use Slot** (Via Blueprint Test)
   - Open `BP_ActionRPGPlayerCharacter` Blueprint
   - In Event Graph, create test function:
     - Get `Inventory Component`
     - Call `Assign Item To Quick Use Slot` node
     - Set `Inventory Slot Index` to a slot with a consumable item
     - Set `Quick Use Slot Index` to `8` (for slot 9) or `9` (for slot 10)
     - Verify: Only consumable items can be assigned to slots 8-9

3. **Test Hotkey 9**
   - Press `Play` button
   - Make sure a consumable item is assigned to quick-use slot 9 (index 8)
   - Press keyboard key `9`
   - Verify: Item is used from inventory
   - Check Output Log for: `PlayerController::OnQuickUseSlot9 - Used quick-use slot 9`

4. **Test Hotkey 0**
   - Assign a consumable item to quick-use slot 10 (index 9)
   - Press keyboard key `0`
   - Verify: Item is used from inventory
   - Check Output Log for: `PlayerController::OnQuickUseSlot10 - Used quick-use slot 10`

5. **Test Slot Clearing**
   - Use item until quantity reaches 0
   - Verify: Quick-use slot clears automatically
   - Check Output Log for: `UInventoryComponent::ClearQuickUseSlot - Cleared quick-use slot...`

6. **Test UI Updates** (If Quick-Use Bar Widget is visible)
   - Assign item to quick-use slot
   - Verify: Item icon appears in quick-use bar widget
   - Verify: Quantity is displayed correctly
   - Use item and verify UI updates

#### 6.2 Test Slot Validation

1. **Test Non-Consumable Assignment**
   - Try assigning a non-consumable item to slots 9-10 (indices 8-9)
   - Verify: Assignment fails (check Output Log for warning)
   - Only consumable items should be assignable

2. **Test Skill Slot Assignment** (Phase 2 - Should Fail)
   - Try assigning item to slots 1-8 (indices 0-7)
   - Verify: Assignment fails (skill slots not available in Phase 2)
   - Check Output Log for: `Skill slots not available in Phase 2`

### Step 7: Optional UI Polish

#### 7.1 Polish Inventory Widget

1. **Open Inventory Widget Blueprint**
   - Open `WBP_InventoryWidget`

2. **Add Background**
   - Add `Image` or `Border` widget as background
   - Set semi-transparent background color
   - Add subtle border or frame

3. **Style Slot Widgets**
   - Open `WBP_InventorySlotWidget`
   - Enhance border styling
   - Add hover effects (border color change, scale, etc.)
   - Add empty state styling (grayed out appearance)

4. **Add Visual Feedback**
   - Add animation for slot updates (fade in/out, scale pulse)
   - Add visual feedback for drag operations
   - Enhance quantity text styling

5. **Compile and Save**
   - Compile all modified widgets
   - Save all changes

#### 7.2 Polish Quick-Use Bar Widget

1. **Open Quick-Use Bar Widget**
   - Open `WBP_QuickUseBarWidget`

2. **Style the Bar**
   - Add background panel with semi-transparent background
   - Add border or frame around the bar
   - Style to match inventory widget theme

3. **Style Slot Widgets**
   - Open `WBP_QuickUseSlotWidget`
   - Enhance styling to match inventory slots
   - Add hover effects
   - Add pressed/active state for hotkey feedback

4. **Add Hotkey Labels**
   - Ensure hotkey labels (9, 0) are clearly visible
   - Style labels to be readable but not intrusive

5. **Compile and Save**
   - Compile all modified widgets
   - Save all changes

#### 7.3 Add Item Tooltip (Optional)

1. **Create Tooltip Widget Blueprint**
   - Navigate to `Content/UI/Tooltips/` (create folder if needed)
   - Create `WBP_ItemTooltipWidget` Blueprint
   - Design tooltip layout:
     - Item name (large text)
     - Item description (small text)
     - Item stats (optional)
     - Item type/rarity (optional)

2. **Implement Hover Detection** (Requires C++ extension)
   - This would require adding hover event handlers to `InventorySlotWidget`
   - For Phase 2, this is optional and can be deferred to Phase 3

3. **Test Tooltip**
   - If implemented, test hover over inventory slots
   - Verify tooltip appears and displays correct item information

---

## Troubleshooting

### Issue: Hotkeys Not Working

**Symptoms:** Pressing 9 or 0 doesn't activate quick-use slots

**Solutions:**
1. **Check Input Actions**
   - Verify `IA_QuickUseSlot9` and `IA_QuickUseSlot10` are created
   - Verify Input Actions have `Value Type` set to `Digital (bool)`

2. **Check Input Mapping Context**
   - Verify `IM_ActionRPG` has mappings for both Input Actions
   - Verify keys are mapped correctly (`Nine` and `Zero`)
   - Verify Input Mapping Context is added to Local Player's Enhanced Input Subsystem

3. **Check PlayerController Blueprint**
   - Verify Input Actions are assigned in Class Defaults
   - Verify `QuickUseSlot9Action` and `QuickUseSlot10Action` are set
   - Check Blueprint compiles without errors

4. **Check Enhanced Input Setup**
   - Verify Enhanced Input is enabled in project settings
   - Verify Input Mapping Context is added to player's Input Subsystem
   - Check `SetupInputComponent` is called (should be automatic in C++)

### Issue: Quick-Use Bar Widget Not Visible

**Symptoms:** Quick-use bar doesn't appear on screen

**Solutions:**
1. **Check Widget Creation**
   - Verify `WBP_QuickUseBarWidget` is created in HUD's `BeginPlay`
   - Verify `Add to Viewport` is called
   - Check `ZOrder` is high enough to be visible

2. **Check Widget Size**
   - Verify widget has explicit size set
   - Check anchors and offsets are set correctly
   - Verify widget is positioned on screen

3. **Check Widget Visibility**
   - Verify widget visibility is set to `Visible` or `Hit Test Invisible`
   - Check if widget is being hidden by other widgets

4. **Check Slots Initialization**
   - Verify `SlotWidgetClass` is set in Blueprint
   - Check if `InitializeSlots` is called in `NativeConstruct`
   - Verify `QuickUseGrid` widget is bound correctly

### Issue: Split Stack Not Going to Target Slot

**Symptoms:** When splitting a stack with Ctrl+drag, the split stack goes to the next available slot instead of the slot where it was dropped

**Solutions:**
1. **Check HandleItemDrop Method**
   - Verify `HandleItemDrop` calls `SplitStackToSlot` (not `SplitStack`) for split operations
   - Check that `bIsSplitOperation` flag is set correctly in ItemDragDropOperation
   - Verify target slot index is passed correctly to `SplitStackToSlot`

2. **Check SplitStackToSlot Method**
   - Verify `SplitStackToSlot` method exists in InventoryComponent
   - Check method places split stack directly in target slot (not using FindEmptySlot)
   - Verify method handles empty target slots correctly
   - Verify method handles stacking to existing stacks correctly

3. **Check Drag Operation**
   - Verify `NativeOnDragDetected` sets `bIsSplitOperation = true` when Ctrl is held
   - Check split quantity is calculated correctly (should be half the stack)
   - Verify `SourceQuantity` is set to full stack quantity

4. **Check Output Log**
   - Look for: `UInventoryComponent::SplitStackToSlot - Split...` messages
   - Verify no errors or warnings related to split operations
   - Check that target slot index matches where you dropped the item

### Issue: RemoveFromParent Warning

**Symptoms:** Getting warning: `UWidget::RemoveFromParent() called on widget which has no UMG parent`

**Solutions:**
1. **Check Widget Lifecycle**
   - Verify widgets added via `AddToViewport()` use visibility toggling instead of `RemoveFromParent()`
   - Check `OnOpenInventory` uses `SetVisibility(ESlateVisibility::Collapsed)` instead of `RemoveFromParent()`
   - Verify `CloseInventory()` uses visibility toggling (not `RemoveFromParent()`)

2. **Widget Management**
   - Widgets added via `AddToViewport()` should stay in viewport and toggle visibility
   - Use `Collapsed` visibility instead of `Hidden` to prevent layout space issues
   - Widget is created once and reused for efficiency

3. **Check Code**
   - Verify `ActionRPGPlayerController::OnOpenInventory` checks `IsInViewport()` before toggling
   - Check `InventoryWidget::CloseInventory()` doesn't call `RemoveFromParent()` on viewport widgets
   - Ensure widget lifecycle is managed by PlayerController, not by widget itself

### Issue: Items Can't Be Assigned to Quick-Use Slots

**Symptoms:** Drag and drop to quick-use slots doesn't work

**Solutions:**
1. **Check Item Type**
   - Verify only consumable items can be assigned
   - Check `ItemDataAsset` has `ItemType` set to `Consumable`
   - Verify item type validation in C++ code

2. **Check Slot Index**
   - Verify only slots 8-9 (quick-use slots 9-10) accept items
   - Check `AssignItemToQuickUseSlot` is called with correct indices

3. **Check Inventory Slot**
   - Verify inventory slot has an item
   - Check inventory slot is not empty

### Issue: Quick-Use Slot Not Clearing After Use

**Symptoms:** Item is used but slot still shows item

**Solutions:**
1. **Check Event Binding**
   - Verify `OnQuickUseSlotChanged` event fires
   - Check if widget is listening to the event
   - Verify `RefreshSlot` is called when event fires

2. **Check Quantity Check**
   - Verify `UseQuickUseSlot` checks if inventory slot is empty
   - Check `ClearQuickUseSlot` is called when item removed
   - Verify quantity reaches 0 after use

---

## Verification Checklist

Before moving to Day 28 (Final Testing), verify:

- [ ] Stack splitting works with Ctrl+drag
- [ ] Normal drag (without Ctrl) moves entire stack
- [ ] Split stack creates new item instance correctly
- [ ] Source slot quantity is reduced correctly
- [ ] Split stack appears in the exact target slot where it was dropped (not next available slot)
- [ ] Split stack can be dropped on empty slots correctly
- [ ] Split stack can be stacked on existing stacks of the same item (if space available)
- [ ] World item drop method is functional (via Blueprint test)
- [ ] Dropped items spawn correctly in world
- [ ] Item is removed from inventory when dropped
- [ ] Input Actions `IA_QuickUseSlot9` and `IA_QuickUseSlot10` are created
- [ ] Input Mapping Context has correct key mappings (9 and 0)
- [ ] PlayerController Blueprint has Input Actions assigned
- [ ] Hotkeys 9-0 activate quick-use slots correctly
- [ ] Items are consumed when hotkey pressed
- [ ] Only consumable items can be assigned to slots 9-10
- [ ] Quick-use slots clear when item is consumed/removed
- [ ] Quick-Use Bar Widget Blueprint created (`WBP_QuickUseBarWidget`)
- [ ] Quick-Use Slot Widget Blueprint created (`WBP_QuickUseSlotWidget`)
- [ ] Quick-Use Bar is added to HUD and visible
- [ ] Quick-use bar displays item icons and quantities correctly
- [ ] UI visual polish applied (optional)
- [ ] Item tooltip displays on hover (optional)
- [ ] No errors in Output Log during testing
- [ ] All edge cases handled correctly
- [ ] Ready for final testing (Day 28)

---

## Next Steps

Once Days 26-27 are complete:

1. **Day 28** will focus on:
   - Final integration testing
   - Performance testing
   - Bug fixes
   - Code cleanup
   - Documentation

2. **Before Day 28**, you should have:
   - Stack splitting functional and tested
   - World item dropping working (C++ method ready, UI integration optional)
   - Quick-use bar system functional
   - Hotkeys working for consumable slots (9-0)
   - Quick-Use Bar Widget visible in game (optional)
   - UI polish applied (optional features)
   - Ready for final testing and cleanup

---

## Notes

- **Stack Splitting:** C++ implementation is complete with two methods:
  - `SplitStack(int32 SlotIndex, int32 SplitQuantity)` - Places split stack in next available slot (for general use)
  - `SplitStackToSlot(int32 SourceSlotIndex, int32 TargetSlotIndex, int32 SplitQuantity)` - Places split stack directly in target slot (for drag and drop operations)
  - Split stack goes directly to the slot where user drops it, not the next available slot
  - Full UI with quantity dialog can be deferred to Phase 3. Basic half-stack split is sufficient for Phase 2.
- **Widget Lifecycle Management:** Widgets added via `AddToViewport()` use visibility toggling (`SetVisibility(ESlateVisibility::Collapsed)`) instead of `RemoveFromParent()` to avoid "no UMG parent" warnings. Widgets are created once and reused for efficiency.
- **World Item Drop:** Fully implemented! Drag items outside the inventory widget to drop them to the world at the mouse cursor location. Supports both normal drag (full stack) and Ctrl+drag (split stack) operations.
- **Quick-Use Bar:** C++ methods and hotkey support are complete. Full UI implementation is optional for Phase 2. Hotkeys work independently of UI.
- **Quick-Use Slots 1-8:** These are prepared for Phase 3 (skills). They should be visible but disabled/non-functional in Phase 2.
- **Item Tooltip:** Basic tooltip is optional for Phase 2. Can be fully implemented in Phase 3.
- **UI Polish:** Basic visual polish is sufficient for Phase 2. Advanced animations and effects can be added in Phase 3.

---

## Quick Reference

### Key Input Actions
- `IA_QuickUseSlot9` → Keyboard `9` (Quick-use slot 9, index 8)
- `IA_QuickUseSlot10` → Keyboard `0` (Quick-use slot 10, index 9)

### Key Blueprint Widgets
- `WBP_QuickUseBarWidget` - Main quick-use bar widget
- `WBP_QuickUseSlotWidget` - Individual quick-use slot widget

### Key Blueprint Files
- `BP_ActionRPGPlayerController` - PlayerController with Input Action assignments
- `BP_ActionRPGHUD` - HUD that creates and displays Quick-Use Bar Widget
- `IM_ActionRPG` - Input Mapping Context with hotkey mappings

### Quick-Use Slot Indices
- **Slots 1-8** (indices 0-7): Skills (Phase 3, prepared but not functional in Phase 2)
- **Slot 9** (index 8): Consumable (Hotkey: 9)
- **Slot 10** (index 9): Consumable (Hotkey: 0)

### Test Commands
- **Stack Split:** Ctrl+drag stackable item (splits stack in half) - Split goes to exact drop location
- **Move Stack:** Normal drag (without Ctrl) moves entire stack
- **Split to Empty Slot:** Ctrl+drag to empty slot places split stack in that exact slot
- **Split to Same Item:** Ctrl+drag to existing stack of same item attempts to stack (respects MaxStackSize)
- **Hotkey 9:** Press keyboard `9` to use quick-use slot 9
- **Hotkey 0:** Press keyboard `0` to use quick-use slot 10

### Key C++ Methods (Days 26-27)
- `SplitStack(int32 SlotIndex, int32 SplitQuantity)` - Split stack to next available slot
- `SplitStackToSlot(int32 SourceSlotIndex, int32 TargetSlotIndex, int32 SplitQuantity)` - Split stack directly to target slot (used for drag operations)
- `DropItemToWorld(int32 SlotIndex, int32 Quantity, const FVector& WorldLocation)` - Drop item to world

---

**Status:** Complete these manual steps before proceeding to Day 28 (Final Testing)

---

**End of Phase 2 Manual Steps - Days 26-27**
