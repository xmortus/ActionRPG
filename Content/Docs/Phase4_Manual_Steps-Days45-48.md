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
2. **Do NOT add another EquipmentComponent.** The C++ player character already creates one.
3. Verify the existing `EquipmentComponent` shows in the Components list.
4. Compile and save.

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
1. Create `WBP_EquipmentWidget` (UserWidget) and reparent it to `UEquipmentWidget`.
2. Layout the root widget:
   - Use a `CanvasPanel` as root.
   - Add three panels: Left (paper‑doll), Center (stats), Right (inventory).
   - Add a bottom row for filter buttons.
3. Paper‑doll area:
   - Add `WBP_EquipmentSlot` widgets for each slot (MainHand, Offhand, Head, Chest, Legs, Hands, Feet, Ring, Amulet).
   - Set each slot widget’s `EquipmentSlot` property in the Details panel.
4. Create `WBP_EquipmentSlot` (UserWidget) and reparent to `UEquipmentSlotWidget`:
   - Add an Image for the slot icon and a background frame.
   - (Optional) Add a Border for selection/hover states.
5. Stats panel:
   - Create `WBP_EquipmentStats` (UserWidget) and reparent to `UEquipmentStatsWidget`.
   - Add TextBlocks for primary stats and secondary stats (or a vertical list).
6. Inventory panel:
   - Create `WBP_EquipmentInventory` (UserWidget) and reparent to `UEquipmentInventoryWidget`.
   - Add a `UniformGridPanel` for item slots.
   - **Recommended:** create a dedicated equipment‑inventory slot widget (avoid reusing `WBP_InventorySlot`, which expects a `UInventoryWidget` parent).
7. Filters row:
   - Add buttons: Armor, Weapons, Food & Potions, Quest Items, All Items.
8. Bind widget events (Blueprint logic details):
   - In `WBP_EquipmentWidget`:
     - Override `OnEquipmentUpdated`: call `UpdatePaperDoll()` (custom event) to refresh all slot widgets.
     - Override `OnInventoryUpdated`: call `EquipmentInventoryWidget->OnInventoryUpdated` (or a `RefreshInventory` custom event).
     - Override `OnStatsUpdated`: call `EquipmentStatsWidget->OnStatsUpdated`.
   - In `WBP_EquipmentSlot`:
     - Override `OnSlotUpdated(Item)`:
       - If `Item` is valid: set slot icon to `Item->GetItemIcon()` (or data asset icon).
       - If `Item` is null: set slot icon to empty/placeholder.
   - In `WBP_EquipmentStats`:
     - Override `OnStatsUpdated`:
       - Read primary stats from `AttributeComponent`.
       - Read secondary stats from `SecondaryAttributeComponent`.
       - Update TextBlocks (STR/AGI/CON/etc., HP/Mana/Armor/Crit/MoveSpeed).
   - In `WBP_EquipmentInventory`:
     - Override `OnInventoryUpdated`:
       - Clear `UniformGridPanel` children.
       - Get `InventoryComponent->GetInventorySlots()`.
       - For each slot:
          - Create your equipment‑inventory slot widget.
         - Call a setup function (slot index, item, quantity).
         - Add to grid (Row = Index / Columns, Column = Index % Columns).
9. Drag & drop wiring:
   - Inventory slot drag → equipment slot drop (equip via `InventoryComponent->EquipItemFromSlot`).
   - Equipment slot drag → inventory drop (unequip via `InventoryComponent->UnequipItemToInventory`).
   - Invalid drops return item to original slot.
10. Filter button logic (optional):
   - On click, set a local category enum and refresh the grid.
   - Default filter = All Items.
11. **Equipment UI toggle:**
    - In `BP_ActionRPGPlayerController`, set:
      - `EquipmentWidgetClass` = `WBP_EquipmentWidget`
      - `OpenEquipmentAction` = your input action (added to your mapping context)
    - This enables the C++ toggle for the equipment UI.
12. Save and compile all widgets.

---

## Step 6: Output Log Validation
1. Open Output Log.
2. Confirm messages for:
   - Equip success/failure
   - Slot validation failures
   - Secondary attribute recalculation on equip/unequip
