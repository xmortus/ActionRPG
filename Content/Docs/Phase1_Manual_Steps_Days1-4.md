# Phase 1 Manual Steps - Days 1-4
**After Code Implementation**  
**Date:** 2025-01-07

---

## Overview

After completing the C++ code implementation for Days 1-4, you need to perform the following manual steps in the Unreal Editor to complete the setup. These steps are required to make the code functional in the editor.

---

## Prerequisites

- ✅ All C++ code compiled successfully
- ✅ Unreal Editor is open
- ✅ Project is loaded in the editor

---

## Step 1: Create Blueprint Classes from C++ Classes

### 1.1 Create Blueprint GameMode

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - In the "Pick Parent Class" dialog, search for `ActionRPGGameMode`
   - Select `ActionRPGGameMode` (not the C++ class, the one that appears in the list)
   - Click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGGameMode`
   - Save it in `Content/Blueprints/Core/`

4. **Configure the Blueprint** (Optional for now)
   - Double-click to open
   - No changes needed yet - leave as default
   - Close the Blueprint editor

### 1.2 Create Blueprint PlayerController

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - Search for `ActionRPGPlayerController`
   - Select it and click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGPlayerController`
   - Save it in `Content/Blueprints/Core/`

4. **Configure the Blueprint** (We'll add Input Actions later)
   - Double-click to open
   - Note: We'll assign Input Actions in Step 3
   - Close the Blueprint editor for now

### 1.3 Create Blueprint Character Classes

#### Create Base Character Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Characters/` folder

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - Search for `ActionRPGCharacter`
   - Select it and click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGCharacter`
   - Save it in `Content/Blueprints/Characters/`

4. **Close the Blueprint** (No configuration needed yet)

#### Create Player Character Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Characters/` folder

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - Search for `ActionRPGPlayerCharacter`
   - Select it and click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGPlayerCharacter`
   - Save it in `Content/Blueprints/Characters/`

4. **Configure the Blueprint**
   - Double-click to open
   - In the `Details` panel, you can adjust:
     - Movement settings (if needed)
     - Mesh (add a skeletal mesh if you have one)
   - Close the Blueprint editor

### 1.4 Create Blueprint GameState (Optional)

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder

2. **Create Blueprint from C++ Class**
   - Right-click → `Blueprint Class`
   - Search for `ActionRPGGameState`
   - Select it and click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGGameState`
   - Save it in `Content/Blueprints/Core/`

### 1.5 Create Blueprint HUD (Optional)

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder (or create `Content/Blueprints/UI/`)

2. **Create Blueprint from C++ Class**
   - Right-click → `Blueprint Class`
   - Search for `ActionRPGHUD`
   - Select it and click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGHUD`
   - Save it in `Content/Blueprints/Core/` or `Content/Blueprints/UI/`

---

## Step 2: Configure Project Settings

### 2.1 Set Default GameMode

1. **Open Project Settings**
   - Go to `Edit` → `Project Settings`
   - Or press `Ctrl + ,` (comma)

2. **Navigate to Game Mode**
   - In the left panel, expand `Game` section
   - Click on `Game Mode`

3. **Set Default GameMode**
   - Find `Default Modes` section
   - Set `Default GameMode` to `BP_ActionRPGGameMode`
   - (If it doesn't appear, make sure you compiled the C++ code and restarted the editor)

4. **Verify Default Classes** (Should be auto-set by GameMode)
   - `Default Pawn Class` should show `BP_ActionRPGPlayerCharacter` (or ActionRPGPlayerCharacter)
   - `Player Controller Class` should show `BP_ActionRPGPlayerController` (or ActionRPGPlayerController)
   - `HUD Class` should show `BP_ActionRPGHUD` (or ActionRPGHUD)

5. **Close Project Settings**

---

## Step 3: Create Input Actions (Content/Input/)

### 3.1 Create IA_Move (Vector2D Input Action)

1. **Navigate to Content Browser**
   - Go to `Content/Input/` folder

2. **Create Input Action**
   - Right-click in Content Browser
   - Select `Input` → `Input Action`
   - Name it: `IA_Move`

3. **Configure IA_Move**
   - Double-click `IA_Move` to open
   - Set `Value Type` to `Vector2D`
   - Leave other settings as default
   - Save and close

### 3.2 Create IA_Look (Vector2D Input Action)

1. **Create Input Action**
   - Right-click in `Content/Input/`
   - Select `Input` → `Input Action`
   - Name it: `IA_Look`

2. **Configure IA_Look**
   - Double-click `IA_Look` to open
   - Set `Value Type` to `Vector2D`
   - Save and close

### 3.3 Create Boolean Input Actions

Create the following Input Actions with `Value Type` set to `Boolean`:

1. **IA_Interact**
   - Right-click in `Content/Input/`
   - `Input` → `Input Action`
   - Name: `IA_Interact`
   - Value Type: `Boolean`
   - Save and close

2. **IA_Attack**
   - Name: `IA_Attack`
   - Value Type: `Boolean`
   - Save and close

3. **IA_Dodge**
   - Name: `IA_Dodge`
   - Value Type: `Boolean`
   - Save and close

4. **IA_OpenInventory**
   - Name: `IA_OpenInventory`
   - Value Type: `Boolean`
   - Save and close

### 3.4 Create Skill Slot Input Actions (Optional for Phase 1)

You can create these now or wait until Phase 2:

- `IA_SkillSlot1` through `IA_SkillSlot8`
- All with `Value Type: Boolean`

---

## Step 4: Create Input Mapping Context

### 4.1 Create IM_ActionRPG

1. **Navigate to Content Browser**
   - Go to `Content/Input/` folder

2. **Create Input Mapping Context**
   - Right-click in Content Browser
   - Select `Input` → `Input Mapping Context`
   - Name it: `IM_ActionRPG`

3. **Open IM_ActionRPG**
   - Double-click `IM_ActionRPG` to open

### 4.2 Map Keyboard/Mouse Inputs

In the Input Mapping Context editor:

1. **Add IA_Move Mapping**
   - Click `+` button to add a new mapping
   - Set `Action` to `IA_Move`
   - Click `+` under `Key` to add key mappings:
     - Add `W` key → Set `Modifiers` → `Swizzle Input Axis Values` → `YXZ` (to swap X and Y)
     - Add `S` key → Set `Modifiers` → `Swizzle Input Axis Values` → `YXZ` → `Negate` the Y value
     - Add `A` key → Set `Modifiers` → `Swizzle Input Axis Values` → `YXZ` → `Negate` the X value
     - Add `D` key → Set `Modifiers` → `Swizzle Input Axis Values` → `YXZ`
   
   **OR** (Simpler approach):
   - Add `W` → Set `Modifiers` → `Negate` → Check `Y` axis
   - Add `S` → No modifiers
   - Add `A` → Set `Modifiers` → `Negate` → Check `X` axis
   - Add `D` → No modifiers

2. **Add IA_Look Mapping**
   - Click `+` to add mapping
   - Set `Action` to `IA_Look`
   - Add `Mouse XY` → No modifiers needed

3. **Add IA_Interact Mapping**
   - Click `+` to add mapping
   - Set `Action` to `IA_Interact`
   - Add `E` key

4. **Add IA_Attack Mapping**
   - Click `+` to add mapping
   - Set `Action` to `IA_Attack`
   - Add `Left Mouse Button`

5. **Add IA_Dodge Mapping**
   - Click `+` to add mapping
   - Set `Action` to `IA_Dodge`
   - Add `Space Bar`

6. **Add IA_OpenInventory Mapping**
   - Click `+` to add mapping
   - Set `Action` to `IA_OpenInventory`
   - Add `Tab` key

7. **Save and Close** the Input Mapping Context

---

## Step 5: Assign Input Actions to PlayerController

### 5.1 Open BP_ActionRPGPlayerController

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/`
   - Double-click `BP_ActionRPGPlayerController` to open

### 5.2 Assign Input Actions

1. **Select the Blueprint Root**
   - Click on the root node (the class name at the top)

2. **In the Details Panel**, find the `Input` category

3. **Assign Input Actions:**
   - `Default Mapping Context` → Set to `IM_ActionRPG`
   - `Move Action` → Set to `IA_Move`
   - `Look Action` → Set to `IA_Look`
   - `Interact Action` → Set to `IA_Interact`
   - `Attack Action` → Set to `IA_Attack`
   - `Dodge Action` → Set to `IA_Dodge`
   - `Open Inventory Action` → Set to `IA_OpenInventory`

4. **Compile the Blueprint**
   - Click `Compile` button (top toolbar)

5. **Save and Close** the Blueprint

---

## Step 6: Test the Setup

### 6.1 Create a Test Level (or use existing)

1. **Open or Create a Level**
   - File → New Level → Choose `Empty Level` or use existing

2. **Add a Player Start**
   - In the `Place Actors` panel, search for `Player Start`
   - Drag it into the level
   - Position it where you want the player to spawn

### 6.2 Test in Editor

1. **Play in Editor**
   - Click `Play` button (top toolbar)
   - Or press `Alt + P`

2. **Test Movement**
   - Press `W/A/S/D` keys
   - Character should move
   - Check console for input logs (if you see the log messages, input is working)

3. **Test Other Inputs**
   - Press `E` → Should see "Interact pressed" in console
   - Press `Left Click` → Should see "Attack pressed" in console
   - Press `Space` → Should see "Dodge pressed" in console
   - Press `Tab` → Should see "Open Inventory pressed" in console

4. **Stop Play**
   - Press `Esc` or click `Stop` button

### 6.3 Verify Console Output

1. **Open Output Log**
   - Window → `Developer Tools` → `Output Log`
   - Or press `Ctrl + Shift + L`

2. **Check for Log Messages**
   - You should see log messages when pressing action keys
   - If you don't see messages, check:
     - Input Actions are assigned correctly
     - Input Mapping Context is set
     - Blueprint is compiled

---

## Step 7: Verify Blueprint Classes in GameMode

### 7.1 Open BP_ActionRPGGameMode

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/`
   - Double-click `BP_ActionRPGGameMode`

2. **Check Default Classes**
   - In the `Details` panel, verify:
     - `Default Pawn Class` is set to `BP_ActionRPGPlayerCharacter`
     - `Player Controller Class` is set to `BP_ActionRPGPlayerController`
     - `HUD Class` is set to `BP_ActionRPGHUD` (if created)

3. **Compile and Save**
   - Click `Compile`
   - Click `Save`
   - Close the Blueprint

---

## Troubleshooting

### Issue: Blueprint Classes Don't Appear in Parent Class List

**Solution:**
1. Close Unreal Editor
2. Recompile C++ code in Visual Studio
3. Reopen Unreal Editor
4. The classes should now appear

### Issue: Input Actions Don't Work

**Checklist:**
- ✅ Input Actions are created in `Content/Input/`
- ✅ Input Mapping Context has all mappings
- ✅ PlayerController Blueprint has Input Actions assigned
- ✅ PlayerController Blueprint is compiled
- ✅ GameMode is set in Project Settings
- ✅ Default Pawn Class is set correctly

### Issue: Character Doesn't Move

**Checklist:**
- ✅ IA_Move is assigned in PlayerController
- ✅ IM_ActionRPG is assigned as Default Mapping Context
- ✅ Movement keys (W/A/S/D) are mapped in Input Mapping Context
- ✅ Character Blueprint is set as Default Pawn
- ✅ Player Start exists in the level

### Issue: Console Logs Don't Appear

**Solution:**
- Make sure `Output Log` window is open
- Check that log level is set to show `Warning` messages
- Verify the input handlers are being called (add breakpoints if needed)

---

## Verification Checklist

Before moving to Days 5-6, verify:

- [ ] All Blueprint classes created from C++ classes
- [ ] GameMode set in Project Settings
- [ ] All Input Actions created (IA_Move, IA_Look, IA_Interact, IA_Attack, IA_Dodge, IA_OpenInventory)
- [ ] Input Mapping Context created and configured (IM_ActionRPG)
- [ ] Input Actions assigned to PlayerController Blueprint
- [ ] PlayerController Blueprint compiled
- [ ] Character moves with W/A/S/D keys
- [ ] Console logs appear when pressing action keys
- [ ] No errors in Output Log

---

## Next Steps

Once all manual steps are complete:

1. **Days 5-6** will focus on:
   - Testing Enhanced Input System thoroughly
   - Fine-tuning input mappings
   - Adding gamepad support (optional)
   - Preparing for Item/Skill system implementation

2. **Before Days 7-8**, you should have:
   - Working input system
   - Character movement functional
   - All core framework classes working

---

## Notes

- **Blueprint Compilation**: Always compile Blueprints after making changes
- **Editor Restart**: Sometimes you need to restart the editor after C++ changes
- **Hot Reload**: C++ changes can be hot-reloaded, but Blueprint changes require compilation
- **Testing**: Test frequently in Play mode to catch issues early

---

**Status:** Complete these manual steps before proceeding to Days 5-6

