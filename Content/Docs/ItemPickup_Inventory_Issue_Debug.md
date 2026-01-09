# Item Pickup Inventory Issue - Debug Guide
**Issue:** Collision works but item not added to inventory or destroyed  
**Date:** 2025-01-07  
**Last Updated:** 2025-01-07 (UE 5.7 Compliance Update)

---

## Problem

Collision/overlap detection is working (OnOverlapBegin is called), but:
- Item is not being added to inventory
- Pickup is not being destroyed

---

## Enhanced Debug Logging Added

I've added comprehensive logging to trace the entire pickup flow. The console will now show:

### CanPickup() Validation Flow:
1. `"ItemPickupActor::CanPickup - Starting validation"`
2. `"ItemPickupActor::CanPickup - ItemData: [Name], Quantity: [X]"`
3. `"ItemPickupActor::CanPickup - InventoryComponent found, Current Weight: X/Y, Items: X, Capacity: X"`
4. `"ItemPickupActor::CanPickup - ItemDatabase found, creating temp item..."`
5. `"ItemPickupActor::CanPickup - Temp item created: [Name] (Quantity: X)"`
6. `"ItemPickupActor::CanPickup - HasSpaceFor returned: TRUE/FALSE"`
7. `"ItemPickupActor::CanPickup - Validation passed!"` (if successful)

### PickupItem() Flow:
1. `"ItemPickupActor::PickupItem - Starting pickup process"`
2. `"ItemPickupActor::PickupItem - Creating item from ItemData: [Name] (Quantity: X)"`
3. `"ItemPickupActor::PickupItem - ItemDatabase found"`
4. `"ItemPickupActor::PickupItem - Item created successfully: [Name] (Quantity: X)"`
5. `"ItemPickupActor::PickupItem - Calling AddItem..."`
6. `"ItemPickupActor::PickupItem - AddItem returned: TRUE/FALSE"`
7. `"ItemPickupActor: Item picked up successfully"` (if successful)
8. `"ItemPickupActor::PickupItem - Destroying pickup actor..."`

---

## Diagnostic Steps

### Step 1: Check Console Logs

1. **Play in Editor**
2. **Walk into pickup**
3. **Open Output Log** (Ctrl+Shift+L)
4. **Look for the log messages above**

### Step 2: Identify Where It Fails

**If you see "CanPickup - Validation passed" but no "PickupItem - Starting":**
- Issue: CanPickup returns true but PickupItem is not called
- Check: Verify OnOverlapBegin is calling PickupItem after CanPickup

**If you see "CanPickup - HasSpaceFor returned: FALSE":**
- Issue: Inventory has no space
- Check:
  - Inventory weight limit
  - Inventory capacity (empty slots)
  - Item weight vs available weight

**If you see "CanPickup - Failed to create item from ItemData":**
- Issue: ItemDatabase cannot create item
- Check:
  - ItemData is set in Blueprint
  - ItemDatabase is initialized
  - ItemID matches an item in database

**If you see "PickupItem - AddItem returned: FALSE":**
- Issue: AddItem failed
- Check:
  - InventoryComponent::AddItem logs (should show why it failed)
  - Weight/capacity limits
  - Item validation

---

## Common Issues & Solutions

### Issue 1: ItemData Not Set

**Symptoms:**
- `"ItemPickupActor::CanPickup - ItemData is NULL"`

**Solution:**
1. Open `BP_ItemPickup` Blueprint
2. Select the actor (root)
3. In Details Panel → `Pickup` category
4. Set `Item Data` to a valid Item Data Asset (e.g., `DA_Item_Consumable_HealthPotion`)

### Issue 2: InventoryComponent Not Attached

**Symptoms:**
- `"ItemPickupActor::CanPickup - Player has no InventoryComponent"`

**Solution:**
1. Open `BP_ActionRPGPlayerCharacter` Blueprint
2. Verify `InventoryComponent` exists in Components panel
3. If missing, add it (see Days 15-16 manual steps)

### Issue 3: Inventory Full (No Space)

**Symptoms:**
- `"ItemPickupActor::CanPickup - HasSpaceFor returned: FALSE"`
- `"InventoryComponent::HasSpaceFor - Weight limit would be exceeded"` or
- `"InventoryComponent::AddItem - No space for item"`

**Solution:**
1. Check inventory weight: `GetCurrentWeight() / GetMaxWeight()`
2. Check inventory capacity: `GetTotalItemCount() / GetMaxCapacity()`
3. Remove items from inventory or increase limits
4. Verify item weight is reasonable

### Issue 4: ItemDatabase Not Initialized

**Symptoms:**
- `"ItemPickupActor::CanPickup - ItemDatabase is NULL"`

**Solution:**
1. Verify ItemDatabase is initialized (should happen automatically)
2. Check console for ItemDatabase initialization logs
3. Verify Item Data Assets are registered with Asset Manager

### Issue 5: Item Creation Failed

**Symptoms:**
- `"ItemPickupActor::CanPickup - Failed to create item from ItemData: [ItemID]"`

**Solution:**
1. Verify ItemID exists in ItemDatabase
2. Check ItemDatabase logs for item registration
3. Verify Item Data Asset is properly configured
4. Check ItemID matches exactly (case-sensitive)

### Issue 6: AddItem Failed

**Symptoms:**
- `"ItemPickupActor::PickupItem - AddItem returned: FALSE"`
- `"InventoryComponent::AddItem - Invalid item or quantity"` or
- `"InventoryComponent::AddItem - No space for item"`

**Solution:**
1. Check InventoryComponent::AddItem logs for specific error
2. Verify item is valid (ItemData is set)
3. Verify quantity > 0
4. Check HasSpaceFor() passed (should have been checked in CanPickup)

### Issue 7: Items Added But Not Persisting

**Symptoms:**
- Logs show items being added successfully
- Debug report shows inventory is empty
- Items appear in logs but not in Details Panel

**Possible Causes:**
1. **BeginPlay being called multiple times** - Check logs for "InventoryComponent::BeginPlay - CALLED!" after items are added
2. **Items being garbage collected** - Verify items are created with proper outer object (GetWorld() or GetOwner())
3. **Slot clearing** - Check UpdateSlotEmptyStatus logs to see if slots are being incorrectly marked as empty

**Solution:**
1. Check console logs for BeginPlay warnings
2. Verify item creation uses proper outer object (should use GetWorld() or GetOwner())
3. Check debug report logs to see slot states
4. Look for "INCONSISTENT STATE" messages in debug reports

---

## Quick Fix Checklist

1. **Verify ItemData is Set:**
   - Open `BP_ItemPickup` Blueprint
   - Set `Item Data` property to valid Item Data Asset
   - Compile and Save

2. **Verify InventoryComponent Exists:**
   - Open `BP_ActionRPGPlayerCharacter` Blueprint
   - Check Components panel for `InventoryComponent`
   - If missing, add it

3. **Check Inventory Space:**
   - Verify inventory is not full
   - Check weight and capacity limits
   - Remove items if needed

4. **Check Console Logs:**
   - Look for specific error messages
   - Follow the log flow to identify failure point

5. **Verify ItemDatabase:**
   - Check console for ItemDatabase initialization
   - Verify items are registered
   - Check ItemID matches

---

## Testing After Fixes

After applying fixes, test:

1. **Walk into pickup**
2. **Check console logs:**
   - Should see "CanPickup - Validation passed"
   - Should see "PickupItem - Starting pickup process"
   - Should see "AddItem returned: TRUE"
   - Should see "Item picked up successfully"
   - Should see "Destroying pickup actor..."
3. **Check inventory:**
   - Item should appear in inventory
   - Quantity should be correct
4. **Check world:**
   - Pickup should be destroyed (disappear)

---

## Code Changes Summary

### Enhanced Logging in CanPickup():
- Logs validation start
- Logs ItemData and Quantity
- Logs InventoryComponent status and current state
- Logs ItemDatabase access
- Logs temp item creation (with template quantity vs pickup quantity)
- Logs HasSpaceFor result (now uses quantity parameter)
- Logs validation result

### Enhanced Logging in PickupItem():
- Logs pickup start
- Logs ItemData and Quantity
- Logs ItemDatabase access
- Logs item template creation (quantity 1, pickup quantity separate)
- Logs AddItem call with quantity parameter
- Logs AddItem result
- Logs success/failure
- Logs destruction

### Enhanced Logging in AddItem():
- Logs when AddItem is called with item details
- Logs stacking attempts and results
- Logs item instance creation with outer object info
- Logs slot updates with verification
- Logs verification step to confirm item was added
- Logs slot state before and after operations

### Debug Reporting Feature:
- Automatic inventory report every 5 seconds
- Shows all slots (empty and filled)
- Shows capacity, weight, and item counts
- Logs inconsistent slot states
- Can be called manually via `ReportInventoryContents()` Blueprint function

### UE 5.7 Compliance Updates:
- Removed `ConditionalBeginDestroy()` calls (GC handles cleanup automatically)
- Updated `CreateItem()` to use `GetTransientPackage()` for temporary items
- Added `BlueprintPure` flags to getter functions
- Improved memory management with proper outer objects
- Added `BlueprintCallable` to `PickupItem()` and `CanPickup()` for Blueprint access

---

## Architecture Notes

### ItemDatabase vs InventoryComponent

**Important:** The ItemDatabase and InventoryComponent serve different purposes:

- **ItemDatabase (Singleton):**
  - Stores ItemDataAssets (templates/definitions)
  - Shared by all players
  - Used to look up item data and create item instances
  - Does NOT store actual inventory items

- **InventoryComponent (Per-Player):**
  - Each player has their own InventoryComponent
  - Stores actual inventory items (UItemBase instances)
  - Unique per player character
  - Items stored here are separate from other players

When you see logs about "ItemDatabase", it's creating item instances from templates, not storing them. Actual items are stored in the player's InventoryComponent.

## Next Steps

1. **Recompile C++ code** in Visual Studio
2. **Restart Unreal Editor**
3. **Test pickup** and check console logs
4. **Check debug reports** (every 5 seconds) to see inventory state
5. **Identify failure point** from logs
6. **Apply appropriate fix** from solutions above
7. **Test again** to verify fix

---

**End of Item Pickup Inventory Issue Debug Guide**
