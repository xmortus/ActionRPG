# Trials and crafting

## Trials

`UTrialDataAsset` defines a stable trial ID, type (Combat, Skill, Exploration, Crafting), objectives with IDs and target counts, entry requirements, repeatability, and rewards. Current data lives in `/Game/Data/Trials`: `DA_Trial_FirstSteps`, `DA_Trial_TrainingCombat`, `DA_Trial_DashPractice`, and `DA_Trial_ForgeIronSword`. `/Game/NewMap` contains their start/goal markers and training enemy. `ATrialTriggerActor` connects world interaction to the trial component.

`UTrialComponent` permits one active trial. Start checks required Combined Level (level 1 can be an entry point), prerequisite trials, and repeatability. Gameplay reports named events: skill activation, enemy defeat, recipe craft, or direct objective advancement by a marker. Counts are capped at target counts. The tracker listens for start/progress/completion. A combat enemy is credited once per active trial, avoiding duplicate kill credit from its respawn.

Completion validates configured rewards before changing state: XP, item quantity/capacity, free attribute points, free skill points, optional skill grant, and optional acquired Endurance. Rewards update their respective owners; attribute and skill point balances stay separate. An Endurance reward exceeding the character's persistent Constitution cap blocks payment and stays pending for retry after the cap is raised. Trial records track completions/failures, and repeatable trials can be started again. Source: [trial component](../../Source/ActionRPG/Private/Components/Progression/TrialComponent.cpp), [trial definition](../../Source/ActionRPG/Public/Trials/TrialDataAsset.h), [trigger](../../Source/ActionRPG/Private/Trials/TrialTriggerActor.cpp), [tracker](../../Source/ActionRPG/Private/UI/Trials/TrialTrackerWidget.cpp).

A trial may instead define exactly two mutually exclusive `RewardChoices`. Its objective can finish while the trial remains active; the tracker shows both packages and pays the selected one only after a choice is clicked. An invalid or blocked choice leaves the reward pending for retry or another selection. Fixed reward fields are ignored for a choice trial. `DA_Trial_FirstSteps` now offers **Growth** (100 XP and 2 free attribute points) or **Learning** (100 XP and 2 free skill points). This is a small game example of the book's alternative Trial rewards; the book's final chests have different contents.

`DA_Trial_TrainingCombat` uses a fixed +1 Endurance reward alongside its existing XP and free point rewards. The tracker previews the Endurance reward. The +1 amount and this Trial's use as its source are current game design.

## Crafting

`UCraftingRecipeDataAsset` contains a recipe ID, ingredient item data/quantities, and an output item/quantity. `/Game/Data/Recipes/DA_Recipe_IronSword` is the current sample: two Iron Ore produce an Iron Sword. `UInventoryComponent::BuildCraftPlan`/`CanCraftRecipe` validate ingredients and output inventory capacity before `CraftRecipe` consumes ingredients and grants output. Successful crafting can report its recipe ID to a crafting trial.

The Blacksmith profession grants a crafting skill. `USkillBlacksmithCrafting` selects a known craftable profession recipe and uses the inventory transaction. The Crafting panel lists recipes, availability, materials, and inventory feedback; it refreshes on relevant inventory, profession, and skill events. Source: [recipe definition](../../Source/ActionRPG/Public/Crafting/CraftingRecipeDataAsset.h), [inventory crafting](../../Source/ActionRPG/Private/Components/Inventory/InventoryComponent.cpp), [Blacksmith skill](../../Source/ActionRPG/Private/Skills/Implementations/SkillBlacksmithCrafting.cpp), [crafting panel](../../Source/ActionRPG/Private/UI/Crafting/CraftingPanelWidget.cpp).

Ordinary quests now have their own component, history, marker, and HUD; they do not use Trial records. See [Quests and Growing Equipment](Quests-and-Growing-Equipment.md).
