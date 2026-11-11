# Next systems work

This note separates tested implementation from book-inspired work that still needs design or content. See [book 1 evidence](The-Infinite-World-Book-1-RPG-Evidence.md) for chapter references and [the PIE test queue](PIE-Test-Queue.md) for current player checks. Character persistence remains deferred by request.

## PIE verification status

The current PIE queue's core gameplay and UI checks passed through Unreal MCP. The two visual follow-ups are the Dash Agility milestone tooltip and the grown Trialbound Sword tooltip; their underlying text was verified, but MCP did not expose the hover overlays. See items 7 and 12 in the test queue. The full `ActionRPG.*` suite passed **50/50** after the skill-stone transaction fix and five-stage Journey route. A continuous player-controlled run remains in item 13.

## This slice implemented

The named Trialbound Sword grows through credited defeats while equipped. Forgekeeper's Request is an ordinary quest with a required ore objective, optional enemy objective, separate completion history, and a distinct reward. See [Quests and Growing Equipment](Quests-and-Growing-Equipment.md). Live PIE verified both quest routes, the sword's first rank/damage increase, retention across unequip/re-equip, and an ordinary sword comparison.

The [Prototype Playthrough](Prototype-Playthrough.md) now guides a single session through First Steps, class level 1, Training Combat, Forgekeeper's Request, and Trialbound Sword rank 1. Unreal MCP completed all five transitions in PIE. The Windows game target built, and the updated package cooked, archived, and passed a headless startup smoke test. A visual standalone run remains.

Class-granted skills and acquired Endurance are implemented. Basic Melee Attack is now a character starting skill, while Mage grants Fireball at class level 1. Warrior retains its level-three Melee Attack skill gate. Training Combat grants one Endurance, subject to the persistent Constitution cap. See [Progression](Progression.md) and [Trials and Crafting](Trials-and-Crafting.md). Earlier Warrior grant/removal/restoration and Training Combat Endurance tests passed under the previous starter setup; the current starter state passed fresh live PIE.

The book also shows spontaneous skill learning through observation and practice, and classes that can specialize or change. Those need specific playable interactions and content before implementation. The project's free skill points currently buy new eligible skills only; this remains a stated game-design difference from book 1, chapter 28.
