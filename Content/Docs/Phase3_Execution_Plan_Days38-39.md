# Phase 3: Execution Plan - Days 38-39
**Focus:** Skill-Granting Items (SkillItem, SkillStone, BeastCore)  
**UE Version:** 5.7  
**Last Updated:** 2026-01-18

---

## Goals
- Implement skill-granting consumable items that unlock skills via `SkillManagerComponent`.
- Integrate items with inventory use flow and item data assets.
- Provide test items and validate unlock behavior in editor.

---

## Scope
**In-scope:**
- `USkillItem`, `USkillStoneItem`, `UBeastCoreItem` (C++ classes)
- Item Data Assets for skill-granting items
- Inventory integration (use → unlock skill → consume item)
- Basic prerequisite checks for BeastCore

**Out of scope (later phases):**
- Attribute system implementation
- Save/Load persistence
- Advanced UI feedback beyond logs

---

## Execution Checklist

### 1) C++ Class Creation
- Create item classes:
  - `USkillItem` (inherits from `UConsumableItem` or `UItemBase`)
  - `USkillStoneItem`
  - `UBeastCoreItem`
- Add required includes and forward declarations:
  - `SkillManagerComponent`
  - `SkillDataAsset`
  - `InventoryComponent` (if needed for consume flow)

### 2) Class Responsibilities
- **USkillItem**
  - Holds `USkillDataAsset* SkillData` reference
  - `Use()` calls `SkillManagerComponent::UnlockSkill()`
  - Consumes item on success
- **USkillStoneItem**
  - Same as SkillItem, but skips requirements (always unlocks)
- **UBeastCoreItem**
  - Holds `USkillDataAsset* SkillData`
  - Holds requirements (attributes, class affinity placeholder)
  - `Use()` validates requirements, then unlocks

### 3) Inventory Integration
- Ensure `Use()` can access:
  - Owner character
  - `SkillManagerComponent`
  - `InventoryComponent` for removal
- Add logging for:
  - Missing components
  - Already-unlocked skill
  - Requirements not met (BeastCore)

### 4) Data Assets
- Create example item data assets:
  - `DA_Item_Skill_Fireball` (SkillItem)
  - `DA_Item_SkillStone_Fireball` (SkillStoneItem)
  - `DA_Item_BeastCore_Wolf` (BeastCoreItem)
- Assign `SkillData` references and basic item fields (name, icon, description).

### 5) Testing
- Give item to player inventory
- Use item
- Verify:
  - Skill unlocked
  - Skill appears in skill manager
  - Item removed from inventory
- Verify failure paths (BeastCore requirements unmet)

---

## Files to Create / Update

**New C++ Files**
- `Source/ActionRPG/Public/Items/SkillItem.h`
- `Source/ActionRPG/Private/Items/SkillItem.cpp`
- `Source/ActionRPG/Public/Items/SkillStoneItem.h`
- `Source/ActionRPG/Private/Items/SkillStoneItem.cpp`
- `Source/ActionRPG/Public/Items/BeastCoreItem.h`
- `Source/ActionRPG/Private/Items/BeastCoreItem.cpp`

**Potential Updates**
- `ItemDatabase` registrations (if required by current pattern)
- `ItemBase` or `ConsumableItem` includes for skill-related fields

---

## Deliverables
- Skill-granting item classes compile and are Blueprint-ready.
- Data assets created for each item type.
- Skill unlock flow works via item use.
- Inventory consumes items correctly.

---

## Manual Steps (UE 5.7)
Use `Content/Docs/Phase3_Manual_Steps_Days38-39.md` for editor setup and testing.

