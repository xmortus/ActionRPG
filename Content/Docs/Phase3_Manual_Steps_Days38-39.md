# Phase 3 Manual Steps - Days 38-39
**Focus:** Skill-Granting Items (SkillItem, SkillStone, BeastCore)  
**UE Version:** 5.7  
**Last Updated:** 2026-01-18

---

## Overview
This guide documents the UE 5.7 editor steps to create Blueprint classes and Data Assets for the skill-granting items implemented in C++.

---

## Prerequisites
- Project compiles successfully after Days 38-39 C++ changes.
- `SkillComponent` and `SkillManagerComponent` are attached to the player.
- `DA_Skill_Fireball` (or another skill data asset) exists.
- Inventory UI and inventory component are working (Phase 2 complete).

---

## Step 1: Create Content Folders (if missing)
1. Open the **Content Browser**.
2. Click the **Content** root in the left panel.
3. Right-click in the empty area and choose **New Folder**.
4. Create (or verify) these folders:
   - `Content/Blueprints/Items`
   - `Content/Blueprints/Items/Consumables`
   - `Content/Data/Items`
   - `Content/Data/Items/Skills`
5. If a folder already exists, skip it and continue.

---

## Step 2: Create Blueprint Item Classes
1. Navigate to `Content/Blueprints/Items/Consumables`.
2. Right-click and choose **Blueprint Class**.
3. Click **All Classes** and select `SkillItem` (C++ class).
4. Name it `BP_SkillItem`.
5. Repeat the process for:
   - `SkillStoneItem` → `BP_SkillStoneItem`
   - `BeastCoreItem` → `BP_BeastCoreItem`
6. Open each Blueprint and **Compile** + **Save** (no default values required yet).

---

## Step 3: Create Item Data Assets
1. Open `Content/Data/Items/Skills`.
2. Right-click and choose **Miscellaneous** → **Data Asset**.
3. Select **ItemDataAsset** (or your item data asset class).
4. Create these assets:
   - `DA_Item_Skill_Fireball`
   - `DA_Item_SkillStone_Fireball`
   - `DA_Item_BeastCore_Wolf`

---

## Step 4: Configure `DA_Item_Skill_Fireball`
1. Open `DA_Item_Skill_Fireball`.
2. In the Details panel, set:
   - **Item**
     - `ItemName`: `Fireball Skill Tome`
     - `ItemDescription`: `Unlocks the Fireball skill.`
     - `ItemType`: `Consumable` (or matching enum used by your item system)
     - `ItemIcon`: assign an icon (optional)
   - **Skill Item**
     - `SkillData`: `DA_Skill_Fireball`
     - `SkillClass`: `BP_Skill_Fireball`
   - **Item Class**
     - `ItemClass`: `BP_SkillItem`
3. Save the asset.

---

## Step 5: Configure `DA_Item_SkillStone_Fireball`
1. Open `DA_Item_SkillStone_Fireball`.
2. In the Details panel, set:
   - **Item**
     - `ItemName`: `Fireball Skill Stone`
     - `ItemDescription`: `Unlocks Fireball regardless of requirements.`
     - `ItemType`: `Consumable`
     - `ItemIcon`: assign an icon (optional)
   - **Skill Stone Item**
     - `SkillData`: `DA_Skill_Fireball`
     - `SkillClass`: `BP_Skill_Fireball`
   - **Item Class**
     - `ItemClass`: `BP_SkillStoneItem`
3. Save the asset.

---

## Step 6: Configure `DA_Item_BeastCore_Wolf`
1. Open `DA_Item_BeastCore_Wolf`.
2. In the Details panel, set:
   - **Item**
     - `ItemName`: `Wolf Beast Core`
     - `ItemDescription`: `Unlocks a skill if requirements are met.`
     - `ItemType`: `Consumable`
   - **Beast Core Item**
     - `SkillData`: `DA_Skill_Fireball` (use any test skill)
     - `SkillClass`: `BP_Skill_Fireball`
     - `RequiredLevel`: set a value (example `5`) if the field exists
     - `RequiredAttributes`: set any placeholder values (if fields exist)
   - **Item Class**
     - `ItemClass`: `BP_BeastCoreItem`
3. Save the asset.

---

## Step 7: Add Items to the Player Inventory (Test)
1. Open `BP_ActionRPGPlayerCharacter`.
2. In **Event Graph**, locate `Event BeginPlay`.
3. Add a `Get Component by Class` node and select `InventoryComponent`.
4. For each data asset (Fireball Tome / Stone / BeastCore):
   - Add a `Add Item` (or your inventory add function).
   - Set `ItemData` to the relevant Data Asset.
   - Set quantity to `1`.
5. Compile and Save.

---

## Step 8: Test Skill Unlock Flow
1. Click **Play** (PIE).
2. Open your inventory UI.
3. Use each item:
   - `Fireball Skill Tome` should unlock Fireball (SkillItem).
   - `Fireball Skill Stone` should always unlock Fireball (SkillStone).
   - `Wolf Beast Core` should unlock Fireball if requirements are met.
4. Confirm:
   - Skill appears in `SkillManagerComponent` (if you have a UI, check skill bar).
   - Item is removed from inventory.
   - Output Log shows unlock messages.

---

## Step 9: Troubleshooting Checklist
- Item Data Assets have `ItemClass` assigned to the correct Blueprint.
- `SkillData` is set on each item asset.
- `SkillManagerComponent` is attached to the player.
- Inventory `Add Item` nodes are firing on BeginPlay.
- Skill unlock logs appear in Output Log.

