# Phase 3 Manual Steps - Day 35
**Focus:** Skill Implementations (MeleeAttack and RangedAttack)

## Overview
This guide covers the Unreal Engine 5.7 editor setup required to test the Day 35 C++ skill implementations:
- `USkillMeleeAttack`
- `USkillRangedAttack`
- `ASkillProjectile`

## Prerequisites
- Project compiles successfully.
- `SkillComponent` and `SkillManagerComponent` are already added to the player (from Days 31-34).
- Folders exist or can be created in `Content/Blueprints/Skills` and `Content/Data/Skills`.

---

## Step 1: Create Content Folders (if missing)
1. Open the Content Browser.
2. If you are in a different root, click the **Content** root in the left panel.
3. Right-click in the empty space and choose **New Folder**.
4. Create (or verify) these folders:
   - `Content/Blueprints/Skills`
   - `Content/Blueprints/Skills/Projectiles`
   - `Content/Data/Skills`
5. If a folder already exists, skip it and continue.

---

## Step 2: Create `BP_SkillProjectile`
1. Open `Content/Blueprints/Skills/Projectiles`.
2. Right-click and choose **Blueprint Class**.
3. Click **All Classes** and type `SkillProjectile` in the search box.
4. Select `SkillProjectile` and name the Blueprint `BP_SkillProjectile`.
5. Open `BP_SkillProjectile`.
6. In the Components panel, select `ProjectileMesh`.
7. In the Details panel, assign a simple mesh (e.g., `Shape_Sphere`) so the projectile is visible.
8. Select `CollisionSphere` and confirm:
   - Collision Enabled: **Query and Physics**
   - Object Type: **WorldDynamic**
   - `Pawn`: **Overlap**
   - `WorldStatic`: **Block**
   - `WorldDynamic`: **Block**
9. Compile, Save, and close the Blueprint.

---

## Step 3: Create Skill Data Assets
1. Open `Content/Data/Skills`.
2. Right-click and choose **Miscellaneous** -> **Data Asset**.
3. Pick **SkillDataAsset** from the list.
4. Name the first asset `DA_Skill_MeleeAttack`.
5. Repeat and create `DA_Skill_RangedAttack`.

---

## Step 4: Configure `DA_Skill_MeleeAttack`
1. Open `DA_Skill_MeleeAttack`.
2. In the Details panel, set:
- **Skill**
  - `SkillType`: `Skill`
  - `SkillID`: `MeleeAttack`
  - `SkillName`: `Melee Attack`
  - `Type`: `Melee`
  - `Category`: `Combat`
  - `CooldownDuration`: `1.0`
  - `ManaCost`: `0.0`
  - `StaminaCost`: `10.0`
  - `CastTime`: `0.0`
  - `Range`: `150.0`
  - `RequiredLevel`: `1`
- **Skill|Combat**
  - `BaseDamage`: `25.0`
  - `DamageType`: `Physical`
  - `AreaOfEffectRadius`: `0.0`
3. Save the asset.

---

## Step 5: Configure `DA_Skill_RangedAttack`
1. Open `DA_Skill_RangedAttack`.
2. In the Details panel, set:
- **Skill**
  - `SkillType`: `Skill`
  - `SkillID`: `RangedAttack`
  - `SkillName`: `Ranged Attack`
  - `Type`: `Ranged`
  - `Category`: `Combat`
  - `CooldownDuration`: `1.5`
  - `ManaCost`: `5.0`
  - `StaminaCost`: `0.0`
  - `CastTime`: `0.0`
  - `Range`: `1200.0`
  - `RequiredLevel`: `1`
- **Skill|Combat**
  - `BaseDamage`: `20.0`
  - `DamageType`: `Physical`
  - `AreaOfEffectRadius`: `0.0`
- **Skill|Projectile**
  - `ProjectileSpeed`: `2000.0`
  - `ProjectileClass`: `BP_SkillProjectile`
3. Save the asset.

---

## Step 6: Create Skill Blueprint Classes
1. Open `Content/Blueprints/Skills`.
2. Right-click and choose **Blueprint Class**.
3. Click **All Classes** and select `SkillMeleeAttack`.
4. Name it `BP_Skill_MeleeAttack`.
5. Repeat to create `BP_Skill_RangedAttack` (parent: `SkillRangedAttack`).
6. Open `BP_Skill_MeleeAttack`, set `SkillData` to `DA_Skill_MeleeAttack`, then Compile and Save.
7. Open `BP_Skill_RangedAttack`, set `SkillData` to `DA_Skill_RangedAttack`, then Compile and Save.

---

## Step 7: Grant Skills to the Player (Blueprint Setup)
1. Open your player Blueprint (e.g., `BP_ActionRPGPlayerCharacter`).
2. In the **Event Graph**, locate `Event BeginPlay` (or add it).
3. Add a `Get Component by Class` node and choose `SkillComponent`.
4. Add two `Construct Object from Class` nodes:
   - Class = `BP_Skill_MeleeAttack`, Outer = `Self`
   - Class = `BP_Skill_RangedAttack`, Outer = `Self`
5. From the `SkillComponent`, call `GrantSkill` for each constructed skill object.
6. Optional (skill slots):
   - Add `Get Component by Class` for `SkillManagerComponent`.
   - Call `UnlockSkill` for each constructed skill.
   - Call `AssignSkillToSlot`:
     - Slot `0` -> `BP_Skill_MeleeAttack`
     - Slot `1` -> `BP_Skill_RangedAttack`
7. Compile and Save the player Blueprint.

---

## Step 8: Test Melee Attack
1. Place a test target in the level:
   - Use any character or pawn that can receive damage.
   - If needed, create `BP_TestDummy` (child of `ActionRPGPlayerCharacter`) and place it in front of the player.
2. Select the test target and set:
   - `Auto Possess AI` = **Placed in World or Spawned** (if applicable)
   - Ensure it has a `CurrentHealth` value in defaults (if using `ActionRPGPlayerCharacter`)
3. Press **Play** (PIE).
4. Trigger the melee skill:
   - If bound to slot: press the hotkey (e.g., `1`)
   - Otherwise, ensure your input mapping calls `ActivateSkill` on the `SkillComponent`
5. Confirm melee hit detection:
   - Output Log shows `SkillMeleeAttack::Activate - Executing melee attack`
   - Output Log shows `SkillMeleeAttack::PerformMeleeHitDetection - Found ...`
   - Output Log shows `SkillMeleeAttack::ApplyDamageToTarget`
6. Confirm the target’s health decreases (if it uses `CurrentHealth`).

---

## Step 9: Test Ranged Attack
1. Press the ranged skill hotkey (e.g., `2`) or trigger it via Blueprint.
2. Verify a projectile spawns at the character and travels forward.
3. If it does not spawn:
   - Reopen `DA_Skill_RangedAttack` and ensure `ProjectileClass` is `BP_SkillProjectile`.
4. On hit, confirm:
   - Output Log shows `SkillProjectile::OnHit` or `SkillProjectile::OnBeginOverlap`
   - The target takes damage
5. Example Output Log (success):
   - `LogTemp: SkillRangedAttack::Activate - Executing ranged attack from BP_ActionRPGPlayerCharacter_C_0`
   - `LogTemp: SkillRangedAttack::SpawnProjectile - Set projectile speed to 2000.00`
   - `LogTemp: SkillRangedAttack::Activate - Spawned projectile BP_SkillProjectile_C_2 at location X=-5.098 Y=233.491 Z=89.650`
   - `LogTemp: SkillRangedAttack::Activate - Granted 5.00 base experience`
   - `LogTemp: SkillComponent::ActivateSkill - Activated skill: Ranged Attack (Target: None, Cooldown: 1.00)`

---

## Step 10: Verify Cooldowns and Experience
1. Attempt to activate each skill multiple times quickly to confirm cooldowns block activation.
2. Wait for cooldown completion and activate again.
3. Watch Output Log for experience logs:
   - `SkillMeleeAttack::Activate - Granted ... experience`
   - `SkillProjectile::OnHit - Granted ... experience`
4. If experience logs do not appear, verify:
   - `SkillComponent` is on the owner actor
   - `GrantSkill` was called on BeginPlay

---

## Notes
- If melee hits do not register, temporarily set the trace debug in `USkillMeleeAttack::PerformMeleeHitDetection()` to `ForDuration`.
- If projectiles do not spawn, confirm `ProjectileClass` is set in `DA_Skill_RangedAttack`.