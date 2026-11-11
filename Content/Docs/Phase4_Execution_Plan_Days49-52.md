# Phase 4 Execution Plan - Days 49-52
**Focus:** Experience + Class/Profession  
**UE Version:** 5.7  
**Last Updated:** 2026-01-22

---

## Overview
Days 49-52 cover progression: an XP component, class/profession selection, class attribute bonuses, and profession-granted skills.

## UE 5.8 progress (2026-10-22)

- The XP, class/profession, data asset, database, character-component, and native UI classes already exist in the 5.8 project. Warrior, Mage, and Blacksmith assets and the three progression widgets are present.
- Class/profession selection now preserves the current choice when a replacement fails requirements. XP input rejects non-finite values, and invalid XP-curve values fall back to default progression instead of risking an endless level-up loop.
- The full `ActionRPGEditor` build succeeded. Native automation tests `ActionRPG.Progression.ClassSelection` and `ActionRPG.Progression.ExperiencePool` passed in a fresh UE 5.8 command-line editor run.
- Live MCP/PIE verification passed for the C/B panel input, ClassDatabase listing (Warrior, Mage, Blacksmith), and allocation of 150 XP to Warrior. The panel showed Warrior level 1 with 50/100 class XP; the status panel showed player level 1, one used class slot, 0 unallocated/150 total XP, one free attribute point, and Strength 4/Constitution 6 after Warrior bonuses.
- After closing the editor, the native profession-row handler built successfully and the rebuilt editor passed both focused progression automation tests again. In fresh PIE, clicking Blacksmith updated the class panel to `Current Professions: Blacksmith`; the status panel showed Blacksmith and `1 / 1` profession slots used.
- Design clarification: professions grant skills, not primary or secondary attribute bonuses. `ClassComponent` now calculates attribute bonuses from selected classes only. The legacy bonus-map fields on profession assets remain serialized but are ignored by runtime bonus calculation. Synchronous Live Coding succeeded, and both focused progression automation tests passed again after this change.
- Non-profession closeout (2026-10-21): a clean UE 5.8 `ActionRPGEditor` build passed after the class-only bonus change. Fresh command-line automation passed all three focused tests: `ClassSelection` (including two-class level sum and class cap), `ExperiencePool`, and `ExperiencePickup`.
- Gameplay XP source: `AExperiencePickupActor` is an opt-in native world actor. A player overlap awards its configured amount to `ExperienceComponent` once, then consumes the pickup. The actor is not placed in the existing level; combat/quest XP sources do not exist yet and can call `AddExperience` when implemented.
- Live UE 5.8 recheck (2026-10-22): the editor launched, Live Coding succeeded for the class XP threshold display fix, and all three focused progression tests passed in-editor. In PIE, entering a temporary pickup awarded 25 XP, one Allocate click assigned it to Warrior, and the class row showed `25 / 100`. The temporary actor was removed without saving the level.
- Profession work resumed (2026-10-22): `DA_Profession_Blacksmith` grants `USkillBlacksmithCrafting` with `DA_Skill_BlacksmithCrafting` and knows `DA_Recipe_IronSword` (2 `DA_Item_IronOre` → 1 existing Iron Sword). Selection grants the skill; removal revokes it and clears hotbar assignments. The skill can craft the first available profession recipe from a hotbar slot, and the inventory transaction checks ingredients, weight, and slots before consuming anything. A clean editor build and all five focused tests passed, including saved-asset references and hotbar craft/revoke behavior. PIE confirmed the saved Blacksmith asset grants the skill to the player. Professions still grant no attributes.
- Remaining crafting UX: no recipe-selection UI. An Iron Ore pickup near PlayerStart in `/Game/NewMap` provides the 2 ore needed for the sole current recipe, which can be activated from the skill bar. A multi-recipe interface belongs to subsequent crafting work.

---

## Goals
- Implement `ExperienceComponent` for unallocated XP pool
- Implement `ClassComponent` for multi-class/profession selection and class-only bonuses
- Grant profession skills without changing primary or secondary attributes
- Add per-class XP curves and attribute points per class level
- Create class/profession data assets and database access

---

## Execution Tasks (C++)

### 1) ExperienceComponent
1. Create `Source/ActionRPG/Public/Components/Progression/ExperienceComponent.h`
2. Create `Source/ActionRPG/Private/Components/Progression/ExperienceComponent.cpp`
3. Implement:
   - Unallocated XP pool
   - `AddExperience`, `GetUnallocatedExperience`, `SpendExperience`
   - Total XP tracking (optional)

### 2) ClassComponent
1. Create `Source/ActionRPG/Public/Components/Progression/ClassComponent.h`
2. Create `Source/ActionRPG/Private/Components/Progression/ClassComponent.cpp`
3. Implement:
   - Multi-class/profession selection with caps
   - Per-class XP/level tracking
   - Requirement checks (level, attributes)
   - Class bonus application hooks (primary/secondary); professions never add attribute bonuses
   - Profession skill grant/removal hooks
   - Events for class/profession changes
   - Allocate unallocated XP to a class
   - Player level = sum of class levels

### 3) Class/Profession Data Assets
1. Create `Source/ActionRPG/Public/Progression/Core/ClassDataAsset.h/.cpp`
2. Create `Source/ActionRPG/Public/Progression/Core/ProfessionDataAsset.h/.cpp`
3. Include:
   - Display name, description, icon
   - Class bonus maps (primary/secondary)
   - Profession skill references (to be defined)
   - Requirements (level, attributes)
   - Class XP curve (optional)
   - AttributePointsPerLevel (per class level)

### 4) Class Database
1. Create `Source/ActionRPG/Public/Data/ClassDatabase.h/.cpp`
2. Implement load and lookup (by ID or name)

### 5) Character Integration
1. Attach `ExperienceComponent` and `ClassComponent` to `AActionRPGPlayerCharacter`
2. Ensure class bonuses can influence attribute components

### 6) UI (Character Status + Class/Profession)
1. Create widgets:
   - `WBP_CharacterStatusPanel` (class/profession slots, current class/profession, XP totals, attributes)
   - `WBP_ClassSelectionPanel` (list of classes + professions)
   - `WBP_ClassEntry` (single class/profession row)
2. Bind UI to components:
   - `ExperienceComponent` (Unallocated/Total XP)
   - `ClassComponent` (selected classes/professions, class XP/levels)
3. Add input to open/close panels (if not already):
   - `OpenCharacterStatusPanelAction`
   - `OpenClassPanelAction`

---

## Verification Checklist
- XP can be granted and stored as unallocated XP
- XP can be allocated to classes and levels them up
- Player level equals combined class levels
- Class selection validates requirements
- Class bonuses affect attributes
- Profession selection grants its designated skills without changing attributes
- Data assets load via ClassDatabase
- UI reflects XP and class/profession selection

---

## Notes
- Keep bonus application centralized to avoid double application.
- Use event-driven updates (no per-frame polling).
