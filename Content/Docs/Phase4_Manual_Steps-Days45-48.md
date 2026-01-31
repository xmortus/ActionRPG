# Phase 4 Manual Steps - Days 45-48
**Focus:** Equipment System Foundation  
**UE Version:** 5.7  
**Last Updated:** 2026-01-22

---

## Step 1: Create Equipment Data Assets
1. Navigate to `Content/Data/Items/Equipment/` (create folders as needed).
2. Create item data assets:
   - `DA_Item_IronSword` (WeaponItem)
   - `DA_Item_WoodenShield` (EquipmentItem)
   - `DA_Item_LeatherHelm` (EquipmentItem)
3. For each data asset:
   - Set `Type` = `Equipment`
   - Assign `ItemClass` to the matching C++ class (WeaponItem/EquipmentItem)
   - Set the **Equipment Slot** field (MainHand/Offhand/Head/etc.)
   - Add **Stat Bonuses** (example: +10 MaxHealth, +5 Armor)
4. Save all assets.

---

## Step 2: Create Equipment Blueprint Classes (Optional)
1. Create Blueprint subclasses (if you want per-item logic):
   - `BP_EquipmentItem`
   - `BP_WeaponItem`
2. Set their defaults to match your data assets.
3. Save.

---

## Step 3: Attach EquipmentComponent to Player
1. Open `BP_ActionRPGPlayerCharacter`.
2. Add component: `EquipmentComponent`.
3. Compile and save.

---

## Step 4: Basic Equip/Unequip Test
1. Place `BP_ActionRPGPlayerCharacter` in a test level.
2. Add equipment items into the inventory (via debug spawn or data setup).
3. In PIE, equip items to valid slots.
4. Verify:
   - Invalid slot equip is rejected
   - Equipped items appear in the correct slot
   - Stat bonuses update secondary attributes

---

## Step 5: Equipment UI Setup (UE 5.7)
1. Create `WBP_EquipmentWidget` (UserWidget).
2. Add layout sections:
   - Left: paper-doll slots (VerticalBox/Canvas for equipment slots)
   - Center: stats panel (primary + secondary)
   - Right: inventory grid (UniformGridPanel)
   - Bottom: filter buttons (Armor, Weapons, Food & Potions, Quest Items, All Items)
3. Create `WBP_EquipmentSlot` (UserWidget) for each equipment slot:
   - Add slot icon + empty background
   - Expose `EquipmentSlot` enum on the widget
4. Create `WBP_EquipmentStats` (UserWidget):
   - Add text for primary + secondary stats (bind later)
5. Create `WBP_EquipmentInventory` (UserWidget):
   - Reuse inventory slot widgets or create equipment-inventory slot widget
6. In `WBP_EquipmentWidget`:
   - Place slot widgets in the paper-doll area
   - Place stats widget in the center panel
   - Place inventory widget on the right
   - Place filter buttons at the bottom
7. Bind to components:
   - Equipment slots → `EquipmentComponent`
   - Inventory grid → `InventoryComponent`
   - Stats panel → `AttributeComponent` + `SecondaryAttributeComponent`
8. Wire drag & drop:
   - Inventory slot drag → equipment slot drop
   - Equipment slot drag → inventory drop
9. Filter buttons (optional):
   - Add buttons: Armor, Weapons, Food & Potions, Quest Items, All Items
   - On click, call an inventory filter function (e.g., set a category enum and refresh grid)
   - Default filter = All Items
10. Save and compile all widgets.

---

## Step 6: Output Log Validation
1. Open Output Log.
2. Confirm messages for:
   - Equip success/failure
   - Slot validation failures
   - Secondary attribute recalculation on equip/unequip
