# RPG mechanics in *The Land of the Undying Lord*

**Source:** J. T. Wright, *The Land of the Undying Lord* (*The Infinite World*, book 1), supplied EPUB: `_OceanofPDF.com_The_Land_of_the_Undying_Lord_The_Infinite_-_JT_Wright.epub`. Reviewed 2026-11-05. Chapter references follow the EPUB table of contents; the corresponding XHTML file is `text/part0000_split_NNN.html`, where `NNN` is the three-digit chapter number. This is a paraphrased research note, not a reproduction of the book. It establishes evidence from **book 1 only**; it does not settle every rule across the series. Statements made by characters are attributed to them where the book has not independently demonstrated a universal rule.

## Status, level, and character paths

| Finding from the book | Where to check | Relation to our game |
| --- | --- | --- |
| Inhabitants have a Status. The initial Status shown for Trent has Level 0; no class, profession, skill, or ability; separate class and profession slot counts; Health, Stamina, Mana; and six named primary attributes. | Ch. 1, opening Status display and explanation | Supports a zero-level character before path selection and the six primary attributes. Our Combined Level display is a game implementation of path levels, not a confirmed exact Status formula. |
| Trent has seven class slots **and** seven profession slots. The characters regard fourteen total slots as extraordinary, and ordinary people may have far fewer. | Ch. 1 Status; ch. 5 discussion with Michael | Supports separate class/profession capacity. Seven each is Trent's exceptional case, not a normal player default. |
| Michael describes first-level access to classes/professions, a Commoner path for many people, and specialization around Level 5 for some. Affinity and prior skills affect viable paths. | Ch. 5; ch. 6 Evaluation discussion | Supports gated and branching class/profession choices. The exact availability rules vary by person and path. |
| Trent directs earned XP into Level, reaches Level 1, and receives a list of eligible classes. He chooses Survivalist, an Advanced class. | Ch. 15, level and class-selection sequence | Our unallocated XP and class selection evoke this sequence. Do not assume XP alone is sufficient for every later class level. |
| Survivalist's next level requires both XP and Create Traps at Level 3. Advanced classes can be harder to level. | Ch. 15, Survivalist Status; ch. 20 discussion | Evidence for skill prerequisites on class advancement, beyond an XP threshold. Our optional milestone system models this; Warrior level 3 currently requires Melee Attack level 2 as game design. |
| Classes may specialize, advance, or even change unexpectedly at later levels; the resulting path can exchange prior bonuses for new ones. | Ch. 38, Alistern's class change | A future branching-path idea, not a general rule that every level offers a choice. |

## Attributes and growth

| Finding from the book | Where to check | Relation to our game |
| --- | --- | --- |
| Strength, Agility, Dexterity, Constitution, Intelligence, and Wisdom appear as primary values on Status. | Ch. 1 and ch. 27 Status displays | Implemented as `EPrimaryAttribute`. |
| Physical training can improve an attribute before a class is chosen: Trent gains Agility from practice while still Level 0 and learns Unarmed Combat Level 1 through training. | Ch. 7, training and later Status check | Supports practice-based growth; our six activity counters and thresholds are game design. |
| Cullen explains Strength as force and stamina efficiency; Agility as movement and the ability to apply strength; Dexterity as fine motor control; Intelligence/Wisdom as mana and skill mastery factors; Constitution as Health and Stamina. | Ch. 11, recruit lecture | Useful design direction, but Cullen's explanation is qualitative. It supplies no fixed attack-speed, damage, regeneration, or stat formulas. |
| The class tier matters for free attribute points: Basic and Specialized classes give four per level, Advanced classes give five. Survivalist's first level grants five. Trent allocates them among primary attributes. | Ch. 15, Cullen's explanation and Survivalist choice | The project's 4/5 rule matches this book passage. Attribute point allocation is distinct from automatic class benefits. |
| Classes can provide direct Health, Stamina, and skill-related benefits. Survivalist grants Stealth and Climb skills, Camouflage ability, and large Health/Stamina changes at Level 1. | Ch. 15, Survivalist description and selection | Supports fixed class grants as well as free points. Exact bonuses depend on class. |
| Endurance appears as a **secondary attribute** rather than one of the six primary attributes. Cullen says secondary attributes are obtained through special actions or Trial rewards and cannot exceed their linked primary attribute. | Ch. 15, Endurance discussion; ch. 27 partial Status | Our `EAcquiredAttribute` now represents earned Endurance separately from calculated `ESecondaryAttribute` values such as Max Health and Armor. The persistent Constitution cap and numeric resource bonuses are game design. |
| A skill level can occasionally grant an attribute point: Trent gains Agility when Dash levels; Kirstin calls that unusual. | Ch. 20, Dash discussion | Dash now has an optional +1 Agility milestone at skill level 2. Other skills have no such award unless configured. |

## Skills, abilities, spells, and points

| Finding from the book | Where to check | Relation to our game |
| --- | --- | --- |
| Skills can be learned through practice, class grants, stones, and other study. Appraisal is learned by observation; Archery follows practice; Survivalist grants skills on selection. | Chs. 7, 15, 17 | Our catalog, class grants, and skill stones model several routes; spontaneous observation/practice unlocks are limited. |
| Skills have their own levels and can improve through use. Dash changes its speed benefit and Stamina cost as it levels; a storage skill's Mana cost is said to decrease with skill level. | Chs. 6, 10, 20 | Our skills gain use XP and levels. Most numeric effect/cost scaling is still provisional and should be implemented per skill, not inferred as a universal formula. |
| Skills and Abilities are distinct. The Duke describes Abilities as generally more powerful/versatile; some are class associated or Trial rewards. Affinity can matter. Spells are discussed alongside them but should not automatically be collapsed into one category. | Ch. 6, Duke's explanation; ch. 23 affinity example | Our current `USkillBase` system does not implement a separate full Ability progression system. |
| Dash Level 1 is described with a Stamina-per-time cost; Appraisal costs Mana per use. | Chs. 6–7 | Supports ability-specific resource costs. Our Dash and skills use current project data, not book-wide cost assumptions. |
| A quest explicitly rewards XP, one free attribute point, and two free skill points. The balances are separate. | Ch. 28, prisoner rescue reward | Supports distinct reward currencies and non-level sources of free points. |
| **Important divergence:** Orion says free skill points may either raise a known skill or learn a new skill, especially if training has stalled. He recommends saving them for rare/new skills. A visible list includes unavailable skills gated by level or class; points can be held for later. | Ch. 28, Orion's explanation and skill-list scene | Our **current game design** spends free skill points only to learn eligible new skills; known skills level through use. This is a deliberate simplification and differs from Orion's stated rule. Do not describe the restriction as book canon. |

## Trials, quests, rewards, and items

| Finding from the book | Where to check | Relation to our game |
| --- | --- | --- |
| Trials take different forms and test, train, and support participants. Some are permanent locations important to cities. Final guardians and other challenges can award rare skills, abilities, and equipment. | Chs. 1, 8 | Our repeatable/one-time trial assets capture objectives and rewards; they do not model the full world/economy of Trials. |
| A quest may have an optional objective and distinct rewards such as XP and an Ability. A Trial or quest can gate progress by a named objective. | Chs. 19, 28 | `UQuestComponent` now tracks Forgekeeper's Request separately from `UTrialComponent`; its optional defeat pays a free skill point as game design. |
| Trent receives named/soul-bound equipment and Endurance from an exceptional Trial. Soul-bound gear grows with the user, though not necessarily at every level. | Chs. 15–16 | Training Combat awards one Endurance; the named Trialbound Sword grows through equipped-owner victories. Ordinary equipment remains static. The particular item, growth trigger, and amounts are game design. |
| The final Trial offers **alternative reward chests**. The Trial-intended option includes a sword, Swordborn ability, five free skill points, and five free attribute points. The alternative includes three free attribute points, two free skill points, an unarmed technique, and a Self-Clean charm; Trent chooses the latter. | Ch. 39, Darak's reward choice | Evidence for mutually exclusive reward packages and separate point balances. Our First Steps trial offers two smaller game-defined options; it does not recreate the book's chests. |
| Stones, scrolls, potions, equipment, and materials matter to progression. A Skill Stone can teach a skill; some skills depend on profession or affinity. | Chs. 6–7, 10, 16–18, 31 | Our item and skill systems cover some of these routes. Individual content and prerequisites need explicit asset definitions. |

## Design use and limits

1. Cite a chapter when adopting a mechanic as book-supported. For exact numeric rules, use the passage itself, not a fan summary or memory.
2. Keep the book's **acquired secondary attributes** separate from this project's **calculated secondary combat stats**. Renaming either without migration would confuse both design and serialized code.
3. Treat attack damage, attack speed, Armor, crit chance, regeneration, anti-farming thresholds, and UI layout as game design unless a specific passage supports the chosen rule. Chapter 11 supports qualitative attribute roles, not coefficients.
4. Preserve the explicit skill-point divergence above if the current design remains. If fidelity becomes the priority, design a separate, carefully costed known-skill level purchase path; that is a future product decision, not an instruction embedded in the EPUB.

Related project guides: [Progression](Progression.md), [Skills](Skills.md), [Combat and Resources](Combat-and-Resources.md), [Trials and Crafting](Trials-and-Crafting.md), [Items and Equipment](Items-and-Equipment.md).
