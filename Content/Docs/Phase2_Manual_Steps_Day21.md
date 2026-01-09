# Phase 2 Manual Steps - Day 21
**Integration Testing**  
**Date:** 2025-01-07  
**Phase:** Phase 2 - Inventory System  
**Day:** 21 - Integration Testing (Inventory Core)

---

## Overview

Day 21 focuses on comprehensive integration testing of the entire inventory system. You will test the complete flow from item pickup to item usage, verify edge cases, debug any issues, and ensure all systems work together correctly. This is a critical day to ensure everything is working before moving to UI implementation.

---

## Prerequisites

- ✅ All C++ code compiled successfully
- ✅ Days 15-20 complete (InventoryComponent, ItemPickupActor, Item Usage System)
- ✅ InventoryComponent attached to PlayerCharacter
- ✅ ItemPickupActor working and configured
- ✅ Health system implemented and working
- ✅ Test Item Data Assets created (HealthPotion, etc.)
- ✅ Test level created with Player Start
- ✅ Unreal Editor is open
- ✅ Project is loaded in the editor
- ✅ No compilation errors in Output Log

**Before Starting:**
1. Close Visual Studio (if open)
2. Return to Unreal Editor
3. Ensure all previous days' work is complete
4. Have Output Log open for debugging

---

## Step 1: Prepare Test Environment

### 1.1 Open Test Level

1. **Open or Create Test Level**
   - Open your test level (or create a new one)
   - Ensure Player Start exists
   - Ensure GameMode is set to `BP_ActionRPGGameMode`

2. **Verify Player Character**
   - Check that PlayerCharacter is set correctly in GameMode
   - Verify PlayerCharacter has InventoryComponent attached
   - Check that PlayerCharacter has health stats (CurrentHealth, MaxHealth)

### 1.2 Place Item Pickups

1. **Open Content Browser**
   - Navigate to `Content/Blueprints/Items/Pickups/`
   - Find your ItemPickup Blueprint (e.g., `BP_ItemPickup_HealthPotion`)

2. **Place Pickups in Level**
   - Drag ItemPickup Blueprint into the level
   - Place 3-5 pickups around the player start
   - Space them out for easy testing
   - Verify each pickup has ItemData assigned

3. **Configure Pickups**
   - Select each pickup in the level
   - In Details Panel, verify:
     - `Item Data` is assigned correctly
     - `Quantity` is set (default: 1)
     - `Interaction Range` is set (default: 150.0)
     - `Auto Pickup on Overlap` is `false` (for manual interaction)

### 1.3 Open Output Log

1. **Open Output Log**
   - Window → `Developer Tools` → `Output Log`
   - Or press `Ctrl + Shift + L`

2. **Clear Log (Optional)**
   - Click `Clear Log` button
   - This helps see only new messages during testing

---

## Step 2: Test Complete Flow

### 2.1 Test Item Pickup

1. **Start Play In Editor (PIE)**
   - Press `Alt + P` or click `Play` button
   - Game should start with player character

2. **Check Initial State**
   - Look at Output Log
   - Should see: `ActionRPGPlayerCharacter: Health initialized - 50.0/100.0`
   - Should see: `ActionRPGPlayerCharacter: Bound to InventoryComponent OnItemUsed event`
   - Should see: `InventoryComponent: Initialized with X slots`

3. **Approach Item Pickup**
   - Move player character near an item pickup
   - Verify pickup is within interaction range (150 units)

4. **Press Interact Key**
   - Press the interact key (default: E)
   - Check Output Log for:
     - `OnInteract called`
     - `ItemPickupActor::TryInteract`
     - `InventoryComponent::AddItem`
     - `InventoryComponent::TryStackItem` (if stacking)
     - Inventory debug report showing the item

5. **Verify Item Added**
   - Check Output Log for inventory debug report
   - Should show item in a slot (not empty)
   - Should show correct quantity
   - Item should appear in Details Panel (if viewing in PIE)

6. **Verify Pickup Destroyed**
   - The pickup actor should be destroyed after successful pickup
   - Check Output Log for: `ItemPickupActor: Destroying pickup`

### 2.2 Test Item Usage

1. **Use Health Potion**
   - In Blueprint Event Graph or via console command, call `UseItem` on slot 0
   - Or create a test Blueprint that calls `UseItem` on button press

2. **Check Output Log**
   - Should see: `InventoryComponent::UseItem - Attempting to use item`
   - Should see: `ActionRPGPlayerCharacter::OnItemUsed - EVENT FIRED!`
   - Should see: `ActionRPGPlayerCharacter::OnItemUsed - Health potion detected!`
   - Should see: `ActionRPGPlayerCharacter::Heal - Healed X health`
   - Should see: `InventoryComponent::UseItem - Consuming 1 quantity`

3. **Verify Health Increased**
   - Check Output Log for new health value
   - Should see: `New health: X/100.0` (where X > 50)
   - Health should increase by 25 (healing amount)

4. **Verify Item Consumed**
   - Check inventory debug report
   - Item quantity should decrease by 1
   - If quantity reaches 0, item should be removed from slot

5. **Test Multiple Uses**
   - Use health potion multiple times
   - Verify health increases each time
   - Verify item is consumed each time
   - Verify item is removed when quantity = 0

### 2.3 Test Complete Flow Multiple Times

1. **Pickup → Use Flow**
   - Pick up a health potion
   - Use it immediately
   - Verify health increases
   - Repeat with multiple potions

2. **Verify Consistency**
   - Each pickup should work correctly
   - Each use should work correctly
   - Health should increase correctly
   - Items should be consumed correctly

---

## Step 3: Test Edge Cases

### 3.1 Test Full Inventory

1. **Fill Inventory to Capacity**
   - Pick up items until inventory is full (50 slots)
   - Or manually add items via Blueprint/console

2. **Attempt Pickup When Full**
   - Try to pick up another item
   - Should fail gracefully
   - Check Output Log for: `HasSpaceFor returned: FALSE`
   - Check Output Log for: `Cannot pickup item - validation failed`

3. **Verify No Crash**
   - Game should not crash
   - Player should still be able to move
   - Inventory should remain stable

### 3.2 Test Item Stacking

1. **Pick Up Same Item Multiple Times**
   - Pick up multiple health potions (or same item)
   - Items should stack in the same slot

2. **Verify Stacking**
   - Check inventory debug report
   - Should show quantity increasing in same slot
   - Should not create new slots for same item

3. **Test Max Stack Size**
   - If MaxStackSize is set (e.g., 10)
   - Pick up items until stack reaches max
   - Next pickup should create new stack in different slot
   - Verify both stacks exist correctly

### 3.3 Test Weight Limits

1. **Add Heavy Items**
   - Add items until weight limit is reached
   - Or manually set weight limit to low value for testing

2. **Attempt Pickup When Over Weight**
   - Try to pick up another item
   - Should fail with weight limit message
   - Check Output Log for weight-related errors

### 3.4 Test Health Potion at Max Health

1. **Heal to Max Health**
   - Use health potions until health reaches 100/100
   - Or manually set CurrentHealth to 100

2. **Attempt to Use Health Potion**
   - Try to use health potion when health is at max
   - Should fail gracefully
   - Check Output Log for: `Cannot use health potion: Health already at max`
   - Item should NOT be consumed

3. **Verify Item Not Consumed**
   - Check inventory debug report
   - Item quantity should remain the same
   - Item should still be in inventory

### 3.5 Test Invalid Operations

1. **Use Item from Empty Slot**
   - Try to use item from slot with no item
   - Should fail gracefully
   - Check Output Log for: `Slot X is empty`

2. **Use Item with Quantity 0**
   - If somehow an item has quantity 0
   - Should fail gracefully
   - Check Output Log for appropriate error

3. **Use Item from Invalid Slot Index**
   - Try to use item from slot index > MaxCapacity
   - Should fail gracefully
   - Check Output Log for: `Invalid slot index`

### 3.6 Test Multiple Pickups Simultaneously

1. **Place Multiple Pickups Close Together**
   - Place 3-5 pickups within interaction range

2. **Press Interact**
   - Should pick up the nearest pickup
   - Other pickups should remain
   - Verify correct pickup was selected

3. **Pick Up All Pickups**
   - Move to each pickup and press interact
   - All should be picked up successfully
   - All should appear in inventory

---

## Step 4: Test Item Type Handling

### 4.1 Test Consumable Items

1. **Use Consumable Item**
   - Use a consumable item (e.g., HealthPotion)
   - Verify item is consumed (quantity decreases)
   - Verify item is removed when quantity = 0
   - Verify effects are applied (health increases)

### 4.2 Test Equipment Items

1. **Use Equipment Item**
   - Use an equipment item (if you have one)
   - Verify item is NOT consumed
   - Verify quantity stays the same
   - Check Output Log for: `Equipment item, not consumed`

### 4.3 Test Skill Items

1. **Use Skill Item**
   - Use a skill item (if you have one)
   - Verify item is NOT consumed
   - Verify quantity stays the same
   - Check Output Log for: `Skill item, not consumed`

### 4.4 Test Misc Items

1. **Use Misc Item**
   - Use a misc item (if you have one)
   - Verify item is NOT consumed
   - Verify quantity stays the same
   - Check Output Log for: `Misc item, not consumed`

---

## Step 5: Debug and Fix Issues

### 5.1 Check Output Log for Errors

1. **Review All Log Messages**
   - Look for any `Error` or `Warning` messages
   - Pay special attention to:
     - Inventory-related errors
     - Item usage errors
     - Health system errors
     - Event binding errors

2. **Common Issues to Look For:**
   - `InventoryComponent is NULL`
   - `ItemData is NULL`
   - `OnItemUsed event not firing`
   - `Health not increasing`
   - `Item not being consumed`
   - `Event binding failed`

### 5.2 Verify Event Bindings

1. **Check OnItemUsed Binding**
   - Look for: `ActionRPGPlayerCharacter: Bound to InventoryComponent OnItemUsed event`
   - If not present, event binding failed
   - Check BeginPlay is being called

2. **Test Event Firing**
   - Use an item
   - Check for: `ActionRPGPlayerCharacter::OnItemUsed - EVENT FIRED!`
   - If not present, event is not firing or not bound

### 5.3 Verify Inventory Debug Reports

1. **Check Debug Reports**
   - Inventory should report contents on every change
   - Reports should show all slots (empty and filled)
   - Reports should be consistent

2. **Look for Inconsistencies**
   - Items showing in logs but not in debug report
   - Slots marked empty but have items
   - Quantity mismatches

### 5.4 Fix Common Issues

1. **Issue: Health Not Increasing**
   - Check ItemID matches "HealthPotion" or "healthpotion"
   - Check OnItemUsed event is bound
   - Check Heal() function is being called
   - Check CurrentHealth is being updated

2. **Issue: Item Not Being Consumed**
   - Check item type is "Consumable"
   - Check RemoveItem is being called
   - Check quantity is decreasing
   - Check slot is being updated

3. **Issue: Event Not Firing**
   - Check event binding in BeginPlay
   - Check InventoryComponent is valid
   - Check OnItemUsed.Broadcast is being called
   - Check function signature matches

4. **Issue: Items Disappearing**
   - Check for GC issues
   - Check TObjectPtr is being used
   - Check NewObject outer is correct
   - Check items are properly referenced

---

## Step 6: Performance Testing

### 6.1 Test with Full Inventory

1. **Fill Inventory**
   - Add items until all 50 slots are filled
   - Use different item types

2. **Test Operations**
   - Use items rapidly
   - Pick up items
   - Move items between slots
   - Check for frame drops

3. **Monitor Performance**
   - Use Stat FPS command in console
   - Check for significant frame drops
   - Verify operations are still responsive

### 6.2 Test Rapid Operations

1. **Rapid Item Usage**
   - Use items as fast as possible
   - Verify no crashes
   - Verify all operations complete correctly

2. **Rapid Pickups**
   - Pick up items rapidly
   - Verify all items are added correctly
   - Verify no items are lost

### 6.3 Check Memory

1. **Monitor Memory Usage**
   - Use Unreal Insights if available
   - Check for memory leaks
   - Verify objects are being garbage collected

2. **Extended Play Test**
   - Play for extended period (10+ minutes)
   - Pick up and use many items
   - Check for memory growth
   - Check for crashes

---

## Step 7: Integration Verification

### 7.1 Verify System Integration

1. **InventoryComponent ↔ ItemPickupActor**
   - Pickups should add items to inventory
   - Validation should work correctly
   - Events should fire correctly

2. **InventoryComponent ↔ PlayerCharacter**
   - Health system should work with items
   - Events should be bound correctly
   - Item usage should affect character stats

3. **ItemDatabase ↔ InventoryComponent**
   - Items should be created correctly
   - Item lookup should work
   - Item data should be accessible

### 7.2 Verify Event System

1. **OnItemUsed Event**
   - Should fire when item is used
   - Should be bound to PlayerCharacter
   - Should trigger healing for health potions

2. **OnInventoryChanged Event**
   - Should fire when inventory changes
   - Should update debug reports
   - Should be consistent

3. **Other Events**
   - OnItemAdded (if used)
   - OnItemRemoved (if used)
   - All should fire correctly

---

## Step 8: Final Verification Checklist

### 8.1 Core Functionality

- [ ] Item pickup works (manual interaction)
- [ ] Items are added to inventory correctly
- [ ] Item usage works correctly
- [ ] Consumable items are consumed
- [ ] Health potion heals player
- [ ] Health potion cannot be used at max health
- [ ] Items are removed when quantity = 0
- [ ] Inventory debug reports work correctly

### 8.2 Edge Cases

- [ ] Full inventory handled correctly
- [ ] Item stacking works correctly
- [ ] Weight limits enforced
- [ ] Invalid operations fail gracefully
- [ ] Multiple pickups handled correctly
- [ ] Max health prevents health potion use

### 8.3 Item Types

- [ ] Consumable items work correctly
- [ ] Equipment items work correctly (not consumed)
- [ ] Skill items work correctly (not consumed)
- [ ] Misc items work correctly (not consumed)

### 8.4 Integration

- [ ] All systems work together
- [ ] Events fire correctly
- [ ] No crashes or errors
- [ ] Performance is acceptable
- [ ] Memory usage is stable

### 8.5 Code Quality

- [ ] No compilation errors
- [ ] No runtime errors
- [ ] Logging is appropriate
- [ ] Code is clean and documented

---

## Step 9: Documentation and Cleanup

### 9.1 Document Issues Found

1. **Create Issue Log**
   - Document any bugs found
   - Document solutions implemented
   - Note any workarounds needed

2. **Update Documentation**
   - Update any relevant docs
   - Note any deviations from plan
   - Document test results

### 9.2 Clean Up Debug Logs

1. **Review Logging**
   - Keep important logs (errors, warnings)
   - Remove excessive verbose logs
   - Keep health-related logs
   - Keep inventory change logs

2. **Update Log Levels**
   - Change verbose logs to LogTemp::Verbose
   - Keep important logs as LogTemp::Log
   - Keep errors as LogTemp::Error/Warning

### 9.3 Prepare for Next Phase

1. **Verify Readiness**
   - All systems working correctly
   - No critical bugs
   - Performance is acceptable
   - Ready for UI implementation

2. **Create Test Summary**
   - Document what was tested
   - Document what works
   - Document any known issues
   - Note any future improvements needed

---

## Troubleshooting

### Issue: Health Not Increasing

**Symptoms:** Health potion used but health doesn't increase

**Solutions:**
1. Check ItemID matches exactly (case-insensitive "healthpotion")
2. Check OnItemUsed event is bound in BeginPlay
3. Check Output Log for event firing
4. Check Heal() function is being called
5. Verify CurrentHealth is being updated

### Issue: Item Not Being Consumed

**Symptoms:** Item used but quantity doesn't decrease

**Solutions:**
1. Check item type is "Consumable"
2. Check RemoveItem is being called
3. Check quantity is > 0 before use
4. Check slot is being updated correctly
5. Verify inventory debug report shows change

### Issue: Event Not Firing

**Symptoms:** OnItemUsed event doesn't trigger

**Solutions:**
1. Check event binding in BeginPlay
2. Check InventoryComponent is valid
3. Check OnItemUsed.Broadcast is being called
4. Check function signature matches delegate
5. Verify BeginPlay is being called

### Issue: Items Disappearing

**Symptoms:** Items added but disappear from inventory

**Solutions:**
1. Check for GC issues (TObjectPtr usage)
2. Check NewObject outer is correct
3. Check items are properly referenced
4. Check inventory slots are not being reset
5. Verify BeginPlay is not clearing slots

### Issue: Pickup Not Working

**Symptoms:** Can't pick up items

**Solutions:**
1. Check interaction range (150 units)
2. Check bAutoPickupOnOverlap is false
3. Check Interact key is bound
4. Check OnInteract is being called
5. Check inventory has space

---

## Success Criteria

### Must Have
- ✅ Complete flow works (pickup → inventory → use)
- ✅ Health potion heals player correctly
- ✅ Health potion cannot be used at max health
- ✅ Items are consumed correctly
- ✅ Edge cases handled gracefully
- ✅ No crashes or critical errors
- ✅ Performance is acceptable

### Nice to Have
- All item types tested
- Extended play test completed
- Memory leak testing done
- Performance profiling completed

---

## Next Steps

After completing Day 21:

1. **Days 22-23: Inventory UI**
   - Create InventoryWidget
   - Create InventorySlotWidget
   - Integrate with InventoryComponent
   - Add visual feedback

2. **Days 24-25: Drag and Drop**
   - Implement drag and drop system
   - Add visual feedback
   - Test slot interactions

3. **Days 26-28: UI Polish and Final Testing**
   - Polish UI visuals
   - Add tooltips and details
   - Final integration testing

---

## Quick Reference

### Test Commands (If Console Available)
- `Stat FPS` - Show FPS counter
- `Stat Unit` - Show performance stats
- `Log LogTemp Verbose` - Enable verbose logging

### Key Log Messages to Watch For
- `ActionRPGPlayerCharacter: Health initialized`
- `ActionRPGPlayerCharacter: Bound to InventoryComponent OnItemUsed event`
- `InventoryComponent::AddItem - Called with Item`
- `InventoryComponent::UseItem - Attempting to use item`
- `ActionRPGPlayerCharacter::OnItemUsed - EVENT FIRED!`
- `ActionRPGPlayerCharacter::Heal - Healed X health`

### Important File Paths
- PlayerCharacter: `Content/Blueprints/Characters/BP_ActionRPGPlayerCharacter`
- ItemPickup: `Content/Blueprints/Items/Pickups/BP_ItemPickup_HealthPotion`
- InventoryComponent: `Content/Blueprints/Components/Inventory/BP_InventoryComponent`

---

**Status:** Complete all testing and verification before proceeding to Days 22-23

---

**End of Phase 2 Manual Steps - Day 21**
