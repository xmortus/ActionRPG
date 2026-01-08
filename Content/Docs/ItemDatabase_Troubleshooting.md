# ItemDatabase Troubleshooting Guide
**Date:** 2025-01-07

---

## Issue: "ItemDatabase: Item not found: HealthPotion"

This error means the ItemDatabase cannot find an item with the ID "HealthPotion". Follow these steps to diagnose and fix the issue.

---

## Issue: "Failed to cast asset to ItemDataAsset"

**Symptom:** Log shows:
```
ItemDatabase: Found 3 item assets to process
ItemDatabase: Failed to cast asset to ItemDataAsset: Item:Item_HealthPotion
ItemDatabase: Initialization complete. Registered 0 items.
```

**Cause:** The assets exist but are not of type `ItemDataAsset`. This usually means:
1. Assets were created as Blueprint classes instead of Data Assets
2. Assets were created as regular `DataAsset` instead of `ItemDataAsset`
3. Assets are not properly configured

**Solution:**

### Step 1: Check Asset Type

1. **Open one of the failing assets** (e.g., `Item_HealthPotion`)
2. **Check the asset type in the Content Browser:**
   - Look at the icon - should be a blue data asset icon
   - If it's a Blueprint icon (yellow/blue), it's a Blueprint class (wrong!)

3. **Check the Details panel:**
   - Look at the top - should say "Item Data Asset"
   - If it says "Blueprint" or "Blueprint Class", it's wrong

### Step 2: Verify Asset Creation Method

**Correct Method:**
1. Right-click in Content Browser
2. Select: `Miscellaneous` → `Data Asset`
3. Choose: `Item Data Asset` (not `Data Asset` or `Blueprint Class`)
4. Name it: `DA_Item_Consumable_HealthPotion`

**Wrong Methods:**
- ❌ Creating a Blueprint class that inherits from `ItemDataAsset`
- ❌ Creating a regular `DataAsset` instead of `ItemDataAsset`
- ❌ Creating a Blueprint from `ItemDataAsset`

### Step 3: Check Asset Manager Configuration

1. **Open Project Settings** → `Game` → `Asset Manager`
2. **Find the "Item" Primary Asset Type**
3. **Verify:**
   - **Asset Base Class:** `/Script/ActionRPG.ItemDataAsset`
   - **Has Blueprint Classes:** `False` (should be unchecked)
   - **Directories to Scan:** Should include `/Game/Data/Items/`

### Step 4: Recreate Assets (If Needed)

If assets are wrong type:

1. **Delete the incorrect assets**
2. **Create new assets using correct method:**
   - Right-click → `Miscellaneous` → `Data Asset`
   - Select `Item Data Asset`
   - Configure properties
   - Set **Item Type** (Primary Asset Type) to `Item`
   - Set **Item ID** to `HealthPotion` (or appropriate ID)

3. **Save and restart editor**

### Step 5: Verify After Recompiling

After recompiling with the enhanced logging:

1. **Play the game**
2. **Check Output Log** for messages like:
   ```
   ItemDatabase: Found asset - ID: Item:Item_HealthPotion, Class: ItemDataAsset_C
   ```
   or
   ```
   ItemDatabase: Found asset - ID: Item:Item_HealthPotion, Class: BlueprintGeneratedClass
   ```

3. **If Class is `BlueprintGeneratedClass`:**
   - The asset is a Blueprint class (wrong!)
   - Delete and recreate as Data Asset

4. **If Class is `ItemDataAsset_C`:**
   - This is a Blueprint class wrapper (might work, but not recommended)
   - Better to use pure C++ Data Assets

5. **If Class is `ItemDataAsset`:**
   - This is correct!
   - Should cast successfully

---

## Issue: "ItemDatabase: Item not found: HealthPotion"

This error means the ItemDatabase cannot find an item with the ID "HealthPotion". Follow these steps to diagnose and fix the issue.

---

## Diagnostic Steps

### Step 1: Check Console Logs on Game Start

When the game starts, look for these messages in the Output Log:

**Expected Messages:**
- `"ItemDatabase: Initializing..."`
- `"ItemDatabase: AssetManager found, scanning for items..."`
- `"ItemDatabase: Found X item assets to process"`
- `"ItemDatabase: Registered item - ID: HealthPotion, Name: Health Potion"`
- `"ItemDatabase: Initialization complete. Registered X items."`

**If you see:**
- `"ItemDatabase: Found 0 item assets to process"` → Items not being scanned
- `"ItemDatabase: AssetManager is not initialized"` → Asset Manager not configured
- `"ItemDatabase: Item Data Asset has empty ItemID"` → Item Data Asset missing Item ID

---

## Common Causes and Solutions

### Cause 1: Item Data Asset Not Created

**Symptom:** No items registered in database

**Solution:**
1. Create the Item Data Asset in `Content/Data/Items/`
2. Name it: `DA_Item_Consumable_HealthPotion`
3. Set **Item ID** to: `HealthPotion` (exactly, case-sensitive)
4. Set **Item Type** (Primary Asset Type) to: `Item`
5. Save the asset
6. Restart the editor

---

### Cause 2: Asset Manager Not Configured

**Symptom:** "Found 0 item assets to process"

**Solution:**
1. **Open Project Settings**
   - Edit → Project Settings
   - Or `Ctrl + ,`

2. **Navigate to Game → Asset Manager**

3. **Add Primary Asset Type:**
   - Click `+` in "Primary Asset Types to Scan"
   - Set:
     - **Primary Asset Type:** `Item`
     - **Asset Base Class:** `ItemDataAsset` (or `DataAsset` if ItemDataAsset doesn't appear)
     - **Directories to Scan:** 
       - Click `+` to add directory
       - Set to: `/Game/Data/Items/`
     - **Rules:** Leave as default

4. **Save Project Settings**

5. **Restart Unreal Editor** (required for Asset Manager to rescan)

---

### Cause 3: Item ID Mismatch

**Symptom:** Item exists but wrong ID

**Check:**
1. Open your Item Data Asset (`DA_Item_Consumable_HealthPotion`)
2. Verify **Item ID** field:
   - Should be exactly: `HealthPotion`
   - Case-sensitive (HealthPotion ≠ healthpotion)
   - No extra spaces
   - Not empty

3. **In Blueprint**, verify you're using the same ID:
   - `Make Literal Name` node should have value: `HealthPotion`
   - Must match exactly (case-sensitive)

---

### Cause 4: Item Type (Primary Asset Type) Not Set

**Symptom:** Item exists but not scanned by Asset Manager

**Solution:**
1. Open your Item Data Asset
2. In the **Details** panel, find **Item Type** (Primary Asset Type)
3. Set it to: `Item` (must match the Primary Asset Type in Project Settings)
4. Save the asset
5. Restart editor

---

### Cause 5: Item Not in Correct Folder

**Symptom:** Asset Manager doesn't find items

**Solution:**
1. Verify item is in: `Content/Data/Items/`
2. Check Project Settings → Asset Manager → Directories to Scan
3. Should include: `/Game/Data/Items/`
4. If folder structure is different, update the scan directory

---

## Step-by-Step Fix

### Complete Setup Checklist

1. **Create Item Data Asset:**
   - [ ] Created in `Content/Data/Items/`
   - [ ] Named: `DA_Item_Consumable_HealthPotion`
   - [ ] **Item Type** set to `Item`
   - [ ] **Item ID** set to `HealthPotion`
   - [ ] **Item Name** set to `Health Potion`
   - [ ] All other properties configured
   - [ ] Asset saved

2. **Configure Asset Manager:**
   - [ ] Primary Asset Type `Item` exists
   - [ ] Directory `/Game/Data/Items/` is in scan list
   - [ ] Base class is set correctly
   - [ ] Project Settings saved

3. **Restart Editor:**
   - [ ] Closed Unreal Editor completely
   - [ ] Reopened editor
   - [ ] Editor finished loading

4. **Verify Registration:**
   - [ ] Play in Editor
   - [ ] Check Output Log for registration messages
   - [ ] Should see: "Registered item - ID: HealthPotion"

5. **Test Lookup:**
   - [ ] Blueprint calls `Get Item Database`
   - [ ] Calls `Get Item Data Asset`
   - [ ] Item ID is exactly `HealthPotion`
   - [ ] Item is found (not NULL)

---

## Manual Registration (Fallback)

If Asset Manager isn't working, you can manually register items:

### Option 1: Register in Blueprint

1. **In Blueprint BeginPlay:**
   - Get Item Database
   - Call a custom function to register items manually
   - (This requires adding a manual registration function)

### Option 2: Register in C++

Add this to a character's BeginPlay:

```cpp
// Manual registration (temporary workaround)
if (UItemDatabase* ItemDB = UItemDatabase::Get())
{
    // Load item manually
    UItemDataAsset* HealthPotion = LoadObject<UItemDataAsset>(nullptr, TEXT("/Game/Data/Items/DA_Item_Consumable_HealthPotion.DA_Item_Consumable_HealthPotion"));
    if (HealthPotion)
    {
        // Manually add to registry (requires making ItemRegistry public or adding a registration function)
    }
}
```

**Note:** This is a workaround. The proper solution is to fix Asset Manager configuration.

---

## Verification Test

### Quick Test in Blueprint

1. **Add to BeginPlay:**
   ```
   Event BeginPlay
   → Get Item Database
   → Get All Item Data Assets
   → Get Array Length
   → Print String (Array Length)
   ```

2. **Expected Result:**
   - Should print the number of registered items
   - If 0, items aren't being registered
   - If > 0, items are registered but ID might be wrong

3. **List All Items:**
   ```
   Event BeginPlay
   → Get Item Database
   → Get All Item Data Assets
   → ForEach Loop
      → Break Item Data Asset
      → Print String (Item ID)
   ```

4. **Expected Result:**
   - Should print all registered Item IDs
   - Verify "HealthPotion" is in the list
   - Check exact spelling and case

---

## Common Mistakes

### Mistake 1: Item ID Has Spaces
- ❌ `"Health Potion"` (has space)
- ✅ `"HealthPotion"` (no space)

### Mistake 2: Wrong Case
- ❌ `"healthpotion"` (lowercase)
- ❌ `"HEALTHPOTION"` (uppercase)
- ✅ `"HealthPotion"` (exact match)

### Mistake 3: Item Type Not Set
- ❌ Item Type is empty
- ✅ Item Type is `Item`

### Mistake 4: Wrong Folder
- ❌ Item in `Content/Items/`
- ✅ Item in `Content/Data/Items/`

### Mistake 5: Asset Manager Not Restarted
- ❌ Changed settings but didn't restart
- ✅ Restart editor after configuration changes

---

## Still Not Working?

If items still aren't found after checking everything:

1. **Verify Item Data Asset Exists:**
   - Check `Content/Data/Items/` folder
   - Verify file exists: `DA_Item_Consumable_HealthPotion.uasset`

2. **Check Item Data Asset Properties:**
   - Open the asset
   - Verify all properties are set correctly
   - Check for any validation errors

3. **Recompile C++ Code:**
   - Close editor
   - Rebuild in Visual Studio
   - Reopen editor

4. **Check for Errors:**
   - Look for red error messages in Output Log
   - Check for asset loading errors
   - Verify no circular dependencies

5. **Try Different Item ID:**
   - Create a test item with ID: `TestItem`
   - See if that one works
   - If it works, the issue is with the specific Item ID

---

**Last Updated:** 2025-01-07

