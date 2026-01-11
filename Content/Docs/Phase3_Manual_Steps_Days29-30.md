# Phase 3 Manual Steps - Days 29-30: Skill System Foundation
**After Code Implementation**  
**Date:** 2025-01-07  
**Phase:** Phase 3 - Skill System  
**Days:** 29-30 - Skill System Foundation

---

## Overview

This document provides detailed manual steps required in the Unreal Engine 5.7 Editor for Days 29-30 of Phase 3 (Skill System Foundation). These steps must be performed after completing the C++ code implementation.

**What We're Setting Up:**
- SkillDataAsset Data Assets (data-driven skill definitions)
- SkillDatabase singleton verification
- Folder structure for skills
- Basic skill system foundation

---

## Prerequisites

- ✅ All C++ code compiled successfully
- ✅ Unreal Editor is open
- ✅ Project is loaded in the editor
- ✅ No compilation errors in Output Log
- ✅ Visual Studio build completed successfully
- ✅ Phase 1 and Phase 2 complete (ItemBase, InventoryComponent, etc.)

---

## Step 1: Verify C++ Code Compiled

### 1.1 Check Output Log

1. **Open Output Log**
   - Window → `Developer Tools` → `Output Log`
   - Or press `Ctrl + Shift + L`

2. **Check for Errors**
   - Look for any compilation errors related to:
     - `SkillDataAsset`
     - `SkillBase`
     - `SkillDatabase`
   - If errors exist, fix them in Visual Studio first

3. **Verify Hot Reload**
   - Look for message: `LogHotReload: Hot reload completed successfully`
   - If hot reload failed, close and reopen Unreal Editor

### 1.2 Verify Classes Are Available

1. **Check Content Browser**
   - In Content Browser, click `View Options` (eye icon)
   - Enable `Show C++ Classes` (if not already enabled)

2. **Search for Skill Classes**
   - In Content Browser search bar, type: `Skill`
   - You should see:
     - `Skill Data Asset` (C++ class)
     - `Skill Base` (C++ class, if visible)
     - `Skill Database` (C++ class, if visible)

3. **Verify Classes Exist**
   - If classes don't appear, check:
     - C++ code compiled successfully
     - Classes are marked with `UCLASS()` macro
     - Project was rebuilt (not just hot reloaded)

---

## Step 2: Create Folder Structure

### 2.1 Create Skills Folder Structure

1. **Navigate to Content Browser**
   - Go to `Content/` folder root

2. **Create Skills Folder**
   - Right-click in Content Browser
   - Select `New Folder`
   - Name it: `Skills`
   - Location: `Content/Skills/`

3. **Create Blueprints Subfolder**
   - Right-click on `Skills` folder
   - Select `New Folder`
   - Name it: `Blueprints`
   - Location: `Content/Skills/Blueprints/`

4. **Create Data Subfolder**
   - Right-click on `Skills` folder
   - Select `New Folder`
   - Name it: `Data`
   - Location: `Content/Skills/Data/`

5. **Verify Folder Structure**
   ```
   Content/
   └── Skills/
       ├── Blueprints/
       └── Data/
   ```

---

## Step 3: Create SkillDataAsset Data Assets

### 3.1 Create Base Skill Data Asset Template

1. **Navigate to Content Browser**
   - Go to `Content/Skills/Data/` folder

2. **Create Skill Data Asset**
   - Right-click in Content Browser
   - Select `Miscellaneous` → `Data Asset`
   - In the "Pick Data Asset Class" dialog:
     - Search for: `Skill Data Asset`
     - Select `Skill Data Asset`
     - Click `Select`

3. **Name the Data Asset**
   - Name it: `DA_Skill_BaseTemplate`
   - This will be a template for creating other skill Data Assets
   - Save it in `Content/Skills/Data/`

4. **Open and Configure Data Asset**
   - Double-click `DA_Skill_BaseTemplate`
   - In Details panel, configure base properties:

   **Basic Information:**
   - `Skill ID`: `SKILL_BASE_TEMPLATE` (FName)
   - `Skill Name`: `Base Skill Template` (FText)
   - `Skill Description`: `Template for creating new skills` (FText)
   - `Skill Icon`: (Leave empty for now, or assign a placeholder texture)

   **Skill Type:**
   - `Skill Type`: `Utility` (ESkillType enum)

   **Cooldown & Costs:**
   - `Cooldown Duration`: `1.0` (float, seconds)
   - `Mana Cost`: `0.0` (float)
   - `Stamina Cost`: `0.0` (float)

   **Timing & Range:**
   - `Cast Time`: `0.0` (float, seconds)
   - `Range`: `0.0` (float, units)

   **Requirements:**
   - `Required Level`: `1` (int32)

   **Combat Properties:**
   - `Base Damage`: `0.0` (float)
   - `Damage Type`: `Physical` (EDamageType enum)
   - `Area of Effect Radius`: `0.0` (float)

   **Projectile Properties:**
   - `Projectile Speed`: `0.0` (float, units/second)
   - `Projectile Class`: (Leave empty, TSubclassOf<AActor>)

   **Movement Properties:**
   - `Ability Move Speed`: `0.0` (float, units/second)
   - `Invincibility Frames`: `0.0` (float, seconds)

5. **Save Template**
   - Click `Save` button
   - Close Data Asset Editor

### 3.2 Create Sample Skill Data Assets

#### Create Melee Attack Skill Data Asset

1. **Duplicate Base Template**
   - Right-click `DA_Skill_BaseTemplate`
   - Select `Duplicate`
   - Rename to: `DA_Skill_MeleeAttack`

2. **Open and Configure**
   - Double-click `DA_Skill_MeleeAttack`
   - Configure properties:

   **Basic Information:**
   - `Skill ID`: `SKILL_MELEE_ATTACK`
   - `Skill Name`: `Melee Attack`
   - `Skill Description`: `A basic melee attack that deals physical damage to nearby enemies.`
   - `Skill Icon`: (Assign a melee attack icon texture if available)

   **Skill Type:**
   - `Skill Type`: `Melee`

   **Cooldown & Costs:**
   - `Cooldown Duration`: `0.5` (0.5 seconds)
   - `Mana Cost`: `0.0`
   - `Stamina Cost`: `10.0`

   **Timing & Range:**
   - `Cast Time`: `0.0`
   - `Range`: `150.0` (150 units melee range)

   **Requirements:**
   - `Required Level`: `1`

   **Combat Properties:**
   - `Base Damage`: `10.0` (float)
   - `Damage Type`: `Physical` (EDamageType enum)
   - `Area of Effect Radius`: `0.0` (no AoE for melee)

   **Projectile Properties:**
   - `Projectile Speed`: `0.0` (not used for melee)
   - `Projectile Class`: (Leave empty, not used for melee)

   **Movement Properties:**
   - `Ability Move Speed`: `0.0` (not used for melee)
   - `Invincibility Frames`: `0.0` (not used for melee)

3. **Save**
   - Click `Save` button
   - Close Data Asset Editor

#### Create Ranged Attack Skill Data Asset

1. **Duplicate Base Template**
   - Right-click `DA_Skill_BaseTemplate`
   - Select `Duplicate`
   - Rename to: `DA_Skill_RangedAttack`

2. **Open and Configure**
   - Double-click `DA_Skill_RangedAttack`
   - Configure properties:

   **Basic Information:**
   - `Skill ID`: `SKILL_RANGED_ATTACK`
   - `Skill Name`: `Ranged Attack`
   - `Skill Description`: `A ranged attack that fires a projectile at enemies.`
   - `Skill Icon`: (Assign a ranged attack icon texture if available)

   **Skill Type:**
   - `Skill Type`: `Ranged`

   **Cooldown & Costs:**
   - `Cooldown Duration`: `1.0`
   - `Mana Cost`: `0.0`
   - `Stamina Cost`: `15.0`

   **Timing & Range:**
   - `Cast Time`: `0.2`
   - `Range`: `1000.0` (1000 units range)

   **Requirements:**
   - `Required Level`: `1`

   **Combat Properties:**
   - `Base Damage`: `15.0` (float)
   - `Damage Type`: `Physical` (EDamageType enum)
   - `Area of Effect Radius`: `0.0` (no AoE for basic ranged)

   **Projectile Properties:**
   - `Projectile Speed`: `2000.0` (float, units/second)
   - `Projectile Class`: (Assign projectile Blueprint class when available)

   **Movement Properties:**
   - `Ability Move Speed`: `0.0` (not used for ranged)
   - `Invincibility Frames`: `0.0` (not used for ranged)

3. **Save**
   - Click `Save` button
   - Close Data Asset Editor

#### Create Dash Skill Data Asset

1. **Duplicate Base Template**
   - Right-click `DA_Skill_BaseTemplate`
   - Select `Duplicate`
   - Rename to: `DA_Skill_Dash`

2. **Open and Configure**
   - Double-click `DA_Skill_Dash`
   - Configure properties:

   **Basic Information:**
   - `Skill ID`: `SKILL_DASH`
   - `Skill Name`: `Dash`
   - `Skill Description`: `Quickly dash forward, covering ground quickly.`
   - `Skill Icon`: (Assign a dash icon texture if available)

   **Skill Type:**
   - `Skill Type`: `Utility`

   **Cooldown & Costs:**
   - `Cooldown Duration`: `3.0` (3 seconds)
   - `Mana Cost`: `0.0`
   - `Stamina Cost`: `20.0`

   **Timing & Range:**
   - `Cast Time`: `0.0`
   - `Range`: `500.0` (500 units dash distance)

   **Requirements:**
   - `Required Level`: `1`

   **Combat Properties:**
   - `Base Damage`: `0.0` (dash doesn't deal damage)
   - `Damage Type`: `Physical` (not applicable for dash)
   - `Area of Effect Radius`: `0.0` (not applicable for dash)

   **Projectile Properties:**
   - `Projectile Speed`: `0.0` (not used for dash)
   - `Projectile Class`: (Leave empty, not used for dash)

   **Movement Properties:**
   - `Ability Move Speed`: `2000.0` (float, units/second - dash speed)
   - `Invincibility Frames`: `0.2` (float, seconds - invincibility duration)

3. **Save**
   - Click `Save` button
   - Close Data Asset Editor

#### Create Fireball (Magic) Skill Data Asset

1. **Duplicate Base Template**
   - Right-click `DA_Skill_BaseTemplate`
   - Select `Duplicate`
   - Rename to: `DA_Skill_Fireball`

2. **Open and Configure**
   - Double-click `DA_Skill_Fireball`
   - Configure properties:

   **Basic Information:**
   - `Skill ID`: `SKILL_FIREBALL`
   - `Skill Name`: `Fireball`
   - `Skill Description`: `Hurl a ball of fire that explodes on impact, dealing area damage.`
   - `Skill Icon`: (Assign a fireball icon texture if available)

   **Skill Type:**
   - `Skill Type`: `Magic`

   **Cooldown & Costs:**
   - `Cooldown Duration`: `2.0` (2 seconds)
   - `Mana Cost`: `25.0`
   - `Stamina Cost`: `0.0`

   **Timing & Range:**
   - `Cast Time`: `0.5` (0.5 seconds cast time)
   - `Range`: `800.0` (800 units range)

   **Requirements:**
   - `Required Level`: `1`

   **Combat Properties:**
   - `Base Damage`: `30.0` (float)
   - `Damage Type`: `Fire` (EDamageType enum)
   - `Area of Effect Radius`: `200.0` (float, units - explosion radius)

   **Projectile Properties:**
   - `Projectile Speed`: `1500.0` (float, units/second - fireball travel speed)
   - `Projectile Class`: (Assign fireball projectile Blueprint class when available)

   **Movement Properties:**
   - `Ability Move Speed`: `0.0` (not used for magic)
   - `Invincibility Frames`: `0.0` (not used for magic)

3. **Save**
   - Click `Save` button
   - Close Data Asset Editor

---

## Step 4: Verify SkillDatabase Integration

### 4.1 Check SkillDatabase Initialization

1. **Open Project Settings**
   - Edit → `Project Settings`

2. **Navigate to Game**
   - In left panel, go to `Game` section
   - Look for any SkillDatabase-related settings (if implemented)

3. **Check Output Log for SkillDatabase**
   - Open Output Log (Window → `Developer Tools` → `Output Log`)
   - Look for messages like:
     - `SkillDatabase: Initialized`
     - `SkillDatabase: Loaded X skills`
   - These messages indicate SkillDatabase is loading skills

### 4.2 Test SkillDatabase Lookup (Optional - If Blueprint Access Available)

1. **Create Test Blueprint**
   - Navigate to `Content/Skills/Blueprints/`
   - Right-click → `Blueprint Class`
   - Select `Actor` as parent class
   - Name it: `BP_TestSkillDatabase`

2. **Test Skill Lookup**
   - Open `BP_TestSkillDatabase`
   - In Event Graph, add `BeginPlay` event
   - Search for: `Get Skill Database` (if Blueprint-callable)
   - Try to get a skill by ID (if function exists)
   - Add a `Print String` node to display result

3. **Compile and Test**
   - Click `Compile` button
   - If compilation succeeds, SkillDatabase is accessible
   - Place actor in level and test in Play mode

4. **Clean Up**
   - Delete `BP_TestSkillDatabase` (it was just for testing)
   - Or keep it for future testing

---

## Step 5: Verify SkillDataAsset Properties

### 5.1 Check All Properties Are Visible

1. **Open Any Skill Data Asset**
   - Double-click `DA_Skill_MeleeAttack`

2. **Verify Properties in Details Panel**
   - Check that all expected properties are visible:
     - ✅ Basic Information (SkillID, SkillName, SkillDescription, SkillIcon)
     - ✅ Skill Type (ESkillType enum)
     - ✅ Skill Category (ESkillCategory enum)
     - ✅ Cooldown & Costs (CooldownDuration, ManaCost, StaminaCost)
     - ✅ Timing & Range (CastTime, Range)
     - ✅ Requirements (RequiredLevel)
     - ✅ Combat Properties (BaseDamage, DamageType, AreaOfEffectRadius)
     - ✅ Projectile Properties (ProjectileSpeed, ProjectileClass)
     - ✅ Movement Properties (AbilityMoveSpeed, InvincibilityFrames)

3. **Check Property Types**
   - Verify enum dropdowns work (SkillType)
   - Verify numeric fields accept values
   - Verify text fields accept input
   - Verify asset references work (SkillIcon)

### 5.2 Test Data Asset Validation

1. **Test Invalid Values**
   - Try setting negative cooldown (should be prevented or warned)
   - Try setting negative costs (should be prevented or warned)
   - Try leaving SkillID empty (should show warning if required)

2. **Check for Validation Warnings**
   - Look for yellow warning icons in Details panel
   - Check Output Log for validation messages

---

## Step 6: Organize Data Assets

### 6.1 Organize by Skill Type (Optional)

1. **Create Subfolders**
   - In `Content/Skills/Data/`, create subfolders:
     - `Melee/`
     - `Ranged/`
     - `Magic/`
     - `Utility/`

2. **Move Data Assets**
   - Move `DA_Skill_MeleeAttack` to `Melee/` folder
   - Move `DA_Skill_RangedAttack` to `Ranged/` folder
   - Move `DA_Skill_Fireball` to `Magic/` folder
   - Move `DA_Skill_Dash` to `Utility/` folder
   - Keep `DA_Skill_BaseTemplate` in root `Data/` folder

3. **Verify Organization**
   ```
   Content/Skills/Data/
   ├── DA_Skill_BaseTemplate
   ├── Melee/
   │   └── DA_Skill_MeleeAttack
   ├── Ranged/
   │   └── DA_Skill_RangedAttack
   ├── Magic/
   │   └── DA_Skill_Fireball
   └── Utility/
       └── DA_Skill_Dash
   ```

---

## Step 7: Create Skill Icons (Optional but Recommended)

### 7.1 Prepare Skill Icons

1. **Create Icon Textures**
   - Create or import skill icon textures
   - Recommended size: 128x128 or 256x256 pixels
   - Format: PNG with transparency

2. **Organize Icons**
   - Create folder: `Content/Textures/Skills/`
   - Place icon textures here:
     - `T_Skill_MeleeAttack_Icon`
     - `T_Skill_RangedAttack_Icon`
     - `T_Skill_Dash_Icon`
     - `T_Skill_Fireball_Icon`

3. **Assign Icons to Data Assets**
   - Open each skill Data Asset
   - Set `Skill Icon` property to corresponding texture
   - Save Data Asset

---

## Step 8: Final Verification

### 8.1 Compile All Data Assets

1. **Check for Compilation Errors**
   - Select all skill Data Assets in Content Browser
   - Right-click → `Validate` (if option available)
   - Check Output Log for any errors

2. **Verify Data Asset References**
   - Open each Data Asset
   - Verify all required properties are set
   - Check for any null references

### 8.2 Test in Game (If SkillBase/SkillComponent Available)

1. **Check if SkillBase Can Be Created**
   - In Content Browser, search for `Skill Base`
   - If visible, verify it can be used (though typically not used directly in Blueprint)

2. **Check Output Log**
   - Play game in editor
   - Check Output Log for:
     - SkillDatabase initialization messages
     - Any errors related to skills
     - Any warnings about missing skill data

---

## Verification Checklist

### Code Compilation
- [ ] All C++ code compiled without errors
- [ ] SkillDataAsset class visible in Content Browser
- [ ] SkillBase class visible in Content Browser (if applicable)
- [ ] SkillDatabase class visible in Content Browser (if applicable)
- [ ] Hot reload completed successfully

### Folder Structure
- [ ] `Content/Skills/` folder created
- [ ] `Content/Skills/Blueprints/` folder created
- [ ] `Content/Skills/Data/` folder created
- [ ] Optional subfolders created (Melee, Ranged, Magic, Utility)

### Data Assets
- [ ] `DA_Skill_BaseTemplate` created and configured
- [ ] `DA_Skill_MeleeAttack` created and configured
- [ ] `DA_Skill_RangedAttack` created and configured
- [ ] `DA_Skill_Dash` created and configured
- [ ] `DA_Skill_Fireball` created and configured
- [ ] All Data Assets have valid SkillID values
- [ ] All Data Assets have SkillName and Description set
- [ ] All Data Assets have appropriate SkillType set
- [ ] All Data Assets have cooldown and cost values set
- [ ] All Data Assets compile without errors

### SkillDatabase
- [ ] SkillDatabase initializes (check Output Log)
- [ ] Skills are loaded by SkillDatabase (check Output Log)
- [ ] No errors related to SkillDatabase in Output Log

### Organization
- [ ] Data Assets organized in appropriate folders
- [ ] Skill icons assigned (if available)
- [ ] Naming conventions followed (DA_Skill_*)

### Testing
- [ ] All Data Assets can be opened and edited
- [ ] Properties are visible and editable
- [ ] No validation warnings in Data Assets
- [ ] No errors in Output Log during play

---

## Common Issues & Solutions

### Issue: SkillDataAsset Not Visible in Content Browser

**Symptoms:** Cannot find "Skill Data Asset" when creating Data Asset

**Solutions:**
1. Verify C++ code compiled successfully
2. Check that `SkillDataAsset` class has `UCLASS()` macro
3. Ensure class inherits from `UDataAsset`
4. Close and reopen Unreal Editor
5. Rebuild project in Visual Studio (not just compile)

### Issue: Properties Not Visible in Data Asset

**Symptoms:** Expected properties don't appear in Details panel

**Solutions:**
1. Verify properties are marked with `UPROPERTY()` macro
2. Check property specifiers (BlueprintReadOnly, EditAnywhere, etc.)
3. Ensure properties are public or have appropriate access specifiers
4. Recompile C++ code
5. Close and reopen Data Asset editor

### Issue: Enum Values Not Showing

**Symptoms:** SkillType dropdown is empty or shows numbers instead of names

**Solutions:**
1. Verify enum is marked with `UENUM(BlueprintType)`
2. Check enum values are properly defined
3. Recompile C++ code
4. Close and reopen Data Asset editor

### Issue: SkillDatabase Not Loading Skills

**Symptoms:** No messages in Output Log about SkillDatabase initialization

**Solutions:**
1. Verify SkillDatabase singleton is initialized (check BeginPlay or similar)
2. Check that skills are registered with SkillDatabase
3. Verify SkillDatabase::Get() is called somewhere in code
4. Check Output Log for errors
5. Verify SkillDatabase class is properly set up

### Issue: Data Asset Validation Errors

**Symptoms:** Yellow warning icons or validation errors in Data Asset

**Solutions:**
1. Check that required properties (SkillID, SkillName) are set
2. Verify property values are within valid ranges
3. Check for null references
4. Review validation logic in SkillDataAsset code
5. Fix any invalid values

---

## Next Steps

After completing Days 29-30 manual steps:

1. **Proceed to Days 31-32:** SkillComponent Implementation
   - Create Blueprint class from SkillComponent
   - Attach to PlayerCharacter
   - Configure component properties

2. **Prepare for Skill Implementations:**
   - Review skill implementation requirements
   - Prepare animation assets (for melee/ranged skills)
   - Prepare particle effects (for magic skills)
   - Prepare projectile actors (for ranged/magic skills)

3. **Documentation:**
   - Document any custom properties added to SkillDataAsset
   - Note any deviations from the implementation plan
   - Update skill creation guidelines for designers

---

**End of Days 29-30 Manual Steps**
