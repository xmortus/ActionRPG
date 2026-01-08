# Phase 1 Manual Steps - Days 11-12
**Character Classes & Integration**  
**Date:** 2025-01-07

---

## Overview

This guide covers the manual steps required in the Unreal Editor to complete Days 11-12 of Phase 1. Most of the C++ code is already complete, so these steps focus on Blueprint configuration, testing, and refinement.

---

## Prerequisites

- ✅ All C++ code compiled successfully
- ✅ Days 1-4 manual steps completed (Blueprint classes created)
- ✅ Days 5-6 manual steps completed (Input Actions created)
- ✅ Unreal Editor is open
- ✅ Project is loaded in the editor

---

## Step 1: Verify/Create Blueprint Character Classes

### 1.1 Check for Existing Blueprint Classes

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Characters/` folder

2. **Check for Blueprint Classes**
   - Look for:
     - `BP_ActionRPGCharacter` (base character)
     - `BP_ActionRPGPlayerCharacter` (player character)

3. **If Blueprints Don't Exist:**
   - Follow Steps 1.2 and 1.3 below
   - If they exist, skip to Step 2

### 1.2 Create Base Character Blueprint (if needed)

1. **Right-click in Content Browser**
   - Location: `Content/Blueprints/Characters/`

2. **Select "Blueprint Class"**

3. **In "Pick Parent Class" Dialog**
   - Search for: `ActionRPGCharacter`
   - Select: `ActionRPGCharacter` (the C++ class)
   - Click: `Select`

4. **Name the Blueprint**
   - Name: `BP_ActionRPGCharacter`
   - Press `Enter` to confirm

5. **Save the Blueprint**
   - Click `Compile` button (if available)
   - Click `Save` button
   - Close Blueprint Editor (no configuration needed yet)

### 1.3 Create Player Character Blueprint (if needed)

1. **Right-click in Content Browser**
   - Location: `Content/Blueprints/Characters/`

2. **Select "Blueprint Class"**

3. **In "Pick Parent Class" Dialog**
   - Search for: `ActionRPGPlayerCharacter`
   - Select: `ActionRPGPlayerCharacter` (the C++ class)
   - Click: `Select`

4. **Name the Blueprint**
   - Name: `BP_ActionRPGPlayerCharacter`
   - Press `Enter` to confirm

5. **Don't close yet - proceed to Step 2**

---

## Step 2: Configure Player Character Blueprint

### 2.1 Open Player Character Blueprint

1. **Double-click `BP_ActionRPGPlayerCharacter`**
   - Opens in Blueprint Editor

2. **Verify Components Panel**
   - You should see:
     - `DefaultSceneRoot` (root component)
     - `Mesh` (Skeletal Mesh Component)
     - `CharacterMovement` (Character Movement Component)
     - `CapsuleComponent` (Collision Capsule)
     - `SpringArm` (Spring Arm Component - from C++)
     - `Camera` (Camera Component - from C++)

### 2.2 Configure Mesh Component (Optional)

**Note:** If you don't have a character mesh yet, you can skip this step and add it later.

1. **Select `Mesh` Component**
   - Click on `Mesh` in the Components panel

2. **In Details Panel:**
   - **Skeletal Mesh:**
     - If you have a character mesh:
       - Click dropdown → Select your mesh
     - If no mesh:
       - Leave as `None` (will use default or add later)

3. **Transform Settings (if mesh is assigned):**
   - **Location:** Usually (0, 0, -90) to align with capsule
   - **Rotation:** Usually (0, 0, 0)
   - **Scale:** Usually (1, 1, 1)
   - Adjust as needed for your mesh

### 2.3 Verify Camera Components

1. **Select `SpringArm` Component**
   - Click on `SpringArm` in Components panel

2. **In Details Panel, verify these properties:**
   - **Target Arm Length:** `500.0` (can adjust if needed)
   - **Socket Offset:**
     - X: `0.0`
     - Y: `0.0`
     - Z: `500.0` (height above character)
   - **Relative Rotation:**
     - Pitch: `-65.0` (looking down)
     - Yaw: `0.0`
     - Roll: `0.0`
   - **Use Pawn Control Rotation:** `False` (unchecked)
   - **Inherit Pitch:** `False` (unchecked)
   - **Inherit Yaw:** `False` (unchecked)
   - **Inherit Roll:** `False` (unchecked)

3. **Select `Camera` Component**
   - Click on `Camera` in Components panel

4. **In Details Panel, verify:**
   - **Field of View:** `90.0` (default, can adjust)
   - **Projection Mode:** `Perspective` (default)
   - **Aspect Ratio:** Leave default

**Note:** These values are set in C++ constructor, but you can override them in Blueprint if needed.

### 2.4 Configure Character Movement

1. **Select `CharacterMovement` Component**
   - Click on `CharacterMovement` in Components panel

2. **In Details Panel, verify/adjust these properties:**

   **Movement Settings:**
   - **Max Walk Speed:** `600.0` (default, adjust as needed)
   - **Max Acceleration:** `2048.0` (default)
   - **Braking Deceleration Walking:** `2048.0` (default)
   - **Ground Friction:** `8.0` (default)

   **Rotation Settings:**
   - **Rotation Rate:**
     - X: `0.0`
     - Y: `540.0` (degrees per second)
     - Z: `0.0`
   - **Orient Rotation to Movement:** `False` (unchecked) ⚠️ **IMPORTANT**
   - **Use Controller Desired Rotation:** `False` (unchecked) ⚠️ **IMPORTANT**

   **Why these settings:**
   - `Orient Rotation to Movement` must be FALSE because we handle rotation manually via mouse cursor
   - `Use Controller Desired Rotation` must be FALSE for the same reason

3. **Other Settings (leave defaults unless needed):**
   - **Can Crouch:** `False` (unless you want crouching)
   - **Can Jump:** `False` (unless you want jumping)
   - **Can Swim:** `True` (default, can disable if no water)

### 2.5 Save Player Character Blueprint

1. **Click `Compile` Button**
   - Top toolbar, green checkmark icon
   - Wait for compilation to finish
   - Check for any errors (should be none)

2. **Click `Save` Button**
   - Top toolbar, disk icon
   - Or press `Ctrl+S`

3. **Close Blueprint Editor**
   - Click `X` or press `Esc`

---

## Step 3: Verify GameMode Configuration

### 3.1 Open GameMode Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder

2. **Open `BP_ActionRPGGameMode`**
   - Double-click to open

### 3.2 Verify Default Pawn Class

1. **In Details Panel (Class Defaults):**
   - Look for `Default Pawn Class` property
   - Should be set to: `BP_ActionRPGPlayerCharacter`
   - If not set:
     - Click dropdown
     - Search for `BP_ActionRPGPlayerCharacter`
     - Select it

2. **Verify Other Default Classes:**
   - **Player Controller Class:** `BP_ActionRPGPlayerController` (or C++ class)
   - **Game State Class:** `BP_ActionRPGGameState` (or C++ class)
   - **HUD Class:** `BP_ActionRPGHUD` (or C++ class)

3. **Save and Close**
   - Click `Compile` (if needed)
   - Click `Save`
   - Close Blueprint Editor

### 3.3 Verify Project Settings

1. **Open Project Settings**
   - Go to `Edit → Project Settings`
   - Or press `Ctrl+Alt+,`

2. **Navigate to Game Mode**
   - Left panel: `Game → Game Mode`

3. **Verify Default GameMode**
   - **Default GameMode:** Should be `BP_ActionRPGGameMode`
   - If not set:
     - Click dropdown
     - Search for `BP_ActionRPGGameMode`
     - Select it

4. **Close Project Settings**
   - Click `X` or press `Esc`

---

## Step 4: Test in Game

### 4.1 Prepare Level

1. **Open Your Level**
   - Open the level you want to test in (e.g., `Lvl_TopDown`)

2. **Verify Player Start**
   - Look for a `Player Start` actor in the level
   - If missing:
     - Press `P` to show player start visualization
     - Or add one: `Place Actors → Game → Player Start`
     - Position it where you want the player to spawn

### 4.2 Play in Editor (PIE)

1. **Start Play**
   - Click `Play` button (top toolbar)
   - Or press `Alt+P`

2. **Test Movement**
   - **W/S Keys:** Should move forward/backward relative to character facing
   - **A/D Keys:** Should strafe left/right relative to character facing
   - Verify movement feels responsive

3. **Test Mouse Look**
   - **Move Mouse:** Character should rotate to face mouse cursor
   - Verify rotation is smooth
   - Verify character only rotates on horizontal plane (no pitch/roll)

4. **Test Camera**
   - **Camera should:** Be positioned above character
   - **View should:** Be top-down (angled down)
   - **Camera should:** Follow character as you move
   - Verify no camera jitter or clipping

5. **Test Input Actions**
   - **E Key:** Should trigger Interact (check console for log)
   - **Left Click:** Should trigger Attack (check console for log)
   - **Space:** Should trigger Dodge (check console for log)
   - **Tab:** Should trigger Open Inventory (check console for log)
   - **1-8 Keys:** Should trigger Skill Slots (check console for logs)

6. **Stop Play**
   - Press `Esc` or click `Stop` button

### 4.3 Check Console for Errors

1. **Open Output Log**
   - Go to `Window → Developer Tools → Output Log`
   - Or press `Ctrl+Shift+L`

2. **Review Logs**
   - Look for any errors (red text)
   - Look for any warnings (yellow text)
   - Verify input actions are logging correctly (if you added logs)

---

## Step 5: Tune Movement and Camera (Optional)

### 5.1 Adjust Movement Speed

If movement feels too fast or too slow:

1. **Open `BP_ActionRPGPlayerCharacter`**

2. **Select `CharacterMovement` Component**

3. **Adjust `Max Walk Speed`**
   - Try values between `300.0` (slow) and `900.0` (fast)
   - Test in-game and iterate

4. **Save and Test**

### 5.2 Adjust Camera Distance

If camera is too close or too far:

1. **Open `BP_ActionRPGPlayerCharacter`**

2. **Select `SpringArm` Component**

3. **Adjust `Target Arm Length`**
   - Increase for farther camera (e.g., `700.0`)
   - Decrease for closer camera (e.g., `300.0`)
   - Test in-game and iterate

4. **Save and Test**

### 5.3 Adjust Camera Angle

If camera angle needs adjustment:

1. **Open `BP_ActionRPGPlayerCharacter`**

2. **Select `SpringArm` Component**

3. **Adjust `Relative Rotation` Pitch**
   - More negative = steeper angle (e.g., `-75.0`)
   - Less negative = shallower angle (e.g., `-45.0`)
   - Test in-game and iterate

4. **Save and Test**

---

## Step 6: Troubleshooting

### Issue: Character Not Spawning

**Symptoms:** Game starts but no character appears

**Solutions:**
1. Verify `BP_ActionRPGGameMode` has `Default Pawn Class` set to `BP_ActionRPGPlayerCharacter`
2. Check Project Settings → Game → GameMode → Default GameMode is set
3. Verify `BP_ActionRPGPlayerCharacter` compiles without errors
4. Check level has a `Player Start` actor
5. Try restarting the editor

### Issue: Character Not Moving

**Symptoms:** Input received but character doesn't move

**Solutions:**
1. Verify `CharacterMovement` component exists
2. Check `Orient Rotation to Movement` is **FALSE**
3. Check `Use Controller Desired Rotation` is **FALSE**
4. Verify `Max Walk Speed` is > 0
5. Check console for errors
6. Verify input actions are bound in PlayerController

### Issue: Character Not Rotating to Mouse

**Symptoms:** Mouse moves but character doesn't rotate

**Solutions:**
1. Verify `RotateToMouseCursor()` is being called (check C++ code)
2. Check PlayerController is valid
3. Verify mouse cursor is visible (check PlayerController settings)
4. Check console for errors
5. Try restarting the editor

### Issue: Camera Not Following Character

**Symptoms:** Camera doesn't move with character

**Solutions:**
1. Verify `SpringArm` is attached to `RootComponent`
2. Check `Camera` is attached to `SpringArm`
3. Verify `SpringArm` properties are set correctly
4. Check for any camera constraints in level
5. Try restarting the editor

### Issue: Movement Feels Wrong

**Symptoms:** Movement is too fast/slow or unresponsive

**Solutions:**
1. Adjust `Max Walk Speed` in Character Movement Component
2. Adjust `Max Acceleration` for responsiveness
3. Adjust `Braking Deceleration Walking` for stopping speed
4. Check input vector is being normalized (check C++ code)
5. Test different values and iterate

---

## Step 7: Final Verification

### 7.1 Compilation Check

1. **Compile All Blueprints**
   - In Content Browser, select all Blueprint files
   - Right-click → `Compile`
   - Wait for compilation to finish
   - Check for errors

2. **Compile C++ Code**
   - In Unreal Editor: `Tools → Refresh Visual Studio Project`
   - Or compile in Visual Studio
   - Verify no compilation errors

### 7.2 Final Test

1. **Play in Editor**
   - Test all movement
   - Test all input actions
   - Verify camera works
   - Check console for errors

2. **Document Any Issues**
   - Note any problems encountered
   - Note solutions found
   - Update progress document if needed

---

## Success Checklist

- ✅ `BP_ActionRPGCharacter` exists
- ✅ `BP_ActionRPGPlayerCharacter` exists and is configured
- ✅ Character Movement Component configured correctly
- ✅ Camera components configured correctly
- ✅ GameMode is set to use `BP_ActionRPGPlayerCharacter`
- ✅ Project Settings has correct Default GameMode
- ✅ Character spawns in-game
- ✅ Movement works with WASD
- ✅ Mouse rotation works
- ✅ Camera follows character
- ✅ All input actions trigger (verified with logs)
- ✅ No errors in console
- ✅ Ready for Phase 2

---

## Next Steps

After completing Days 11-12:

1. **Review Progress**
   - Update progress document
   - Note any deviations from plan

2. **Prepare for Phase 2**
   - Review Inventory System Design
   - Prepare UI assets
   - Plan inventory UI layout

---

**End of Days 11-12 Manual Steps**
