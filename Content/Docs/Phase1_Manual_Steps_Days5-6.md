# Phase 1 Manual Steps - Days 5-6: Enhanced Input System Setup
**After Code Implementation**  
**Date:** 2025-01-07

---

## Overview

Days 5-6 focus on setting up the Enhanced Input System in the Unreal Editor. The C++ code is already implemented in the PlayerController, but you need to create the Input Actions and Input Mapping Context assets manually.

**Status:** ✅ C++ code is complete - Only manual editor steps remain

---

## Prerequisites

- ✅ C++ code compiled successfully
- ✅ `BP_ActionRPGPlayerController` Blueprint created (from Days 3-4)
- ✅ Unreal Editor is open
- ✅ Project is loaded

---

## Step 1: Create Input Actions

All Input Actions will be created in `Content/Input/` folder.

### 1.1 Create IA_Move (Vector2D Input Action)

1. **Navigate to Content Browser**
   - Go to `Content/Input/` folder
   - (Create the folder if it doesn't exist)

2. **Create Input Action**
   - Right-click in Content Browser
   - Select `Input` → `Input Action`
   - Name it: `IA_Move`

3. **Configure IA_Move**
   - Double-click `IA_Move` to open
   - In the `Details` panel:
     - **Value Type:** Set to `Vector2D`
     - **Triggers:** Leave as default (or set to `Ongoing` if you want continuous input)
   - **Save** (Ctrl+S) and close

### 1.2 Create IA_Look (Vector2D Input Action)

1. **Create Input Action**
   - Right-click in `Content/Input/`
   - Select `Input` → `Input Action`
   - Name it: `IA_Look`

2. **Configure IA_Look**
   - Double-click `IA_Look` to open
   - In the `Details` panel:
     - **Value Type:** Set to `Vector2D`
     - **Triggers:** Leave as default (or set to `Ongoing`)
   - **Save** and close

### 1.3 Create Boolean Input Actions

Create the following Input Actions with `Value Type` set to `Boolean`:

#### IA_Interact
1. Right-click in `Content/Input/`
2. `Input` → `Input Action`
3. Name: `IA_Interact`
4. Open and set:
   - **Value Type:** `Boolean`
   - **Triggers:** `Started` (or leave default)
5. Save and close

#### IA_Attack
1. Create Input Action: `IA_Attack`
2. **Value Type:** `Boolean`
3. **Triggers:** `Started`
4. Save and close

#### IA_Dodge
1. Create Input Action: `IA_Dodge`
2. **Value Type:** `Boolean`
3. **Triggers:** `Started`
4. Save and close

#### IA_OpenInventory
1. Create Input Action: `IA_OpenInventory`
2. **Value Type:** `Boolean`
3. **Triggers:** `Started`
4. Save and close

### 1.4 Create Skill Slot Input Actions (Optional for Phase 1)

You can create these now or wait until Phase 2:

- `IA_SkillSlot1` through `IA_SkillSlot8`
- All with:
  - **Value Type:** `Boolean`
  - **Triggers:** `Started`

**Note:** These are already created based on your folder listing, so you can skip this step.

---

## Step 2: Create Input Mapping Context

### 2.1 Create IM_ActionRPG

1. **Navigate to Content Browser**
   - Go to `Content/Input/` folder

2. **Create Input Mapping Context**
   - Right-click in Content Browser
   - Select `Input` → `Input Mapping Context`
   - Name it: `IM_ActionRPG`

3. **Open IM_ActionRPG**
   - Double-click `IM_ActionRPG` to open the editor

### 2.2 Map Keyboard/Mouse Inputs to Actions

In the Input Mapping Context editor, you'll see a list of mappings. Add the following:

#### Map IA_Move (W/A/S/D Keys)

1. **Click the `+` button** to add a new mapping

2. **Set the Action:**
   - In the `Action` dropdown, select `IA_Move`

3. **Add Key Mappings:**
   - Click the `+` button under `Key` to add key mappings
   
   **For W Key (Forward):**
   - Click `+` under Key
   - Select `W` key
   - No modifiers needed (W = forward = positive X)
   
   **For S Key (Backward):**
   - Click `+` under Key
   - Select `S` key
   - Click `+` under `Modifiers`
   - Select `Negate`
   - Check `X` axis (this makes S = negative X = backward)
   
   **For A Key (Left):**
   - Click `+` under Key
   - Select `A` key
   - Click `+` under `Modifiers`
   - Select `Negate`
   - Check `Y` axis (this makes A = negative Y = left)
   
   **For D Key (Right):**
   - Click `+` under Key
   - Select `D` key
   - No modifiers needed (D = right = positive Y)

**Alternative Simpler Method:**
If the modifier approach is confusing, you can:
- Map W to X axis positive
- Map S to X axis negative (with Negate modifier on X)
- Map A to Y axis negative (with Negate modifier on Y)
- Map D to Y axis positive

#### Map IA_Look (Mouse XY)

1. **Click `+` to add new mapping**
2. **Set Action:** `IA_Look`
3. **Add Key Mapping:**
   - Click `+` under Key
   - Select `Mouse XY` (or `Mouse 2D-Axis`)
   - No modifiers needed
   - This automatically maps mouse movement to Vector2D

#### Map IA_Interact (E Key)

1. **Click `+` to add new mapping**
2. **Set Action:** `IA_Interact`
3. **Add Key Mapping:**
   - Click `+` under Key
   - Select `E` key
   - No modifiers needed

#### Map IA_Attack (Left Mouse Button)

1. **Click `+` to add new mapping**
2. **Set Action:** `IA_Attack`
3. **Add Key Mapping:**
   - Click `+` under Key
   - Select `Left Mouse Button`
   - No modifiers needed

#### Map IA_Dodge (Space Bar)

1. **Click `+` to add new mapping**
2. **Set Action:** `IA_Dodge`
3. **Add Key Mapping:**
   - Click `+` under Key
   - Select `Space Bar`
   - No modifiers needed

#### Map IA_OpenInventory (Tab Key)

1. **Click `+` to add new mapping**
2. **Set Action:** `IA_OpenInventory`
3. **Add Key Mapping:**
   - Click `+` under Key
   - Select `Tab` key
   - No modifiers needed

#### Map Skill Slots (1-8 Keys) - Optional

If you created the skill slot actions:

1. **For each IA_SkillSlot1 through IA_SkillSlot8:**
   - Add mapping
   - Set Action to corresponding `IA_SkillSlot#`
   - Map to number keys `1` through `8`

### 2.3 Save Input Mapping Context

1. **Save** the Input Mapping Context (Ctrl+S)
2. **Close** the editor

---

## Step 3: Assign Input Actions to PlayerController Blueprint

### 3.1 Open BP_ActionRPGPlayerController

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/`
   - Double-click `BP_ActionRPGPlayerController` to open

### 3.2 Assign Input Actions in Details Panel

1. **Select the Blueprint Root**
   - Click on the class name at the top of the Blueprint editor (the root node)

2. **In the Details Panel**, scroll to find the `Input` category

3. **Assign Input Actions:**
   - **Default Mapping Context:** 
     - Set to `IM_ActionRPG` (drag from Content Browser or use dropdown)
   
   - **Move Action:**
     - Set to `IA_Move`
   
   - **Look Action:**
     - Set to `IA_Look`
   
   - **Interact Action:**
     - Set to `IA_Interact`
   
   - **Attack Action:**
     - Set to `IA_Attack`
   
   - **Dodge Action:**
     - Set to `IA_Dodge`
   
   - **Open Inventory Action:**
     - Set to `IA_OpenInventory`

4. **Compile the Blueprint**
   - Click the `Compile` button in the top toolbar
   - Check for any errors (should be none if everything is assigned)

5. **Save the Blueprint**
   - Click `Save` (Ctrl+S)
   - Close the Blueprint editor

---

## Step 4: Verify GameMode Configuration

### 4.1 Check BP_ActionRPGGameMode

1. **Open** `Content/Blueprints/Core/BP_ActionRPGGameMode`

2. **Verify Default Classes:**
   - `Default Pawn Class` = `BP_ActionRPGPlayerCharacter`
   - `Player Controller Class` = `BP_ActionRPGPlayerController`
   - `HUD Class` = `BP_ActionRPGHUD` (if created)

3. **Compile and Save**

### 4.2 Check Project Settings

1. **Open Project Settings**
   - Edit → Project Settings
   - Or press `Ctrl + ,`

2. **Navigate to Game → Game Mode**
   - Verify `Default GameMode` = `BP_ActionRPGGameMode`

3. **Close Project Settings**

---

## Step 5: Test the Input System

### 5.1 Create/Open a Test Level

1. **Open or Create a Level**
   - File → New Level → `Empty Level`
   - Or use an existing level

2. **Add Player Start**
   - In the `Place Actors` panel, search for `Player Start`
   - Drag it into the level
   - Position it where you want the player to spawn

### 5.2 Test in Play Mode

1. **Open Output Log**
   - Window → Developer Tools → `Output Log`
   - Or press `Ctrl + Shift + L`
   - Set log level to show `Log` and `Warning` messages

2. **Play in Editor**
   - Click `Play` button (top toolbar)
   - Or press `Alt + P`

3. **Test Movement (W/A/S/D)**
   - Press `W` → Character should move forward
   - Press `S` → Character should move backward
   - Press `A` → Character should move left
   - Press `D` → Character should move right
   - Check Output Log for "OnMove called" messages

4. **Test Mouse Look**
   - Move mouse around
   - Character should rotate to face mouse cursor (if implemented)
   - Check Output Log for any look-related messages

5. **Test Action Inputs**
   - Press `E` → Should see "Interact pressed" in console
   - Press `Left Click` → Should see "Attack pressed" in console
   - Press `Space` → Should see "Dodge pressed" in console
   - Press `Tab` → Should see "Open Inventory pressed" in console

6. **Stop Play**
   - Press `Esc` or click `Stop` button

### 5.3 Verify Console Output

In the Output Log, you should see:

**On Startup:**
- `"ActionRPGPlayerController BeginPlay"`
- `"Input Mapping Context added successfully"`
- `"SetupInputComponent called"`
- `"EnhancedInputComponent found"`
- `"MoveAction bound"`
- `"LookAction bound"`

**When Pressing Keys:**
- `"OnMove called: X=..., Y=..."` when pressing W/A/S/D
- `"Interact pressed"` when pressing E
- `"Attack pressed"` when clicking left mouse
- `"Dodge pressed"` when pressing Space
- `"Open Inventory pressed"` when pressing Tab

---

## Troubleshooting

### Issue: Input Actions Don't Appear in Dropdown

**Solution:**
- Make sure Input Actions are saved
- Close and reopen the Blueprint
- Recompile the Blueprint
- Restart the editor if needed

### Issue: "MoveAction is NULL" in Logs

**Problem:** Input Action not assigned to PlayerController

**Solution:**
1. Open `BP_ActionRPGPlayerController`
2. Verify `Move Action` is set to `IA_Move`
3. Compile the Blueprint
4. Test again

### Issue: "DefaultMappingContext is NULL" in Logs

**Problem:** Input Mapping Context not assigned

**Solution:**
1. Open `BP_ActionRPGPlayerController`
2. Verify `Default Mapping Context` is set to `IM_ActionRPG`
3. Compile the Blueprint
4. Test again

### Issue: Movement Doesn't Work

**Checklist:**
- ✅ `IA_Move` is Vector2D type
- ✅ `IM_ActionRPG` has W/A/S/D keys mapped to `IA_Move`
- ✅ `BP_ActionRPGPlayerController` has `Move Action` assigned
- ✅ `BP_ActionRPGPlayerController` has `Default Mapping Context` assigned
- ✅ Blueprint is compiled
- ✅ Character is spawned in level

### Issue: Mouse Look Doesn't Work

**Note:** Mouse look functionality depends on implementation. If you're using the cursor-to-world approach:
- Make sure mouse cursor is enabled
- Check that `IA_Look` is mapped to `Mouse XY`
- Verify the Look function is implemented in the character

### Issue: Keys Don't Trigger Actions

**Checklist:**
- ✅ Input Actions are created and saved
- ✅ Input Mapping Context has correct key mappings
- ✅ Keys are mapped to correct actions
- ✅ Modifiers are set correctly (if needed)
- ✅ PlayerController Blueprint has actions assigned
- ✅ Blueprint is compiled

---

## Verification Checklist

Before considering Days 5-6 complete, verify:

- [ ] All Input Actions created (`IA_Move`, `IA_Look`, `IA_Interact`, `IA_Attack`, `IA_Dodge`, `IA_OpenInventory`)
- [ ] `IA_Move` is Vector2D type
- [ ] All other actions are Boolean type
- [ ] `IM_ActionRPG` Input Mapping Context created
- [ ] W/A/S/D keys mapped to `IA_Move` in `IM_ActionRPG`
- [ ] Mouse XY mapped to `IA_Look` in `IM_ActionRPG`
- [ ] E key mapped to `IA_Interact`
- [ ] Left Mouse Button mapped to `IA_Attack`
- [ ] Space Bar mapped to `IA_Dodge`
- [ ] Tab key mapped to `IA_OpenInventory`
- [ ] `BP_ActionRPGPlayerController` has all Input Actions assigned
- [ ] `BP_ActionRPGPlayerController` has `Default Mapping Context` = `IM_ActionRPG`
- [ ] `BP_ActionRPGPlayerController` is compiled
- [ ] Character moves with W/A/S/D keys
- [ ] Console logs show input when pressing keys
- [ ] All action inputs trigger console messages

---

## Quick Reference: Input Mapping Summary

| Action | Input Action | Key/Mouse | Type |
|--------|-------------|-----------|------|
| Move Forward | IA_Move | W | Vector2D (X+) |
| Move Backward | IA_Move | S | Vector2D (X-, Negate) |
| Move Left | IA_Move | A | Vector2D (Y-, Negate) |
| Move Right | IA_Move | D | Vector2D (Y+) |
| Look | IA_Look | Mouse XY | Vector2D |
| Interact | IA_Interact | E | Boolean |
| Attack | IA_Attack | Left Mouse | Boolean |
| Dodge | IA_Dodge | Space | Boolean |
| Open Inventory | IA_OpenInventory | Tab | Boolean |

---

## Next Steps

After completing Days 5-6:

1. **Days 7-8** will focus on:
   - Item System Foundation
   - Creating ItemDataAsset and ItemBase classes
   - ItemDatabase implementation

2. **Before Days 7-8**, you should have:
   - ✅ Working input system
   - ✅ Character movement functional
   - ✅ All input actions working
   - ✅ Ready to implement item system

---

## Notes

- **Input Actions** are data assets that define what type of input they expect
- **Input Mapping Context** maps physical keys/mouse to Input Actions
- **PlayerController** connects Input Actions to game logic
- Always **compile Blueprints** after making changes
- **Test frequently** in Play mode to catch issues early

---

**Status:** Complete these manual steps to finish Days 5-6

