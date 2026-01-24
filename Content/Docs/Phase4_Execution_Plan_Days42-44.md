# Phase 4 Execution Plan - Days 42-44
**Focus:** Primary Attributes + Secondary Attributes  
**UE Version:** 5.7  
**Last Updated:** 2026-01-22

---

## Overview
Days 42-44 cover the Attribute foundation: Primary attributes (STR, AGI, CON, DEX, INT, WIS), Secondary attributes (derived stats), and data asset definitions needed for a data-driven progression system.

---

## Goals
- Implement `AttributeComponent` for primary stats
- Implement `SecondaryAttributeComponent` for derived stats
- Define attribute data assets and supporting types
- Establish event-driven recalculation flow (no per-frame recompute)

---

## Execution Tasks (C++)

### 1) AttributeComponent (Primary Stats)
1. Create `Source/ActionRPG/Public/Components/Progression/AttributeComponent.h`
2. Create `Source/ActionRPG/Private/Components/Progression/AttributeComponent.cpp`
3. Implement:
   - Primary attribute storage (STR, AGI, CON, DEX, INT, WIS)
   - Getter/setter methods and clamping
   - Change delegates (e.g., `OnAttributeChanged`)
4. Add Blueprint accessors and events

### 2) SecondaryAttributeComponent (Derived Stats)
1. Create `Source/ActionRPG/Public/Components/Progression/SecondaryAttributeComponent.h`
2. Create `Source/ActionRPG/Private/Components/Progression/SecondaryAttributeComponent.cpp`
3. Implement:
   - Derived stat storage (HP, Mana, Crit, Armor, MoveSpeed, etc.)
   - Formula-driven recalculation
   - Event-driven recalculation on primary attribute change
4. Add Blueprint accessors and events

### 3) Progression Types + Data Assets
1. Create `Source/ActionRPG/Public/Progression/Core/ProgressionTypes.h`
2. Create `Source/ActionRPG/Public/Progression/Core/PrimaryAttributeDataAsset.h/.cpp`
3. Create `Source/ActionRPG/Public/Progression/Core/SecondaryAttributeDataAsset.h/.cpp`
4. Implement:
   - Attribute enums and data structures
   - Data Asset definitions for attribute metadata (display name, min/max, base values)

### 4) Character Integration
1. Attach `AttributeComponent` and `SecondaryAttributeComponent` to `AActionRPGPlayerCharacter`
2. Bind primary attribute change events to secondary recalculation

---

## Verification Checklist
- AttributeComponent compiles and is Blueprint-accessible
- SecondaryAttributeComponent recalculates on attribute changes
- Data assets load and apply default values
- Player character contains both components

---

## Notes
- Keep recalculation event-driven to avoid performance issues.
- Do not hardcode formulas in UI; use component values.
