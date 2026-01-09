# InventoryComponent Settings Guide
**Understanding Blueprint Details Panel Settings**

---

## Overview

This guide explains what settings/properties are visible in the BP_InventoryComponent Details panel and where to find them.

---

## Settings Categories in Details Panel

### 1. Component Tick

**Location:** Details Panel → Component Tick category

**Properties:**
- `Start with Tick Enabled` - Should be **UNCHECKED** (component doesn't tick)
- `Tick Interval (secs)` - Set to `0.0` (not used since tick is disabled)
- `Tick Even when Paused` - Unchecked
- `Allow Tick on Dedicated Server` - Checked (for future multiplayer)
- `Tick Group` - Set to "During Physics" (default)

**Note:** Even though tick is disabled, UE still shows these settings. This is normal. The component will NOT tick because `bCanEverTick = false` in C++.

---

### 2. Inventory Category

**Location:** Details Panel → Inventory category

**Properties:**
- `Inventory Slots` (Array) - 50 array elements (runtime slots, not editable directly)
- `Max Capacity` (int32) - Default: 50 (editable)
- `Max Weight` (float) - Default: 100.0 (editable)

**Note:** Inventory Slots array shows 50 elements, but they're initialized at runtime. You don't need to edit them directly.

---

### 3. Inventory|Events Subcategory

**Location:** Details Panel → Inventory category → Expand to see Events subcategory

**Events (Delegate Properties):**
- `On Inventory Changed` - Delegate with "+" button
- `On Item Added` - Delegate with "+" button
- `On Item Removed` - Delegate with "+" button
- `On Item Used` - Delegate with "+" button

**How to Use:**
1. Expand the "Inventory" category
2. Look for the "Events" subcategory (or scroll down in Inventory category)
3. Each event has a "+" button next to it
4. Click "+" to create a Custom Event handler in Event Graph

**Note:** If events don't appear:
- Make sure you're viewing **Class Defaults** or the component is **selected**
- Scroll down in the Inventory category (events are at the bottom)
- Recompile C++ code if you just added them
- Recompile Blueprint
- Restart editor if still not visible

---

## Viewing Context Matters

### Class Defaults View (BP_InventoryComponent Blueprint)

When viewing the Blueprint class itself (Class Defaults):
- ✅ All properties are visible (Inventory Slots, Max Capacity, Max Weight)
- ✅ Component Tick settings are visible
- ⚠️ **Events might not always show** in Class Defaults view
- ✅ Methods are available in Event Graph

### Component Instance View (Component on Actor)

When the component is added to an actor (e.g., BP_ActionRPGPlayerCharacter):
- ✅ All properties are visible
- ✅ Events are **more reliably visible** here
- ✅ Events can be bound via "+" button
- ✅ Component Tick settings are visible

**Recommendation:** To bind events, select the component in the actor's Components panel (not in Class Defaults).

---

## Expected Settings Summary

### ✅ Should Be Visible

**Inventory Category:**
1. `Inventory Slots` - Array (50 elements)
2. `Max Capacity` - int32 (50)
3. `Max Weight` - float (100.0)

**Inventory|Events Subcategory (After Recompiling):**
1. `On Inventory Changed` - Delegate
2. `On Item Added` - Delegate
3. `On Item Removed` - Delegate
4. `On Item Used` - Delegate

**Component Tick:**
1. `Start with Tick Enabled` - Unchecked (component doesn't tick)
2. Other tick settings (as listed above)

### ⚠️ Might Not Show in Class Defaults

- Events (delegates) sometimes only show when component is on an actor
- This is normal UE behavior for `BlueprintAssignable` delegates

### ❌ Should NOT Be Visible

- None - all settings should be visible (somewhere)

---

## If Settings Are Missing

### Step 1: Verify C++ Code Compiled

1. Check Output Log for compilation errors
2. If errors exist, fix them in Visual Studio
3. Rebuild C++ project
4. Hot reload or restart editor

### Step 2: Verify Blueprint Compilation

1. Open BP_InventoryComponent
2. Click "Compile" button
3. Check for compilation errors
4. Fix any errors

### Step 3: Check View Mode

1. **For Events:** View component when it's on an actor (not in Class Defaults)
   - Open BP_ActionRPGPlayerCharacter
   - Select InventoryComponent in Components panel
   - Check Details panel for events

2. **For Properties:** Either view works
   - Class Defaults view
   - Component instance view

### Step 4: Refresh Editor

1. Close and reopen Unreal Editor
2. This forces full refresh of exposed properties

### Step 5: Verify Category

1. Events should be in "Inventory|Events" subcategory
2. Properties should be in "Inventory" category
3. Make sure categories are expanded
4. Scroll down if needed

---

## How Events Work

### Dynamic Multicast Delegates

The events are declared as `DECLARE_DYNAMIC_MULTICAST_DELEGATE` with `UPROPERTY(BlueprintAssignable)`. This means:

1. **They appear as delegate properties** (not separate event nodes)
2. **They can be bound** via "+" button in Details panel
3. **They can be bound** via "Assign" nodes in Event Graph
4. **They fire automatically** when Broadcast() is called in C++

### Binding in Blueprint

**Method 1: Details Panel "+" Button**
1. Select component instance (on actor)
2. Find event in Details panel
3. Click "+" next to event name
4. Custom Event node appears in Event Graph

**Method 2: Event Graph "Assign" Node**
1. In Event Graph, right-click
2. Search for: "Assign On Item Added" (or other event name)
3. Connect component reference to Target pin
4. Create Custom Event and connect to delegate pin

---

## Verification Checklist

After recompiling with the updated code:

- [ ] Open BP_InventoryComponent or BP_ActionRPGPlayerCharacter
- [ ] Select InventoryComponent in Components panel
- [ ] In Details panel, find "Inventory" category
- [ ] Expand "Inventory" category
- [ ] Scroll down to find "Events" subcategory (or events listed in Inventory category)
- [ ] Verify all 4 events are visible:
  - [ ] On Inventory Changed
  - [ ] On Item Added
  - [ ] On Item Removed
  - [ ] On Item Used
- [ ] Each event should have a "+" button
- [ ] Component Tick shows "Start with Tick Enabled" as Unchecked
- [ ] Max Capacity shows as 50
- [ ] Max Weight shows as 100.0

---

## Code Changes Made

**Updated:**
1. Events category changed to "Inventory|Events" (creates subcategory)
2. Added DisplayName metadata for better readability
3. Component Tick properly disabled in constructor
4. Verified all UPROPERTY specifiers are correct

**After Recompiling:**
- Events should appear in "Inventory|Events" subcategory
- Events should be visible when component is selected on an actor
- All properties should be visible

---

## Next Steps

1. **Recompile C++ Code** in Visual Studio
2. **Hot Reload** in Unreal Editor (or restart editor)
3. **Recompile Blueprint** (BP_InventoryComponent)
4. **Verify Events Appear** in Details panel
5. **Test Event Binding** by clicking "+" button

---

## Troubleshooting

### Events Still Not Visible After Recompiling

**Try:**
1. Close Unreal Editor completely
2. Rebuild C++ project from Visual Studio
3. Reopen Unreal Editor
4. Open BP_ActionRPGPlayerCharacter (not BP_InventoryComponent)
5. Select InventoryComponent in Components panel
6. Check Details panel for events

**If still not visible:**
- Check Output Log for any errors
- Verify C++ code has `BlueprintAssignable` specifier
- Verify category is "Inventory|Events"
- Check that delegates are declared correctly

### Component Tick Shows as Enabled

**This is normal if:**
- You're looking at Blueprint Class Defaults
- UE shows tick settings even when disabled

**To verify tick is actually disabled:**
- The component will NOT tick during gameplay
- `bCanEverTick = false` in C++ prevents ticking
- Tick settings in Details are just UI controls (component ignores them)

---

**End of InventoryComponent Settings Guide**
