# Phase 3 Manual Steps - Days 31-32: SkillComponent Implementation
**After Code Implementation**  
**Date:** 2025-01-07  
**Phase:** Phase 3 - Skill System  
**Days:** 31-32 - SkillComponent Implementation

---

## Overview

This document provides detailed manual steps required in the Unreal Engine 5.7 Editor for Days 31-32 of Phase 3 (SkillComponent Implementation). These steps must be performed after completing the C++ code implementation.

**What We're Setting Up:**
- SkillComponent Blueprint class
- Component attachment to PlayerCharacter
- Component configuration and testing
- Basic skill management verification

---

## Prerequisites

- ✅ All C++ code compiled successfully
- ✅ Unreal Editor is open
- ✅ Project is loaded in the editor
- ✅ No compilation errors in Output Log
- ✅ Visual Studio build completed successfully
- ✅ Days 29-30 complete (SkillDataAsset, SkillBase, SkillDatabase)
- ✅ SkillComponent C++ class implemented

---

## Step 1: Verify C++ Code Compiled

### 1.1 Check Output Log

1. **Open Output Log**
   - Window → `Developer Tools` → `Output Log`
   - Or press `Ctrl + Shift + L`

2. **Check for Errors**
   - Look for any compilation errors related to:
     - `SkillComponent`
     - `FSkillInstance`
     - Any delegate or event declarations
   - If errors exist, fix them in Visual Studio first

3. **Verify Hot Reload**
   - Look for message: `LogHotReload: Hot reload completed successfully`
   - If hot reload failed, close and reopen Unreal Editor

### 1.2 Verify SkillComponent Class Is Available

1. **Check Content Browser**
   - In Content Browser, click `View Options` (eye icon)
   - Enable `Show C++ Classes` (if not already enabled)

2. **Search for SkillComponent**
   - In Content Browser search bar, type: `SkillComponent`
   - You should see:
     - `Skill Component` (C++ class)

3. **Verify Class Exists**
   - If class doesn't appear, check:
     - C++ code compiled successfully
     - Class is marked with `UCLASS()` macro
     - Class inherits from `UActorComponent`
     - Project was rebuilt (not just hot reloaded)

---

## Step 2: Create Folder Structure

### 2.1 Create Components Folder Structure

1. **Navigate to Content Browser**
   - Go to `Content/` folder root

2. **Verify/Create Components Folder**
   - Navigate to `Content/Blueprints/Components/`
   - If it doesn't exist, create it:
     - Right-click in Content Browser
     - Select `New Folder`
     - Name it: `Components`
     - Location: `Content/Blueprints/Components/`

3. **Create Skills Subfolder**
   - Right-click on `Components` folder
   - Select `New Folder`
   - Name it: `Skills`
   - Location: `Content/Blueprints/Components/Skills/`

4. **Verify Folder Structure**
   ```
   Content/
   └── Blueprints/
       └── Components/
           └── Skills/
   ```

---

## Step 3: Create Blueprint Class from SkillComponent

### 3.1 Create Blueprint Class

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Components/Skills/` folder

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - In the "Pick Parent Class" dialog:
     - Search for: `SkillComponent` or `Skill Component`
     - Select `Skill Component`
     - Click `Select`

3. **Name the Blueprint**
   - Name it: `BP_SkillComponent`
   - Save it in `Content/Blueprints/Components/Skills/`

4. **Open and Verify Blueprint**
   - Double-click `BP_SkillComponent`
   - Blueprint Editor should open
   - Check `Class Defaults` tab
   - Verify properties are visible (if any exposed)

---

## Step 4: Configure SkillComponent Properties

### 4.1 Check Component Properties

1. **Open Blueprint Editor**
   - Ensure `BP_SkillComponent` is open in Blueprint Editor

2. **Check Class Defaults**
   - Click `Class Defaults` tab
   - In Details panel, check for exposed properties:
     - `Should Tick For Cooldowns` (bool) - should default to true
     - `Experience Multiplier` (float) - should default to 1.0
   - Note: Some properties may not be exposed until C++ code is fully implemented

3. **Check Component Tick Settings**
   - Verify `Can Ever Tick` is enabled (for cooldown updates)
   - Check `Tick Interval` (default should be 0.0 for every frame)

4. **Compile Blueprint**
   - Click `Compile` button (top toolbar)
   - Check for compilation errors in Output Log
   - If compilation succeeds, click `Save` button

---

## Step 5: Attach Component to Player Character

### 5.1 Open Player Character Blueprint

1. **Navigate to Player Character**
   - Go to `Content/Blueprints/Characters/`
   - Double-click `BP_ActionRPGPlayerCharacter`
   - Blueprint Editor opens

2. **Verify Current Components**
   - Check Components panel (left side)
   - Verify `InventoryComponent` exists (from Phase 2)
   - Note other existing components

### 5.2 Add SkillComponent

1. **Add Component**
   - In Components panel, click `+ Add` button
   - Search for: `Skill` or `Skill Component` or `BP_SkillComponent`
   - Select: `Skill Component` (or `BP Skill Component` if Blueprint version)
   - Component is added to Components list

2. **Rename Component (Optional)**
   - Select `SkillComponent` in Components list
   - Press `F2` or right-click → `Rename`
   - Rename to: `SkillComponent` (if needed)

3. **Configure Component Properties**
   - Select `SkillComponent` in Components list
   - In Details panel, check available properties:
     - `Should Tick For Cooldowns`: Leave as `true` (default)
     - `Experience Multiplier`: Leave as `1.0` (default)
     - Other properties may not be visible yet (runtime only)

4. **Verify Component Settings**
   - Check `Component Tick` section:
     - `Can Ever Tick`: Should be `true`
     - `Start With Tick Enabled`: Should be `true`
     - `Tick Interval`: `0.0` (every frame)

### 5.3 Compile and Save

1. **Compile Blueprint**
   - Click `Compile` button
   - Check Output Log for any errors or warnings
   - Verify compilation succeeds

2. **Save Blueprint**
   - Click `Save` button
   - Close Blueprint Editor (or keep open for testing)

---

## Step 6: Test Component Attachment

### 6.1 Test in Editor

1. **Open Test Level**
   - Open your test level (or create a new one)
   - Place a `Player Start` actor if needed

2. **Play in Editor (PIE)**
   - Click `Play` button (top toolbar)
   - Game starts in Play mode

3. **Check Output Log**
   - Open Output Log (Window → `Developer Tools` → `Output Log`)
   - Look for messages:
     - `SkillComponent: BeginPlay called` (if logging added)
     - Any initialization messages
     - Any error messages related to SkillComponent

4. **Verify Component Initialization**
   - Component should initialize automatically when player spawns
   - Check for any warnings or errors in Output Log

### 6.2 Verify Component Integration

1. **Check Component in Blueprint**
   - Open `BP_ActionRPGPlayerCharacter` in Blueprint Editor
   - In Event Graph, search for: `Get Skill Component`
   - Should find the node (component is properly integrated)

2. **Verify Component Methods Available**
   - In Event Graph, right-click
   - Type: `Skill Component` (or component name)
   - Check available functions:
     - `Activate Skill`
     - `Can Activate Skill`
     - `Grant Skill`
     - `Remove Skill`
     - `Get Skill Level`
     - `Add Skill Experience`
     - Other methods as defined in C++

---

## Step 7: Basic Component Testing (If Skills Available)

### 7.1 Test Skill Granting (If SkillDatabase Has Skills)

1. **Create Test Blueprint Script**
   - Open `BP_ActionRPGPlayerCharacter`
   - In Event Graph, add `BeginPlay` event
   - Add `Get Skill Component` node
   - Add `Get Skill Database` node (static function)

2. **Test Granting a Skill**
   - Add `Get Skill Data Asset` node (from SkillDatabase)
   - Use a test SkillID (e.g., `SKILL_MELEE_ATTACK` if available)
   - Add `Create Skill` node (from SkillDatabase)
   - Add `Grant Skill` node (from SkillComponent)
   - Connect: SkillDatabase → Create Skill → Grant Skill
   - Add `Print String` node to show success message

3. **Compile and Test**
   - Click `Compile` button
   - Save Blueprint
   - Play in Editor
   - Check Output Log for:
     - Skill creation messages
     - Skill grant messages
     - Any errors

### 7.2 Test Skill Cooldowns

1. **Add Cooldown Test**
   - In Event Graph, add a timer or delay node
   - Add `Get Skill Component` node
   - Add `Get Skill Instance` node (if available)
   - Add `Print String` to show cooldown remaining

2. **Test Tick Updates**
   - Verify TickComponent is updating cooldowns
   - Check Output Log for cooldown update messages (if logging added)

---

## Step 8: Verify Event Bindings (Optional)

### 8.1 Check Event Delegates

1. **Open SkillComponent Blueprint**
   - Open `BP_SkillComponent` in Blueprint Editor

2. **Check Event Dispatchers**
   - In Event Graph, right-click
   - Search for: `On Skill Activated` or `On Skill Cooldown Changed`
   - Should see event dispatchers if properly exposed

3. **Test Event Binding**
   - Create a simple event listener
   - Bind to `On Skill Activated` event
   - Add `Print String` to show when skill is activated
   - Test in game

---

## Verification Checklist

### Code Compilation
- [ ] All C++ code compiled without errors
- [ ] SkillComponent class visible in Content Browser
- [ ] Hot reload completed successfully
- [ ] No errors in Output Log

### Folder Structure
- [ ] `Content/Blueprints/Components/Skills/` folder created

### Blueprint Creation
- [ ] `BP_SkillComponent` Blueprint class created successfully
- [ ] Blueprint compiles without errors
- [ ] Component properties visible in Class Defaults

### Component Attachment
- [ ] SkillComponent added to `BP_ActionRPGPlayerCharacter`
- [ ] Component appears in Components panel
- [ ] Component properties configurable in Details panel
- [ ] Component Tick settings correct (Can Ever Tick = true)

### Component Integration
- [ ] Component initializes on BeginPlay
- [ ] `Get Skill Component` node available in Blueprint
- [ ] Component methods accessible in Blueprint Event Graph
- [ ] No errors in Output Log during Play mode

### Basic Functionality
- [ ] Component can grant skills (if test skills available)
- [ ] Component can remove skills
- [ ] Component tracks skill cooldowns (Tick updates)
- [ ] Component tracks skill levels and experience
- [ ] Events fire correctly (if tested)

---

## Common Issues & Solutions

### Issue: SkillComponent Not Visible in Content Browser

**Symptoms:** Cannot find "Skill Component" when creating Blueprint

**Solutions:**
1. Verify C++ code compiled successfully
2. Check that `SkillComponent` class has `UCLASS()` macro with `BlueprintType` and `Blueprintable`
3. Ensure class inherits from `UActorComponent`
4. Close and reopen Unreal Editor
5. Rebuild project in Visual Studio (not just compile)

### Issue: Cannot Add Component to Player Character

**Symptoms:** SkillComponent doesn't appear in "Add Component" menu

**Solutions:**
1. Verify component class is properly marked as `BlueprintSpawnableComponent` in UCLASS macro
2. Check if component needs to be added via C++ constructor instead
3. Verify `BP_ActionRPGPlayerCharacter` is the correct Blueprint (not base C++ class)
4. Try restarting Unreal Editor
5. Check Output Log for any warnings about component registration

### Issue: Component Methods Not Visible in Blueprint

**Symptoms:** Cannot find SkillComponent methods in Event Graph

**Solutions:**
1. Verify methods are marked with `UFUNCTION(BlueprintCallable)`
2. Check method parameters are Blueprint-compatible (no complex C++ types)
3. Compile C++ code after adding BlueprintCallable specifiers
4. Close and reopen Blueprint Editor
5. Verify component reference is valid (Get Skill Component node)

### Issue: Component Doesn't Tick

**Symptoms:** Cooldowns don't update, TickComponent not called

**Solutions:**
1. Verify `PrimaryComponentTick.bCanEverTick = true` in constructor
2. Check component's `Can Ever Tick` property in Details panel
3. Verify `Start With Tick Enabled` is true
4. Check if component is enabled (`SetComponentTickEnabled(true)`)
5. Verify TickGroup is appropriate (default should work)

### Issue: Component Not Initializing

**Symptoms:** BeginPlay not called, component doesn't initialize

**Solutions:**
1. Verify `BeginPlay()` is overridden in C++ code
2. Check Output Log for initialization errors
3. Verify component is attached before BeginPlay is called
4. Check if owner actor is valid
5. Verify component is not disabled

### Issue: Skills Cannot Be Granted

**Symptoms:** GrantSkill fails or doesn't work

**Solutions:**
1. Verify skill instance is created correctly (via SkillDatabase::CreateSkill)
2. Check if skill already exists (component may prevent duplicates)
3. Verify ActiveSkills array is initialized
4. Check Output Log for specific error messages
5. Verify SkillDataAsset is valid and has SkillID set

---

## Next Steps

After completing Days 31-32 manual steps:

1. **Proceed to Days 33-34:** SkillManagerComponent Implementation
   - Create Blueprint class from SkillManagerComponent
   - Attach to PlayerCharacter
   - Configure skill bar management
   - Test skill unlocking and slot assignment

2. **Prepare for Skill Implementations (Days 35-37):**
   - Review skill implementation requirements
   - Prepare animation assets (for melee/ranged skills)
   - Prepare particle effects (for magic skills)
   - Prepare projectile actors (for ranged/magic skills)

3. **Documentation:**
   - Document any custom properties added to SkillComponent
   - Note any deviations from the implementation plan
   - Update skill component usage guidelines

---

**End of Days 31-32 Manual Steps**
