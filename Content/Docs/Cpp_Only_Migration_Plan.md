# C++-Only Migration Plan

**Target:** UE 5.7  
**Status:** Proposed  
**Last Updated:** 2026-10-18

## Goal and boundaries

Replace all project Blueprint classes and Widget Blueprints with native C++ classes and Slate UI. Keep Unreal content assets that are data or art: maps, meshes, materials, textures, sounds, animations, Input Actions/Mapping Contexts, and Data Assets. This is a no-Blueprint-logic target, not a no-assets target.

The final project must contain no required `BP_*`, `WBP_*`, or Animation Blueprint classes. Native classes become the configured game framework defaults, gameplay objects are selected by C++ data/configuration, and Slate owns the runtime HUD and menus.

## UE6 alignment (revised recommendation)

Epic has announced that UE6 will move its gameplay programming model toward Verse and a new Verse-based Scene Graph. Actors and Blueprints will remain in early UE6 releases and are planned to have conversion tools once the new framework is mature. UE6 Early Access is targeted for the end of 2027, and Epic explicitly says the current UE6 development stream and Verse implementation are not ready for general adoption.

Sources: [Epic's UE6 transition announcement](https://www.unrealengine.com/news/the-road-to-ue-6) and [current UE programming guidance](https://dev.epicgames.com/documentation/unreal-engine/coding-in-unreal-engine-blueprint-vs-cplusplus).

**Decision:** use this plan to reach a **C++-first UE5.7 codebase now**, but do not represent it as a direct migration to Verse. It removes fragile Blueprint dependencies and centralizes game rules in code, which makes a later conversion more tractable. It does not eliminate the future need to adapt from Actor/C++ architecture to Verse/Scene Graph.

For that reason, all UI conversion must be isolated behind a native presentation interface. Slate is appropriate when the immediate goal is truly no Widget Blueprints, but Epic has not published a final UE6 UI migration path; do not invest in a broad visual redesign just to anticipate UE6.

## Current dependency inventory

| Area | Existing Blueprint dependency | Native replacement |
| --- | --- | --- |
| Game framework | `BP_ActionRPGGameMode`, `BP_ActionRPGPlayerController`, player/game state/character BPs | Existing `AActionRPG*` classes configured through `DefaultEngine.ini` and constructor defaults |
| Components | `BP_*Component` subclasses | Instantiate/configure the existing native components in owning actor constructors |
| Items and pickups | Item/skill/pickup Blueprint subclasses | Native item/pickup subclasses and data-driven spawn definitions |
| UI | `BP_ActionRPGHUD` and all `WBP_*` assets | Slate `SCompoundWidget` views owned by the native player controller/HUD |
| Animation | `ABP_Unarmed` | Native `UAnimInstance`/animation state solution; treat as a dedicated high-risk workstream |

## Migration rules

- Do not delete or rename a Blueprint until a replacement has been tested in PIE and cooked successfully.
- Do not replace Data Assets with hard-coded constants. Item, skill, class, profession, and attribute Data Assets remain the editable source of game data.
- New gameplay code must not use `BlueprintCallable`, `BlueprintImplementableEvent`, `BlueprintNativeEvent`, `BindWidget`, or Blueprint-assigned `TSubclassOf` properties unless explicitly retained as a temporary compatibility bridge.
- Use `UDeveloperSettings` or C++ `TSoftObjectPtr` configuration for asset references rather than Blueprint defaults.
- Each migration slice requires: compile, PIE smoke test, packaged-development smoke test, and a redirect/reference audit.
- Keep gameplay state and rules data-driven and independent of UI, actor subclasses, and engine-specific event graphs. This is the main preparation for a future Verse/Scene Graph conversion.
- Add an explicit UE6 review gate before each major migration phase; use Epic's published conversion tooling when it becomes available rather than attempting to predict it.

## Phase 0 — Baseline and guardrails

1. Finish the current equipment drag/drop validation before refactoring UI.
2. Create native automation tests for inventory, equipment, skills, experience, and class behavior; these protect existing C++ systems while their presentation layer changes.
3. Record a PIE smoke-test matrix: spawn, move, open each panel, pick up an item, equip/unequip, activate a skill, gain XP, select a class.
4. Add a CI check that fails on new project Blueprint classes or Widget Blueprints after each respective migration milestone.

**Exit criteria:** Current behavior is demonstrated and covered by repeatable tests; no migration starts from an unverified Blueprint behavior.

## Phase 1 — Native game framework and input

1. Configure `/Script/ActionRPG.ActionRPGGameMode`, `ActionRPGPlayerController`, player character, HUD, game state, and player state directly in `Config/DefaultEngine.ini`.
2. Move all defaults currently stored in `BP_ActionRPGGameMode`, `BP_ActionRPGPlayerController`, and `BP_ActionRPGPlayerCharacter` into C++ constructors or a new `UActionRPGProjectSettings` class.
3. Keep Input Action and Input Mapping Context assets, but load/register them from `AActionRPGPlayerController` using native soft references.
4. Replace Blueprint-only component instances with the existing C++ components created by the C++ character/controller constructors.
5. Migrate map GameMode overrides and placed player references to the native classes.

**Exit criteria:** The game starts, possesses the player, accepts input, and opens no framework Blueprint classes.

## Phase 2 — Native gameplay archetypes and pickups

1. Replace the `BP_Skill_*`, `BP_SkillProjectile`, `BP_SkillItem`, `BP_BeastCoreItem`, `BP_SkillStoneItem`, and `BP_WeaponItem` hierarchy with concrete C++ classes only where behavior differs.
2. For variations that differ only in values, move those values to existing `USkillDataAsset` and `UItemDataAsset` assets rather than creating native classes per item.
3. Give `AItemPickupActor` a native, data-driven initialization path: item-data soft reference, quantity, visual asset reference, and collision settings.
4. Replace the `BP_ItemPickup_*` variants with spawned native `AItemPickupActor` instances initialized from Data Assets. Use a migration commandlet/editor utility to update placed map actors safely.
5. Replace test Blueprint actors with native test fixtures or functional-test maps.

**Exit criteria:** Item pickup, inventory, equipment, and skills work without any `BP_*` gameplay class.

## Phase 3 — Replace UMG Widget Blueprints with Slate (optional, C++-only target)

1. Add a `UI/Slate` native layer and build a root `SActionRPGGameHUD` view managed by `AActionRPGPlayerController` or `AActionRPGHUD`.
2. Implement the screens in dependency order:
   - inventory slot/grid and context menu;
   - quick-use bar and unlocked skills;
   - equipment paper doll, stats, and inventory grid;
   - character status and class selection.
3. Replace every `BindWidget` field, `BlueprintNativeEvent`, and Blueprint-assigned widget class with Slate widget ownership, `TSharedPtr`/`SWeakWidget`, and native delegates.
4. Implement drag/drop through Slate `OnDragDetected`, `OnDragOver`, and `OnDrop`; retain the existing component methods as the sole source of inventory/equipment mutations.
5. Replace controller `TSubclassOf<UUserWidget>` properties and `CreateWidget` calls with native panel creation/show/hide methods.
6. Remove `BP_ActionRPGHUD` and all `WBP_*` assets only after every screen has parity in PIE.

**Exit criteria:** No runtime UI depends on `UUserWidget`, `WidgetBlueprint`, or Blueprint bindings; all panels are Slate and event-driven.

**UE6 guardrail:** do not redesign visual behavior during this phase. Keep Slate views thin and backed by stable native view-model/presentation interfaces so the UI can later be adapted if Epic publishes a Verse/Scene Graph UI workflow.

## Phase 4 — Native animation solution

1. Inventory all Animation Blueprints, beginning with `ABP_Unarmed`, and document every state, transition, notify, and blend requirement.
2. Implement a native `UActionRPGAnimInstance` that derives movement/combat state from the pawn and components in `NativeInitializeAnimation`/`NativeUpdateAnimation`.
3. Prefer C++-triggered animation montages for attacks, skills, hit reactions, and death. Decide separately whether locomotion can use a native single-node/blend implementation or needs a custom native animation node.
4. Replace each Animation Blueprint only after visual and gameplay parity testing on the actual meshes.

**Exit criteria:** No project Animation Blueprint is required at runtime. This is the highest-risk phase and should not block Phases 1–3.

## Phase 5 — Cutover and cleanup

1. Run Reference Viewer and asset-registry checks to confirm no map, Data Asset, config entry, or C++ soft reference targets `BP_*`, `WBP_*`, or `ABP_*` project assets.
2. Change Asset Manager scan settings only if they still include Blueprint classes; existing Data Asset scan entries should remain.
3. Package and smoke-test a Development build, then a Shipping build.
4. Make a recoverable source-control checkpoint before deleting obsolete Blueprint assets, then remove them in small, verified batches and fix redirectors in Unreal Editor.
5. Update the documentation index and the project rule to make native C++/Slate the default for all new features.

**Exit criteria:** Clean cook/package, no runtime Blueprint class dependencies, and the smoke-test matrix passes.

## Recommended first implementation slice

Start with **Phase 0 and Phase 1**: native game-framework defaults, native input configuration, and automated verification of the currently active equipment work. This delivers the strongest UE5.7 benefit with the least rework risk.

Only then decide whether to take the equipment UI through the optional Slate slice. If the immediate requirement is strictly no Widget Blueprints, it is the right proof-of-concept. If the objective is only future UE6 alignment, retain the current UMG presentation temporarily and avoid spending on a full Slate rewrite before Epic publishes the UI direction for Scene Graph.

## Explicit non-goals

- Do not remove Data Assets, maps, meshes, material instances, textures, audio, or Input Action assets solely because they are `.uasset` files.
- Do not duplicate game-rule logic in Slate; it must call the existing C++ components.
- Do not bulk-delete content. Blueprint removal happens only after reference and packaged-build validation.
