# Phase 2: Inventory Component Events Guide
**How to Use Events in Blueprint**

---

## Overview

The InventoryComponent exposes 4 dynamic multicast delegate events that can be bound in Blueprint:

1. **On Inventory Changed** - Fired when any inventory slot changes
2. **On Item Added** - Fired when an item is successfully added
3. **On Item Removed** - Fired when an item is removed
4. **On Item Used** - Fired when an item is used from inventory

---

## Important Note

**These events appear as delegate properties in the Details panel**, NOT as separate event nodes in the Event Graph. They are located in the **"Inventory" category** alongside other properties like Max Capacity and Max Weight.

---

## Method 1: Bind via Details Panel (Easiest)

### Step-by-Step:

1. **Open PlayerCharacter Blueprint**
   - Open `BP_ActionRPGPlayerCharacter`
   - Or open `BP_InventoryComponent` directly (if you created one)

2. **Select InventoryComponent**
   - In `Components` panel (left side), select `InventoryComponent`
   - Or if using standalone Blueprint, select the component instance

3. **Find Events in Details Panel**
   - In `Details` panel (right side), scroll to **"Inventory"** category
   - Expand the category if needed
   - You should see:
     - `Inventory Slots` (Array)
     - `Max Capacity` (int32)
     - `Max Weight` (float)
     - `On Inventory Changed` (Delegate - has a "+" button)
     - `On Item Added` (Delegate - has a "+" button)
     - `On Item Removed` (Delegate - has a "+" button)
     - `On Item Used` (Delegate - has a "+" button)

4. **Bind to an Event**
   - Find the event you want (e.g., `On Item Added`)
   - Click the **"+"** button next to the event name
   - This automatically creates a Custom Event node in the Event Graph
   - The Custom Event will be named something like: `OnItemAdded` or `Event On Item Added`

5. **Implement Event Handler**
   - The Custom Event node will appear in Event Graph
   - The event will have parameters based on the delegate signature:
     - `On Item Added`: Parameter: `Item` (ItemBase object)
     - `On Item Removed`: Parameters: `Item` (ItemBase object), `Quantity` (int32)
     - `On Inventory Changed`: Parameters: `SlotIndex` (int32), `Item` (ItemBase object)
     - `On Item Used`: Parameter: `Item` (ItemBase object)
   - Connect your logic to this Custom Event node

---

## Method 2: Bind via Event Graph

### Step-by-Step:

1. **Open Event Graph**
   - Open your Blueprint (PlayerCharacter or InventoryComponent)
   - Go to `Event Graph` tab

2. **Get InventoryComponent Reference**
   - If binding from PlayerCharacter:
     - Right-click in Event Graph
     - Search for: `Get Inventory Component`
     - Drag out the output pin

3. **Create Assign Node**
   - Right-click in Event Graph
   - Search for: `Assign On Item Added` (or the event you want)
   - Or search for: `On Item Added` and select the "Assign" version

4. **Connect to Component**
   - Connect the InventoryComponent reference to the "Target" pin
   - You'll see a delegate input pin

5. **Create Custom Event**
   - Right-click in Event Graph
   - Search for: `Custom Event`
   - Or use `Add Custom Event` from the context menu
   - Name it something like: `OnItemAddedHandler`
   - Make sure the Custom Event has matching parameters

6. **Connect Event to Delegate**
   - Connect your Custom Event to the delegate input pin
   - Your Custom Event will now be called when the event fires

---

## Example: Binding OnItemAdded Event

### Using Method 1 (Details Panel):

1. Select InventoryComponent in Components panel
2. In Details panel, find "On Item Added" in Inventory category
3. Click "+" button next to "On Item Added"
4. A Custom Event appears in Event Graph: `Event On Item Added`
5. The event has parameter: `Item` (ItemBase object)
6. Connect logic to handle when item is added:
   ```
   Event On Item Added (Item: ItemBase)
   ├── Print String: "Item Added: " + Item.ItemName
   └── (Your custom logic here)
   ```

### Using Method 2 (Event Graph):

1. In Event Graph, get InventoryComponent reference
2. Right-click → Search for "Assign On Item Added"
3. Connect InventoryComponent to "Target" pin
4. Create Custom Event: "HandleItemAdded" with parameter: `Item` (ItemBase)
5. Connect Custom Event to delegate pin
6. Implement logic in Custom Event

---

## Event Signatures

### On Inventory Changed
```cpp
Parameters:
- SlotIndex (int32) - The slot that changed
- Item (ItemBase object) - The item in the slot (or null if slot is now empty)
```
**Fired when:** Any inventory slot changes (item added, removed, moved, swapped)

### On Item Added
```cpp
Parameters:
- Item (ItemBase object) - The item that was added
```
**Fired when:** An item is successfully added to inventory

### On Item Removed
```cpp
Parameters:
- Item (ItemBase object) - The item that was removed
- Quantity (int32) - The quantity that was removed
```
**Fired when:** An item (or quantity) is removed from inventory

### On Item Used
```cpp
Parameters:
- Item (ItemBase object) - The item that was used
```
**Fired when:** An item is used from inventory (via UseItem method)

---

## Troubleshooting

### Events Don't Appear in Details Panel

**Check:**
1. ✅ Component is selected in Components panel
2. ✅ "Inventory" category is expanded in Details panel
3. ✅ You're scrolling down in Details panel (events are after other properties)
4. ✅ Blueprint has been compiled
5. ✅ C++ code has been compiled

**If still not visible:**
- Restart Unreal Editor
- Recompile C++ code in Visual Studio
- Recompile Blueprint
- Check Output Log for errors

### "+" Button Doesn't Work

**Check:**
1. ✅ Blueprint is in edit mode (not read-only)
2. ✅ You have compilation errors (fix them first)
3. ✅ Component is properly created

**Alternative:**
- Use Method 2 (Event Graph binding) instead

### Events Don't Fire

**Check:**
1. ✅ Event is properly bound (Custom Event node exists)
2. ✅ Component is initialized (check BeginPlay)
3. ✅ Methods are actually being called (AddItem, RemoveItem, UseItem)
4. ✅ Check Output Log for any errors

**Debug:**
- Add Print String nodes to event handlers to verify they're being called
- Add breakpoints in C++ code to verify Broadcast() is being called

---

## Best Practices

1. **Use Method 1 (Details Panel)** for quick setup - it's the easiest
2. **Use Method 2 (Event Graph)** for more complex bindings or programmatic control
3. **Name Custom Events clearly** - e.g., "HandleItemAdded" instead of "Event 1"
4. **Handle null parameters** - Events may pass null ItemBase objects when slots become empty
5. **Unbind events when needed** - Use "Clear" or "Assign None" to remove bindings

---

## Quick Reference

### Where to Find Events
- **Location:** Details Panel → Inventory Category
- **Component:** Select InventoryComponent in Components panel
- **Appearance:** Delegate properties with "+" buttons

### How to Bind
- **Method 1:** Click "+" button in Details panel
- **Method 2:** Use "Assign" nodes in Event Graph

### Event Names
- `On Inventory Changed` (OnInventoryChanged)
- `On Item Added` (OnItemAdded)
- `On Item Removed` (OnItemRemoved)
- `On Item Used` (OnItemUsed)

---

**End of Events Guide**
