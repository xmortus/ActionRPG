# Phase 2 Manual Steps - Days 17-18
**After Code Implementation**  
**Date:** 2025-01-07  
**Phase:** Phase 2 - Inventory System  
**Days:** 17-18 - Item Pickup Actor

---

## Overview

After completing the C++ code implementation for Days 17-18, you need to perform the following manual steps in the Unreal Editor to complete the ItemPickupActor setup. These steps are required to make the pickup actor functional in the editor and testable in-game.

---

## Prerequisites

- ✅ All C++ code compiled successfully
- ✅ Days 15-16 complete (InventoryComponent functional)
- ✅ InventoryComponent attached to PlayerCharacter
- ✅ ItemDatabase working and initialized
- ✅ Test Item Data Assets created (HealthPotion, etc.)
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
     - Component creation errors

3. **Verify Hot Reload**
   - Look for message: `LogHotReload: Hot reload completed successfully`
   - If hot reload failed, close and reopen Unreal Editor

### 1.2 Verify ItemPickupActor Class

1. **Check Content Browser**
   - Navigate to `Content/` folder
   - Try to create a Blueprint Class
   - Search for `ItemPickupActor`
   - If it appears in the list, C++ code is ready

2. **If Class Doesn't Appear:**
   - Recompile C++ code in Visual Studio
   - Restart Unreal Editor
   - Try again

---

## Step 2: Create Folder Structure

### 2.1 Create Content Folders

1. **Navigate to Content Browser**
   - Go to `Content/` folder

2. **Create Blueprints/Items Folder**
   - Right-click in Content Browser
   - Select `New Folder`
   - Name it: `Items`
   - Location: `Content/Blueprints/Items/`

3. **Create Pickups Subfolder (Optional)**
   - Right-click on `Items` folder
   - Select `New Folder`
   - Name it: `Pickups`
   - Location: `Content/Blueprints/Items/Pickups/`

**Final Structure Should Be:**
```
Content/
└── Blueprints/
    └── Items/
        └── Pickups/
            └── (BP_ItemPickup will go here)
```

---

## Step 3: Create Blueprint Class from ItemPickupActor

### 3.1 Create Base ItemPickup Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Items/Pickups/` folder (or `Content/Blueprints/Items/`)

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - In the "Pick Parent Class" dialog, search for `ItemPickupActor`
   - Select `ItemPickupActor` (the C++ class)
   - Click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ItemPickup`
   - Press `Enter` to confirm
   - Save it in `Content/Blueprints/Items/Pickups/`

4. **Don't close yet - proceed to Step 4**

---

## Step 4: Configure Blueprint Components

### 4.1 Open Blueprint Editor

1. **Double-click `BP_ItemPickup`**
   - Opens in Blueprint Editor

2. **Verify Components Panel**
   - You should see:
     - `DefaultSceneRoot` (root component)
     - `CollisionComponent` (Sphere Component)
     - `MeshComponent` (Static Mesh Component)

### 4.2 Configure CollisionComponent

1. **Select `CollisionComponent`**
   - Click on `CollisionComponent` in the Components panel

2. **In Details Panel, configure:**

   **Collision Settings:**
   - **Collision Enabled:** `Query Only`
   - **Object Type:** `WorldDynamic`
   - **Collision Responses:**
     - **Pawn:** `Overlap`
     - **WorldStatic:** `Ignore` (or Block if needed)
     - **WorldDynamic:** `Ignore`
     - **Other settings:** Leave as default

   **Shape Settings:**
   - **Sphere Radius:** `50.0` (adjust as needed)
   - This determines the pickup range

   **Component Settings:**
   - **Component Tags:** Leave empty (or add custom tags)
   - **Mobility:** `Movable` (default)

3. **Verify Attachment**
   - `CollisionComponent` should be attached to `DefaultSceneRoot`
   - If not, drag it to attach

### 4.3 Configure MeshComponent

1. **Select `MeshComponent`**
   - Click on `MeshComponent` in the Components panel

2. **In Details Panel, configure:**

   **Static Mesh:**
   - **Static Mesh:** 
     - For testing: Use a simple mesh (Cube, Sphere, or Cylinder)
     - You can find these in: `Engine/BasicShapes/`
     - Or use a custom mesh if you have one
   - **If no mesh assigned:** Pickup will be invisible (collision still works)

   **Transform:**
   - **Location:** `(0, 0, 0)` (relative to CollisionComponent)
   - **Rotation:** `(0, 0, 0)`
   - **Scale:** `(1, 1, 1)` or adjust to fit item size

   **Materials:**
   - **Element 0:** Leave empty for now (can assign material later)
   - Or assign a material instance for visual feedback

   **Component Settings:**
   - **Mobility:** `Movable` (default)
   - **Collision Enabled:** `No Collision` (collision handled by CollisionComponent)

3. **Verify Attachment**
   - `MeshComponent` should be attached to `CollisionComponent` (or RootComponent)
   - If not, drag it to attach

### 4.4 Configure Item Data (Class Defaults)

1. **Select the Blueprint Root**
   - Click on the class name at the top (the root node)

2. **In Details Panel, find `Pickup` category:**

   **Item Data:**
   - **Item Data:** 
     - Leave as `None` for generic pickup (set in instance)
     - Or assign a test Item Data Asset (e.g., `DA_Item_Consumable_HealthPotion`)
   - **Quantity:** `1` (default, can be overridden per instance)

3. **Save the Blueprint**
   - Click `Compile` button (top toolbar)
   - Check for any errors (should be none)
   - Click `Save` (Ctrl+S)

---

## Step 5: Create Item-Specific Pickup Blueprints (Optional)

### 5.1 Create Health Potion Pickup

1. **Right-click on `BP_ItemPickup`**
   - In Content Browser

2. **Select `Duplicate`**
   - Creates a copy

3. **Rename the Copy**
   - Name it: `BP_ItemPickup_HealthPotion`

4. **Open the Blueprint**
   - Double-click to open

5. **Configure Item Data:**
   - Select Blueprint Root
   - In Details Panel → `Pickup` category:
     - **Item Data:** Set to `DA_Item_Consumable_HealthPotion`
     - **Quantity:** `1` (or set to desired quantity)

6. **Customize Visuals (Optional):**
   - Select `MeshComponent`
   - Assign a different mesh if desired
   - Assign a material (e.g., red material for health potion)
   - Adjust scale if needed

7. **Compile and Save**

### 5.2 Create Other Item Pickups (Optional)

Repeat Step 5.1 for other item types:
- `BP_ItemPickup_SkillItem` - For skill items
- `BP_ItemPickup_Weapon` - For weapons
- `BP_ItemPickup_Generic` - Keep as generic (set ItemData per instance)

---

## Step 6: Add Visual Effects (Optional)

### 6.1 Add Rotation Animation

1. **Open `BP_ItemPickup`**
   - Go to `Event Graph` tab

2. **Create Rotation Timeline:**
   - Right-click in Event Graph
   - Search for `Timeline`
   - Select `Add Timeline`
   - Name it: `RotationTimeline`

3. **Configure Timeline:**
   - Double-click `RotationTimeline` node
   - Set **Length:** `2.0` (seconds for full rotation)
   - Set **Loop:** `True`
   - Set **Auto Play:** `True`
   - Add a **Float Track** named `Rotation`

4. **Set Up Rotation Curve:**
   - In Timeline editor, set keyframes:
     - At `0.0`: Value `0.0`
     - At `2.0`: Value `360.0`
   - Set curve to linear (straight line)

5. **Connect to Mesh Rotation:**
   - Back in Event Graph:
   - Connect `Event BeginPlay` → `RotationTimeline` → `Play`
   - Connect `RotationTimeline` → `Update` → `Set Relative Rotation`
   - Set `Set Relative Rotation` Target: `MeshComponent`
   - Set `New Rotation` Z (Yaw): Connect to `Rotation` output from Timeline

6. **Compile and Save**

### 6.2 Add Glow Effect (Optional)

1. **Create Material Instance:**
   - Right-click in Content Browser
   - `Materials & Textures` → `Material Instance Constant`
   - Name it: `MI_ItemPickup_Glow`
   - Parent: Create a material with emissive parameter

2. **Assign to MeshComponent:**
   - Open `BP_ItemPickup`
   - Select `MeshComponent`
   - Assign `MI_ItemPickup_Glow` to Material slot

3. **Adjust Glow Intensity:**
   - Open Material Instance
   - Adjust emissive intensity
   - Save

---

## Step 7: Add Pickup to Test Level

### 7.1 Open Test Level

1. **Open Your Test Level**
   - Open the level you want to test in (e.g., `Lvl_TopDown` or `NewMap`)

2. **Verify Player Start Exists**
   - Look for a `Player Start` actor
   - If missing, add one: `Place Actors → Game → Player Start`

### 7.2 Place Item Pickup Actors

1. **Place Generic Pickup:**
   - In `Place Actors` panel, search for `BP_ItemPickup`
   - Drag it into the level
   - Position it near the player start (for easy testing)

2. **Configure Instance:**
   - Select the placed `BP_ItemPickup` actor
   - In Details Panel → `Pickup` category:
     - **Item Data:** Set to `DA_Item_Consumable_HealthPotion` (or any test item)
     - **Quantity:** `1` (or desired quantity)

3. **Place Multiple Pickups (Optional):**
   - Place 2-3 more pickups with different items
   - Configure each with different Item Data Assets
   - Position them around the level

4. **Verify Placement:**
   - Make sure pickups are on the ground (not floating or buried)
   - Adjust Z position if needed
   - Check that collision spheres are visible (press `G` to toggle)

---

## Step 8: Test Pickup in Game

### 8.1 Play in Editor

1. **Start Play**
   - Click `Play` button (top toolbar)
   - Or press `Alt + P`

2. **Navigate to Pickup**
   - Move character toward the pickup
   - Walk into the collision sphere

3. **Observe Pickup Behavior:**
   - Pickup should disappear when overlapped
   - Item should be added to inventory
   - Check console for pickup messages

### 8.2 Verify Inventory Update

1. **Check Inventory (If UI Created):**
   - Open inventory (Tab key, if bound)
   - Verify item appears in inventory
   - Check quantity is correct

2. **Check Console Logs:**
   - Open Output Log (Ctrl+Shift+L)
   - Look for messages:
     - `"ItemPickupActor: Player overlapped"`
     - `"ItemPickupActor: Item picked up: [ItemName]"`
     - `"InventoryComponent: Item added: [ItemName]"`

3. **Test Multiple Pickups:**
   - Pick up multiple items
   - Verify all items added to inventory
   - Check stacking works (if same item)

### 8.3 Test Edge Cases

1. **Test Full Inventory:**
   - Fill inventory to capacity (add items manually or via pickups)
   - Attempt to pick up another item
   - Pickup should remain (not destroyed)
   - Check console for "Inventory full" message

2. **Test Stacking:**
   - Pick up an item that already exists in inventory
   - Verify quantity increases (if stackable)
   - Verify item stacks correctly

3. **Test Weight Limit:**
   - Fill inventory to weight limit
   - Attempt to pick up heavy item
   - Should fail if over weight limit
   - Pickup should remain

4. **Stop Play**
   - Press `Esc` or click `Stop` button

---

## Step 9: Troubleshooting

### Issue: Pickup Not Detecting Overlap

**Symptoms:** Player walks through pickup, nothing happens

**Solutions:**
1. **Check CollisionComponent Settings:**
   - Open `BP_ItemPickup`
   - Select `CollisionComponent`
   - Verify:
     - Collision Enabled: `Query Only`
     - Object Type: `WorldDynamic`
     - Response to Pawn: `Overlap`

2. **Check Player Collision:**
   - Open `BP_ActionRPGPlayerCharacter`
   - Select `CapsuleComponent`
   - Verify collision is enabled
   - Check collision responses

3. **Check Sphere Radius:**
   - Increase `CollisionComponent` sphere radius
   - Try `100.0` or larger
   - Test again

4. **Check Overlap Event Binding:**
   - Verify C++ code binds `OnComponentBeginOverlap` in `BeginPlay()`
   - Check console for errors

5. **Check Actor Visibility:**
   - Press `G` in viewport to show collision
   - Verify collision sphere is visible
   - Check actor is not hidden

### Issue: Item Not Added to Inventory

**Symptoms:** Overlap detected but item not added

**Solutions:**
1. **Check PlayerCharacter Has InventoryComponent:**
   - Open `BP_ActionRPGPlayerCharacter`
   - Verify `InventoryComponent` exists in Components panel
   - If missing, add it (see Days 15-16 manual steps)

2. **Check ItemData is Set:**
   - Select placed pickup in level
   - Verify `Item Data` property is set (not None)
   - Set to a valid Item Data Asset

3. **Check Console Logs:**
   - Look for error messages
   - Check for "CanPickup failed" messages
   - Check for "AddItem failed" messages

4. **Check Inventory Space:**
   - Verify inventory has space
   - Check capacity and weight limits
   - Try with empty inventory

5. **Check ItemDatabase:**
   - Verify ItemDatabase is initialized
   - Check console for ItemDatabase logs
   - Verify Item Data Asset exists in database

### Issue: Pickup Not Destroying

**Symptoms:** Item added but pickup remains

**Solutions:**
1. **Check DestroyPickup() is Called:**
   - Verify C++ code calls `Destroy()` after successful pickup
   - Check console for errors preventing destruction

2. **Check Actor Settings:**
   - Select pickup in level
   - In Details Panel, check:
     - **Actor Hidden in Game:** Should be `False`
     - **Actor Enable Collision:** Should be `True`

3. **Check for Errors:**
   - Look for errors in Output Log
   - Check for warnings about destruction

### Issue: Visual Representation Not Showing

**Symptoms:** Pickup invisible or wrong mesh

**Solutions:**
1. **Check MeshComponent Has Mesh:**
   - Open `BP_ItemPickup`
   - Select `MeshComponent`
   - Verify `Static Mesh` is assigned
   - If None, assign a mesh (Cube, Sphere, etc.)

2. **Check Mesh Scale:**
   - Verify scale is not `(0, 0, 0)`
   - Set to `(1, 1, 1)` or appropriate size

3. **Check Component Visibility:**
   - Verify `MeshComponent` is visible
   - Check `Hidden in Game` is `False`

4. **Check Material:**
   - Assign a material if mesh is invisible
   - Use a simple material for testing

### Issue: Full Inventory Not Handled

**Symptoms:** Item added even when inventory is full

**Solutions:**
1. **Check CanPickup() Validation:**
   - Verify C++ code checks `HasSpaceFor()` in `CanPickup()`
   - Check console for validation messages

2. **Check InventoryComponent:**
   - Verify `HasSpaceFor()` method works correctly
   - Test with manually filled inventory

3. **Check Weight/Capacity Limits:**
   - Verify limits are set correctly
   - Check limits are enforced

---

## Step 10: Final Verification

### 10.1 Compilation Check

1. **Compile All Blueprints**
   - In Content Browser, select all Blueprint files
   - Right-click → `Compile`
   - Wait for compilation to finish
   - Check for errors

2. **Compile C++ Code**
   - In Unreal Editor: `Tools → Refresh Visual Studio Project`
   - Or compile in Visual Studio
   - Verify no compilation errors

### 10.2 Final Test

1. **Play in Editor**
   - Test all pickup scenarios
   - Test normal pickup
   - Test full inventory
   - Test stacking
   - Test weight limits

2. **Check Console**
   - Look for any errors
   - Verify pickup messages appear
   - Check inventory update messages

3. **Document Any Issues**
   - Note any problems encountered
   - Note solutions found
   - Update progress document if needed

---

## Verification Checklist

Before considering Days 17-18 complete, verify:

- [ ] ItemPickupActor C++ class compiles without errors
- [ ] Blueprint class `BP_ItemPickup` created
- [ ] Components configured correctly (CollisionComponent, MeshComponent)
- [ ] Collision settings correct (Overlap detection enabled)
- [ ] Item Data property can be set in Blueprint
- [ ] Pickup spawns in world correctly
- [ ] Visual representation displays (mesh visible)
- [ ] Player overlap detected correctly
- [ ] Item added to inventory on pickup
- [ ] Pickup destroyed after successful pickup
- [ ] Full inventory scenario handled correctly
- [ ] Stacking works correctly
- [ ] Weight limit enforced
- [ ] Multiple pickups work independently
- [ ] Console logs appear correctly
- [ ] No crashes or errors
- [ ] Ready for Day 19-20 (Item Usage System)

---

## Quick Reference: Component Settings

### CollisionComponent Settings
- **Collision Enabled:** `Query Only`
- **Object Type:** `WorldDynamic`
- **Pawn Response:** `Overlap`
- **Sphere Radius:** `50.0` (adjust as needed)

### MeshComponent Settings
- **Static Mesh:** Assign a mesh (Cube, Sphere, etc.)
- **Collision Enabled:** `No Collision`
- **Mobility:** `Movable`
- **Scale:** `(1, 1, 1)` or appropriate size

### Pickup Properties
- **Item Data:** Set to Item Data Asset (or None for generic)
- **Quantity:** `1` (default, can be overridden)

---

## Next Steps

After completing Days 17-18:

1. **Day 19-20** will focus on:
   - Item Usage System
   - UseItem implementation in InventoryComponent
   - Item consumption logic
   - Input integration for item usage

2. **Before Day 19-20**, verify:
   - ✅ ItemPickupActor working
   - ✅ Items can be picked up
   - ✅ Items appear in inventory
   - ✅ Ready to implement usage system

---

## Notes

- **Collision Detection:** Uses overlap events, not collision
- **Item Creation:** Uses ItemDatabase to create ItemBase instances
- **Inventory Integration:** Requires InventoryComponent on PlayerCharacter
- **Visual Feedback:** Can be enhanced with particles, sounds, animations
- **Testing:** Test thoroughly with different item types and scenarios

---

**Status:** Complete these manual steps to finish Days 17-18

**Next:** Day 19-20 - Item Usage System
