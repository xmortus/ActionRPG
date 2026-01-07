# Movement Troubleshooting Guide
**Date:** 2025-01-07

---

## Issue: Cannot Move or Look Around

This guide helps diagnose and fix movement issues in the ActionRPG project.

---

## Diagnostic Steps

### Step 1: Check Console Logs

1. **Open Output Log**
   - Press `Ctrl + Shift + L` or
   - Window → Developer Tools → Output Log

2. **Look for these log messages when pressing W/A/S/D:**
   - ✅ `"OnMove called: X=..., Y=..."` - Input is reaching the controller
   - ✅ `"Move called with Vector: X=..., Y=..."` - Input is reaching the character
   - ✅ `"Movement applied: Direction=(...)"` - Movement is being applied
   - ❌ `"MoveAction is NULL"` - Input Action not assigned
   - ❌ `"DefaultMappingContext is NULL"` - Input Mapping Context not assigned
   - ❌ `"Pawn is not AActionRPGPlayerCharacter"` - Wrong pawn class

### Step 2: Verify Blueprint Configuration

#### Check BP_ActionRPGPlayerController

1. **Open** `Content/Blueprints/Core/BP_ActionRPGPlayerController`

2. **In Details Panel, check Input category:**
   - ✅ `Default Mapping Context` = `IM_ActionRPG`
   - ✅ `Move Action` = `IA_Move`
   - ✅ `Look Action` = `IA_Look`
   - ✅ All other actions assigned

3. **Compile the Blueprint**
   - Click `Compile` button
   - Check for errors

#### Check BP_ActionRPGPlayerCharacter

1. **Open** `Content/Blueprints/Characters/BP_ActionRPGPlayerCharacter`

2. **Verify:**
   - Character has a valid mesh (or at least a capsule)
   - Character Movement Component exists
   - No errors in Blueprint

3. **Compile the Blueprint**

#### Check BP_ActionRPGGameMode

1. **Open** `Content/Blueprints/Core/BP_ActionRPGGameMode`

2. **Verify Default Classes:**
   - `Default Pawn Class` = `BP_ActionRPGPlayerCharacter`
   - `Player Controller Class` = `BP_ActionRPGPlayerController`

3. **Compile the Blueprint**

### Step 3: Verify Input Actions

1. **Check Input Actions exist:**
   - `Content/Input/IA_Move` - Should be Vector2D type
   - `Content/Input/IA_Look` - Should be Vector2D type
   - All other actions should be Boolean type

2. **Open IA_Move:**
   - Value Type = `Vector2D`
   - Triggers = `Ongoing` (or `Triggered`)

### Step 4: Verify Input Mapping Context

1. **Open** `Content/Input/IM_ActionRPG`

2. **Check Mappings:**
   - `IA_Move` should have:
     - `W` key mapped
     - `S` key mapped
     - `A` key mapped
     - `D` key mapped
   - `IA_Look` should have:
     - `Mouse XY` mapped

3. **Verify Key Mappings:**
   - Each key should have the correct action assigned
   - No duplicate mappings

### Step 5: Check Project Settings

1. **Open Project Settings**
   - Edit → Project Settings
   - Or `Ctrl + ,`

2. **Navigate to Game → Game Mode**
   - `Default GameMode` = `BP_ActionRPGGameMode`

3. **Verify Default Classes:**
   - Should match what's in BP_ActionRPGGameMode

### Step 6: Check Level Setup

1. **Verify Player Start exists:**
   - In your level, search for `Player Start`
   - Should be placed in the level
   - Character should spawn at Player Start location

2. **Check for multiple Player Starts:**
   - Only one Player Start should be active
   - Multiple Player Starts can cause issues

---

## Common Issues and Solutions

### Issue 1: "MoveAction is NULL" in Logs

**Problem:** Input Action not assigned to PlayerController Blueprint

**Solution:**
1. Open `BP_ActionRPGPlayerController`
2. In Details panel, find `Input` category
3. Set `Move Action` to `IA_Move`
4. Compile Blueprint
5. Test again

### Issue 2: "DefaultMappingContext is NULL" in Logs

**Problem:** Input Mapping Context not assigned

**Solution:**
1. Open `BP_ActionRPGPlayerController`
2. Set `Default Mapping Context` to `IM_ActionRPG`
3. Compile Blueprint
4. Test again

### Issue 3: "Pawn is not AActionRPGPlayerCharacter" in Logs

**Problem:** Wrong pawn class or pawn not spawned

**Solution:**
1. Check `BP_ActionRPGGameMode`
2. Verify `Default Pawn Class` = `BP_ActionRPGPlayerCharacter`
3. Check Project Settings → Game Mode
4. Restart editor if needed

### Issue 4: Input Actions Don't Trigger

**Problem:** Input Mapping Context not properly configured

**Solution:**
1. Open `IM_ActionRPG`
2. Verify all key mappings are correct
3. Check that `IA_Move` has W/A/S/D keys mapped
4. Verify key modifiers (if any) are correct

### Issue 5: Character Doesn't Move (but logs show input)

**Problem:** Movement code issue or Character Movement Component problem

**Solution:**
1. Check that Character has CharacterMovementComponent
2. Verify movement mode is correct (Walking, not Flying)
3. Check if character is stuck or colliding
4. Verify movement speed is not zero

### Issue 6: Movement in Wrong Direction

**Problem:** Axis mapping or movement calculation issue

**Solution:**
1. Check Input Mapping Context key mappings
2. Verify axis swizzling (if used)
3. Check movement code uses controller rotation correctly

---

## Testing Checklist

Use this checklist to verify everything is set up:

- [ ] All Input Actions created in `Content/Input/`
- [ ] `IA_Move` is Vector2D type
- [ ] `IM_ActionRPG` has all key mappings
- [ ] `BP_ActionRPGPlayerController` has all Input Actions assigned
- [ ] `BP_ActionRPGPlayerController` has `Default Mapping Context` assigned
- [ ] `BP_ActionRPGPlayerController` is compiled
- [ ] `BP_ActionRPGPlayerCharacter` exists and is compiled
- [ ] `BP_ActionRPGGameMode` has correct default classes
- [ ] `BP_ActionRPGGameMode` is compiled
- [ ] Project Settings → Game Mode → Default GameMode = `BP_ActionRPGGameMode`
- [ ] Level has a Player Start
- [ ] Console logs show input when pressing keys
- [ ] Character moves when pressing W/A/S/D

---

## Debug Commands

### Enable Verbose Logging

In Output Log, set log level to `Very Verbose` to see detailed movement logs.

### Test Input Directly

1. Open `BP_ActionRPGPlayerController`
2. Add a custom event
3. Call `Move` function directly with test values
4. See if character moves

### Check Character Movement Component

1. Select character in Play mode
2. In Details panel, check `Character Movement` component
3. Verify:
   - `Max Walk Speed` > 0
   - `Movement Mode` = `Walking`
   - No constraints preventing movement

---

## Still Not Working?

If movement still doesn't work after checking everything:

1. **Restart Unreal Editor**
   - Sometimes hot-reload doesn't catch all changes

2. **Recompile C++ Code**
   - Close editor
   - Rebuild in Visual Studio
   - Reopen editor

3. **Check for Conflicts**
   - Make sure no other input systems are active
   - Check DefaultInput.ini for conflicts

4. **Create Fresh Test**
   - Create a new simple level
   - Add Player Start
   - Test with minimal setup

---

**Last Updated:** 2025-01-07

