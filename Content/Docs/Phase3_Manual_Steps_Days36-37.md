# Phase 3 Manual Steps - Days 36-37
**Focus:** Skill Implementations (Dash and Fireball)

## Overview
This guide covers the Unreal Engine 5.7 editor setup required to test the Day 36-37 skills:
- `USkillDash`
- `USkillMagic` (Fireball example)

## Prerequisites
- Project compiles successfully.
- `SkillComponent` and `SkillManagerComponent` are already added to the player (from Days 31-34).
- `BP_SkillProjectile` exists (from Day 35) or can be created if missing.
- `USkillDash` and `USkillMagic` C++ classes exist (implemented in code).

---

## Step 1: Create Content Folders (if missing)
1. Open the Content Browser.
2. If you are in a different root, click the **Content** root in the left panel.
3. Right-click and choose **New Folder**.
4. Create (or verify) these folders:
   - `Content/Blueprints/Skills`
   - `Content/Blueprints/Skills/Projectiles`
   - `Content/Data/Skills`
5. If a folder already exists, skip it and continue.

---

## Step 2: Create Skill Data Assets
1. Open `Content/Data/Skills`.
2. Right-click and choose **Miscellaneous** -> **Data Asset**.
3. Pick **SkillDataAsset** from the list.
4. Name the first asset `DA_Skill_Dash`.
5. Repeat and create `DA_Skill_Fireball`.

---

## Step 3: Configure `DA_Skill_Dash`
1. Open `DA_Skill_Dash`.
2. In the Details panel, set:
- **Skill**
  - `SkillType`: `Utility` (or `Magic` if you want it grouped with magic skills)
  - `SkillCategory`: `Movement`
  - `SkillName`: `Dash`
  - `SkillDescription`: `Quickly dash forward.`
- **Cooldown/Cost**
  - `Cooldown`: `1.5`
  - `ManaCost`: `0.0`
  - `StaminaCost`: `10.0`
- **Movement**
  - `AbilityMoveSpeed`: `1200.0`
  - `InvincibilityFrames`: `0.25`
3. Save the asset.

---

## Step 4: Configure `DA_Skill_Fireball`
1. Open `DA_Skill_Fireball`.
2. In the Details panel, set:
- **Skill**
  - `SkillType`: `Magic`
  - `SkillCategory`: `Combat`
  - `SkillName`: `Fireball`
  - `SkillDescription`: `Launch a fireball that explodes on impact.`
- **Cooldown/Cost**
  - `Cooldown`: `2.0`
  - `ManaCost`: `15.0`
  - `StaminaCost`: `0.0`
- **Combat**
  - `BaseDamage`: `20.0`
  - `DamageType`: `Fire`
  - `AreaOfEffectRadius`: `150.0`
- **Projectile**
  - `ProjectileSpeed`: `2000.0`
  - `ProjectileClass`: `BP_SkillProjectile`
3. Save the asset.

---

## Step 5: Create Skill Blueprint Classes
1. Open `Content/Blueprints/Skills`.
2. Right-click and choose **Blueprint Class**.
3. Click **All Classes** and select `SkillDash`.
4. Name it `BP_Skill_Dash`.
5. Repeat to create `BP_Skill_Fireball` (parent: `SkillMagic`).
6. Open `BP_Skill_Dash`, set `SkillData` to `DA_Skill_Dash`, then Compile and Save.
7. Open `BP_Skill_Fireball`, set `SkillData` to `DA_Skill_Fireball`, then Compile and Save.

---

## Step 6: Grant Skills to the Player (Blueprint Setup)
1. Open your player Blueprint (e.g., `BP_ActionRPGPlayerCharacter`).
2. In the **Event Graph**, locate `Event BeginPlay` (or add it).
3. Add a `Get Component by Class` node and choose `SkillComponent`.
4. Add two `Construct Object from Class` nodes:
   - Class = `BP_Skill_Dash`, Outer = `Self`
   - Class = `BP_Skill_Fireball`, Outer = `Self`
5. From the `SkillComponent`, call `GrantSkill` for each constructed skill object.
6. Optional (skill slots):
   - Add `Get Component by Class` for `SkillManagerComponent`.
   - Call `UnlockSkill` for each constructed skill.
   - Call `AssignSkillToSlot`:
     - Slot `2` -> `BP_Skill_Dash`
     - Slot `3` -> `BP_Skill_Fireball`
7. Compile and Save the player Blueprint.

---

## Step 7: Prepare a Test Target (Fireball)
1. Place a test target in the level:
   - Use any character or pawn that can receive damage.
   - If needed, create `BP_TestDummy` (child of `ActionRPGPlayerCharacter`) and place it in front of the player.
2. Select the test target and set:
   - `Auto Possess AI` = **Placed in World or Spawned** (if applicable)
   - Ensure it has a `CurrentHealth` value in defaults (if using `ActionRPGPlayerCharacter`)

---

## Step 8: Test Dash
1. Press **Play** (PIE).
2. Trigger the dash skill:
   - If bound to slot: press the hotkey (e.g., `3`)
   - Otherwise, ensure your input mapping calls `ActivateSkill` on the `SkillComponent`
3. Confirm dash movement:
   - Character moves forward in a short burst
   - Dash respects cooldown
4. If dash does not move the character:
   - Re-check `AbilityMoveSpeed` and `InvincibilityFrames`
   - Confirm the dash skill is granted to the player

---

## Step 9: Test Fireball
1. Trigger the fireball skill:
   - If bound to slot: press the hotkey (e.g., `4`)
   - Otherwise, ensure your input mapping calls `ActivateSkill` on the `SkillComponent`
2. Verify a projectile spawns at the character and travels forward.
3. On hit, confirm:
   - The target takes damage
   - The projectile is destroyed
4. If it does not spawn:
   - Reopen `DA_Skill_Fireball` and ensure `ProjectileClass` is `BP_SkillProjectile`.

---

## Step 10: Troubleshooting Checklist
- `BP_Skill_Dash` and `BP_Skill_Fireball` are compiled and have `SkillData` assigned.
- `SkillComponent` and `SkillManagerComponent` are attached to the player.
- `DA_Skill_Fireball` has `ProjectileClass` and `ProjectileSpeed` set.
- Input actions for skill slots are mapped and triggered.
