# Phase 3 Manual Steps - Days 40-41
**Focus:** Quick-Use Bar Skill UI Integration  
**UE Version:** 5.7  
**Last Updated:** 2026-01-21

---

## Overview
This guide updates the existing Quick-Use bar UI to display skills in slots 1-8 while keeping consumables in slots 9-10.

---

## Prerequisites
- Quick-Use bar UI exists (`WBP_QuickUseBarWidget`, `WBP_QuickUseSlotWidget`).
- `SkillComponent` and `SkillManagerComponent` are attached to the player.
- Skills can be assigned to skill bar slots (1-8) via `SkillManagerComponent`.

---

## Step 1: Update `WBP_QuickUseSlotWidget` Layout
1. Open `Content/UI/QuickUse/WBP_QuickUseSlotWidget`.
2. Add a **cooldown overlay** (suggested: `Image` or `ProgressBar`) named `CooldownOverlay`.
3. Ensure the following widgets exist and are named exactly:
   - `SlotBorder`
   - `ItemIcon`
   - `QuantityText`
   - `HotkeyText`
   - `CooldownOverlay`
4. Set `CooldownOverlay` visibility to **Hidden** by default.
5. Save.

---

## Step 2: Update `WBP_QuickUseBarWidget` (If Needed)
1. Open `Content/UI/QuickUse/WBP_QuickUseBarWidget`.
2. Verify `Slot Widget Class` is set to `WBP_QuickUseSlotWidget`.
3. Save.

---

## Step 3: Verify Skill Slot UI Behavior
1. Play In Editor (PIE).
2. Assign a skill to slot 1 in your Skill Manager test flow.
3. Confirm the slot shows:
   - Skill icon
   - Hotkey `1`
4. Trigger the skill and verify:
   - `CooldownOverlay` becomes visible
   - Overlay updates as cooldown decreases

---

## Step 4: Verify Consumable Slots (9-10)
1. Add a consumable item to quick-use slot 9 or 10.
2. Confirm:
   - Item icon and quantity display correctly
   - Skill cooldown overlay does not appear

---

## Step 5: Troubleshooting Checklist
- `CooldownOverlay` exists and is named correctly in `WBP_QuickUseSlotWidget`.
- Skills are assigned to slots 1-8 in `SkillManagerComponent`.
- `SkillComponent` is present on the player and updates cooldowns.
- Quick-Use bar is added to HUD and visible.

---

## Expected Output (PIE)
- Slots 1-8 display skill icons and cooldown overlays.
- Slots 9-10 continue to display consumables with quantities.

---

## Step 6: Create Unlocked Skills Panel
1. Navigate to `Content/UI/Skills/`.
2. Create Widget Blueprint:
   - Parent: `UnlockedSkillsPanelWidget`
   - Name: `WBP_UnlockedSkillsPanel`
3. Open `WBP_UnlockedSkillsPanel` and add:
   - `UniformGridPanel` named `SkillGrid`
4. Set `Entry Widget Class` to `WBP_UnlockedSkillEntry`.
5. Save.

---

## Step 7: Create Unlocked Skill Entry Widget
1. Navigate to `Content/UI/Skills/`.
2. Create Widget Blueprint:
   - Parent: `UnlockedSkillEntryWidget`
   - Name: `WBP_UnlockedSkillEntry`
3. Add widgets and name them:
   - `SkillIcon` (Image)
   - `SkillNameText` (Text)
   - `AssignButton` (Button)
4. Save.

---

## Step 8: Add Panel to UI
1. Open `WBP_InventoryWidget` (or your HUD widget).
2. Add `WBP_UnlockedSkillsPanel` to the layout.
3. Position as desired (side panel or tab).
4. Save.

---

## Step 9: Verify Assignment Flow
1. PIE.
2. Unlock a skill (use SkillItem).
3. Confirm it appears in the Unlocked Skills panel.
4. Click Assign and confirm:
   - Skill is assigned to a Quick-Use slot (first empty slot)
   - Quick-Use bar updates with icon and cooldown overlay
