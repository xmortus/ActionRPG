# Phase 4 Execution Plan - Days 49-52
**Focus:** Experience + Class/Profession  
**UE Version:** 5.7  
**Last Updated:** 2026-01-22

---

## Overview
Days 49-52 cover progression: an XP component, class/profession component, and data assets/databases needed for selection and bonuses.

---

## Goals
- Implement `ExperienceComponent` for unallocated XP pool
- Implement `ClassComponent` for multi-class/profession selection and bonuses
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
   - Bonus application hooks (primary/secondary)
   - Events for class/profession changes
   - Allocate unallocated XP to a class
   - Player level = sum of class levels

### 3) Class/Profession Data Assets
1. Create `Source/ActionRPG/Public/Progression/Core/ClassDataAsset.h/.cpp`
2. Create `Source/ActionRPG/Public/Progression/Core/ProfessionDataAsset.h/.cpp`
3. Include:
   - Display name, description, icon
   - Bonus maps (primary/secondary)
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
- Data assets load via ClassDatabase
- UI reflects XP and class/profession selection

---

## Notes
- Keep bonus application centralized to avoid double application.
- Use event-driven updates (no per-frame polling).
