# Phase 1 Execution Plan - Days 11-12
**Character Classes & Integration**  
**Date:** 2025-01-07  
**Status:** Ready for Implementation

---

## Overview

Days 11-12 focus on finalizing the Character Classes, creating Blueprint classes, and ensuring full integration with the input system. Most of the C++ code is already complete from earlier days, so this phase focuses on Blueprint configuration, testing, and refinement.

---

## Prerequisites

### Completed Work
- ✅ Days 1-4: Core Gameplay Framework Classes
- ✅ Days 5-6: Enhanced Input System Setup
- ✅ Days 7-8: Item System Foundation
- ✅ Days 9-10: Skill System Foundation
- ✅ ActionRPGCharacter (Base) - C++ class created
- ✅ ActionRPGPlayerCharacter - C++ class created with:
  - Movement system
  - Camera system (SpringArm + Camera)
  - Mouse look/rotation
  - Input integration

### Required Assets
- Character mesh (optional - can use default)
- Character animations (optional - can use default)
- Any custom materials/textures (optional)

---

## Day 11: Character Blueprint Setup & Configuration

### Task 1: Verify/Create Blueprint Character Classes

#### 1.1 Verify Base Character Blueprint
- **Check if `BP_ActionRPGCharacter` exists**
  - Location: `Content/Blueprints/Characters/`
  - If missing, create from `ActionRPGCharacter` C++ class
  - Purpose: Base character for NPCs/enemies (future use)

#### 1.2 Verify/Create Player Character Blueprint
- **Check if `BP_ActionRPGPlayerCharacter` exists**
  - Location: `Content/Blueprints/Characters/`
  - If missing, create from `ActionRPGPlayerCharacter` C++ class
  - Purpose: Player-controlled character

### Task 2: Configure Player Character Blueprint

#### 2.1 Open Player Character Blueprint
- Open `BP_ActionRPGPlayerCharacter` in Blueprint Editor

#### 2.2 Configure Mesh Component
- **Add/Configure Skeletal Mesh**
  - If you have a character mesh:
    - Select `Mesh` component in Components panel
    - Set `Skeletal Mesh` property to your character mesh
    - Adjust `Mesh` transform (Location, Rotation, Scale) if needed
  - If no mesh available:
    - Can use default or placeholder
    - Mesh will be added in Phase 2

#### 2.3 Configure Camera Components
- **Verify SpringArm Component**
  - Should be visible in Components panel (created in C++)
  - Properties to verify:
    - `Target Arm Length`: 500.0 (can adjust)
    - `Socket Offset`: (0, 0, 500) (can adjust)
    - `Relative Rotation`: (-65, 0, 0) (can adjust)
  - **Note:** These are set in C++ constructor, but can be overridden in Blueprint

- **Verify Camera Component**
  - Should be attached to SpringArm
  - Properties to verify:
    - `Field of View`: 90.0 (default, can adjust)
    - `Projection Mode`: Perspective (default)

#### 2.4 Configure Character Movement
- **Select Character Movement Component**
  - Properties to verify/adjust:
    - `Max Walk Speed`: 600.0 (default, adjust as needed)
    - `Max Acceleration`: 2048.0 (default)
    - `Braking Deceleration Walking`: 2048.0 (default)
    - `Ground Friction`: 8.0 (default)
    - `Rotation Rate`: (0, 540, 0) - Yaw rotation speed
    - `Orient Rotation to Movement`: **FALSE** (important for top-down)
    - `Use Controller Desired Rotation`: **FALSE** (we handle rotation manually)

#### 2.5 Save Blueprint
- Click `Compile` button
- Click `Save` button
- Close Blueprint Editor

### Task 3: Verify GameMode Configuration

#### 3.1 Check GameMode Blueprint
- Open `BP_ActionRPGGameMode` (from Days 1-4)
- Verify `Default Pawn Class` is set to `BP_ActionRPGPlayerCharacter`
- If not set, set it now
- Save and close

#### 3.2 Verify Project Settings
- Go to `Edit → Project Settings`
- Navigate to `Game → GameMode`
- Verify `Default GameMode` is set to `BP_ActionRPGGameMode`
- If not set, set it now

---

## Day 12: Integration Testing & Refinement

### Task 1: In-Game Testing

#### 1.1 Basic Movement Test
- **Play in Editor (PIE)**
  - Press `Play` button
  - Test WASD movement:
    - W/S should move forward/backward relative to character facing
    - A/D should strafe left/right relative to character facing
  - Verify movement feels responsive
  - Check for any jitter or issues

#### 1.2 Mouse Look Test
- **Test Mouse Rotation**
  - Move mouse cursor around screen
  - Character should rotate to face mouse cursor position
  - Verify rotation is smooth
  - Check that character rotates on horizontal plane only (no pitch/roll)

#### 1.3 Camera Test
- **Verify Camera View**
  - Camera should be positioned above character
  - View should be top-down (angled down)
  - Camera should follow character movement
  - Verify no camera jitter or clipping

#### 1.4 Input System Test
- **Test All Input Actions**
  - Movement (WASD) - ✅ Should work
  - Mouse Look - ✅ Should work
  - Interact (E) - Test with debug log
  - Attack (Left Click) - Test with debug log
  - Dodge (Space) - Test with debug log
  - Open Inventory (Tab) - Test with debug log
  - Skill Slots (1-8) - Test with debug log

### Task 2: Performance & Polish

#### 2.1 Movement Tuning
- **Adjust Movement Speed** (if needed)
  - Open `BP_ActionRPGPlayerCharacter`
  - Adjust `Max Walk Speed` in Character Movement Component
  - Test and iterate until movement feels good

- **Adjust Rotation Speed** (if needed)
  - Can add rotation interpolation in C++ if needed
  - Current implementation rotates instantly (may want smoothing)

#### 2.2 Camera Tuning
- **Adjust Camera Distance** (if needed)
  - Open `BP_ActionRPGPlayerCharacter`
  - Adjust `Target Arm Length` in SpringArm Component
  - Or modify in C++ constructor

- **Adjust Camera Angle** (if needed)
  - Adjust `Relative Rotation` in SpringArm Component
  - Or modify in C++ constructor

#### 2.3 Debug Logging
- **Review Console Output**
  - Check for any errors or warnings
  - Verify input actions are triggering correctly
  - Remove any excessive debug logs (keep important ones)

### Task 3: Code Review & Cleanup

#### 3.1 Review Character Code
- **ActionRPGCharacter.cpp**
  - Verify constructor and BeginPlay are minimal
  - Add comments if needed

- **ActionRPGPlayerCharacter.cpp**
  - Review movement logic
  - Review camera setup
  - Review mouse rotation logic
  - Add comments for complex logic
  - Verify all debug logs are appropriate

#### 3.2 Code Standards Check
- ✅ Copyright headers
- ✅ Proper includes
- ✅ Forward declarations
- ✅ Blueprint exposure (UPROPERTY, UFUNCTION)
- ✅ Naming conventions
- ✅ Code comments

### Task 4: Documentation

#### 4.1 Update Progress Document
- Document any issues encountered
- Document solutions found
- Document any deviations from plan
- Document final configuration values

#### 4.2 Create Troubleshooting Guide
- Document common issues and solutions
- Add to main troubleshooting document if needed

---

## Deliverables

### Must Have
- ✅ `BP_ActionRPGCharacter` Blueprint created
- ✅ `BP_ActionRPGPlayerCharacter` Blueprint created and configured
- ✅ Character movement working correctly
- ✅ Mouse look/rotation working correctly
- ✅ Camera setup working correctly
- ✅ All input actions responding
- ✅ GameMode configured correctly
- ✅ No compilation errors
- ✅ No runtime errors

### Nice to Have
- Character mesh assigned (can defer to Phase 2)
- Character animations (can defer to Phase 2)
- Movement feel tuned to perfection
- Camera feel tuned to perfection

---

## Success Criteria

### Day 11 Complete When:
- ✅ Blueprint character classes exist
- ✅ Player character Blueprint is configured
- ✅ GameMode is configured
- ✅ Project compiles without errors

### Day 12 Complete When:
- ✅ Character spawns correctly in-game
- ✅ Movement works with WASD
- ✅ Mouse rotation works
- ✅ Camera follows character correctly
- ✅ All input actions trigger (verified with logs)
- ✅ No errors or warnings in console
- ✅ Movement and camera feel good
- ✅ Ready for Phase 2

---

## Common Issues & Solutions

### Issue: Character Not Spawning
**Symptoms:** Game starts but no character appears  
**Solutions:**
- Verify GameMode Default Pawn Class is set
- Check Project Settings → Game → GameMode
- Verify Blueprint compiles without errors
- Check for spawn point in level

### Issue: Character Not Moving
**Symptoms:** Input received but character doesn't move  
**Solutions:**
- Verify Character Movement Component exists
- Check `Orient Rotation to Movement` is FALSE
- Verify movement input is being passed to character
- Check Max Walk Speed is > 0

### Issue: Character Not Rotating to Mouse
**Symptoms:** Mouse moves but character doesn't rotate  
**Solutions:**
- Verify `RotateToMouseCursor()` is called in Tick
- Check PlayerController is valid
- Verify `DeprojectMousePositionToWorld` is working
- Check for any rotation constraints

### Issue: Camera Not Following Character
**Symptoms:** Camera doesn't move with character  
**Solutions:**
- Verify SpringArm is attached to RootComponent
- Check Camera is attached to SpringArm
- Verify SpringArm properties are set correctly
- Check for any camera constraints

### Issue: Movement Feels Wrong
**Symptoms:** Movement is too fast/slow or unresponsive  
**Solutions:**
- Adjust Max Walk Speed
- Adjust Max Acceleration
- Adjust Braking Deceleration
- Check input vector is normalized correctly

---

## Next Steps (Phase 2 Preview)

After Days 11-12 completion, Phase 2 will focus on:
- Inventory Component implementation
- Item Pickup Actor
- Inventory UI system
- Drag and drop functionality

**Preparation for Phase 2:**
- Review Inventory System Design
- Prepare UI assets
- Plan inventory UI layout

---

## Notes

- Most C++ code is already complete from earlier days
- Focus is on Blueprint configuration and testing
- Character mesh and animations can be added later
- Movement and camera tuning is iterative - don't spend too long on perfection
- Save frequently and test often

---

**End of Days 11-12 Execution Plan**
