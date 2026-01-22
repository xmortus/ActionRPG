# Phase 3: Execution Plan - Days 40-41
**Focus:** Quick-Use Bar Skill UI Integration + Unlocked Skills Panel  
**UE Version:** 5.7  
**Last Updated:** 2026-01-21

---

## Goals
- Extend the existing Quick-Use bar UI to display skills in slots 1-8.
- Show skill icons, cooldown overlays, and hotkey labels on the same bar.
- Keep consumable behavior for slots 9-10.
- Provide an Unlocked Skills panel to assign skills to Quick-Use slots.

---

## Scope
**In-scope:**
- `UQuickUseBarWidget` and `UQuickUseSlotWidget` UI logic updates
- Skill slot visuals (icon, cooldown overlay, readiness state)
- Event bindings to `USkillManagerComponent` / `USkillComponent`
- Unlocked Skills panel UI for assigning skills to slots 1-8

**Out of scope (later phases):**
- Save/load of slot assignments
- Advanced animations and audio feedback
- Skill tooltips with detailed stats

---

## Execution Checklist

### 1) UI Data Sources
- Ensure `UQuickUseBarWidget` can access:
  - `USkillManagerComponent` (for slot assignments)
  - `USkillComponent` (for cooldown data)
- Decide source of skill icon (use `USkillDataAsset::SkillIcon`).

### 2) Slot Rendering Rules
- Slots 1-8 (indices 0-7):
  - Display assigned skill icon
  - Show cooldown overlay based on remaining / duration
  - Show hotkey label (1-8)
- Slots 9-10 (indices 8-9):
  - Keep existing consumable behavior
  - Display item icon and quantity

### 3) QuickUseSlotWidget Behavior
- If slot type is Skill:
  - Use skill icon, cooldown overlay, and ready state
  - Disable quantity text (or hide)
- If slot type is Consumable:
  - Use existing item icon + quantity behavior

### 4) Event Integration
- Bind to:
  - `USkillManagerComponent::OnSkillSlotChanged`
  - `USkillManagerComponent::OnSkillSlotCleared`
  - `USkillManagerComponent::OnSkillUnlocked` (optional refresh)
- Refresh relevant slots on events.

### 5) Unlocked Skills Panel
- Create a simple list/grid UI showing unlocked skills.
- Add click or drag behavior to assign a selected skill to a Quick-Use slot (1-8).
- Panel can live inside Inventory UI or as a standalone widget.

### 6) Testing
- Assign skills to slots 1-8 and verify UI updates.
- Trigger cooldowns and confirm overlay updates.
- Ensure consumable slots 9-10 still work as before.
- Verify Unlocked Skills panel shows newly unlocked skills and can assign to slots.

---

## Files to Update
- `Source/ActionRPG/Public/UI/QuickUse/QuickUseBarWidget.h`
- `Source/ActionRPG/Private/UI/QuickUse/QuickUseBarWidget.cpp`
- `Source/ActionRPG/Public/UI/QuickUse/QuickUseSlotWidget.h`
- `Source/ActionRPG/Private/UI/QuickUse/QuickUseSlotWidget.cpp`
- (Blueprints) `WBP_QuickUseBarWidget`, `WBP_QuickUseSlotWidget`
- (Blueprints) `WBP_UnlockedSkillsPanel` (new)

---

## Deliverables
- Quick-Use bar displays skills in slots 1-8 with icons and cooldowns.
- Consumable slots 9-10 unchanged and functional.
- UI updates when skills are assigned/cleared.
- Unlocked Skills panel supports assigning skills to slots.

---

## Manual Steps (UE 5.7)
### 1) Update Quick-Use Slot Widget
- Open `Content/UI/QuickUse/WBP_QuickUseSlotWidget`.
- Add a cooldown overlay widget (Image or ProgressBar) named `CooldownOverlay`.
- Ensure existing widget names match:
  - `SlotBorder`, `ItemIcon`, `QuantityText`, `HotkeyText`, `CooldownOverlay`
- Save.

### 2) Verify Quick-Use Bar Widget
- Open `Content/UI/QuickUse/WBP_QuickUseBarWidget`.
- Confirm `Slot Widget Class` is `WBP_QuickUseSlotWidget`.
- Save.

### 3) Create Unlocked Skills Panel
- Create `WBP_UnlockedSkillsPanel` (parent: `UserWidget`).
- Add a `ScrollBox` or `UniformGridPanel` for skill entries.
- Create a `WBP_UnlockedSkillEntry` widget with:
  - `SkillIcon` (Image)
  - `SkillName` (Text)
  - Optional `Assign` button or drag handler
- Save.

### 4) Hook Panel into UI
- Add the panel to the Inventory UI or HUD:
  - If Inventory: open `WBP_InventoryWidget` and add panel to layout.
  - If HUD: open `BP_ActionRPGHUD` and add widget on BeginPlay.
- Save.

### 5) Wire Panel to SkillManagerComponent
- In the panel blueprint:
  - Get `SkillManagerComponent` from the player.
  - Populate entries from `GetUnlockedSkills()`.
  - Bind to `OnSkillUnlocked` to refresh list.

### 6) Assignment Behavior
- Add click/drag behavior so selecting an unlocked skill:
  - Calls `AssignSkillToSlot(SlotIndex, Skill)` on `SkillManagerComponent`.
  - Refreshes the Quick-Use bar.

### 7) PIE Verification
- Unlock a skill (use SkillItem).
- Confirm it appears in Unlocked Skills panel.
- Assign it to a slot and verify:
  - Slot shows skill icon and cooldown overlay.
  - Hotkey label is correct.
