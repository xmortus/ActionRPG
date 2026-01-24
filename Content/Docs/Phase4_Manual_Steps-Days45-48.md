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

## Step 5: Output Log Validation
1. Open Output Log.
2. Confirm messages for:
   - Equip success/failure
   - Slot validation failures
   - Secondary attribute recalculation on equip/unequip
