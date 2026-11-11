# Prototype playthrough

This is the current **single-session** route in `/Game/NewMap`. It uses the existing trial, class, combat, quest, skill, and item systems. Character persistence remains deferred. The Journey line in the Trials card advances from the systems' runtime events; it does not grant rewards or bypass requirements.

## Main route

Open the game and choose **Start Game**. In PIE, press **P** to open the pause menu; **Resume** continues and **Exit Game** stops play. Escape also opens the pause menu in the packaged game, while the editor reserves Escape for stopping PIE.

1. Enter **START: First Steps** southwest of the player start, then reach **GOAL: First Steps** north of that marker. Choose Growth or Learning in the Trials card. Either choice awards 100 XP; the two free-point balances remain distinct.
2. The character starts with Melee Attack known and assigned to the main hand. Open **Classes & Professions [B]**, select Warrior or Mage, and assign 100 XP to reach class level 1. Mage grants Fireball; Warrior keeps a level-three gate requiring Melee Attack skill level 2. If the first 100 XP was spent on another path, Dash Practice is a repeatable XP source.
3. Enter **START: Training Combat** southeast of the player start and defeat the Training Enemy northeast of the marker using the starting Melee Attack or another learned attack. The trial awards 150 XP, 5 free attribute points, 6 free skill points, and +1 Endurance subject to the Constitution cap.
4. Enter **QUEST: Forgekeeper's Request** northwest of the player start. The pickup north of the start contains two Iron Ore; two more single ore pickups lie west along the route toward the Forgekeeper. Press **F** to collect them; touching them does not collect them. **F** chooses the nearest pickup within its interaction range. Return to the quest marker with two ore for 75 XP and a Trialbound Sword. Defeating the Training Enemy while the quest is active adds one free skill point. Four ore are available in the level, enough to complete this quest and craft an Iron Sword in either order.
5. Open **Equipment [U]** and equip the Trialbound Sword from Carried Items. Defeat the repeatable Training Enemy three times while it is equipped. The Journey line counts `0/3`, `1/3`, and `2/3`; the sword then reaches rank 1, its Physical bonus grows from +22 to +25, and the line reads **JOURNEY COMPLETE**.

Dash Practice and Forge an Iron Sword remain optional activities after this route. The Trialbound Sword can continue growing to rank 3 through credited defeats while equipped.

## Verification and remaining work

- Unreal MCP found the PlayerStart, five trial markers, Forgekeeper quest marker, Training Enemy, test dummy, and eleven item pickups in the saved level. The two extra Iron Ore pickups lie on the floor along the Forgekeeper route; the Melee Attack stone pickup was removed. The trial markers have clear geometric separation. A continuous player-controlled walk remains the final accessibility check.
- All eleven saved pickup actors have valid item data and `bAutoPickupOnOverlap=false`. Unreal MCP reloaded the level and verified Iron Ore quantities of 2, 1, and 1, plus a matte black material override on all three ore cubes. In an earlier PIE check, Iron Ore remained on the ground after overlap, then **F** collected two ore through `TryInteract` and inventory. The controller prefers the nearest pickup in range, independent of the cursor position.
- The native start/pause menu was verified in PIE: **Start Game** began play, **P** opened the pause menu, **Resume** closed it, and **Exit Game** ended PIE from both the start and pause menus.
- The death screen now also offers **Exit Game** beside Respawn. Unreal MCP verified that Exit Game stopped PIE, Respawn returned to full health, and the focused player-death regression passed.
- The full `ActionRPG.*` regression suite passed **50/50** after the menu and proximity interaction changes.
- Unreal MCP PIE verified every Journey transition, reward selection, Warrior XP allocation, Training Combat completion, ore collection, quest turn-in, sword equip, all three growth counter values, and the rank-1 completion message. Teleportation, a component equip call, and player-credited damage commands were used for the route verification; a continuous human-controlled run remains to be done.
- The trial markers now include their trial names. Unreal MCP read back all five saved marker texts, and the editor map reopened cleanly with 37 actors.
- The skill-stone transaction regression passed: a failed runtime unlock leaves the item carried, while a successful unlock consumes it once. The full `ActionRPG.*` suite passed **50/50** after the five-stage route change.
- The Windows package was rebuilt, cooked, and archived after the menu, pickup, and death-screen exit changes at `PrototypeBuild/Windows/ActionRPG.exe`. Its headless startup smoke test kept both game processes alive for 20 seconds. A visual standalone run remains to be done. The remaining two hover-overlay checks are in [PIE Test Queue](PIE-Test-Queue.md).
- Before calling this a complete prototype, perform a continuous route playtest, tune its pacing and encounter difficulty, and improve the graybox environment's visual/audio cues. Additional world regions and save/load are later scope.
