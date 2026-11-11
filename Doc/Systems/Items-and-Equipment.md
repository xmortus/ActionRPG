# Items, inventory, equipment, and quick use

## Data and instances

`UItemDataAsset` defines item identity, display, weight, stack size, use behavior, equipment requirements/bonuses, weapon damage/type, and resistance bonuses. `UItemBase` is a runtime item instance held in an inventory slot. Item creation and lookup use the item database. World `AItemPickupActor` instances carry item data and quantity; interaction or configured overlap requests a pickup.

## Carried inventory

`UInventoryComponent` owns fixed slots, quantities, weight limit, and item quick-use bindings. `HasSpaceFor` checks weight, free space in matching partial stacks, and empty slots for the **entire requested quantity** before a pickup. `AddItem`, `RemoveItem`, move/swap, split stack, use, drop, and craft operations update slots and broadcast changes. Stack merges keep item-instance and slot quantities aligned. A failed pickup or oversized world drop leaves the source state intact. Partial consumable drops preserve the quick-use binding; a full removal clears it. When a stack merges completely, a binding can follow the surviving instance unless that destination is already bound.

`UseItem` checks whether the item can have an effect before consumption. Potions restore resources only if restoration succeeds. Skill items/stones can unlock their configured skill subject to their item logic; inventory now requires the runtime skill grant to succeed before broadcasting use or consuming the item. World drops spawn an item pickup and remove the source quantity only after validation. Source: [inventory](../../Source/ActionRPG/Private/Components/Inventory/InventoryComponent.cpp), [item data](../../Source/ActionRPG/Public/Items/Core/ItemDataAsset.h), [pickup](../../Source/ActionRPG/Private/Items/Pickups/ItemPickupActor.cpp).

## Equipped items

`UEquipmentComponent` owns the equipment slot map and broadcasts changes. Equipping validates the item's target slot, level and persistent attribute requirements, and whether the slot is free. It does not silently replace an occupied slot. Inventory-to-equipment and equipment-to-inventory transactions are coordinated by `UInventoryComponent`; unequip checks inventory slot/weight capacity before removing gear. Primary/secondary components recalculate equipment bonuses after equipment events. Main-hand weapon damage is additive to melee skill damage; weapon type determines the typed hit. Source: [equipment](../../Source/ActionRPG/Private/Components/Inventory/EquipmentComponent.cpp), [inventory transfers](../../Source/ActionRPG/Private/Components/Inventory/InventoryComponent.cpp).

## UI

The standalone Inventory and the carried-items grid inside Equipment both support item tooltips, context actions, and drag/drop operations. Paper-doll targets validate slot compatibility and show valid/invalid feedback. Equipment shows carried capacity/weight, equipped slots, vitals, combat stats, resistances, and weapon bonus. Consumable quick-use slots use item-instance bindings; skill hotkeys use the separate skill manager. Source: [inventory widget](../../Source/ActionRPG/Private/UI/Inventory/InventoryWidget.cpp), [equipment widget](../../Source/ActionRPG/Private/UI/Equipment/EquipmentWidget.cpp), [equipment inventory](../../Source/ActionRPG/Private/UI/Equipment/EquipmentInventoryWidget.cpp).

Example content: `/Game/Data/Items/Data/Equipment/DA_Item_Weapon_Sword`, `/Game/Data/Items/Data/Consumables/DA_Item_Consumable_HealthPotion`, `DA_Item_Consumable_StaminaPotion`, and `/Game/Data/Items/Data/Materials/DA_Item_IronOre`.

The exceptional `/Game/Data/Items/Data/Equipment/DA_Item_Weapon_TrialboundSword` uses instance-owned growth and a first-owner bind; ordinary equipment remains static. See [Quests and Growing Equipment](Quests-and-Growing-Equipment.md).
