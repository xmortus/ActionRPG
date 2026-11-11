# PIE combat and regeneration checks

Open `NewMap` and start Play In Editor. Use the console (`~`) or the editor's bottom **Cmd** field while PIE is running.

## Move panels in PIE

Open Inventory, Equipment, Skills, Character Status, Classes, or Crafting. Hold the left mouse button on the panel title and drag it. The panel stays within the game viewport when it fits; on a smaller viewport its title remains reachable. Close buttons, tabs, item slots, and skill assignments remain clickable. Positions last for the current PIE session only.

## Test all six attributes in one PIE session

Open Character Status with **C**. The **Overview** tab shows paths and attribute totals, **Training** shows the six practice counters, and **Sources** shows the full breakdown for each attribute. Each training line shows accepted practice toward its next permanent +1 and the total earned gain. The test actions are:

The summary labels its level **Combined Level**. Hover over it to see that active class and profession levels are added together. It shows 0 with no active class or profession and rises when a path is selected. Level 1 paths remain available as starting choices. The Overview page lists class and profession levels separately.

On **Overview**, hover over an attribute name or value for a quick source breakdown. Open **Sources** to read and scroll through all six breakdowns. They list base values, allocated free points, class gains, equipment, training, and active effects; an attribute limit adjustment appears when the total is capped.

| Attribute | Action that earns practice |
| --- | --- |
| Strength | Damage a living enemy with melee. |
| Agility | Dodge an enemy strike or dodge away from a nearby living enemy. Press **Space Bar**. |
| Constitution | Survive a training enemy strike that deals at least 5 health damage. |
| Dexterity | Damage a living enemy with Ranged Attack. Dodging only trains Agility. |
| Intelligence | Hit a living enemy with Fireball. Aim toward it and press **7**; the starter Fireball costs 8 mana. |
| Wisdom | Learn Minor Regeneration from the Skills panel and assign it to slot **8**. Restore at least 2 missing health with it after `RPGTestDamageSelf 30`. Healing at full health does not count. |

The southeast `TrainingEnemy` attacks in close range and becomes hittable again four seconds after defeat. Move out of range or heal as needed. Training actions have cooldowns and per-minute limits, so an immediate repeat may not increase a counter. All six paths are session-only until persistence is added.

## Resource regeneration

1. Enter `RPGTestDamageSelf 30` and watch the Health bar fall.
2. Wait five seconds. Health should begin recovering at the configured passive rate of 2 per second.
3. Learn Minor Regeneration with a free skill point, then assign it to slot **8** from the **Known** tab. Damage the player again, close the console, and press **8** while the game viewport has focus. The skill costs 8 mana from the starting 10-mana pool and adds 3 base health plus 0.5 per effective Wisdom point each second for eight seconds. With starting Wisdom 1, that is 3.5 health per second. The Mana bar should fall when the cast succeeds. Its five-second cooldown prevents immediate recasting.
4. Stamina and mana can be checked by spending them through ordinary skills and movement actions. Their configured passive recovery delays are one and two seconds, respectively.

## Active effect display

1. Learn and assign Minor Regeneration as described above. Damage yourself with `RPGTestDamageSelf 30`, close the console, and press **8** with the game viewport focused.
2. **Minor Regeneration** should appear above the resource bars with its remaining whole seconds. It should count down and disappear when the eight-second effect expires.
3. While the effect is active, press **C** and open the **Effects** tab. It should show the same effect, stack count, remaining duration, and health restored per tick. The list should say **No active effects** after expiration.

## Learning skills with free skill points

1. In PIE, enter `RPGTestGrantSkillPoints 3` in the console. Open the Skills panel; it should show **Free skill points: 3**.
2. Open the **Learn** tab. Minor Regeneration and Dash are available for one point each. Major Regeneration costs two points and requires player level 2 plus Minor Regeneration. Ranged Attack costs one point and requires player level 2 and persistent Dexterity 2. Hover over an entry to read its full requirements; an unmet requirement disables **Learn** and shows the reason.
3. Click **Learn** on Minor Regeneration. The balance should fall to 2 and Minor Regeneration should appear under **Known**. It cannot be purchased again. Right-click it to assign it to slot **8**. Switch between **Known** and **Learn**, then click **X** to close the panel.
4. Learning starts the new skill at level 1 with no use XP. Use the assigned skill to earn its own XP and levels; learning other skills does not change those values. The catalog costs and gates are provisional game balance.

## Death and respawn

1. Enter `RPGTestDamageSelf 9999`.
2. The **You have fallen** screen should appear. Health should stay at zero; passive healing and healing effects cannot revive the player.
3. After the configured three-second delay, click **RESPAWN**. The player should return to PlayerStart at full health and the death screen should close.

`RPGTestDamageSelf` works only on a living player in PIE. It applies damage through Unreal's normal damage path and does not change an editor asset or level. Stop and restart PIE to reset the session.

For class progression checks, `RPGTestGrantXP 225` adds 225 unallocated and total XP to the PIE player. It changes only the current play session and lets you test the Warrior level 3 Melee Attack requirement without completing multiple trials first.

## Strength training

1. In PIE, enter `RPGTrainingStatus`. The message appears on screen and in the Output Log. It shows accepted melee practice, permanent training gain, persistent Strength, and unspent free attribute points.
2. Hit a living enemy with the melee attack. Only a hit that actually damages an enemy counts. Enter `RPGTrainingStatus` again to inspect progress.
3. By default, 20 accepted hits grant +1 permanent Strength and reset the practice counter. The default rule accepts at most one melee practice event every 0.5 seconds, one per target every 3 seconds, and 10 events in each 60-second window. The values are provisional game balance and can be changed in the player's `TrainingComponent.Rules` defaults.
4. Verify that free attribute points stay the same when Strength grows. Stop and restart PIE to reset training for now; save/load remains deferred.

The southeast `TrainingEnemy` also completes the one-time combat trial when defeated. It becomes hittable again four seconds after each defeat, so you can keep practicing on it after the trial ends. Trial rewards are not granted again.

## Agility dodge training

1. Open Character Status in PIE to see the Strength and Agility training counters under **Attribute Training**.
2. Approach the living training enemy, then press the bound Dodge action while moving away from it. A dodge costs 15 stamina and has a two-second cooldown. It briefly avoids incoming damage.
3. A dodge counts as Agility practice when it avoids an enemy hit, or when it moves at least 100 units and increases separation from a nearby enemy by at least 80 units. Dodging in empty space or toward the enemy does not earn practice.
4. The default Agility milestone is 12 accepted dodges for +1 permanent Agility. Practice is limited to one event per three seconds, one per target per eight seconds, and six per minute. These are provisional balance values in `TrainingComponent.Rules`.

## Constitution survival training

1. Approach the living southeast training enemy and stay within melee range. Its close-range strike deals 8 damage every 2.5 seconds while you remain nearby. Watch the health bar and retreat or use healing before you die.
2. Survive a hit that actually deals at least 5 damage. Open Character Status with **C** and check **Constitution (survive enemy hit)** under **Attribute Training**. Dodged hits, self damage, and fatal hits do not earn Constitution practice.
3. The default milestone is 12 accepted hits for +1 permanent Constitution. Practice is limited to one event every three seconds, one per enemy every eight seconds, and six per minute. These are provisional balance values in `TrainingComponent.Rules`.

## Dexterity, Intelligence, and Wisdom training

1. **Dexterity:** Ranged Attack requires player level 2 and persistent Dexterity 2. Meet those requirements, learn it for one free skill point, and assign it to an open hotkey in **Known**. A projectile that actually damages a living enemy earns practice. Twelve accepted ranged hits grant +1 Dexterity; dodges and melee hits do not.
2. **Intelligence:** Fireball is assigned to hotkey **7** in `NewMap`. Its mana cost is 8 so the starting character can cast it. A direct hit or splash that actually damages a living enemy earns practice; hitting scenery with no enemy in range does not. Twelve accepted magic hits grant +1 Intelligence. The caster is excluded from their own projectile and splash damage.
3. **Wisdom:** Learn Minor Regeneration and assign it to **8**, then press **8** while injured. A healing tick must restore at least 2 health to earn practice. Passive regeneration and healing at full health do not count. Twelve accepted healing ticks grant +1 Wisdom.
4. Each path accepts at most one practice event every three seconds and six per minute. Dexterity and Intelligence also limit practice against one enemy to once every eight seconds. These values are provisional game balance in `TrainingComponent.Rules`.

## Equipment inventory parity

1. In PIE, press **U** to open Equipment. The carried items area should show the same slots as the standalone Inventory panel, plus current weight and used slot capacity.
2. Right-click a carried item. Its menu should offer the same available actions as Inventory: **Use**, **Drop**, **Split** for stacks, and **Equip** for equipment items. Equip should place a compatible item in its paper-doll slot.
3. Drag an item to another carried slot to move or swap it. Hold **Ctrl** while starting a drag on a stack to move half to an empty slot. Drag equipped gear back into a carried slot to unequip it.
4. Hover over a carried item for its name and description. A valid drag target should highlight green; an invalid target should highlight red. Cancel a drag and verify the item remains in its original slot.
5. Close Equipment with **U** or its close button while a context menu is open. The menu should close with the panel. Open standalone Inventory and verify that item changes appear there too.
