# Phase 4 Execution Plan - Days 45-48
**Focus:** Equipment System Foundation  
**UE Version:** 5.7  
**Last Updated:** 2026-01-22

---

## Overview
Days 45-48 establish the equipment system: a component for slot management, equipment item classes, data assets for equipment definitions, and integration of equipment bonuses into secondary attributes.

---

## Goals
- Implement `EquipmentComponent` with slot management and events
- Add equipment item classes (`EquipmentItem`, `WeaponItem`)
- Extend `ItemDataAsset` to support equipment slots and stat bonuses (if not present)
- Aggregate equipment bonuses into `SecondaryAttributeComponent`
- Build equipment UI layout (paper-doll + stats + inventory grid)

---

## Execution Tasks (C++)

### 1) EquipmentComponent (Core Slot System)
1. Create `Source/ActionRPG/Public/Components/Inventory/EquipmentComponent.h`
2. Create `Source/ActionRPG/Private/Components/Inventory/EquipmentComponent.cpp`
3. Implement:
   - Equipment slots enum (MainHand, Offhand, Head, Chest, Legs, Hands, Feet, Ring, Amulet, etc.)
   - Equip/Unequip, swap validation, and events
   - Slot queries and current equipment map

### 2) Equipment Items
1. Create `Source/ActionRPG/Public/Items/Equipment/EquipmentItem.h/.cpp`
2. Create `Source/ActionRPG/Public/Items/Equipment/WeaponItem.h/.cpp`
3. Implement:
   - Slot type requirements
   - Stat bonus definitions
   - Optional equip requirements (level, primary attributes)

### 3) ItemDataAsset Extensions (Equipment Data)
1. Extend `UItemDataAsset` if needed:
   - Equipment slot type
   - Stat bonus map (e.g., `TMap<ESecondaryAttribute, float>`)
   - Optional equip requirements

### 4) Secondary Attribute Integration
1. Add aggregation path for equipment bonuses:
   - Collect bonuses from `EquipmentComponent`
   - Apply additive bonuses in `SecondaryAttributeComponent::RecalculateSecondaryAttributes`

### 5) Character Integration
1. Attach `EquipmentComponent` to `AActionRPGPlayerCharacter`
2. Ensure equip/unequip triggers secondary attribute recalculation

### 6) Equipment UI (Paper-Doll + Inventory)
1. Create widgets:
   - `WBP_EquipmentWidget` (overall layout)
   - `WBP_EquipmentSlot` (paper-doll slot)
   - `WBP_EquipmentStats` (stats panel)
   - `WBP_EquipmentInventory` (inventory grid + filters)
2. Layout targets:
   - Left: equipment paper-doll slots (head, chest, legs, hands, feet, main/offhand, ring, amulet)
   - Center: stats panel (primary + secondary)
   - Right: inventory grid with icons/stacks
   - Bottom: category filters (Armor, Weapons, Food & Potions, Quest Items, All Items)
3. Bind to `EquipmentComponent` and `InventoryComponent`
4. Wire drag & drop between inventory and equipment slots

---

## Verification Checklist
- EquipmentComponent compiles and is Blueprint-accessible
- Equipment items can be equipped into valid slots
- Equipment bonuses affect secondary attributes
- Equip/Unequip events fire and update UI safely
- Equipment UI displays slots, stats, and inventory grid

---

## Notes
- Keep equipment bonus aggregation centralized to avoid double application.
- Equip/Unequip should be event-driven (no per-frame polling).
