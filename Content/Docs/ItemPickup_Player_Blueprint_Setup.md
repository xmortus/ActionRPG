# Item Pickup - Player Blueprint Setup Guide
**Question:** Are there any Blueprint nodes that need to be setup on the player to make the item pickup work?  
**Date:** 2025-01-07

---

## Short Answer

**No Blueprint nodes are required** - the pickup system works automatically through C++ overlap detection. However, you need to **verify** that certain components and settings are configured correctly in the player Blueprint.

---

## What Works Automatically (C++)

The item pickup system is designed to work automatically:

1. **Overlap Detection:** ItemPickupActor binds to `OnComponentBeginOverlap` in C++ BeginPlay - no Blueprint setup needed
2. **InventoryComponent:** Created automatically in C++ constructor - no Blueprint setup needed
3. **Pickup Logic:** All handled in C++ - no Blueprint nodes needed

---

## What Needs to be Verified in Blueprint

While no Blueprint nodes are required, you should **verify** these settings:

### 1. InventoryComponent Exists

**Location:** `BP_ActionRPGPlayerCharacter` → Components Panel

**Check:**
- ✅ `InventoryComponent` should exist in the Components panel
- ✅ It should be visible and not hidden
- ✅ It should be attached to the character (not a separate actor)

**If Missing:**
- The C++ code creates it automatically, so if it's missing, there may be a compilation issue
- Recompile C++ code and restart editor
- If still missing, manually add it:
  1. Click `Add Component` button
  2. Search for `Inventory Component`
  3. Add it to the character

**Settings to Verify:**
- Open `InventoryComponent` in Details Panel
- Check `Max Capacity` (default: 20)
- Check `Max Weight` (default: 100.0)
- These can be adjusted per instance if needed

---

### 2. Player Collision Settings

**Location:** `BP_ActionRPGPlayerCharacter` → Components Panel → `CapsuleComponent`

**Check:**
- ✅ Collision Enabled: `Query and Physics` or `Query Only`
- ✅ Object Type: `Pawn`
- ✅ Collision should not be disabled

**Why:** The ItemPickupActor's CollisionComponent responds to `ECC_Pawn`, so the player must be a Pawn with collision enabled.

**If Wrong:**
1. Select `CapsuleComponent` in Components panel
2. In Details Panel → Collision:
   - **Collision Enabled:** Set to `Query and Physics` or `Query Only`
   - **Object Type:** Should be `Pawn` (default for Character)
   - Verify collision is not set to `No Collision`

---

### 3. Character Class Verification

**Location:** `BP_ActionRPGPlayerCharacter` → Class Settings

**Check:**
- ✅ Parent Class: `ActionRPGPlayerCharacter` (C++ class)
- ✅ Not a base `Character` or `Pawn` class

**Why:** ItemPickupActor casts to `AActionRPGPlayerCharacter` specifically. If the player is a different class, the cast will fail.

**If Wrong:**
- The Blueprint should already inherit from `ActionRPGPlayerCharacter`
- If it doesn't, recreate the Blueprint from the correct parent class

---

## Optional: Blueprint Event Bindings (Not Required)

While not required for basic pickup functionality, you can optionally add Blueprint nodes to:

### Option 1: Listen to Inventory Events

**Purpose:** Update UI or play effects when items are picked up

**Location:** `BP_ActionRPGPlayerCharacter` → Event Graph

**Nodes to Add:**
1. **Event BeginPlay**
2. **Get Inventory Component** (from self)
3. **Bind Event to On Item Added** (from InventoryComponent)
4. **Custom Event** to handle item added

**Example Flow:**
```
Event BeginPlay
  → Get Inventory Component
    → Bind Event to On Item Added
      → Custom Event: OnItemPickedUp
        → (Your logic: Update UI, Play Sound, etc.)
```

**Note:** This is optional - the pickup will work without this. This is only if you want to react to pickups in Blueprint.

---

### Option 2: Debug Logging

**Purpose:** Verify InventoryComponent is working

**Location:** `BP_ActionRPGPlayerCharacter` → Event Graph

**Nodes to Add:**
1. **Event BeginPlay**
2. **Get Inventory Component**
3. **Print String** (to verify component exists)

**Example:**
```
Event BeginPlay
  → Get Inventory Component
    → Print String: "InventoryComponent: [Component Name]"
```

---

## Verification Checklist

Before testing item pickup, verify:

- [ ] **InventoryComponent exists** in Components panel
- [ ] **InventoryComponent is visible** (not hidden)
- [ ] **CapsuleComponent collision enabled** (Query and Physics or Query Only)
- [ ] **CapsuleComponent Object Type** is Pawn
- [ ] **Parent Class** is ActionRPGPlayerCharacter
- [ ] **No compilation errors** in Blueprint
- [ ] **C++ code compiled** successfully

---

## Common Issues

### Issue 1: InventoryComponent Not Visible

**Symptoms:** Component doesn't appear in Components panel

**Solution:**
1. Recompile C++ code
2. Restart Unreal Editor
3. Open Blueprint - component should appear
4. If still missing, manually add it (see above)

### Issue 2: Collision Not Working

**Symptoms:** Player walks through pickup, no overlap detected

**Solution:**
1. Check CapsuleComponent collision settings
2. Verify collision is enabled
3. Verify Object Type is Pawn
4. Check ItemPickupActor collision settings (see collision troubleshooting guide)

### Issue 3: Cast Fails

**Symptoms:** Console shows "Overlap with non-player actor"

**Solution:**
1. Verify Blueprint parent class is `ActionRPGPlayerCharacter`
2. Not a base Character or custom class
3. Check Blueprint class settings

---

## Testing

After verifying settings:

1. **Place ItemPickup in level**
2. **Set ItemData** on pickup
3. **Play in Editor**
4. **Walk into pickup**
5. **Check console logs:**
   - Should see "Player overlapped"
   - Should see "Item picked up successfully"
   - Should see "Destroying pickup actor"
6. **Check inventory:**
   - Item should appear in inventory
   - Pickup should be destroyed

---

## Summary

**Required Blueprint Setup:** None - everything works automatically

**Required Verification:**
- ✅ InventoryComponent exists
- ✅ CapsuleComponent collision enabled
- ✅ Parent class is ActionRPGPlayerCharacter

**Optional Blueprint Nodes:**
- Event bindings for UI updates
- Debug logging
- Custom pickup effects

---

**End of Player Blueprint Setup Guide**
