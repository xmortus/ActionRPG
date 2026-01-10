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

1. **Open Test Level**
   - Open your test level with items
   - Make sure you have stackable items in inventory (quantity > 1)

2. **Test Ctrl+Drag Split**
   - Open inventory (press inventory hotkey)
   - Find a stackable item with quantity > 1
   - Hold `Ctrl` + Left-click and drag the item to an empty slot
   - Verify: Half the stack should move to the new slot
   - Verify: Original slot should have reduced quantity

3. **Test Right-Click Drag Split**
   - Find another stackable item
   - Right-click and drag the item to an empty slot
   - Verify: Half the stack should move to the new slot
   - Verify: Split operation works correctly

4. **Test Split to Full Stack**
   - Try dragging split stack to an existing stack of the same item
   - Verify: Items stack correctly if there's room

5. **Check Output Log**
   - Open `Output Log` (Window → Developer Tools → Output Log)
   - Look for log messages: `UInventoryComponent::SplitStack - Split...`
   - Verify no errors occur during split operations

### Step 2: Test World Item Dropping

#### 2.1 Verify World Drop Functionality

**Note:** World item dropping requires additional UI implementation (e.g., a "Drop" button or key) to trigger `DropItemToWorld`. For now, you can test it via Blueprint Event Graph.

1. **Test via Blueprint (Optional)**
   - Open `BP_ActionRPGPlayerCharacter` Blueprint
   - In Event Graph, create a test function:
     - Get `Inventory Component`
     - Call `Drop Item To World` node
     - Set `Slot Index` to a slot with an item
     - Set `Quantity` (or use full stack)
     - Calculate `World Location` (e.g., player location + forward vector * 150)

2. **Test in Play Mode**
   - Press `Play` button
   - Trigger the drop function (via Blueprint test or custom input)
   - Verify: `ItemPickupActor` spawns in world at specified location
   - Verify: Item appears in world and can be picked up again
   - Verify: Item is removed from inventory (or quantity reduced)

3. **Check Output Log**
   - Look for log messages: `UInventoryComponent::DropItemToWorld - Dropped...`
   - Verify no errors occur during drop operations

**Note:** Full world drop integration (drag outside inventory to drop) can be implemented in Phase 3. The C++ method is functional and ready to use.

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
- [ ] Stack splitting works with right-click drag
- [ ] Split stack creates new item instance correctly
- [ ] Source slot quantity is reduced correctly
- [ ] Split stack appears in new slot
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

- **Stack Splitting:** C++ implementation is complete. Full UI with quantity dialog can be deferred to Phase 3. Basic half-stack split is sufficient for Phase 2.
- **World Item Drop:** C++ method is functional. Full UI integration (drag outside inventory to drop) can be implemented in Phase 3.
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
- **Stack Split:** Ctrl+drag or right-click drag stackable item
- **Hotkey 9:** Press keyboard `9` to use quick-use slot 9
- **Hotkey 0:** Press keyboard `0` to use quick-use slot 10

---

**Status:** Complete these manual steps before proceeding to Day 28 (Final Testing)

---

**End of Phase 2 Manual Steps - Days 26-27**
