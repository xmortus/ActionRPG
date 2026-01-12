# Phase 3: Skill System - Manual Steps for Days 33-34
**SkillManagerComponent Implementation**  
**UE 5.7 Editor Setup Guide**  
**Last Updated:** 2025-01-07

---

## Overview

This guide provides step-by-step instructions for setting up the `SkillManagerComponent` in the Unreal Engine 5.7 editor after the C++ implementation is complete. The `SkillManagerComponent` manages skill unlocks and skill bar slot assignments (8 slots for hotkeys 1-8).

---

## Prerequisites

Before starting, ensure:
- ✅ Phase 3 Days 29-30 complete (`SkillDataAsset`, `SkillBase`, `SkillDatabase`)
- ✅ Phase 3 Days 31-32 complete (`SkillComponent` implemented and attached to player)
- ✅ Project compiles successfully in Visual Studio
- ✅ `SkillComponent` is attached to `BP_ActionRPGPlayerCharacter`
- ✅ At least one `SkillDataAsset` exists (e.g., `DA_Skill_MeleeAttack`, `DA_Skill_Fireball`)

---

## Step 1: Compile the Project

1. **Open Visual Studio 2022**
   - Open the solution file: `ActionRPG.sln`
   - Build the project: `Build > Build Solution` (or press `Ctrl+Shift+B`)
   - Wait for compilation to complete
   - Verify no errors (warnings are acceptable)

2. **Verify Compilation**
   - Check Output window for "Build succeeded"
   - If errors occur, review error messages and fix before proceeding

---

## Step 2: Create Blueprint Class from SkillManagerComponent

1. **Open Unreal Engine 5.7 Editor**
   - Launch the editor
   - Open the `ActionRPG` project

2. **Create Blueprint Class**
   - In Content Browser, navigate to: `Content/Blueprints/Components/Skills/`
   - If folder doesn't exist, create it:
     - Right-click in Content Browser → `New Folder`
     - Name: `Skills`
   - Right-click in `Content/Blueprints/Components/Skills/` folder
   - Select: `Blueprint Class`
   - In the "Pick Parent Class" dialog:
     - Search for: `SkillManagerComponent`
     - Select: `Skill Manager Component` (C++ class)
     - Click: `Select`

3. **Name the Blueprint**
   - Name: `BP_SkillManagerComponent`
   - Press `Enter` to confirm

4. **Open the Blueprint**
   - Double-click `BP_SkillManagerComponent` to open in Blueprint Editor

5. **Configure Default Settings** (Optional)
   - In the `Details` panel (right side):
     - `Skill Component` - Leave empty (will be auto-found at runtime)
     - No other settings need to be changed at this time
   - Close the Blueprint Editor

---

## Step 3: Attach SkillManagerComponent to Player Character

1. **Open Player Character Blueprint**
   - Navigate to: `Content/Blueprints/Characters/`
   - Open: `BP_ActionRPGPlayerCharacter` (or your player character Blueprint)

2. **Add SkillManagerComponent**
   - In the `Components` panel (left side):
     - Click: `Add Component` button (or `+ Add` button)
     - Search for: `Skill Manager Component`
     - Select: `Skill Manager Component` (or `BP_SkillManagerComponent` if you want to use the Blueprint version)
     - Component will be added to the Components list

3. **Configure SkillManagerComponent**
   - Select the `Skill Manager Component` in the Components panel
   - In the `Details` panel (right side):
     - **Skill Component** (optional):
       - If `SkillComponent` is already attached, you can manually assign it here
       - Or leave empty - the component will auto-find it at runtime via `FindComponentByClass`
     - **Unlocked Skills** - Leave empty (populated at runtime)
     - **Skill Bar Slots** - Leave empty (populated at runtime)

4. **Verify Component Attachment**
   - Ensure `SkillManagerComponent` appears in the Components list
   - Ensure `SkillComponent` is also present (from Days 31-32)
   - Both components should be attached to the player character

5. **Compile and Save**
   - Click: `Compile` button (top toolbar)
   - Wait for compilation to complete
   - Click: `Save` button (or `Ctrl+S`)
   - Close the Blueprint Editor

---

## Step 4: Test SkillManagerComponent in Editor

1. **Open Level**
   - Open your test level (or create a new one)
   - Place the player character in the level

2. **Play in Editor (PIE)**
   - Click: `Play` button (top toolbar)
   - Or press: `Alt+P`

3. **Test Basic Functionality** (via Blueprint or C++ code)
   - Open the `Output Log` window: `Window > Developer Tools > Output Log`
   - Look for log messages:
     - `"SkillManagerComponent::BeginPlay - Initializing skill manager"`
     - `"SkillManagerComponent::BeginPlay - SkillComponent found and linked"`
     - `"SkillManagerComponent: Initialized with X unlocked skills, 8 skill bar slots"`

4. **Verify Component Initialization**
   - The component should initialize without errors
   - Skill bar slots (0-7) should be created
   - SkillComponent should be found and linked

---

## Step 5: Create Test Blueprint for Skill Unlocking (Optional)

This step creates a simple test Blueprint to verify skill unlocking works.

1. **Create Test Blueprint**
   - Navigate to: `Content/Blueprints/Test/` (create folder if needed)
   - Right-click → `Blueprint Class`
   - Parent Class: `Actor`
   - Name: `BP_TestSkillUnlock`

2. **Open Blueprint**
   - Double-click `BP_TestSkillUnlock`

3. **Add Event BeginPlay Node**
   - In Event Graph:
     - Right-click → `Event BeginPlay`
     - Drag from execution pin

4. **Get Player Character**
   - Right-click → `Get Player Character`
   - Connect execution pin

5. **Get Skill Manager Component**
   - From Player Character pin:
     - Drag out → `Get Skill Manager Component`
   - Store in variable or use directly

6. **Get Skill Database**
   - Right-click → `Get Skill Database` (static function)
   - Connect execution pin

7. **Create Skill from Data Asset**
   - From Skill Database:
     - Drag out → `Create Skill`
   - Set `Skill ID`: `"MeleeAttack"` (or your skill ID)
   - Connect execution pin

8. **Unlock Skill**
   - From Skill Manager Component:
     - Drag out → `Unlock Skill`
   - Connect `Create Skill` output to `Skill` input
   - Connect execution pin

9. **Compile and Save**
   - Click: `Compile`
   - Click: `Save`
   - Close Blueprint Editor

10. **Test in Level**
    - Place `BP_TestSkillUnlock` in your test level
    - Play in Editor
    - Check Output Log for unlock messages

---

## Step 6: Test Skill Bar Slot Assignment (Optional)

1. **Create Test Blueprint for Slot Assignment**
   - Create: `BP_TestSkillBar` (similar to above)
   - Or add to existing test Blueprint

2. **Add Key Input Event**
   - In Event Graph:
     - Right-click → `Keyboard Events > 1` (or any key)
     - This will test slot 0 (hotkey 1)

3. **Get Skill Manager Component**
   - Get from player character (as in Step 5)

4. **Get Unlocked Skills**
   - From Skill Manager Component:
     - Drag out → `Get Unlocked Skills`
     - Get first skill from array (if any exist)

5. **Assign Skill to Slot**
   - From Skill Manager Component:
     - Drag out → `Assign Skill To Slot`
     - Set `Slot Index`: `0` (for hotkey 1)
     - Connect skill from `Get Unlocked Skills` to `Skill` input

6. **Test Activation**
   - Add another key event (e.g., `Keyboard Events > 1`)
   - From Skill Manager Component:
     - Drag out → `Activate Skill From Slot`
     - Set `Slot Index`: `0`

7. **Compile, Save, and Test**
   - Compile and save the Blueprint
   - Place in level and test

---

## Step 7: Verify Integration with SkillComponent

1. **Check SkillComponent Integration**
   - When `UnlockSkill()` is called:
     - Skill should be added to `UnlockedSkills` array
     - Skill should be granted to `SkillComponent` (check via `Get Skills` on SkillComponent)
   - When `ActivateSkillFromSlot()` is called:
     - Should call `SkillComponent::ActivateSkill()`
     - Should respect cooldowns and resource costs

2. **Test Cooldown Integration**
   - Unlock a skill
   - Assign to slot 0
   - Activate skill from slot
   - Try to activate again immediately (should fail due to cooldown)
   - Wait for cooldown to complete
   - Activate again (should succeed)

3. **Test Resource Cost Integration**
   - Activate skill with sufficient resources (if resource system exists)
   - Try to activate with insufficient resources (should fail)
   - Check Output Log for resource validation messages

---

## Step 8: Verify Events and Delegates

1. **Test OnSkillUnlocked Event**
   - Create a Blueprint that binds to `On Skill Unlocked` event
   - Unlock a skill
   - Verify event fires

2. **Test OnSkillSlotChanged Event**
   - Bind to `On Skill Slot Changed` event
   - Assign skill to slot
   - Verify event fires with correct slot index and skill

3. **Test OnSkillSlotCleared Event**
   - Bind to `On Skill Slot Cleared` event
   - Clear a slot
   - Verify event fires with correct slot index

---

## Step 9: Test Skill Prerequisites (Basic)

1. **Test Level Requirements** (stubbed for Phase 4)
   - Currently, level requirements are logged but not enforced
   - Check Output Log for level requirement messages
   - In Phase 4, this will be fully implemented

2. **Test Attribute Requirements** (stubbed for Phase 4)
   - Currently, attribute requirements always pass
   - In Phase 4, this will check actual attributes

3. **Test Prerequisite Skills** (stubbed for now)
   - Currently, prerequisite skills are not checked
   - This can be added later when `SkillDataAsset` includes prerequisite skill IDs

4. **Test Class Requirements** (stubbed for Phase 4)
   - Currently, class requirements always pass
   - In Phase 4, this will check character class

---

## Step 10: Test Edge Cases

1. **Test Invalid Slot Indices**
   - Try to assign skill to slot -1 (should fail)
   - Try to assign skill to slot 8 (should fail)
   - Try to assign skill to slot 10 (should fail)
   - Check Output Log for warning messages

2. **Test Unlocking Already Unlocked Skill**
   - Unlock a skill
   - Try to unlock the same skill again
   - Should return `true` (already unlocked) and log a message

3. **Test Assigning Unlocked Skill**
   - Try to assign a skill that hasn't been unlocked
   - Should fail and log a warning

4. **Test Slot Swapping**
   - Assign skill A to slot 0
   - Assign skill B to slot 0
   - Slot 0 should now contain skill B
   - Skill A should be removed from slot 0

5. **Test Clearing Empty Slot**
   - Try to clear an already empty slot
   - Should log a message but not error

6. **Test Activating from Empty Slot**
   - Try to activate skill from empty slot
   - Should fail and log a message

---

## Step 11: Integration with Input System (Preparation for Day 42)

1. **Verify Input Actions Exist**
   - Check that `IA_SkillSlot1` through `IA_SkillSlot8` exist in:
     - `Content/Input/IA_SkillSlot1` through `IA_SkillSlot8`
   - If they don't exist, create them (will be needed for Day 42)

2. **Test Manual Activation**
   - For now, test activation via Blueprint (as in Step 6)
   - In Day 42, these will be bound to hotkeys 1-8

---

## Step 12: Final Verification Checklist

Before considering Days 33-34 complete, verify:

- [ ] `SkillManagerComponent` compiles successfully
- [ ] `BP_SkillManagerComponent` Blueprint created
- [ ] Component attached to `BP_ActionRPGPlayerCharacter`
- [ ] Component initializes without errors in PIE
- [ ] `SkillComponent` is found and linked at runtime
- [ ] Skill bar slots (0-7) are initialized
- [ ] `UnlockSkill()` works and grants skill to `SkillComponent`
- [ ] `AssignSkillToSlot()` works for valid slots (0-7)
- [ ] `GetSkillAtSlot()` returns correct skill
- [ ] `ActivateSkillFromSlot()` calls `SkillComponent::ActivateSkill()`
- [ ] Cooldowns are respected when activating from slot
- [ ] Resource costs are validated (if resource system exists)
- [ ] Events fire correctly (`OnSkillUnlocked`, `OnSkillSlotChanged`, `OnSkillSlotCleared`)
- [ ] Invalid slot indices are rejected
- [ ] Slot swapping works (assigning to occupied slot)
- [ ] Clearing slots works
- [ ] Prerequisites validation functions exist (even if stubbed)
- [ ] Output Log shows appropriate messages for all operations

---

## Troubleshooting

### Issue: SkillManagerComponent Not Found in Add Component Menu

**Solution:**
- Ensure project compiled successfully in Visual Studio
- Close and reopen Unreal Editor
- Regenerate project files: `Tools > Refresh Visual Studio Project`
- Restart Unreal Editor

### Issue: SkillComponent Not Found at Runtime

**Solution:**
- Ensure `SkillComponent` is attached to the same actor as `SkillManagerComponent`
- Check that `SkillComponent` is added before `SkillManagerComponent` in Components list (or manually assign in Details panel)
- Verify both components are enabled

### Issue: Skills Not Unlocking

**Solution:**
- Check that skill prerequisites are met (currently stubbed, so should always pass)
- Verify `SkillDatabase::CreateSkill()` returns a valid skill
- Check Output Log for error messages
- Ensure `SkillComponent` is attached and found

### Issue: Skill Activation Fails

**Solution:**
- Check that skill is unlocked
- Check that skill is assigned to slot
- Verify `SkillComponent::CanActivateSkill()` returns true
- Check cooldown status
- Check resource costs (if resource system exists)
- Review Output Log for specific error messages

### Issue: Events Not Firing

**Solution:**
- Ensure event bindings are set up in Blueprint
- Verify events are being called (check Output Log)
- Check that multicast delegates are properly declared
- Ensure Blueprint is compiled and saved

---

## Next Steps

After completing Days 33-34:

1. **Day 35:** Skill Implementations - MeleeAttack & RangedAttack
2. **Day 36-37:** Skill Implementations - Dash & Magic
3. **Day 38-39:** Skill-Granting Items (SkillItem, SkillStone, BeastCore)
4. **Day 40-41:** Skill Bar UI (SkillBarWidget, SkillSlotWidget)
5. **Day 42:** Skill Hotkey Integration (will use `SkillManagerComponent::ActivateSkillFromSlot()`)

---

## Notes

- **Skill Bar Slots:** Slots 0-7 map to hotkeys 1-8 (slot 0 = hotkey 1, slot 7 = hotkey 8)
- **Prerequisites:** Attribute, level, and class requirement checks are stubbed for Phase 4 implementation
- **Skill Persistence:** Save/load system for skill unlocks and slot assignments will be implemented in a later phase
- **Component Order:** `SkillComponent` should be attached before `SkillManagerComponent` (or manually assigned) for proper initialization

---

**End of Manual Steps for Days 33-34**
