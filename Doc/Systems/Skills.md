# Skills and free skill points

## Two responsibilities

`USkillManagerComponent` owns unlocked skills, a **free skill point** balance, catalog purchase rules, eight skill-bar assignments, and separate main-hand/offhand assignments. `USkillComponent` owns runtime skill instances: levels, use XP, cooldowns, and activation. A skill is a `USkillBase` instance linked to a `USkillDataAsset` definition. Current examples include Melee Attack, Ranged Attack, Fireball, Dash, Minor/Major Regeneration, and Blacksmith Crafting under `/Game/Data/Skills`.

## Learning and assigning

The Skills panel has **Known** and **Learn** tabs. The Learn catalog charges the skill's `SkillPointCost` only when Combined Level, persistent attributes, selected class/class level, and prerequisite skill IDs are satisfied. Level 1 is an entry gate. Skill stones unlock their named skill without the catalog's prerequisites or a free skill point charge. Live PIE verified a stone granting its skill and being consumed. Class, profession, and trial grants also unlock without spending free skill points. The character starts with Melee Attack known and assigned to the main hand. Fireball and Minor Regeneration are not starting known skills; Fireball can be learned from its world stone or granted by Mage at class level 1. The Melee Attack stone asset remains in content but has no pickup in the prototype level. The point balance is independent of free attribute points.

Known skills can be assigned to skill-bar slots 1–8 or main/offhand actions. The quick-use bar reflects assignments. Removing/revoking a skill clears its assignments. Right-click/context actions and drag/drop provide assignment paths in the UI. Sources: [manager](../../Source/ActionRPG/Private/Components/Skills/SkillManagerComponent.cpp), [Skills panel](../../Source/ActionRPG/Private/UI/Skills/UnlockedSkillsPanelWidget.cpp), [skill definition](../../Source/ActionRPG/Public/Skills/Core/SkillDataAsset.h).

## Activation and growth

The controller asks the manager to activate an assigned skill. `USkillComponent` checks unlock, cooldown, skill-specific validity, and resource availability. It reserves mana/stamina, calls the implementation, refunds a failed activation, and on success starts cooldown, grants skill use XP, broadcasts activation, and reports skill use to an active skill trial. Damaging hits can grant additional hit XP. Skills gain levels **through use XP**; free skill points are for choosing new skills, never for buying skill levels. The Known tab displays each skill's level and progress to the next level.

Use XP grants must be positive and finite, including after the component's experience multiplier. Level growth stops at the integer level limit; invalid values cannot enter the level-up loop.

Cooldown ticking is localized to `USkillComponent`; it does not imply other character systems tick. Source: [skill runtime](../../Source/ActionRPG/Private/Components/Skills/SkillComponent.cpp), [base](../../Source/ActionRPG/Private/Skills/Core/SkillBase.cpp), [quick-use bar](../../Source/ActionRPG/Private/UI/QuickUse/QuickUseBarWidget.cpp).

Melee Attack, Ranged Attack, and Fireball use a configurable damage multiplier from their use-based skill level. Their current assets grant 3% more final attack damage per level above one, capped at 2×. Equipment refreshes its damage number when the assigned main-hand skill levels up. The percentage and cap are game balance, not a book-wide skill rule.

Dash has a gameplay benefit from its use-based levels. Each level above one increases launch speed by the skill asset's `DashSpeedPercentPerLevel`, capped by `MaxDashSpeedMultiplier`, and reduces its stamina cost by `DashStaminaReductionPerLevel` down to `MinDashStaminaCost`. The current Dash asset starts at 1200 speed and 10 stamina; its new default tuning is 5% speed and 0.5 less stamina per level, capped at twice the base speed and a 5-stamina floor. The Known Skills tooltip shows its current speed and stamina cost and uses each skill's effective cooldown. These rates are game balance; the book supports changing Dash speed and cost as it levels but does not establish these coefficients. The separate dodge action is unaffected. Source: [Dash implementation](../../Source/ActionRPG/Private/Skills/Implementations/SkillDash.cpp), [skill data](../../Source/ActionRPG/Public/Skills/Core/SkillDataAsset.h), [skill entry](../../Source/ActionRPG/Private/UI/Skills/UnlockedSkillEntryWidget.cpp).

`AttributeMilestones` optionally grants permanent primary attribute growth at a specified use-based skill level. Each milestone uses a stable source ID, so removing and regranting a skill cannot award the same growth twice. Dash currently grants +1 Agility at skill level 2; ordinary skills have no attribute milestone. Known and Learn tooltips show configured rewards. This rare award is inspired by Dash's unusual growth in book 1, chapter 20.
