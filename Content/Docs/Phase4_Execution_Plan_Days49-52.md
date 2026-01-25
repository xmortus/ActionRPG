# Phase 4 Execution Plan - Days 49-52
**Focus:** Experience + Class/Profession  
**UE Version:** 5.7  
**Last Updated:** 2026-01-22

---

## Overview
Days 49-52 cover progression: an XP component, class/profession component, and data assets/databases needed for selection and bonuses.

---

## Goals
- Implement `ExperienceComponent` for XP, level thresholds, and events
- Implement `ClassComponent` for class/profession selection and bonuses
- Create class/profession data assets and database access

---

## Execution Tasks (C++)

### 1) ExperienceComponent
1. Create `Source/ActionRPG/Public/Components/Progression/ExperienceComponent.h`
2. Create `Source/ActionRPG/Private/Components/Progression/ExperienceComponent.cpp`
3. Implement:
   - Current XP, current level
   - XP thresholds (data-driven or curve)
   - `AddExperience`, `GetCurrentXP`, `GetCurrentLevel`
   - Level-up events

### 2) ClassComponent
1. Create `Source/ActionRPG/Public/Components/Progression/ClassComponent.h`
2. Create `Source/ActionRPG/Private/Components/Progression/ClassComponent.cpp`
3. Implement:
   - Selected class/profession tracking
   - Requirement checks (level, attributes)
   - Bonus application hooks (primary/secondary)
   - Events for class/profession changes

### 3) Class/Profession Data Assets
1. Create `Source/ActionRPG/Public/Progression/Core/ClassDataAsset.h/.cpp`
2. Create `Source/ActionRPG/Public/Progression/Core/ProfessionDataAsset.h/.cpp`
3. Include:
   - Display name, description, icon
   - Bonus maps (primary/secondary)
   - Requirements (level, attributes)

### 4) Class Database
1. Create `Source/ActionRPG/Public/Data/ClassDatabase.h/.cpp`
2. Implement load and lookup (by ID or name)

### 5) Character Integration
1. Attach `ExperienceComponent` and `ClassComponent` to `AActionRPGPlayerCharacter`
2. Ensure class bonuses can influence attribute components

### 6) UI (Experience + Class/Profession)
1. Create widgets:
   - `WBP_ExperiencePanel` (XP totals, unallocated XP)
   - `WBP_ClassSelectionPanel` (list of classes + professions)
   - `WBP_ClassEntry` (single class/profession row)
2. Bind UI to components:
   - `ExperienceComponent` (Unallocated/Total XP)
   - `ClassComponent` (selected classes/professions, class XP/levels)
3. Add input to open/close panels (if not already)

---

## Verification Checklist
- XP can be granted and triggers level-up
- Class selection validates requirements
- Class bonuses affect attributes
- Data assets load via ClassDatabase
- UI reflects XP and class/profession selection

---

## Notes
- Keep bonus application centralized to avoid double application.
- Use event-driven updates (no per-frame polling).
