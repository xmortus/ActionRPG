# ActionRPG systems reference

Last reviewed: 2026-11-10 against `Source/ActionRPG`, project configuration, and the live Unreal Engine 5.8 editor through Unreal MCP. The system guides document **implemented behavior**; the separate book evidence note summarizes the supplied novel. `/Game` paths are Unreal asset paths; source links are relative to this file. Older plans in `Content/Docs` may describe intended or superseded behavior.

## Start here

| System | Guide | Runtime owner |
| --- | --- | --- |
| Project structure and event flow | [Architecture](Architecture.md) | Player character, controller, components, HUD |
| Movement, camera, world actors, data lookup | [World and Interaction](World-and-Interaction.md) | Player character/controller, pickups, databases |
| Primary and acquired attributes, XP, classes, professions, training | [Progression](Progression.md) | `UAttributeComponent`, `UAcquiredAttributeComponent`, `UExperienceComponent`, `UClassComponent`, `UTrainingComponent` |
| Derived stats, resources, damage, death, effects | [Combat and Resources](Combat-and-Resources.md) | `USecondaryAttributeComponent`, characters, `UStatusEffectComponent` |
| Learning, assigning, using, and leveling skills | [Skills](Skills.md) | `USkillManagerComponent`, `USkillComponent` |
| Pickup, inventory, equipment, consumables, quick use | [Items and Equipment](Items-and-Equipment.md) | `UInventoryComponent`, `UEquipmentComponent` |
| Ordinary quests and a named growing sword | [Quests and Growing Equipment](Quests-and-Growing-Equipment.md) | `UQuestComponent`, `UWeaponItem` |
| Trials and crafting | [Trials and Crafting](Trials-and-Crafting.md) | `UTrialComponent`, `UInventoryComponent`, crafting skill |
| Player UI and PIE checks | [UI and Testing](UI-and-Testing.md) | Player controller and UMG widgets |
| Manual checks awaiting the player | [PIE Test Queue](PIE-Test-Queue.md) | Two hover overlays, continuous prototype journey, packaged visual run |
| Book 1 source evidence for RPG mechanics | [The Infinite World: Book 1 RPG Evidence](The-Infinite-World-Book-1-RPG-Evidence.md) | Chapter references, project alignment, and differences |
| Next implementation order | [Next Systems Work](Next-Work.md) | Acceptance checks and content-backed book-inspired features |
| Single-session prototype route | [Prototype Playthrough](Prototype-Playthrough.md) | First Steps, class, combat, Forgekeeper, growing sword, completion guidance |

## Project rules for future changes

- This is an RPG inspired by J. T. Wright's *The Infinite World*. The six attributes, class/profession paths, practice, skills, and trials are the design pillars. Exact rates and formulas in this project are **game design** unless a book passage is recorded.
- Use Unreal MCP before changing editor assets: check editor/PIE state, actual asset paths, and recent Output Log entries. Save affected assets and inspect the result. Keep core gameplay in C++ and expose content parameters through data assets and Blueprints.
- Primary attributes come from `UAttributeComponent`; acquired Endurance comes from `UAcquiredAttributeComponent`; current and maximum resources come from `USecondaryAttributeComponent`. Free attribute points and free skill points are different currencies.
- Persistence remains deferred by user request. Current session state should not be described as saved character progression.
- `Source/ActionRPG/Private/Tests` contains automation coverage. Do not read or edit `Intermediate`, `Binaries`, `Saved`, `DerivedDataCache`, or `.vs` for project analysis.

## Useful entry points

- [Player character](../../Source/ActionRPG/Private/Characters/ActionRPGPlayerCharacter.cpp), [player controller](../../Source/ActionRPG/Private/Core/ActionRPGPlayerController.cpp), [HUD](../../Source/ActionRPG/Private/UI/HUD/ActionRPGHUD.cpp)
- [Current proposal](../../Content/Docs/Proposed%20changes.md) records decisions and remaining design work; [PIE combat checks](../../Content/Docs/PIE_Combat_Testing.md) has a longer playtest script.
- Current map: `/Game/NewMap`. The data asset families are under `/Game/Data/Attributes`, `/Game/Data/Classes`, `/Game/Data/Professions`, `/Game/Data/Skills`, `/Game/Data/Items`, `/Game/Data/Effects`, `/Game/Data/Trials`, and `/Game/Data/Recipes`.
