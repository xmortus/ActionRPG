# Phase 4: Attribute & Progression System - Implementation Plan
**Duration:** Week 7-9 (21 days)  
**Based on:** Architecture & Design Plan v1.4  
**Last Updated:** 2026-01-22

---

## Table of Contents
1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Week 1 Breakdown](#week-1-breakdown)
4. [Week 2 Breakdown](#week-2-breakdown)
5. [Week 3 Breakdown](#week-3-breakdown)
6. [Detailed Implementation Steps](#detailed-implementation-steps)
7. [Testing Checklist](#testing-checklist)
8. [Success Criteria](#success-criteria)
9. [Common Issues & Solutions](#common-issues--solutions)

---

## Overview

### Phase 4 Goals
- Implement AttributeComponent (Primary Attributes)
- Implement SecondaryAttributeComponent with formulas
- Implement EquipmentComponent (Equipment system)
- Create attribute Data Assets
- Create equipment Data Assets (EquipmentItem, WeaponItem)
- Implement ExperienceComponent (XP system)
- Implement ClassComponent (Class/Profession system)
- Create class and profession Data Assets
- Build attribute and class UI widgets
- Build equipment UI (EquipmentWidget, EquipmentSlotWidget)
- Build equipment UI layout similar to paper-doll + inventory + stats
- Integrate equipment bonuses into SecondaryAttributeComponent

### Phase 4 Deliverables
- Primary attribute system (STR, AGI, CON, DEX, INT, WIS)
- Secondary attribute calculation system with equipment integration
- Equipment system with slot management
- Equipment item classes (EquipmentItem, WeaponItem)
- Equipment bonuses system
- Equipment UI with drag and drop
- Equipment UI layout with paper-doll slots, stats, inventory grid, filters
- XP management and allocation system
- Class/Profession leveling system
- Class specialization system
- Attribute and class UI

### Success Metrics
- Attributes can be modified and queried from Blueprint and C++
- Secondary attributes update when primary attributes or equipment change
- Equipment items can be equipped/unequipped and validate requirements
- Equipment bonuses apply to secondary attributes
- XP accrues and levels update correctly
- Class/Profession selection and bonuses apply
- Attribute, equipment, and class UI displays and updates correctly

---

## Prerequisites

### Required Knowledge
- Unreal Engine 5.7 component system
- UMG Widget system
- Data Assets and Asset Manager
- Delegate/event patterns in UE

### Required Tools
- Unreal Engine 5.7 installed
- Visual Studio 2022 (or compatible IDE)
- Phase 1-3 complete (all deliverables verified)

### Pre-Phase Setup
- Phase 1 complete (ItemBase, ItemDataAsset, ItemDatabase)
- Phase 2 complete (InventoryComponent, ItemPickupActor, Inventory UI)
- Phase 3 complete (Skill system, skill UI, skill items)
- Project compiles and runs in editor

---

## Week 1 Breakdown

### Day 42-44: Primary Attributes + Secondary Attributes

#### Tasks
1. **Create AttributeComponent**
   - Location: `Source/ActionRPG/Public/Components/Progression/AttributeComponent.h`
   - Primary attributes: STR, AGI, CON, DEX, INT, WIS
   - Set/Get/Modify, clamping, and change delegates

2. **Create SecondaryAttributeComponent**
   - Location: `Source/ActionRPG/Public/Components/Progression/SecondaryAttributeComponent.h`
   - Derived stats (HP, Mana, Crit, Armor, MoveSpeed, etc.)
   - Recalculate on primary attribute change
   - Event-driven updates (no per-frame recalculation)

3. **Create Attribute Data Assets**
   - `PrimaryAttributeDataAsset`
   - `SecondaryAttributeDataAsset`
   - `ProgressionTypes.h` enums/structs

#### Deliverables
- Primary attribute system implemented
- Secondary attribute formulas implemented
- Attribute data assets created

---

## Week 2 Breakdown

### Day 45-48: Equipment System Foundation

#### Tasks
1. **Create EquipmentComponent**
   - Location: `Source/ActionRPG/Public/Components/Inventory/EquipmentComponent.h`
   - Slot definitions (MainHand, Offhand, Head, Chest, Legs, etc.)
   - Equip/Unequip validation and events

2. **Create Equipment Items**
   - `EquipmentItem` and `WeaponItem`
   - Extend `ItemDataAsset` for equipment slot/type and stat bonuses

3. **Integrate Equipment Bonuses**
   - Equipment bonuses apply to SecondaryAttributeComponent
   - Validate requirements with AttributeComponent

4. **Equipment UI Layout (Paper-Doll + Inventory)**
   - Create `EquipmentWidget` (overall layout: paper-doll + stats + inventory)
   - Create `EquipmentSlotWidget` (equipment slots + quick equip targets)
   - Create `EquipmentStatsWidget` (primary + secondary stats panel)
   - Create `EquipmentInventoryWidget` (inventory grid + filters)
   - Layout targets:
     - Left panel: paper-doll equipment slots
     - Center panel: stats (primary + secondary, damage/defense)
     - Right panel: inventory grid with item icons and stack counts
     - Bottom filters: Armor, Weapons, Food & Potions, Quest Items, All Items
   - Wire drag & drop from inventory to equipment slots and back

#### Deliverables
- Equipment system with slot management
- Equipment item classes
- Equipment bonuses apply to stats
- Equipment UI layout wired to components

---

## Week 3 Breakdown

### Day 49-52: Experience + Class/Profession

#### Tasks
1. **Create ExperienceComponent**
   - XP accumulation, level thresholds, and events

2. **Create ClassComponent**
   - Class/profession selection
   - Class bonuses and requirement checks

3. **Class/Profession Data Assets**
   - `ClassDataAsset`, `ProfessionDataAsset`
   - `ClassDatabase` setup

#### Deliverables
- XP system with level progression
- Class/Profession system with bonuses
- Data assets for classes/professions

---

## Detailed Implementation Steps

### 1) Attribute Components
- Implement AttributeComponent and SecondaryAttributeComponent
- Add events for primary and secondary attribute changes
- Validate formulas and data asset binding

### 2) Equipment System
- Implement EquipmentComponent
- Create EquipmentItem and WeaponItem
- Hook equipment bonuses into secondary attributes

### 3) Experience + Class System
- Implement ExperienceComponent and ClassComponent
- Add Data Assets and databases
- Handle class selection and bonus application

### 4) UI Integration
- Create widgets:
  - `AttributeWidget`
  - `EquipmentWidget` (overall layout: paper-doll + stats + inventory)
  - `EquipmentSlotWidget` (paper-doll slots + quick equip targets)
  - `EquipmentStatsWidget` (derived stats panel)
  - `EquipmentInventoryWidget` (inventory grid + filters)
  - `ClassWidget`
- Bind widgets to components and update on events

#### Equipment UI Layout Notes (target style)
- **Left panel:** paper-doll equipment slots (head, chest, legs, hands, feet, main hand, offhand, ring, amulet)
- **Center panel:** character stats (primary + secondary, damage/defense)
- **Right panel:** inventory grid with item icons and stack counts
- **Bottom filters:** category buttons (Armor, Weapons, Food & Potions, Quest Items, All Items)
- **Drag & drop:** items move between inventory and equipment slots; invalid drops return to inventory

---

## Testing Checklist

### Attribute System
- Modify primary attributes and verify updates
- Secondary attributes recalculate correctly
- Equipment bonuses adjust secondary attributes

### Equipment System
- Equip/unequip items
- Slot validation (wrong slot rejection)
- Requirement validation (level/attribute requirements)

### XP + Class System
- XP gain and level-up triggers
- Class selection and bonuses
- Profession selection validation

### UI
- Attribute UI updates on changes
- Equipment UI updates on equip/unequip
- Class UI displays selection and bonuses

---

## Success Criteria
- Attributes and secondary attributes are reliable and event-driven
- Equipment integrates with stats correctly
- XP and class progression systems are functional
- UI reflects live component state
- All systems compile and run without errors

---

## Common Issues & Solutions
- **Secondary stats not updating:** ensure AttributeComponent and EquipmentComponent broadcast changes
- **Equipment not applying bonuses:** check equipment bonus aggregation in SecondaryAttributeComponent
- **Class bonuses missing:** verify ClassComponent applies modifiers on selection
---
name: phase4-implementation-plan
overview: Create a Phase 4 implementation plan (Attributes, Equipment, Progression) aligned to Architecture_Design_Plan Phase 4 goals and deliverables, including key components, data assets, UI, and integration/testing steps.
todos:
  - id: phase4-attributes
    content: Implement Attribute + SecondaryAttribute components and data assets
    status: pending
  - id: phase4-equipment
    content: Implement EquipmentComponent and Equipment/Weapon items
    status: pending
  - id: phase4-progression
    content: Implement XP + Class/Profession components and data assets
    status: pending
  - id: phase4-ui
    content: Build Attribute/Equipment/Class UI widgets and bindings
    status: pending
  - id: phase4-integration
    content: Integrate components, bonuses, validation, and update docs
    status: pending
isProject: false
---

# Phase 4 Implementation Plan

## Scope (from Architecture_Design_Plan Phase 4)

Focus on Attributes, Secondary Attributes, Equipment, XP, and Class/Profession systems, plus their UI and integration.

## Deliverables Mapping

- Primary attributes: `AttributeComponent`, primary attribute data assets
- Secondary attributes: `SecondaryAttributeComponent` w/ formulas + equipment bonuses
- Equipment system: `EquipmentComponent`, `EquipmentItem`, `WeaponItem`, equipment slots + UI
- XP + Class/Profession: `ExperienceComponent`, `ClassComponent`, class/profession data assets, class UI

## Plan Outline

### 1) Attribute System (Primary)

- Add `[d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Components/Progression/AttributeComponent.h](d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Components/Progression/AttributeComponent.h)` + `.cpp`:
- Core attributes (STR, AGI, CON, DEX, INT, WIS)
- Set/Get/Modify, clamping, and change delegates
- Add primary attribute data assets:
- `[d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Progression/Core/PrimaryAttributeDataAsset.h](d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Progression/Core/PrimaryAttributeDataAsset.h)`
- Supporting types in `ProgressionTypes.h`

### 2) Secondary Attributes + Formulas

- Add `[d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Components/Progression/SecondaryAttributeComponent.h](d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Components/Progression/SecondaryAttributeComponent.h)` + `.cpp`:
- Derived stats (HP, Mana, Crit, Armor, MoveSpeed, etc.)
- Recalculate on primary attribute change
- Hook to EquipmentComponent bonuses
- Add `SecondaryAttributeDataAsset` + formula definitions

### 3) Equipment System + Items

- Add `[d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Components/Inventory/EquipmentComponent.h](d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Components/Inventory/EquipmentComponent.h)` + `.cpp`:
- Slot definitions (MainHand, Offhand, Head, Chest, Legs, etc.)
- Equip/Unequip validation, requirements, and events
- Add `EquipmentItem` and `WeaponItem` classes in Items/Equipment
- Extend `ItemDataAsset` to include equipment slot/type + stat bonuses if not already

### 4) Experience + Class/Profession

- Add `[d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Components/Progression/ExperienceComponent.h](d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Components/Progression/ExperienceComponent.h)` + `.cpp`:
- XP accumulation, level thresholds, and events
- Add `[d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Components/Progression/ClassComponent.h](d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/Components/Progression/ClassComponent.h)` + `.cpp`:
- Class/profession selection, bonuses, and requirement checks
- Add class/profession data assets + `ClassDatabase`

### 5) UI (Attributes, Equipment, Class)

- Add UI widgets:
- `[d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/UI/Progression/AttributeWidget.h](d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/UI/Progression/AttributeWidget.h)`
- `[d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/UI/Equipment/EquipmentWidget.h](d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/UI/Equipment/EquipmentWidget.h)`
- `[d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/UI/Equipment/EquipmentSlotWidget.h](d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/UI/Equipment/EquipmentSlotWidget.h)`
- `[d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/UI/Progression/ClassWidget.h](d:/Unreal Projects/ActionRPG/Source/ActionRPG/Public/UI/Progression/ClassWidget.h)`
- Hook widgets to components and update on change delegates

### 6) Integration & Validation

- Wire components onto `AActionRPGPlayerCharacter`
- Ensure equipment bonuses apply to secondary attributes
- Validate equip requirements against primary attributes and class requirements
- Update HUD/Inventory UI to open attribute and equipment panels

### 7) Testing & Docs

- Add quick verification steps for:
- Equip/unequip and stat changes
- XP gain and level-up
- Class selection and bonuses
- Update Phase 4 manual steps doc with UE editor setup

## Files to Create/Update (Expected)

- Components: AttributeComponent, SecondaryAttributeComponent, EquipmentComponent, ExperienceComponent, ClassComponent
- Data Assets: PrimaryAttributeDataAsset, SecondaryAttributeDataAsset, ClassDataAsset, ProfessionDataAsset
- Items: EquipmentItem, WeaponItem, ItemDataAsset extensions
- UI: AttributeWidget, EquipmentWidget, EquipmentSlotWidget, ClassWidget
- Databases: ClassDatabase, AttributeDatabase

## Manual Steps (UE 5.7)

- Create data assets for attributes/classes/professions
- Create widget blueprints for Attribute, Equipment, Class UIs
- Configure equipment slot layout in `WBP_EquipmentWidget`
- Assign input to open Attribute/Equipment/Class panels

## Risks/Notes

- Secondary attribute recalculation should be event-driven to avoid per-frame cost
- Equipment bonuses must be centralized to prevent double-application
