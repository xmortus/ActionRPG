# Top-Down Camera Setup Guide
**Date:** 2025-01-07

---

## Overview

The top-down camera system has been implemented in C++. This guide explains what was done and any manual configuration needed in Blueprint.

---

## C++ Implementation

### ✅ What's Been Implemented

#### Camera Components
- **SpringArmComponent** - Positions camera above character
- **CameraComponent** - The actual camera that renders the view

#### Camera Settings (Default Values)
- **Target Arm Length:** 1000.0f (camera distance from character)
- **Spring Arm Rotation:** -60 degrees pitch (looking down at angle)
- **Spring Arm Position:** 500 units above character
- **Camera follows character** automatically via SpringArm

#### Character Rotation
- Character rotates to face mouse cursor position
- Rotation updates every frame in Tick()
- Uses mouse-to-world projection to find cursor position on ground plane

#### Mouse Cursor
- Mouse cursor is visible (crosshairs)
- Click events enabled
- Mouse over events enabled

---

## Blueprint Configuration (Optional)

### Configure Camera in BP_ActionRPGPlayerCharacter

The camera components are created in C++, but you can adjust settings in Blueprint:

1. **Open** `Content/Blueprints/Characters/BP_ActionRPGPlayerCharacter`

2. **In the Components Panel**, you should see:
   - `SpringArm` (SpringArmComponent)
   - `Camera` (CameraComponent)

3. **Select SpringArm Component**
   - In Details panel, you can adjust:
     - **Target Arm Length** - Distance from character (default: 1000)
     - **Socket Offset** - Position offset
     - **Target Offset** - Where camera looks relative to character
     - **Camera Lag** - Smooth camera following (optional)
     - **Camera Rotation Lag** - Smooth rotation (optional)

4. **Select Camera Component**
   - In Details panel, you can adjust:
     - **Field of View** - Camera FOV (default: 90)
     - **Aspect Ratio** - Camera aspect ratio
     - **Post Process Settings** - Visual effects

5. **Compile and Save** the Blueprint

---

## Camera Settings Reference

### Recommended Settings for Top-Down Action RPG

#### SpringArm Settings
- **Target Arm Length:** 800-1200 (adjust for desired zoom level)
- **Socket Offset:** (0, 0, 500) - Position above character
- **Target Offset:** (0, 0, 0) - Look at character center
- **Camera Lag Speed:** 5.0 (optional, for smooth following)
- **Camera Rotation Lag Speed:** 0.0 (no rotation lag for top-down)

#### Camera Settings
- **Field of View:** 75-90 degrees
- **Aspect Ratio:** Auto (uses viewport)
- **Projection Mode:** Perspective

#### Spring Arm Rotation
- **Pitch:** -60 to -75 degrees (steeper = more top-down)
- **Yaw:** 0 (no rotation)
- **Roll:** 0 (no roll)

---

## How It Works

### Camera Positioning
1. **SpringArm** is attached to character's root
2. **SpringArm** extends 1000 units away from character
3. **SpringArm** is rotated -60 degrees (looking down)
4. **SpringArm** is positioned 500 units above character
5. **Camera** is attached to SpringArm end
6. Camera automatically follows character movement

### Character Rotation
1. **Mouse cursor position** is projected to world space
2. **Intersection point** with ground plane is calculated
3. **Direction** from character to intersection is calculated
4. **Character rotates** to face that direction
5. Updates every frame for smooth rotation

### Mouse Input
- Mouse cursor is visible (crosshairs)
- Mouse position is tracked
- Character faces wherever mouse cursor points

---

## Testing

### Verify Camera Setup

1. **Play in Editor**
   - Press `Play` or `Alt + P`

2. **Check Camera View**
   - Camera should be above character
   - Camera should look down at character
   - Camera should follow character when moving

3. **Test Mouse Look**
   - Move mouse cursor around
   - Character should rotate to face cursor
   - Rotation should be smooth

4. **Test Movement**
   - Press W/A/S/D to move
   - Camera should follow character
   - Character should face mouse cursor while moving

---

## Troubleshooting

### Issue: Camera Not Visible / Wrong View

**Solutions:**
- Check that SpringArm and Camera components exist in Blueprint
- Verify camera is set as view target in BeginPlay
- Check SpringArm rotation and position
- Verify Target Arm Length is not zero

### Issue: Character Doesn't Rotate to Mouse

**Checklist:**
- ✅ Mouse cursor is visible
- ✅ Mouse click events are enabled
- ✅ PlayerController has mouse cursor enabled
- ✅ Look function is being called (check logs)
- ✅ Character has valid controller

### Issue: Camera Too Close/Far

**Solution:**
- Adjust `Target Arm Length` in SpringArm component
- Default is 1000, try 800-1200 range
- Can be adjusted in Blueprint

### Issue: Camera Angle Wrong

**Solution:**
- Adjust SpringArm rotation pitch
- Default is -60 degrees
- More negative = steeper angle (more top-down)
- Less negative = shallower angle

### Issue: Camera Doesn't Follow Character

**Solution:**
- Verify SpringArm is attached to RootComponent
- Check that character is moving (not stuck)
- Verify camera is set as view target in BeginPlay

---

## Advanced Configuration

### Smooth Camera Following

To add smooth camera following:

1. **In Blueprint**, select SpringArm component
2. **Enable Camera Lag:**
   - `Enable Camera Lag` = true
   - `Camera Lag Speed` = 5.0 (adjust for smoothness)
3. **Enable Camera Rotation Lag** (optional):
   - `Enable Camera Rotation Lag` = true
   - `Camera Rotation Lag Speed` = 2.0

### Camera Zoom

To add camera zoom (scroll wheel):

1. **Add Input Action** for zoom
2. **Bind to scroll wheel** in Input Mapping Context
3. **Modify Target Arm Length** based on input
4. **Clamp values** (e.g., 500-2000)

### Camera Shake

To add camera shake for impacts:

1. **Create Camera Shake Blueprint**
2. **Call in C++** when needed:
   ```cpp
   if (CameraComponent)
   {
       GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(ShakeClass);
   }
   ```

---

## Code Reference

### Key Files
- `Source/ActionRPG/Public/Characters/ActionRPGPlayerCharacter.h` - Camera component declarations
- `Source/ActionRPG/Private/Characters/ActionRPGPlayerCharacter.cpp` - Camera setup and rotation logic
- `Source/ActionRPG/Private/Core/ActionRPGPlayerController.cpp` - Mouse cursor setup

### Key Functions
- `RotateToMouseCursor()` - Rotates character to face mouse cursor
- `BeginPlay()` - Sets camera as view target
- Constructor - Creates and configures camera components

---

## Default Values Summary

| Setting | Value | Description |
|---------|-------|-------------|
| Target Arm Length | 1000.0 | Camera distance from character |
| Spring Arm Pitch | -60.0° | Camera angle (looking down) |
| Spring Arm Z Offset | 500.0 | Height above character |
| Mouse Cursor | Crosshairs | Cursor type |
| Camera FOV | 90.0° | Field of view |

---

**Status:** ✅ Camera system implemented - Ready for testing

