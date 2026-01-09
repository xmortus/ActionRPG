# Phase 2 Manual Steps - Days 15-16
**After Code Implementation**  
**Date:** 2025-01-07  
**Phase:** Phase 2 - Inventory System  
**Days:** 15-16 - Inventory Component Foundation

---

## Overview

After completing the C++ code implementation for Days 15-16, you need to perform the following manual steps in the Unreal Editor to complete the InventoryComponent setup. These steps are required to make the component functional in the editor and attach it to the player character.

---

## Prerequisites

- ✅ All C++ code compiled successfully
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

3. **Verify Hot Reload**
   - Look for message: `LogHotReload: Hot reload completed successfully`
   - If hot reload failed, close and reopen Unreal Editor

### 1.2 Check Content Browser

1. **Navigate to Content Browser**
   - Make sure `Content/` folder is visible

2. **Verify Folders Exist**
   - Check that `Content/Blueprints/Components/` exists
   - If not, create it (see Step 2.1)

---

## Step 2: Create Folder Structure

### 2.1 Create Content Folders (If Not Created)

1. **Navigate to Content Browser**
   - Go to `Content/` folder

2. **Create Blueprints/Components Folder**
   - Right-click in Content Browser
   - Select `New Folder`
   - Name it: `Components`
   - Location: `Content/Blueprints/Components/`

3. **Create Inventory Subfolder**
   - Right-click on `Components` folder
   - Select `New Folder`
   - Name it: `Inventory`
   - Location: `Content/Blueprints/Components/Inventory/`

**Final Structure Should Be:**
```
Content/
└── Blueprints/
    └── Components/
        └── Inventory/
            └── (BP_InventoryComponent will go here)
```

---

## Step 3: Create Blueprint Class from InventoryComponent

### 3.1 Create Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Components/Inventory/` folder

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - In the "Pick Parent Class" dialog:
     - Search for: `InventoryComponent`
     - You should see: `Inventory Component` in the list
     - If you don't see it:
       - Make sure C++ code compiled successfully
       - Restart Unreal Editor
       - Check class name matches exactly
     - Select `Inventory Component` (the one that appears in the component list)
     - Click `Select`

3. **Name the Blueprint**
   - Name it: `BP_InventoryComponent`
   - Save it in `Content/Blueprints/Components/Inventory/`

4. **Verify Blueprint Created**
   - The Blueprint should appear in Content Browser
   - Icon should show as a component (gear icon)

### 3.2 Open and Configure Blueprint

1. **Double-click BP_InventoryComponent**
   - Blueprint Editor should open

2. **Check Defaults Tab**
   - Click on `Class Defaults` button (top toolbar)
   - Or select `Class Defaults` in the viewport dropdown

3. **Verify Properties Are Visible**
   - In the `Details` panel, you should see:
     - **Inventory** category:
       - `Inventory Slots` (Array, should be empty initially)
       - `Max Capacity` (int32, default: 50)
       - `Max Weight` (float, default: 100.0)
     - **Events** category:
       - `On Inventory Changed` (Event)
       - `On Item Added` (Event)
       - `On Item Removed` (Event)
       - `On Item Used` (Event)

4. **Adjust Default Values (Optional)**
   - Set `Max Capacity` to desired value (default: 50 is fine)
   - Set `Max Weight` to desired value (default: 100.0 is fine)
   - These can also be set in PlayerCharacter Blueprint

5. **Compile Blueprint**
   - Click `Compile` button (top toolbar)
   - Should compile without errors
   - Look for green checkmark

6. **Save Blueprint**
   - Click `Save` button (top toolbar)
   - Or press `Ctrl + S`

7. **Close Blueprint Editor**
   - Close the Blueprint Editor window

---

## Step 4: Attach Component to Player Character

### 4.1 Open Player Character Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Characters/` folder

2. **Open Player Character Blueprint**
   - Double-click `BP_ActionRPGPlayerCharacter`
   - Blueprint Editor should open

### 4.2 Add Inventory Component

1. **Open Components Panel**
   - On the left side, find the `Components` panel
   - If not visible, go to: `Window` → `Components`

2. **Add Component**
   - Click `+ Add` button at the top of Components panel
   - Search for: `Inventory`
   - Select: `Inventory Component`
   - OR search for: `BP_InventoryComponent`
   - Select: `BP Inventory Component` (the Blueprint version)

3. **Verify Component Added**
   - The component should appear in the Components list
   - Name should be: `InventoryComponent` or `BP_InventoryComponent`
   - Icon should be a gear (component icon)

4. **Rename Component (Optional)**
   - Select the component in Components list
   - In `Details` panel, find `Name` field
   - Set to: `InventoryComponent` (for consistency)

### 4.3 Configure Component Properties

1. **Select InventoryComponent in Components List**
   - Click on `InventoryComponent` in Components panel

2. **Check Details Panel**
   - In `Details` panel, you should see:
     - **Inventory** category with properties
     - **Events** category with events

3. **Set Default Values (Optional)**
   - **Max Capacity**: Set to desired value (default: 50)
   - **Max Weight**: Set to desired value (default: 100.0)
   - Leave `Inventory Slots` empty (will be populated at runtime)

4. **Verify Component Settings**
   - Component should be `Instance Editable` by default
   - Component should be `Visible` by default
   - Component should be `Can Edit Class` if needed

### 4.4 Compile and Save

1. **Compile Blueprint**
   - Click `Compile` button (top toolbar)
   - Should compile without errors
   - Look for green checkmark or success message

2. **Save Blueprint**
   - Click `Save` button (top toolbar)
   - Or press `Ctrl + S`

3. **Check for Warnings**
   - Look for any yellow warnings in Output Log
   - Most warnings are informational, but check if any are critical

---

## Step 5: Test Component Attachment

### 5.1 Verify Component in Editor

1. **Select PlayerCharacter in Viewport (If in Level)**
   - If you have a level open with the player character
   - Select the player character in the viewport

2. **Check Details Panel**
   - In `Details` panel, scroll to find `Inventory Component`
   - You should see the InventoryComponent component listed
   - Expand it to see properties

3. **Verify Properties Are Accessible**
   - Properties should be visible and editable
   - Can modify Max Capacity and Max Weight in editor

### 5.2 Test in Play Mode

1. **Open Test Level**
   - Open your test level (or create a new one)
   - Make sure Player Start exists
   - Make sure GameMode is set to `BP_ActionRPGGameMode`

2. **Start Play**
   - Press `Play` button (or `P` key)
   - Game should start normally

3. **Check Output Log**
   - Open `Output Log` (Window → Developer Tools → Output Log)
   - Look for any errors related to InventoryComponent
   - Should not see any component-related errors

4. **Verify Component Initialized**
   - Component should initialize automatically on BeginPlay
   - No errors should occur during initialization

5. **Stop Play**
   - Press `Esc` or click `Stop` button
   - Return to editor

---

## Step 6: Verify Component Integration

### 6.1 Check Component Reference

1. **Open PlayerCharacter Blueprint**
   - Open `BP_ActionRPGPlayerCharacter`

2. **Open Event Graph**
   - Click `Event Graph` tab (or `Graph` button)

3. **Test Component Access**
   - Right-click in graph
   - Search for: `Get Inventory Component`
   - Should find: `Get Inventory Component` node
   - If found, component is properly integrated
   - If not found:
     - Check component is actually added to character
     - Recompile Blueprint
     - Restart editor if needed

### 6.2 Verify Component Methods (Optional)

1. **In Event Graph**
   - Right-click in graph
   - Search for: `Inventory Component`
   - You should see various methods:
     - `Add Item`
     - `Remove Item`
     - `Get Item At`
     - `Use Item`
     - etc.

2. **Test Method Access**
   - If methods appear, component is properly exposed
   - If methods don't appear:
     - Check C++ code has `BlueprintCallable` on methods
     - Recompile C++ code
     - Recompile Blueprint
     - Restart editor if needed

---

## Step 7: Create Test Setup (Optional)

### 7.1 Create Test Blueprint (For Later Testing)

1. **Create Test Blueprint**
   - Create a simple Blueprint for testing inventory
   - Can be done in later days when testing item usage

2. **Note Component Reference**
   - Remember that InventoryComponent is now attached to player
   - Can be accessed from any Blueprint that references player

---

## Troubleshooting

### Issue: InventoryComponent Doesn't Appear in Parent Class Picker

**Symptoms:** Can't find InventoryComponent when creating Blueprint

**Solutions:**
1. **Check Compilation**
   - Make sure C++ code compiled without errors
   - Check Visual Studio build output
   - Fix any compilation errors first

2. **Restart Unreal Editor**
   - Close Unreal Editor completely
   - Reopen Unreal Editor
   - Wait for project to fully load
   - Try creating Blueprint again

3. **Check Class Name**
   - Verify class name in C++ is exactly `UInventoryComponent`
   - Check `UCLASS()` macro is correct
   - Verify `BlueprintSpawnableComponent` meta tag is present

4. **Check Module Name**
   - Verify module name matches project name (ActionRPG)
   - Check `ACTIONRPG_API` macro is used

5. **Check Folder Location**
   - Make sure header file is in `Public/Components/Inventory/`
   - Make sure cpp file is in `Private/Components/Inventory/`
   - Rebuild project from scratch if needed

### Issue: Component Doesn't Appear in Components Panel

**Symptoms:** Can't find InventoryComponent when adding to PlayerCharacter

**Solutions:**
1. **Check Blueprint is Created**
   - Make sure `BP_InventoryComponent` was created successfully
   - Verify it appears in Content Browser

2. **Use Search Function**
   - In Components panel, use search box
   - Search for: `Inventory` or `BP_InventoryComponent`
   - Should find the component

3. **Check Component Type**
   - Make sure you're looking for a Component (gear icon)
   - Not an Actor or other type

4. **Recompile C++ Code**
   - Close Unreal Editor
   - Rebuild C++ project in Visual Studio
   - Reopen Unreal Editor
   - Try again

### Issue: Component Properties Not Visible

**Symptoms:** Can't see inventory properties in Details panel

**Solutions:**
1. **Check Component is Selected**
   - Make sure component is selected in Components panel
   - Details panel should show component properties

2. **Check Category**
   - Properties are in "Inventory" category
   - Make sure category is expanded

3. **Check UPROPERTY Specifiers**
   - Verify `EditAnywhere` or `BlueprintReadWrite` is set
   - Verify `Category = "Inventory"` is set
   - Recompile if needed

4. **Restart Editor**
   - Sometimes editor needs restart after adding new properties
   - Close and reopen Unreal Editor

### Issue: Blueprint Compile Errors

**Symptoms:** Blueprint won't compile after adding component

**Solutions:**
1. **Check Error Message**
   - Read the error message in Output Log
   - Error usually indicates what's wrong

2. **Check Component Attachment**
   - Make sure component is properly added
   - Check component name doesn't conflict

3. **Check Dependencies**
   - Verify ItemBase and ItemDataAsset are available
   - Check all includes are correct in C++

4. **Recompile C++ First**
   - Always compile C++ code before Blueprint
   - Fix C++ errors before Blueprint errors

---

## Verification Checklist

Before moving to Days 17-18, verify:

- [ ] InventoryComponent C++ code compiles without errors
- [ ] Blueprint class `BP_InventoryComponent` created successfully
- [ ] Component added to `BP_ActionRPGPlayerCharacter`
- [ ] Component properties visible in Details panel
- [ ] Component methods accessible in Blueprint Event Graph
- [ ] Blueprint compiles without errors
- [ ] No errors in Output Log during Play mode
- [ ] Component initializes correctly in game

---

## Next Steps

Once Days 15-16 are complete:

1. **Days 17-18** will focus on:
   - Creating ItemPickupActor class
   - Implementing pickup logic
   - Integrating with InventoryComponent
   - Testing item pickup flow

2. **Before Days 17-18**, you should have:
   - Working InventoryComponent attached to player
   - Component can add/remove items (tested via Blueprint or C++)
   - Component events fire correctly
   - Ready for ItemPickupActor integration

---

## Notes

- **Component Lifecycle:** InventoryComponent will initialize automatically when PlayerCharacter spawns
- **Runtime vs Editor:** Component properties can be edited in editor, but slot contents are runtime only
- **Blueprint vs C++:** Can use either C++ component or Blueprint component; Blueprint allows easier tweaking
- **Testing:** Can test component methods via Blueprint Event Graph or C++ code
- **Performance:** Component is lightweight; inventory operations are efficient

---

## Quick Reference

### Component Paths
- **C++ Header:** `Source/ActionRPG/Public/Components/Inventory/InventoryComponent.h`
- **C++ Implementation:** `Source/ActionRPG/Private/Components/Inventory/InventoryComponent.cpp`
- **Blueprint:** `Content/Blueprints/Components/Inventory/BP_InventoryComponent`
- **Player Character:** `Content/Blueprints/Characters/BP_ActionRPGPlayerCharacter`

### Key Properties
- `Max Capacity`: 50 (default)
- `Max Weight`: 100.0 (default)
- `Inventory Slots`: Array of FInventorySlot (runtime)

### Key Methods (To Test Later)
- `Add Item`: Add item to inventory
- `Remove Item`: Remove item from inventory
- `Get Item At`: Get item at specific slot
- `Use Item`: Use item from inventory
- `Get Current Weight`: Calculate total weight

---

**Status:** Complete these manual steps before proceeding to Days 17-18

---

**End of Phase 2 Manual Steps - Days 15-16**
