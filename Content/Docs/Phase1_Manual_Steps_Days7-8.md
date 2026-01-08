# Phase 1 Manual Steps - Days 7-8: Item System Foundation
**After Code Implementation**  
**Date:** 2025-01-07

---

## Overview

Days 7-8 focus on setting up the Item System Foundation. The C++ code is complete, but you need to configure the Asset Manager and create test Item Data Assets in the Unreal Editor.

**Status:** ✅ C++ code is complete - Manual editor steps remain

---

## Prerequisites

- ✅ C++ code compiled successfully
- ✅ Unreal Editor is open
- ✅ Project is loaded

---

## Step 1: Configure Asset Manager for Primary Assets

### 1.1 Open Project Settings

1. **Open Project Settings**
   - Go to `Edit` → `Project Settings`
   - Or press `Ctrl + ,`

2. **Navigate to Game → Asset Manager**
   - In the left panel, expand `Game` section
   - Click on `Asset Manager`

### 1.2 Configure Primary Asset Types

1. **Add Primary Asset Type for Items**
   - In the `Primary Asset Types to Scan` section
   - Click the `+` button to add a new type

2. **Configure the Asset Type:**
   - **Primary Asset Type:** `Item`
   - **Asset Base Class:** Set to `ItemDataAsset` (or leave as `DataAsset` if ItemDataAsset doesn't appear)
   - **Directories to Scan:** 
     - Add: `/Game/Data/Items/`
     - This tells the Asset Manager where to find item data assets
   - **Rules:** Leave as default

3. **Save Project Settings**
   - Click `Save` or close the window

**Note:** If `ItemDataAsset` doesn't appear in the dropdown, you can:
- Use `DataAsset` as the base class
- Or configure it after creating your first Item Data Asset

---

## Step 2: Create Test Item Data Assets

### 2.1 Navigate to Data Folder

1. **Go to Content Browser**
   - Navigate to `Content/Data/Items/` folder
   - (Create the folder if it doesn't exist)

### 2.2 Create Test Item: Health Potion

1. **Create Item Data Asset**
   - Right-click in Content Browser
   - Select `Miscellaneous` → `Data Asset`
   - In the "Pick Data Asset Class" dialog, search for `ItemDataAsset`
   - Select `ItemDataAsset` and click `Select`

2. **Name the Asset**
   - Name it: `DA_Item_Consumable_HealthPotion`
   - Save it in `Content/Data/Items/`

3. **Configure the Item**
   - Double-click `DA_Item_Consumable_HealthPotion` to open
   - In the `Details` panel, configure:
     - **Item Type:** Set to `Item` (Primary Asset Type)
     - **Item ID:** `HealthPotion` (or `Item_HealthPotion`)
     - **Item Name:** `Health Potion`
     - **Item Description:** `Restores health when consumed`
     - **Item Icon:** (Leave empty for now, or assign a texture)
     - **Type:** `Consumable`
     - **Rarity:** `Common`
     - **Max Stack Size:** `10`
     - **Weight:** `0.5`
     - **Value:** `25`

4. **Save and Close** the Data Asset

### 2.3 Create Test Item: Skill Item

1. **Create Another Item Data Asset**
   - Right-click in `Content/Data/Items/`
   - `Miscellaneous` → `Data Asset`
   - Select `ItemDataAsset`

2. **Name:** `DA_Item_Skill_Fireball`

3. **Configure:**
   - **Item Type:** `Item`
   - **Item ID:** `SkillItem_Fireball`
   - **Item Name:** `Fireball Skill Scroll`
   - **Item Description:** `Teaches the Fireball skill when consumed`
   - **Type:** `Skill Item`
   - **Rarity:** `Uncommon`
   - **Max Stack Size:** `1`
   - **Weight:** `0.1`
   - **Value:** `100`

4. **Save and Close**

### 2.4 Create Test Item: Weapon

1. **Create Item Data Asset:** `DA_Item_Weapon_Sword`

2. **Configure:**
   - **Item Type:** `Item`
   - **Item ID:** `Weapon_Sword`
   - **Item Name:** `Iron Sword`
   - **Item Description:** `A basic iron sword`
   - **Type:** `Equipment`
   - **Rarity:** `Common`
   - **Max Stack Size:** `1`
   - **Weight:** `3.0`
   - **Value:** `50`

3. **Save and Close**

---

## Step 3: Verify Asset Manager Configuration

### 3.1 Check Primary Asset Type

1. **Open Project Settings** → `Game` → `Asset Manager`

2. **Verify Configuration:**
   - Primary Asset Type `Item` exists
   - Directory `/Game/Data/Items/` is in the scan list
   - Base class is set correctly

### 3.2 Scan for Assets

1. **In Asset Manager Settings:**
   - Click `Scan Primary Asset Types` button (if available)
   - Or restart the editor to trigger automatic scan

2. **Verify Items are Found:**
   - The Asset Manager should detect your Item Data Assets
   - Check the Output Log for scan results

---

## Step 4: Test Item System in Blueprint

### 4.1 Create Test Blueprint

1. **Create a Test Actor or use existing character**
   - For testing, you can create a simple Blueprint
   - Or add test code to existing character Blueprint

2. **Test Item Database**

   **Option A: Test in Blueprint Event Graph**
   
   **Step-by-Step Instructions:**
   
   1. **Open Blueprint**
      - Open `BP_ActionRPGPlayerCharacter` (or any Blueprint)
      - Go to the `Event Graph` tab
   
   2. **Add Test Event**
      - Right-click in Event Graph
      - Search for `Event BeginPlay` or create a `Custom Event`
      - If using Custom Event, name it `TestItemDatabase`
   
   3. **Get Item Database Instance**
      - Right-click in Event Graph
      - Search for `Get Item Database` (or `Item Database Get`)
      - **Note:** This is a static function, so it appears as a blue node without a target pin
      - This calls the static `Get()` function to get the singleton instance
      - The output pin will be of type `Item Database Object Reference`
      - Drag from the output pin to use it
   
   4. **Call Get Item Data Asset**
      - Search for `Get Item Data Asset` node
      - Connect the `Item Database` output to the `Target` input
      - For `Item ID` input:
        - Right-click on the `Item ID` pin
        - Select `Split Struct Pin` (if needed)
        - Right-click again and select `Make Literal Name`
        - Or type directly: Right-click on pin → `Set Value` → Type `HealthPotion`
        - Alternatively: Create a `Make Literal Name` node and set value to `HealthPotion`
   
   5. **Print Result**
      - Drag from the `Item Data Asset` output pin
      - Search for `Print String` node
      - Connect the output
      - In `In String`, use `Format Text` or `To String` to display item name
      - Or use `Break Item Data Asset` to access individual properties
      - Connect `Item Name` to `Print String`
   
   6. **Alternative: Use Break Node**
      - After `Get Item Data Asset`, add `Break Item Data Asset` node
      - Connect the Item Data Asset output
      - This gives you access to all properties (Name, Description, Type, etc.)
      - Connect `Item Name` to `Print String` to verify
   
   7. **Compile and Test**
      - Click `Compile` button
      - Click `Save`
      - Play in Editor
      - Check Output Log for the printed item name
   
   **Example Node Setup:**
   ```
   Event BeginPlay
   → Get Item Database
   → Get Item Data Asset
      ├─ Target: (Item Database output)
      └─ Item ID: Make Literal Name ("HealthPotion")
   → Break Item Data Asset
      └─ Item Data Asset: (from Get Item Data Asset)
   → Print String
      └─ In String: Item Name (from Break node)
   ```
   
   **Visual Guide:**
   - `Event BeginPlay` node (white)
   - `Get Item Database` node (blue - static function)
   - `Get Item Data Asset` node (blue - member function)
   - `Make Literal Name` node (green - set to "HealthPotion")
   - `Break Item Data Asset` node (yellow - breaks struct)
   - `Print String` node (white - prints to screen/log)

   **Option B: Test in C++ (if you add test code)**
   
   Add this to `ActionRPGPlayerCharacter.cpp` in `BeginPlay()`:
   ```cpp
   // Test Item Database
   if (UItemDatabase* ItemDB = UItemDatabase::Get())
   {
       if (UItemDataAsset* HealthPotion = ItemDB->GetItemDataAsset(FName("HealthPotion")))
       {
           UE_LOG(LogTemp, Log, TEXT("Found item: %s"), *HealthPotion->ItemName.ToString());
       }
       else
       {
           UE_LOG(LogTemp, Warning, TEXT("HealthPotion not found in database"));
       }
   }
   ```

   **Expected Results:**
   - If item exists: Print String should show "Health Potion" (or your item name)
   - If item doesn't exist: Item Data Asset will be NULL, Print String may show empty or error
   - Check Output Log for "ItemDatabase: Registered item..." messages on game start
   - Check Output Log for "ItemDatabase: Item not found..." if lookup fails

### 4.2 Test Item Creation

1. **In Blueprint Event Graph:**
   
   **Step-by-Step:**
   
   1. **Get Item Database** (same as Step 4.1)
      - Add `Get Item Database` node
   
   2. **Create Item**
      - Drag from Item Database output
      - Search for `Create Item` node
      - Connect Item Database to `Target` input
      - For `Item ID`: Use `Make Literal Name` with value `HealthPotion`
      - For `Quantity`: Use `Make Literal Int` with value `5` (or type directly)
   
   3. **Verify Item Creation**
      - Drag from `Item` output pin
      - Add `Break Item Base` node (if available) or check properties
      - Or add `Is Valid` node to check if item was created
      - Connect to `Print String` to show result
   
   4. **Check Item Properties**
      - From the created Item, you can access:
        - `Get Item ID` - Should return "HealthPotion"
        - `Get Item Name` - Should return "Health Potion"
        - `Get Item Type` - Should return "Consumable"
        - `Quantity` - Should be 5
   
   **Example Node Setup:**
   ```
   Event BeginPlay
   → Get Item Database
   → Create Item
      ├─ Target: (Item Database)
      ├─ Item ID: Make Literal Name ("HealthPotion")
      └─ Quantity: Make Literal Int (5)
   → Get Item Name (from Item output)
   → Print String (Item Name)
   ```

2. **Test Item Usage:**
   
   **In Blueprint:**
   - From the created Item output, search for `Use` node
   - Connect the Item to the `Target` input
   - Add `Print String` after to verify it executed
   - Or bind to `On Item Used` event (if using event dispatcher)
   
   **Expected Results:**
   - Console should show: "ItemBase::Use - Item used: Health Potion"
   - `OnItemUsed` event should fire (if you bind to it)
   - Check Output Log for usage messages

---

## Step 5: Verify Item Database Initialization

### 5.1 Check Console Logs

1. **Open Output Log**
   - Window → Developer Tools → `Output Log`
   - Or press `Ctrl + Shift + L`

2. **Look for Item Database Logs:**
   - When game starts, you should see:
     - `"ItemDatabase: Registered item HealthPotion"`
     - `"ItemDatabase: Registered item SkillItem_Fireball"`
     - `"ItemDatabase: Registered item Weapon_Sword"`

3. **If Items Not Found:**
   - Check Asset Manager configuration
   - Verify Item Data Assets are in correct folder
   - Check Item ID is set correctly
   - Verify Primary Asset Type matches

---

## Troubleshooting

### Issue: ItemDataAsset Doesn't Appear in Data Asset Creation

**Solution:**
1. Close and reopen Unreal Editor
2. Recompile C++ code
3. The class should appear after recompilation
4. If still not appearing, check that the class is marked with `UCLASS(BlueprintType)`

### Issue: Asset Manager Doesn't Find Items

**Checklist:**
- ✅ Primary Asset Type `Item` is configured
- ✅ Directory `/Game/Data/Items/` is in scan list
- ✅ Item Data Assets are in `Content/Data/Items/` folder
- ✅ Item ID is set (not empty)
- ✅ Item Type (Primary Asset Type) is set to `Item`
- ✅ Editor has been restarted after configuration

### Issue: ItemDatabase Returns NULL

**Solutions:**
1. Check that `UItemDatabase::Get()` is called after game starts
2. Verify Asset Manager is initialized
3. Check console logs for initialization errors
4. Verify items are registered in the database

### Issue: CreateItem Returns NULL

**Checklist:**
- ✅ Item ID matches exactly (case-sensitive)
- ✅ Item exists in ItemDatabase registry
- ✅ ItemDataAsset is valid
- ✅ Check console for "Item not found" warnings

### Issue: "Get Item Database" Node Not Found in Blueprint

**Solution:**
1. Make sure C++ code is compiled
2. Close and reopen the Blueprint
3. Try searching for "Item Database" (without "Get")
4. The static function should appear as a blue node
5. If still not found, restart the editor

### Issue: "Get Item Data Asset" Node Not Found

**Solution:**
1. Make sure you have the Item Database reference first
2. Drag from the Item Database output pin
3. Search for "Get Item Data Asset" in the context menu
4. The function should appear as a member function of ItemDatabase

### Issue: Item ID Input Doesn't Accept String

**Solution:**
- Item ID is an FName, not FString
- Use `Make Literal Name` node instead of `Make Literal String`
- Or right-click on the pin and select "Set Value" to type directly
- The value should be: `HealthPotion` (no quotes needed when typing directly)

---

## Verification Checklist

Before considering Days 7-8 complete, verify:

- [ ] Asset Manager configured with Primary Asset Type `Item`
- [ ] At least 3 test Item Data Assets created
- [ ] Item Data Assets have unique Item IDs
- [ ] Item Data Assets are in `Content/Data/Items/` folder
- [ ] ItemDatabase initializes and finds items (check logs)
- [ ] Can lookup items by ID using ItemDatabase
- [ ] Can create ItemBase instances from ItemDataAsset
- [ ] Item usage works (Use() function)
- [ ] No errors in console related to items

---

## Quick Reference: Item Data Asset Properties

When creating Item Data Assets, configure these properties:

| Property | Type | Description | Example |
|----------|------|-------------|---------|
| Item Type | Primary Asset Type | Must be "Item" | `Item` |
| Item ID | FName | Unique identifier | `HealthPotion` |
| Item Name | FText | Display name | `Health Potion` |
| Item Description | FText | Tooltip description | `Restores health` |
| Item Icon | Texture2D | Item icon image | (Optional) |
| Type | EItemType | Item category | `Consumable` |
| Rarity | EItemRarity | Item rarity | `Common` |
| Max Stack Size | int32 | Max quantity per stack | `10` |
| Weight | float | Item weight | `0.5` |
| Value | int32 | Item value/price | `25` |

---

## Code Reference

### Key Files Created
- `Source/ActionRPG/Public/Items/Core/ItemTypes.h` - Enums and types
- `Source/ActionRPG/Public/Items/Core/ItemDataAsset.h` - Data asset class
- `Source/ActionRPG/Private/Items/Core/ItemDataAsset.cpp` - Data asset implementation
- `Source/ActionRPG/Public/Items/Core/ItemBase.h` - Base item class
- `Source/ActionRPG/Private/Items/Core/ItemBase.cpp` - Base item implementation
- `Source/ActionRPG/Public/Data/ItemDatabase.h` - Database singleton
- `Source/ActionRPG/Private/Data/ItemDatabase.cpp` - Database implementation

### Key Functions
- `UItemDatabase::Get()` - Get singleton instance
- `GetItemDataAsset(ItemID)` - Lookup item by ID
- `CreateItem(ItemID, Quantity)` - Create item instance
- `UItemBase::Use()` - Use the item
- `UItemBase::CanUse()` - Check if item can be used

---

## Next Steps

After completing Days 7-8:

1. **Days 9-10** will focus on:
   - Skill System Foundation
   - SkillDataAsset and SkillBase classes
   - SkillDatabase implementation

2. **Before Days 9-10**, you should have:
   - ✅ Item system working
   - ✅ ItemDatabase functional
   - ✅ Can create and use items
   - ✅ Ready to implement skill system

---

## Notes

- **Primary Asset Types:** Must be configured in Project Settings for Asset Manager to work
- **Item IDs:** Must be unique and match exactly (case-sensitive)
- **Asset Manager:** Automatically scans and registers items on startup
- **ItemDatabase:** Singleton pattern - only one instance exists
- **ItemBase:** Can be created from ItemDataAsset using ItemDatabase

---

**Status:** Complete these manual steps to finish Days 7-8

