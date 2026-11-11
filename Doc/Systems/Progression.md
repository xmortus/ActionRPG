# Progression and primary attributes

## Attribute authority

`EPrimaryAttribute` contains Strength, Agility, Constitution, Dexterity, Intelligence, and Wisdom. `UAttributeComponent` combines base values from `/Game/Data/Attributes/DA_PrimaryAttributes_Base`, allocated points, class/profession growth, training, equipment, and temporary modifier sources. `SetModifierSource(SourceId, Bonuses, bPersistent)` replaces a named source; `RemoveModifierSource` reverses only that source. `GetAttribute` is the effective value. `GetPersistentAttribute` excludes temporary/equipment gains for requirement checks. `GetAttributeBreakdown` feeds the Sources tab and hover explanations.

Free attribute points live here. Allocation changes an attribute permanently for the current session. Sources are recalculated and `OnPrimaryAttributeChanged` informs secondary stats and UI. See [attribute implementation](../../Source/ActionRPG/Private/Components/Progression/AttributeComponent.cpp).

## XP, class, and profession paths

`UExperienceComponent` tracks both **total earned XP** and **unallocated XP**. `AddExperience` raises both; `SpendExperience` reduces the unallocated pool. `UClassComponent` spends that pool to level a chosen class or profession. Class XP requirements can use the class's experience curve; profession leveling uses its configured progression values. Selected paths apply fixed, data-defined attribute bonuses. Class tiers also grant free attribute points for each newly reached class level: Basic and Specialized grant 4; Advanced grants 5. The award bookkeeping avoids granting the same class levels twice. Professions grant fixed attribute growth and can grant a profession skill, but no free attribute points.

Classes can also define optional `SkillLevelRequirements` for a target class level. Assigned XP remains banked when the XP threshold is reached but a required skill level is missing. A skill that reaches the milestone through use triggers class advancement automatically; the normal fixed gains and free points are awarded then. The progression panel shows the missing skill and its level. This is a game design rule inspired by class advancement requirements in the book, not a claim that every class uses the same milestone.

Class assets can define `GrantedSkills` with a target class level, skill class, and skill data. A selected class grants each eligible skill without spending free skill points; the class panel previews the grant. Removing the class revokes only its owned skill instance and clears its assignment. Reselecting the class restores the same instance and its use-earned skill rank/XP for this session. If another source already unlocked the skill, the class does not replace that instance; revoking the independent source while the class remains selected restores the class grant. Abilities remain a separate design concept from ordinary skills.

A requirement for class level 1 also gates first selection or adding that class. Reaching the skill level through use then permits selection; no existing class currently uses this entry gate.

`GetPlayerLevel()` is **Combined Level**: the sum of selected class and profession levels. It is 0 with no selected paths. Level 1 paths are starting choices and can be selected at Combined Level 0. Higher required levels and persistent attribute requirements still gate selection. A profession must have earned its first XP level before selection. Selecting/removing a profession grants/revokes its profession skill. Class and profession progress are distinct from the combined display.

Current content: `/Game/Data/Classes/DA_Class_Warrior`, `DA_Class_Mage`; `/Game/Data/Professions/DA_Profession_Blacksmith`. The character starts with Melee Attack independently of class selection, so Warrior no longer lists it as a level-one grant. Mage grants Fireball at class level 1; Fireball is not known at startup. Warrior's third class level still requires Melee Attack skill level 2. Source: [XP](../../Source/ActionRPG/Private/Components/Progression/ExperienceComponent.cpp), [class/profession](../../Source/ActionRPG/Private/Components/Progression/ClassComponent.cpp), [class data](../../Source/ActionRPG/Public/Progression/Core/ClassDataAsset.h), [profession data](../../Source/ActionRPG/Public/Progression/Core/ProfessionDataAsset.h).

## Acquired Endurance

`UAcquiredAttributeComponent` tracks earned Endurance separately from six primary attributes and calculated secondary stats. It starts at 0. Its cap is the character's **persistent Constitution**, so temporary or equipment Constitution does not increase the cap. Raising persistent Constitution raises capacity; it does not grant Endurance. The Character Status Overview and Sources tabs display earned Endurance and its cap.

The current acquisition path is the Training Combat Trial's +1 Endurance reward. `/Game/Data/Attributes/DA_SecondaryAttributes_Base` adds 5 maximum Health, 2 maximum Stamina, and 5 Stamina per second of passive regeneration for each earned Endurance. The starting regeneration rate is 20 per second, so Endurance 1 restores 25 per second after the existing 1-second delay. The rate updates when Endurance changes, including during an active recovery timer. Existing current Health and Stamina are preserved when their maximums rise. An Endurance reward that would exceed the cap remains pending rather than paying a partial reward. Book 1 describes Endurance increasing Health and Stamina and helping Stamina refill quickly (chapters 15 and 21); the cap implementation, Trial placement, and numeric coefficients are project design choices. Source: [acquired attributes](../../Source/ActionRPG/Private/Components/Progression/AcquiredAttributeComponent.cpp), [derived stats](../../Source/ActionRPG/Private/Components/Progression/SecondaryAttributeComponent.cpp), [regeneration tuning](../../Source/ActionRPG/Public/Progression/Core/SecondaryAttributeDataAsset.h).

## Practice growth

`UTrainingComponent` counts accepted actions per attribute/activity. At the threshold it adds permanent growth through a `Training.<Attribute>` source and resets that practice counter; earned growth remains. Rules can enforce an activity cooldown, same-target cooldown, and per-window cap. Defaults are 20 successful melee hits for Strength and 12 for each other listed path; these are tunable game values.

| Attribute | Accepted activity |
| --- | --- |
| Strength | Damaging melee hit (`MeleeHit`) |
| Agility | Successful combat dodge (`CombatDodge`) |
| Constitution | Surviving a meaningful enemy hit (`SurviveEnemyHit`) |
| Dexterity | Damaging ranged projectile hit (`RangedHit`) |
| Intelligence | Damaging magic hit, including valid splash targets (`MagicHit`) |
| Wisdom | Effective healing by a player-sourced skill effect (`EffectiveSkillHeal`) |

The Training tab shows current practice and total earned gain. Rules are [defined here](../../Source/ActionRPG/Private/Components/Progression/TrainingComponent.cpp). Practice does not award free attribute points.
