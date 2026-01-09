# Phase 2 Manual Steps - Days 19-20
**After Code Implementation**  
**Date:** 2025-01-07  
**Phase:** Phase 2 - Inventory System  
**Days:** 19-20 - Item Usage System

---

## Overview

After completing the C++ code implementation for Days 19-20, you need to perform the following manual steps in the Unreal Editor to complete the Item Usage System setup. These steps involve testing item usage, configuring item effects, and ensuring proper integration with the character system.

---

## Prerequisites

- ✅ All C++ code compiled successfully
- ✅ Days 15-18 complete (InventoryComponent and ItemPickupActor functional)
- ✅ InventoryComponent attached to PlayerCharacter
- ✅ ItemPickupActor working and items can be picked up
- ✅ Test Item Data Assets created (HealthPotion, etc.)
- ✅ PlayerCharacter has stats system (Health, Mana, Stamina) - if using consumables
- ✅ Unreal Editor is open
- ✅ Project is loaded in the editor
- ✅ No compilation errors in Output Log

**Before Starting:**
1. Close Visual Studio (if open)
2. Return to Unreal Editor
3. Wait for Hot Reload to complete (if automatic)
4. If Hot Reload failed, restart Unreal Editor

---

## Step 1: Verify C++ Code Compiled

### 1.1 Check Compilation Status

1. **Open Output Log**
   - Window → `Developer Tools` → `Output Log`
   - Or press `Ctrl + Shift + L`

2. **Check for Errors**
   - Look for any compilation errors
   - If errors exist, fix them in Visual Studio first
   - Common issues:
     - Missing includes
     - Typo in class names
     - Missing forward declarations
     - UseItem implementation errors

3. **Verify Hot Reload**
   - Look for message: `LogHotReload: Hot reload completed successfully`
   - If hot reload failed, close and reopen Unreal Editor

### 1.2 Verify UseItem Function

1. **Check InventoryComponent Blueprint**
   - Navigate to `Content/Blueprints/Components/Inventory/`
   - Open `BP_InventoryComponent`
   - In the Details Panel, verify `UseItem` function is visible
   - It should be under the "Inventory" category

2. **If Function Doesn't Appear:**
   - Recompile C++ code in Visual Studio
   - Restart Unreal Editor
   - Try again

---

## Step 2: Test Basic Item Usage

### 2.1 Prepare Test Environment

1. **Open Test Level**
   - Open your test level (or create a new one)
   - Ensure PlayerCharacter is placed in the level

2. **Add Items to Inventory (Via Blueprint)**
   - Option A: Use ItemPickupActor in level
   - Option B: Manually add items via Blueprint
   - Option C: Use console commands (if implemented)

### 2.2 Test UseItem via Blueprint

1. **Create Test Blueprint**
   - Navigate to `Content/Blueprints/`
   - Create a new Blueprint (or use existing test Blueprint)
   - Add an Event (e.g., "Event BeginPlay" or custom event)

2. **Call UseItem Function**
   - Get reference to PlayerCharacter
   - Get InventoryComponent from PlayerCharacter
   - Call `UseItem` function with slot index (e.g., 0)
   - Add logging to verify function is called

3. **Test in PIE (Play In Editor)**
   - Press `Alt + P` to start Play In Editor
   - Trigger the event that calls UseItem
   - Check Output Log for usage messages
   - Verify item quantity decreases (for consumables)
   - Verify item is removed when quantity = 0

### 2.3 Verify Console Logs

1. **Check Output Log**
   - Look for messages like:
     - `InventoryComponent::UseItem - Used item: [ItemName]`
     - `ItemBase::Use - Item used: [ItemName]`
   - Verify no error messages appear

2. **Test Different Scenarios**
   - Use item from slot 0
   - Use item from slot 1
   - Try to use from empty slot (should fail gracefully)
   - Use consumable item multiple times until quantity = 0

---

## Step 3: Configure Item Effects (Consumables)

### 3.1 Set Up Character Stats (If Not Already Done)

1. **Check PlayerCharacter Blueprint**
   - Open `BP_ActionRPGPlayerCharacter`
   - Verify stats variables exist:
     - Health (float)
     - MaxHealth (float)
     - Mana (float) - optional
     - MaxMana (float) - optional
     - Stamina (float) - optional
     - MaxStamina (float) - optional

2. **If Stats Don't Exist:**
   - Add float variables for each stat
   - Initialize in BeginPlay
   - Add getters/setters if needed

### 3.2 Connect Item Usage to Stats

1. **Bind to OnItemUsed Event**
   - In PlayerCharacter Blueprint or a separate component
   - Get InventoryComponent reference
   - Bind to `OnItemUsed` event
   - Check item type and apply effects

2. **Example Blueprint Setup:**
   ```
   Event BeginPlay
   → Get InventoryComponent
   → Bind Event to OnItemUsed
   
   OnItemUsed Event
   → Get Item from event
   → Get ItemData from Item
   → Switch on ItemType
   → If Consumable:
      → Get ItemData properties (HealingAmount, etc.)
      → Apply to character stats
      → Play sound effect
      → Spawn particle effect
   ```

3. **For Health Potion Example:**
   - Check if ItemID == "HealthPotion"
   - Get healing amount from ItemData (or hardcode for now)
   - Add to current Health
   - Clamp Health to MaxHealth
   - Log healing amount

### 3.3 Add Visual/Audio Feedback

1. **Sound Effects (Optional)**
   - Create or import sound effect for item use
   - In OnItemUsed event, play sound at player location
   - Use `Play Sound at Location` node

2. **Particle Effects (Optional)**
   - Create or import particle effect for item use
   - In OnItemUsed event, spawn particle at player location
   - Use `Spawn Emitter at Location` node

3. **UI Feedback (Optional)**
   - Show floating text (e.g., "+50 Health")
   - Update health bar if UI exists
   - Flash inventory slot briefly

---

## Step 4: Test Item Type Handling

### 4.1 Test Consumable Items

1. **Create Test Consumable**
   - Ensure you have a consumable item (e.g., HealthPotion)
   - Item Type should be set to "Consumable" in ItemDataAsset
   - Add item to inventory

2. **Test Usage**
   - Call UseItem on consumable slot
   - Verify:
     - Item quantity decreases by 1
     - Item is removed when quantity = 0
     - OnItemUsed event fires
     - Effects are applied (if connected)

3. **Test Edge Cases**
   - Use last item in stack (quantity should go to 0, item removed)
   - Try to use item with quantity = 0 (should fail)
   - Use item from invalid slot (should fail gracefully)

### 4.2 Test Equipment Items

1. **Create Test Equipment**
   - Ensure you have an equipment item (e.g., Weapon, Armor)
   - Item Type should be set to "Equipment" in ItemDataAsset
   - Add item to inventory

2. **Test Usage**
   - Call UseItem on equipment slot
   - Verify:
     - Item is NOT consumed (quantity stays same)
     - OnItemUsed event fires
     - Equipment system can handle it (Phase 3)
   - For now, equipment items may just log usage

### 4.3 Test Other Item Types

1. **Test Skill Items**
   - Create test skill item
   - Item Type should be set to "Skill Item"
   - Use item (should not consume, prepare for Phase 3)

2. **Test Misc Items**
   - Create test misc item
   - Item Type should be set to "Miscellaneous"
   - Use item (should not consume, just log usage)

---

## Step 5: Add Right-Click Use Support (Optional)

### 5.1 Create Right-Click Handler

1. **In Inventory UI (When Implemented)**
   - Add right-click event to inventory slot widget
   - On right-click, call UseItem with slot index
   - This will be implemented in Day 22-23 (Inventory UI)

2. **For Now (Testing)**
   - Create a simple test Blueprint
   - Add right-click input action
   - Call UseItem on slot 0 (for testing)

### 5.2 Test Right-Click Usage

1. **In PIE**
   - Right-click on inventory slot (when UI exists)
   - Or trigger right-click event in test Blueprint
   - Verify item is used correctly

---

## Step 6: Integration Testing

### 6.1 Test Complete Flow

1. **Pickup → Inventory → Use Flow**
   - Spawn ItemPickupActor in level
   - Walk to pickup and press Interact (or auto-pickup if enabled)
   - Verify item added to inventory
   - Use item from inventory
   - Verify item consumed correctly
   - Verify effects applied (if consumable)

2. **Multiple Items Flow**
   - Pick up multiple items
   - Use different items from different slots
   - Verify each item works correctly
   - Verify stacking still works after use

### 6.2 Test Edge Cases

1. **Full Inventory**
   - Fill inventory to capacity
   - Try to use items
   - Verify items can still be used
   - Verify removed items free up space

2. **Weight Limits**
   - Fill inventory to weight limit
   - Use heavy items
   - Verify weight decreases correctly
   - Verify can pick up new items after use

3. **Stack Management**
   - Create stack of 5 items
   - Use item (should reduce to 4)
   - Use 4 more times (should remove item)
   - Verify slot becomes empty

---

## Step 7: Debug and Troubleshooting

### 7.1 Common Issues

1. **Item Not Being Used**
   - Check Output Log for error messages
   - Verify UseItem is being called
   - Check if CanUse() returns false
   - Verify item exists in slot
   - Check item quantity > 0

2. **Item Not Being Consumed**
   - Verify item type is "Consumable"
   - Check RemoveItem is being called
   - Verify quantity decreases correctly
   - Check if item is removed when quantity = 0

3. **Effects Not Applying**
   - Verify OnItemUsed event is bound
   - Check item type handling in Blueprint
   - Verify character stats exist
   - Check stat modification logic

4. **Events Not Firing**
   - Verify OnItemUsed is broadcast
   - Check event binding in Blueprint
   - Verify InventoryComponent reference is valid
   - Check if component is initialized

### 7.2 Debug Tools

1. **Use ReportInventoryContents**
   - Call `ReportInventoryContents` before and after use
   - Compare inventory state
   - Verify item quantity changes

2. **Add Breakpoints (If Using Blueprint Debugger)**
   - Set breakpoint in OnItemUsed event
   - Step through item usage logic
   - Verify each step executes correctly

3. **Console Commands (If Implemented)**
   - Use console to manually add items
   - Use console to trigger UseItem
   - Test different scenarios quickly

---

## Step 8: Polish and Final Checks

### 8.1 Verify All Item Types Work

1. **Test Each Item Type**
   - Consumable: ✅ Works, consumes, applies effects
   - Equipment: ✅ Works, doesn't consume (equip in Phase 3)
   - Skill Item: ✅ Works, doesn't consume (grant skill in Phase 3)
   - Misc: ✅ Works, doesn't consume

2. **Verify Error Handling**
   - Empty slot: ✅ Fails gracefully
   - Invalid slot: ✅ Fails gracefully
   - Quantity = 0: ✅ Fails gracefully
   - CanUse() = false: ✅ Fails gracefully

### 8.2 Performance Check

1. **Test with Many Items**
   - Fill inventory with many items
   - Use items rapidly
   - Verify no performance issues
   - Check for memory leaks (use Unreal Insights if available)

### 8.3 Documentation

1. **Update Notes**
   - Document any issues found
   - Document solutions implemented
   - Note any deviations from plan
   - Update test results

---

## Step 9: Prepare for Next Phase

### 9.1 Verify Integration Points

1. **Check Inventory UI Integration (Day 22-23)**
   - UseItem function is ready for UI integration
   - OnItemUsed event is ready for UI binding
   - Slot indices are correct for UI display

2. **Check Equipment System (Phase 3)**
   - Equipment items are marked correctly
   - UseItem can be extended for equip/unequip
   - Item type handling is ready

3. **Check Skill System (Phase 3)**
   - Skill items are marked correctly
   - UseItem can be extended for skill granting
   - Item type handling is ready

### 9.2 Clean Up

1. **Remove Test Blueprints (Optional)**
   - Remove temporary test Blueprints
   - Keep useful test setups for future testing

2. **Organize Content**
   - Ensure all Blueprints are in correct folders
   - Organize test items and pickups
   - Clean up unused assets

---

## Success Criteria

### Must Have
- ✅ UseItem function works correctly
- ✅ Consumable items are consumed properly
- ✅ Item quantity decreases on use
- ✅ Item is removed when quantity = 0
- ✅ OnItemUsed event fires correctly
- ✅ Different item types handled correctly
- ✅ Error handling works (empty slots, invalid slots, etc.)

### Nice to Have
- Visual feedback (particles, UI updates)
- Audio feedback (sound effects)
- Character stats integration (health/mana/stamina)
- Right-click use support
- Quick-use slots (Phase 3)

---

## Next Steps

After completing Days 19-20:

1. **Day 21: Integration Testing**
   - Test complete inventory flow
   - Fix any remaining issues
   - Prepare for UI implementation

2. **Days 22-23: Inventory UI**
   - Create InventoryWidget
   - Create InventorySlotWidget
   - Integrate UseItem with UI
   - Add right-click use support

3. **Phase 3: Equipment and Skills**
   - Implement equipment system
   - Implement skill granting
   - Extend UseItem for equipment/skills

---

## Troubleshooting Quick Reference

| Issue | Solution |
|-------|----------|
| Item not used | Check UseItem is called, verify slot has item, check CanUse() |
| Item not consumed | Verify item type is Consumable, check RemoveItem logic |
| Effects not applied | Check OnItemUsed binding, verify item type handling |
| Events not firing | Verify event broadcast, check component reference |
| Quantity not decreasing | Check RemoveItem call, verify quantity > 0 |
| Item not removed | Check quantity = 0 logic, verify UpdateSlotEmptyStatus |

---

**End of Days 19-20 Manual Steps Guide**
