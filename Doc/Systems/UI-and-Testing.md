# UI, input, and verification

## Panels

`AActionRPGPlayerController` binds the project Enhanced Input actions and opens the Inventory, Equipment, Skills, Character Status, Class, and Crafting panels. Check the live input mapping asset/Blueprint before changing key bindings. The current MCP-inspected mapping uses **C** for Character Status, **B** for Classes & Professions, **K** for Skills, **U** for Equipment, **R** for Crafting, mouse buttons for assigned main/offhand skills, and 1–8 for skill slots. The 9/10 quick-use paths are handled separately. Panel close buttons and draggable handles are implemented in UMG/C++.

| View | Main content | Code |
| --- | --- | --- |
| HUD | Vitals, quick-use bar, active effects, trial tracker, quest tracker, death overlay | [HUD](../../Source/ActionRPG/Private/UI/HUD/ActionRPGHUD.cpp), [vitals](../../Source/ActionRPG/Private/UI/HUD/VitalBarsWidget.cpp), [quest tracker](../../Source/ActionRPG/Private/UI/Quests/QuestTrackerWidget.cpp) |
| Character Status | Overview, Training, Sources, Effects; combined level, acquired Endurance/cap, and separate point balances | [status widget](../../Source/ActionRPG/Private/UI/Progression/CharacterStatusPanelWidget.cpp) |
| Class | Current/available class and profession paths, XP allocation, requirements, and skill grant preview | [class panel](../../Source/ActionRPG/Private/UI/Progression/ClassSelectionPanelWidget.cpp) |
| Skills | Known skill levels/use XP, Learn catalog, assignment actions | [skills panel](../../Source/ActionRPG/Private/UI/Skills/UnlockedSkillsPanelWidget.cpp) |
| Inventory and Equipment | Carried slots, context menu, paper doll, weight, combat breakdown | [inventory](../../Source/ActionRPG/Private/UI/Inventory/InventoryWidget.cpp), [equipment](../../Source/ActionRPG/Private/UI/Equipment/EquipmentWidget.cpp) |
| Crafting | Known recipes, ingredients, output, availability | [crafting](../../Source/ActionRPG/Private/UI/Crafting/CraftingPanelWidget.cpp) |

The trial tracker keeps a completed choice trial active until the player selects one of its two reward buttons. It displays both packages before selection and shows the awarded package after completion. It also previews fixed acquired Endurance rewards.

The same tracker now shows a **Journey** line for the single-session [prototype route](Prototype-Playthrough.md). Trial, class, quest, inventory, and equipment delegates advance it through First Steps, class level 1, Training Combat, Forgekeeper's Request, and Trialbound Sword rank 1. The line is guidance; the underlying gameplay systems remain the source of progress and rewards.

The separate top-right quest card appears after a quest is accepted. It shows required carried-item progress, optional enemy progress, the base/optional rewards, turn-in feedback, and completion. The quest marker text faces the current camera angle like Trial markers.

Most panels read their owning pawn's components and subscribe to change delegates. The Character Status screen is assembled in native code; do not assume a missing `WBP_CharacterStatusPanel` asset means the feature is absent. The current `/Game/UI` assets include the Inventory, Equipment, Classes, Skills, and QuickUse widgets. `UDraggablePanelHandleWidget` supplies panel movement.

## Verification workflow

1. Through Unreal MCP, confirm the editor is connected, PIE state, relevant `/Game` asset paths/defaults, and recent Output Log entries.
2. For a C++ change, use Live Coding if the connected editor offers it. Otherwise close the editor cleanly after saving dirty content, run a normal editor build, reopen it, and check compile/Blueprint errors.
3. Run the focused `ActionRPG.*` automation tests for changed behavior through MCP. The test sources live in `Source/ActionRPG/Private/Tests` and cover progression, training, trials, combat, inventory/crafting, and UI integration.
4. Test the visible action in PIE. Use [the detailed combat script](../../Content/Docs/PIE_Combat_Testing.md) for the six training routes, effects, resource recovery, and panels.

PIE console helpers include `RPGTrainingStatus`, `RPGTestDamageSelf 30`, `RPGTestGrantSkillPoints 3`, and `RPGTestGrantXP 225`. They affect only the current PIE session. The current [manual test queue](PIE-Test-Queue.md) gives the exact checks for the recent progression and trial changes.

## Current boundaries

- Character progression, inventory, and UI state are session runtime state; save/load is deferred.
- Melee damage and cooldown now scale with effective Strength and Agility using tunable fields on the skill data asset. These coefficients are game balance values, not book formulas.
- Content rates, thresholds, cooldowns, and damage formulas are project balance values. Record book evidence before calling a specific value a rule from *The Infinite World*.
- The old implementation plans in `Content/Docs` are historical. Start at this folder and verify code/assets when a detail matters.
