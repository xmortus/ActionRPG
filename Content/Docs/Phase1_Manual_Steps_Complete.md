# Phase 1: Foundation - Complete Manual Steps Guide
**Project:** ActionRPG  
**Phase:** Phase 1 - Foundation  
**Duration:** 14 days (Days 1-14)  
**Date:** 2025-01-07

---

## Table of Contents

1. [Overview](#overview)
2. [Days 1-4: Blueprint Classes & Project Setup](#days-1-4-blueprint-classes--project-setup)
3. [Days 5-6: Enhanced Input System Setup](#days-5-6-enhanced-input-system-setup)
4. [Days 7-8: Item System Foundation](#days-7-8-item-system-foundation)
5. [Days 9-10: Skill System Foundation](#days-9-10-skill-system-foundation)
6. [Days 11-12: Character Classes & Integration](#days-11-12-character-classes--integration)
7. [Troubleshooting Guide](#troubleshooting-guide)
8. [Quick Reference Tables](#quick-reference-tables)

---

## Overview

This guide covers all manual steps required in the Unreal Editor to complete Phase 1 of the ActionRPG project. These steps must be performed after the C++ code implementation is complete.

**Important:** These are editor-only steps. The C++ code should already be compiled and ready before starting these manual steps.

---

## Days 1-4: Blueprint Classes & Project Setup

**After Code Implementation**  
**Prerequisites:** ✅ All C++ code compiled successfully

---

### Step 1: Create Blueprint Classes from C++ Classes

#### 1.1 Create Blueprint GameMode

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - In the "Pick Parent Class" dialog, search for `ActionRPGGameMode`
   - Select `ActionRPGGameMode` (not the C++ class, the one that appears in the list)
   - Click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGGameMode`
   - Save it in `Content/Blueprints/Core/`

4. **Configure the Blueprint** (Optional for now)
   - Double-click to open
   - No changes needed yet - leave as default
   - Close the Blueprint editor

#### 1.2 Create Blueprint PlayerController

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - Search for `ActionRPGPlayerController`
   - Select it and click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGPlayerController`
   - Save it in `Content/Blueprints/Core/`

4. **Configure the Blueprint** (We'll add Input Actions later)
   - Double-click to open
   - Note: We'll assign Input Actions in Step 3
   - Close the Blueprint editor for now

#### 1.3 Create Blueprint Character Classes

**Create Base Character Blueprint:**

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Characters/` folder

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - Search for `ActionRPGCharacter`
   - Select it and click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGCharacter`
   - Save it in `Content/Blueprints/Characters/`

4. **Close the Blueprint** (No configuration needed yet)

**Create Player Character Blueprint:**

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Characters/` folder

2. **Create Blueprint from C++ Class**
   - Right-click in Content Browser
   - Select `Blueprint Class`
   - Search for `ActionRPGPlayerCharacter`
   - Select it and click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGPlayerCharacter`
   - Save it in `Content/Blueprints/Characters/`

4. **Configure the Blueprint**
   - Double-click to open
   - In the `Details` panel, you can adjust:
     - Movement settings (if needed)
     - Mesh (add a skeletal mesh if you have one)
   - Close the Blueprint editor

#### 1.4 Create Blueprint GameState (Optional)

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder

2. **Create Blueprint from C++ Class**
   - Right-click → `Blueprint Class`
   - Search for `ActionRPGGameState`
   - Select it and click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGGameState`
   - Save it in `Content/Blueprints/Core/`

#### 1.5 Create Blueprint HUD (Optional)

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder (or create `Content/Blueprints/UI/`)

2. **Create Blueprint from C++ Class**
   - Right-click → `Blueprint Class`
   - Search for `ActionRPGHUD`
   - Select it and click `Select`

3. **Name the Blueprint**
   - Name it: `BP_ActionRPGHUD`
   - Save it in `Content/Blueprints/Core/` or `Content/Blueprints/UI/`

---

### Step 2: Configure Project Settings

#### 2.1 Set Default GameMode

1. **Open Project Settings**
   - Go to `Edit` → `Project Settings`
   - Or press `Ctrl + ,` (comma)

2. **Navigate to Game Mode**
   - In the left panel, expand `Game` section
   - Click on `Game Mode`

3. **Set Default GameMode**
   - Find `Default Modes` section
   - Set `Default GameMode` to `BP_ActionRPGGameMode`
   - (If it doesn't appear, make sure you compiled the C++ code and restarted the editor)

4. **Verify Default Classes** (Should be auto-set by GameMode)
   - `Default Pawn Class` should show `BP_ActionRPGPlayerCharacter` (or ActionRPGPlayerCharacter)
   - `Player Controller Class` should show `BP_ActionRPGPlayerController` (or ActionRPGPlayerController)
   - `HUD Class` should show `BP_ActionRPGHUD` (or ActionRPGHUD)

5. **Close Project Settings**

---

### Step 3: Create Input Actions (Content/Input/)

#### 3.1 Create IA_Move (Vector2D Input Action)

1. **Navigate to Content Browser**
   - Go to `Content/Input/` folder

2. **Create Input Action**
   - Right-click in Content Browser
   - Select `Input` → `Input Action`
   - Name it: `IA_Move`

3. **Configure IA_Move**
   - Double-click `IA_Move` to open
   - Set `Value Type` to `Vector2D`
   - Leave other settings as default
   - Save and close

#### 3.2 Create IA_Look (Vector2D Input Action)

1. **Create Input Action**
   - Right-click in `Content/Input/`
   - Select `Input` → `Input Action`
   - Name it: `IA_Look`

2. **Configure IA_Look**
   - Double-click `IA_Look` to open
   - Set `Value Type` to `Vector2D`
   - Save and close

#### 3.3 Create Boolean Input Actions

Create the following Input Actions with `Value Type` set to `Boolean`:

1. **IA_Interact**
   - Right-click in `Content/Input/`
   - `Input` → `Input Action`
   - Name: `IA_Interact`
   - Value Type: `Boolean`
   - Save and close

2. **IA_Attack**
   - Name: `IA_Attack`
   - Value Type: `Boolean`
   - Save and close

3. **IA_Dodge**
   - Name: `IA_Dodge`
   - Value Type: `Boolean`
   - Save and close

4. **IA_OpenInventory**
   - Name: `IA_OpenInventory`
   - Value Type: `Boolean`
   - Save and close

#### 3.4 Create Skill Slot Input Actions (Optional for Phase 1)

You can create these now or wait until Phase 2:

- `IA_SkillSlot1` through `IA_SkillSlot8`
- All with `Value Type: Boolean`

---

### Step 4: Create Input Mapping Context

#### 4.1 Create IM_ActionRPG

1. **Navigate to Content Browser**
   - Go to `Content/Input/` folder

2. **Create Input Mapping Context**
   - Right-click in Content Browser
   - Select `Input` → `Input Mapping Context`
   - Name it: `IM_ActionRPG`

3. **Open IM_ActionRPG**
   - Double-click `IM_ActionRPG` to open

#### 4.2 Map Keyboard/Mouse Inputs

In the Input Mapping Context editor:

1. **Add IA_Move Mapping**
   - Click `+` button to add a new mapping
   - Set `Action` to `IA_Move`
   - Click `+` under `Key` to add key mappings:
     - Add `W` key → No modifiers needed (W = forward = positive X)
     - Add `S` key → Click `+` under `Modifiers` → Select `Negate` → Check `X` axis
     - Add `A` key → Click `+` under `Modifiers` → Select `Negate` → Check `Y` axis
     - Add `D` key → No modifiers needed (D = right = positive Y)

2. **Add IA_Look Mapping**
   - Click `+` to add mapping
   - Set `Action` to `IA_Look`
   - Add `Mouse XY` → No modifiers needed

3. **Add IA_Interact Mapping**
   - Click `+` to add mapping
   - Set `Action` to `IA_Interact`
   - Add `E` key

4. **Add IA_Attack Mapping**
   - Click `+` to add mapping
   - Set `Action` to `IA_Attack`
   - Add `Left Mouse Button`

5. **Add IA_Dodge Mapping**
   - Click `+` to add mapping
   - Set `Action` to `IA_Dodge`
   - Add `Space Bar`

6. **Add IA_OpenInventory Mapping**
   - Click `+` to add mapping
   - Set `Action` to `IA_OpenInventory`
   - Add `Tab` key

7. **Add Skill Slot Mappings (Optional)**
   - For each `IA_SkillSlot1` through `IA_SkillSlot8`:
     - Add mapping
     - Set Action to corresponding `IA_SkillSlot#`
     - Map to number keys `1` through `8`

8. **Save and Close** the Input Mapping Context

---

### Step 5: Assign Input Actions to PlayerController

#### 5.1 Open BP_ActionRPGPlayerController

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/`
   - Double-click `BP_ActionRPGPlayerController` to open

#### 5.2 Assign Input Actions

1. **Select the Blueprint Root**
   - Click on the root node (the class name at the top)

2. **In the Details Panel**, find the `Input` category

3. **Assign Input Actions:**
   - `Default Mapping Context` → Set to `IM_ActionRPG`
   - `Move Action` → Set to `IA_Move`
   - `Look Action` → Set to `IA_Look`
   - `Interact Action` → Set to `IA_Interact`
   - `Attack Action` → Set to `IA_Attack`
   - `Dodge Action` → Set to `IA_Dodge`
   - `Open Inventory Action` → Set to `IA_OpenInventory`
   - `Skill Slot 1 Action` through `Skill Slot 8 Action` → Set to corresponding `IA_SkillSlot#` (if created)

4. **Compile the Blueprint**
   - Click `Compile` button (top toolbar)

5. **Save and Close** the Blueprint

---

### Step 6: Test the Setup

#### 6.1 Create a Test Level (or use existing)

1. **Open or Create a Level**
   - File → New Level → Choose `Empty Level` or use existing

2. **Add a Player Start**
   - In the `Place Actors` panel, search for `Player Start`
   - Drag it into the level
   - Position it where you want the player to spawn

#### 6.2 Test in Editor

1. **Play in Editor**
   - Click `Play` button (top toolbar)
   - Or press `Alt + P`

2. **Test Movement**
   - Press `W/A/S/D` keys
   - Character should move
   - Check console for input logs (if you see the log messages, input is working)

3. **Test Other Inputs**
   - Press `E` → Should see "Interact pressed" in console
   - Press `Left Click` → Should see "Attack pressed" in console
   - Press `Space` → Should see "Dodge pressed" in console
   - Press `Tab` → Should see "Open Inventory pressed" in console

4. **Stop Play**
   - Press `Esc` or click `Stop` button

#### 6.3 Verify Console Output

1. **Open Output Log**
   - Window → `Developer Tools` → `Output Log`
   - Or press `Ctrl + Shift + L`

2. **Check for Log Messages**
   - You should see log messages when pressing action keys
   - If you don't see messages, check:
     - Input Actions are assigned correctly
     - Input Mapping Context is set
     - Blueprint is compiled

---

### Verification Checklist for Days 1-4

Before moving to Days 5-6, verify:

- [ ] All Blueprint classes created from C++ classes
- [ ] GameMode set in Project Settings
- [ ] All Input Actions created (IA_Move, IA_Look, IA_Interact, IA_Attack, IA_Dodge, IA_OpenInventory)
- [ ] Input Mapping Context created and configured (IM_ActionRPG)
- [ ] Input Actions assigned to PlayerController Blueprint
- [ ] PlayerController Blueprint compiled
- [ ] Character moves with W/A/S/D keys
- [ ] Console logs appear when pressing action keys
- [ ] No errors in Output Log

---

## Days 5-6: Enhanced Input System Setup

**After Code Implementation**  
**Prerequisites:** ✅ Days 1-4 complete, C++ code compiled successfully

---

### Step 1: Verify Input Actions (Already Created in Days 1-4)

If you haven't created all Input Actions yet, refer to **Days 1-4, Step 3** above.

**Required Input Actions:**
- ✅ `IA_Move` (Vector2D)
- ✅ `IA_Look` (Vector2D)
- ✅ `IA_Interact` (Boolean)
- ✅ `IA_Attack` (Boolean)
- ✅ `IA_Dodge` (Boolean)
- ✅ `IA_OpenInventory` (Boolean)
- ✅ `IA_SkillSlot1-8` (Boolean) - Optional

---

### Step 2: Verify Input Mapping Context (Already Created in Days 1-4)

If you haven't created the Input Mapping Context yet, refer to **Days 1-4, Step 4** above.

**Required Mappings:**
- ✅ W/A/S/D → `IA_Move`
- ✅ Mouse XY → `IA_Look`
- ✅ E → `IA_Interact`
- ✅ Left Mouse → `IA_Attack`
- ✅ Space → `IA_Dodge`
- ✅ Tab → `IA_OpenInventory`
- ✅ 1-8 → `IA_SkillSlot1-8` (Optional)

---

### Step 3: Verify PlayerController Configuration

#### 3.1 Open BP_ActionRPGPlayerController

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/`
   - Double-click `BP_ActionRPGPlayerController` to open

#### 3.2 Verify Input Actions Assignment

1. **Select the Blueprint Root**
   - Click on the class name at the top of the Blueprint editor

2. **In the Details Panel**, scroll to find the `Input` category

3. **Verify All Input Actions Are Assigned:**
   - **Default Mapping Context:** Should be `IM_ActionRPG`
   - **Move Action:** Should be `IA_Move`
   - **Look Action:** Should be `IA_Look`
   - **Interact Action:** Should be `IA_Interact`
   - **Attack Action:** Should be `IA_Attack`
   - **Dodge Action:** Should be `IA_Dodge`
   - **Open Inventory Action:** Should be `IA_OpenInventory`
   - **Skill Slot Actions:** Should be assigned to `IA_SkillSlot1-8` (if created)

4. **Compile the Blueprint**
   - Click the `Compile` button in the top toolbar
   - Check for any errors (should be none if everything is assigned)

5. **Save the Blueprint**
   - Click `Save` (Ctrl+S)
   - Close the Blueprint editor

---

### Step 4: Test the Input System

#### 4.1 Create/Open a Test Level

1. **Open or Create a Level**
   - File → New Level → `Empty Level`
   - Or use an existing level

2. **Add Player Start**
   - In the `Place Actors` panel, search for `Player Start`
   - Drag it into the level
   - Position it where you want the player to spawn

#### 4.2 Test in Play Mode

1. **Open Output Log**
   - Window → Developer Tools → `Output Log`
   - Or press `Ctrl + Shift + L`
   - Set log level to show `Log` and `Warning` messages

2. **Play in Editor**
   - Click `Play` button (top toolbar)
   - Or press `Alt + P`

3. **Test Movement (W/A/S/D)**
   - Press `W` → Character should move forward
   - Press `S` → Character should move backward
   - Press `A` → Character should move left
   - Press `D` → Character should move right
   - Check Output Log for "OnMove called" messages

4. **Test Mouse Look**
   - Move mouse around
   - Character should rotate to face mouse cursor (if implemented)
   - Check Output Log for any look-related messages

5. **Test Action Inputs**
   - Press `E` → Should see "Interact pressed" in console
   - Press `Left Click` → Should see "Attack pressed" in console
   - Press `Space` → Should see "Dodge pressed" in console
   - Press `Tab` → Should see "Open Inventory pressed" in console

6. **Stop Play**
   - Press `Esc` or click `Stop` button

#### 4.3 Verify Console Output

In the Output Log, you should see:

**On Startup:**
- `"ActionRPGPlayerController BeginPlay"`
- `"Input Mapping Context added successfully"`
- `"SetupInputComponent called"`
- `"EnhancedInputComponent found"`
- `"MoveAction bound"`
- `"LookAction bound"`

**When Pressing Keys:**
- `"OnMove called: X=..., Y=..."` when pressing W/A/S/D
- `"Interact pressed"` when pressing E
- `"Attack pressed"` when clicking left mouse
- `"Dodge pressed"` when pressing Space
- `"Open Inventory pressed"` when pressing Tab

---

### Verification Checklist for Days 5-6

Before considering Days 5-6 complete, verify:

- [ ] All Input Actions created (`IA_Move`, `IA_Look`, `IA_Interact`, `IA_Attack`, `IA_Dodge`, `IA_OpenInventory`)
- [ ] `IA_Move` is Vector2D type
- [ ] All other actions are Boolean type
- [ ] `IM_ActionRPG` Input Mapping Context created
- [ ] W/A/S/D keys mapped to `IA_Move` in `IM_ActionRPG`
- [ ] Mouse XY mapped to `IA_Look` in `IM_ActionRPG`
- [ ] E key mapped to `IA_Interact`
- [ ] Left Mouse Button mapped to `IA_Attack`
- [ ] Space Bar mapped to `IA_Dodge`
- [ ] Tab key mapped to `IA_OpenInventory`
- [ ] `BP_ActionRPGPlayerController` has all Input Actions assigned
- [ ] `BP_ActionRPGPlayerController` has `Default Mapping Context` = `IM_ActionRPG`
- [ ] `BP_ActionRPGPlayerController` is compiled
- [ ] Character moves with W/A/S/D keys
- [ ] Console logs show input when pressing keys
- [ ] All action inputs trigger console messages

---

## Days 7-8: Item System Foundation

**After Code Implementation**  
**Prerequisites:** ✅ Days 1-6 complete, C++ code compiled successfully

---

### Step 1: Configure Asset Manager for Primary Assets

#### 1.1 Open Project Settings

1. **Open Project Settings**
   - Go to `Edit` → `Project Settings`
   - Or press `Ctrl + ,`

2. **Navigate to Game → Asset Manager**
   - In the left panel, expand `Game` section
   - Click on `Asset Manager`

#### 1.2 Configure Primary Asset Types

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

### Step 2: Create Test Item Data Assets

#### 2.1 Navigate to Data Folder

1. **Go to Content Browser**
   - Navigate to `Content/Data/Items/` folder
   - (Create the folder if it doesn't exist)

#### 2.2 Create Test Item: Health Potion

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

#### 2.3 Create Test Item: Skill Item

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

#### 2.4 Create Test Item: Weapon

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

### Step 3: Verify Asset Manager Configuration

#### 3.1 Check Primary Asset Type

1. **Open Project Settings** → `Game` → `Asset Manager`

2. **Verify Configuration:**
   - Primary Asset Type `Item` exists
   - Directory `/Game/Data/Items/` is in the scan list
   - Base class is set correctly

#### 3.2 Scan for Assets

1. **In Asset Manager Settings:**
   - Click `Scan Primary Asset Types` button (if available)
   - Or restart the editor to trigger automatic scan

2. **Verify Items are Found:**
   - The Asset Manager should detect your Item Data Assets
   - Check the Output Log for scan results

---

### Step 4: Test Item System in Blueprint

#### 4.1 Test Item Database

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

4. **Call Get Item Data Asset**
   - Search for `Get Item Data Asset` node
   - Connect the `Item Database` output to the `Target` input
   - For `Item ID` input:
     - Right-click on the `Item ID` pin
     - Select `Make Literal Name`
     - Set value to `HealthPotion`
     - Or type directly: Right-click on pin → `Set Value` → Type `HealthPotion`

5. **Print Result**
   - Drag from the `Item Data Asset` output pin
   - Search for `Break Item Data Asset` node
   - Connect the Item Data Asset output
   - This gives you access to all properties (Name, Description, Type, etc.)
   - Connect `Item Name` to `Print String` to verify

6. **Compile and Test**
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

#### 4.2 Test Item Creation

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
      - Add `Is Valid` node to check if item was created
      - Connect to `Print String` to show result

   4. **Check Item Properties**
      - From the created Item, you can access:
        - `Get Item ID` - Should return "HealthPotion"
        - `Get Item Name` - Should return "Health Potion"
        - `Get Item Type` - Should return "Consumable"
        - `Quantity` - Should be 5

---

### Step 5: Verify Item Database Initialization

#### 5.1 Check Console Logs

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

### Verification Checklist for Days 7-8

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

## Days 9-10: Skill System Foundation

**After Code Implementation**  
**Prerequisites:** ✅ Days 7-8 complete (Item System working), C++ code compiled successfully

---

### Step 1: Configure Asset Manager for Skills

#### 1.1 Open Project Settings

1. **Open Project Settings**
   - Go to `Edit` → `Project Settings`
   - Or press `Ctrl + ,`

2. **Navigate to Game → Asset Manager**
   - In the left panel, expand `Game` section
   - Click on `Asset Manager`

#### 1.2 Add Primary Asset Type for Skills

1. **Add Primary Asset Type for Skills**
   - In the `Primary Asset Types to Scan` section
   - You should already have `Item` type from Days 7-8
   - Click the `+` button to add a new type

2. **Configure the Asset Type:**
   - **Primary Asset Type:** `Skill`
   - **Asset Base Class:** Set to `SkillDataAsset` (or leave as `DataAsset` if SkillDataAsset doesn't appear)
   - **Directories to Scan:** 
     - Add: `/Game/Data/Skills/`
     - This tells the Asset Manager where to find skill data assets
   - **Rules:** Leave as default

3. **Save Project Settings**
   - Click `Save` or close the window

**Note:** If `SkillDataAsset` doesn't appear in the dropdown:
- Use `DataAsset` as the base class temporarily
- Or configure it after creating your first Skill Data Asset
- Recompile C++ code and restart editor

---

### Step 2: Create Test Skill Data Assets

#### 2.1 Navigate to Data Folder

1. **Go to Content Browser**
   - Navigate to `Content/Data/Skills/` folder
   - (Create the folder if it doesn't exist)
   - Right-click in Content Browser → `New Folder` → Name it `Skills`

#### 2.2 Create Test Skill: Fireball (Magic/Combat)

1. **Create Skill Data Asset**
   - Right-click in `Content/Data/Skills/`
   - Select `Miscellaneous` → `Data Asset`
   - In the "Pick Data Asset Class" dialog, search for `SkillDataAsset`
   - Select `SkillDataAsset` and click `Select`

2. **Name the Asset**
   - Name it: `DA_Skill_Magic_Fireball`
   - Save it in `Content/Data/Skills/`

3. **Configure the Skill**
   - Double-click `DA_Skill_Magic_Fireball` to open
   - In the `Details` panel, configure:
     - **Skill Type:** Set to `Skill` (Primary Asset Type)
     - **Skill ID:** `Fireball` (or `Skill_Fireball`)
     - **Skill Name:** `Fireball`
     - **Skill Description:** `Launches a fireball at the target, dealing fire damage`
     - **Skill Icon:** (Leave empty for now, or assign a texture)
     - **Type:** `Magic`
     - **Category:** `Combat`
     - **Cooldown Duration:** `5.0` (seconds)
     - **Mana Cost:** `25.0`
     - **Stamina Cost:** `0.0` (or set a value)
     - **Cast Time:** `1.0` (seconds)
     - **Range:** `1000.0` (units)

4. **Save and Close** the Data Asset

#### 2.3 Create Test Skill: Heal (Magic/Support)

1. **Create Another Skill Data Asset**
   - Right-click in `Content/Data/Skills/`
   - `Miscellaneous` → `Data Asset`
   - Select `SkillDataAsset`

2. **Name:** `DA_Skill_Magic_Heal`

3. **Configure:**
   - **Skill Type:** `Skill`
   - **Skill ID:** `Heal` (or `Skill_Heal`)
   - **Skill Name:** `Heal`
   - **Skill Description:** `Restores health to the target`
   - **Type:** `Magic`
   - **Category:** `Support`
   - **Cooldown Duration:** `10.0`
   - **Mana Cost:** `30.0`
   - **Stamina Cost:** `0.0`
   - **Cast Time:** `2.0`
   - **Range:** `500.0`

4. **Save and Close**

#### 2.4 Create Test Skill: Dash (Utility/Movement)

1. **Create Skill Data Asset:** `DA_Skill_Utility_Dash`

2. **Configure:**
   - **Skill Type:** `Skill`
   - **Skill ID:** `Dash` (or `Skill_Dash`)
   - **Skill Name:** `Dash`
   - **Skill Description:** `Quickly dash forward, covering distance instantly`
   - **Type:** `Utility`
   - **Category:** `Movement`
   - **Cooldown Duration:** `3.0`
   - **Mana Cost:** `10.0`
   - **Stamina Cost:** `0.0`
   - **Cast Time:** `0.0` (instant)
   - **Range:** `500.0` (dash distance)

3. **Save and Close**

---

### Step 3: Verify Asset Manager Configuration

#### 3.1 Check Primary Asset Types

1. **Open Project Settings** → `Game` → `Asset Manager`

2. **Verify Configuration:**
   - Primary Asset Type `Item` exists (from Days 7-8)
   - Primary Asset Type `Skill` exists (newly added)
   - Directory `/Game/Data/Skills/` is in the scan list for `Skill` type
   - Base class is set correctly

#### 3.2 Scan for Assets

1. **In Asset Manager Settings:**
   - Click `Scan Primary Asset Types` button (if available)
   - Or restart the editor to trigger automatic scan

2. **Verify Skills are Found:**
   - The Asset Manager should detect your Skill Data Assets
   - Check the Output Log for scan results
   - You should see messages about skills being registered

---

### Step 4: Test Skill System in Blueprint

#### 4.1 Test Skill Database

**Option A: Test in Blueprint Event Graph**

**Step-by-Step Instructions:**

1. **Open Blueprint**
   - Open `BP_ActionRPGPlayerCharacter` (or any Blueprint)
   - Go to the `Event Graph` tab

2. **Add Test Event**
   - Right-click in Event Graph
   - Search for `Event BeginPlay` or create a `Custom Event`
   - If using Custom Event, name it `TestSkillDatabase`

3. **Get Skill Database Instance**
   - Right-click in Event Graph
   - Search for `Get Skill Database` (or `Skill Database Get`)
   - **Note:** This is a static function, so it appears as a blue node without a target pin
   - This calls the static `Get()` function to get the singleton instance
   - The output pin will be of type `Skill Database Object Reference`

4. **Call Get Skill Data Asset**
   - Search for `Get Skill Data Asset` node
   - Connect the `Skill Database` output to the `Target` input
   - For `Skill ID` input:
     - Right-click on the `Skill ID` pin
     - Select `Make Literal Name`
     - Set value to `Fireball`
     - Or type directly: Right-click on pin → `Set Value` → Type `Fireball`

5. **Print Result**
   - Drag from the `Skill Data Asset` output pin
   - Search for `Break Skill Data Asset` node
   - Connect the Skill Data Asset output
   - This gives you access to all properties (Name, Description, Type, etc.)
   - Connect `Skill Name` to `Print String` to verify

6. **Compile and Test**
   - Click `Compile` button
   - Click `Save`
   - Play in Editor
   - Check Output Log for the printed skill name

#### 4.2 Test Skill Creation

1. **In Blueprint Event Graph:**

   **Step-by-Step:**

   1. **Get Skill Database** (same as Step 4.1)
      - Add `Get Skill Database` node

   2. **Create Skill**
      - Drag from Skill Database output
      - Search for `Create Skill` node
      - Connect Skill Database to `Target` input
      - For `Skill ID`: Use `Make Literal Name` with value `Fireball`

   3. **Verify Skill Creation**
      - Drag from `Skill` output pin
      - Add `Is Valid` node to check if skill was created
      - Connect to `Print String` to show result

   4. **Check Skill Properties**
      - From the created Skill, you can access:
        - `Get Skill ID` - Should return "Fireball"
        - `Get Skill Name` - Should return "Fireball"
        - `Get Skill Type` - Should return "Magic"
        - `Cooldown Remaining` - Should be 0.0 initially

#### 4.3 Test Skill Activation

1. **In Blueprint:**

   **Step-by-Step:**

   1. **Create Skill** (from Step 4.2)
      - Get Skill Database
      - Create Skill with ID "Fireball"

   2. **Check if Skill Can Activate**
      - Drag from Skill output
      - Search for `Can Activate` node
      - Connect Skill to `Target` input
      - For `Target` parameter: Leave empty (null) or pass an actor
      - This returns a boolean

   3. **Activate Skill**
      - Drag from Skill output
      - Search for `Activate` node
      - Connect Skill to `Target` input
      - For `Target` parameter: Leave empty (null) or pass an actor
      - Add `Print String` after to verify it executed

   4. **Check Cooldown**
      - After activation, check `Cooldown Remaining` property
      - Should be set to `Cooldown Duration` from Data Asset (5.0 for Fireball)
      - Add `Print String` to display cooldown value

---

### Step 5: Verify Skill Database Initialization

#### 5.1 Check Console Logs

1. **Open Output Log**
   - Window → Developer Tools → `Output Log`
   - Or press `Ctrl + Shift + L`

2. **Look for Skill Database Logs:**
   - When game starts, you should see:
     - `"SkillDatabase: Registered skill Fireball"`
     - `"SkillDatabase: Registered skill Heal"`
     - `"SkillDatabase: Registered skill Dash"`

3. **If Skills Not Found:**
   - Check Asset Manager configuration
   - Verify Skill Data Assets are in correct folder
   - Check Skill ID is set correctly
   - Verify Primary Asset Type matches

---

### Verification Checklist for Days 9-10

Before considering Days 9-10 complete, verify:

- [ ] Asset Manager configured with Primary Asset Type `Skill`
- [ ] At least 3 test Skill Data Assets created
- [ ] Skill Data Assets have unique Skill IDs
- [ ] Skill Data Assets are in `Content/Data/Skills/` folder
- [ ] SkillDatabase initializes and finds skills (check logs)
- [ ] Can lookup skills by ID using SkillDatabase
- [ ] Can create SkillBase instances from SkillDataAsset
- [ ] Skill activation works (Activate() function)
- [ ] Cooldown is set after activation
- [ ] `CanActivate()` validation works
- [ ] No errors in console related to skills

---

## Days 11-12: Character Classes & Integration

**After Code Implementation**  
**Prerequisites:** ✅ Days 1-10 complete, C++ code compiled successfully

---

### Step 1: Verify/Create Blueprint Character Classes

#### 1.1 Check for Existing Blueprint Classes

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Characters/` folder

2. **Check for Blueprint Classes**
   - Look for:
     - `BP_ActionRPGCharacter` (base character)
     - `BP_ActionRPGPlayerCharacter` (player character)

3. **If Blueprints Don't Exist:**
   - Follow Steps 1.2 and 1.3 below
   - If they exist, skip to Step 2

#### 1.2 Create Base Character Blueprint (if needed)

1. **Right-click in Content Browser**
   - Location: `Content/Blueprints/Characters/`

2. **Select "Blueprint Class"**

3. **In "Pick Parent Class" Dialog**
   - Search for: `ActionRPGCharacter`
   - Select: `ActionRPGCharacter` (the C++ class)
   - Click: `Select`

4. **Name the Blueprint**
   - Name: `BP_ActionRPGCharacter`
   - Press `Enter` to confirm

5. **Save the Blueprint**
   - Click `Compile` button (if available)
   - Click `Save` button
   - Close Blueprint Editor (no configuration needed yet)

#### 1.3 Create Player Character Blueprint (if needed)

1. **Right-click in Content Browser**
   - Location: `Content/Blueprints/Characters/`

2. **Select "Blueprint Class"**

3. **In "Pick Parent Class" Dialog**
   - Search for: `ActionRPGPlayerCharacter`
   - Select: `ActionRPGPlayerCharacter` (the C++ class)
   - Click: `Select`

4. **Name the Blueprint**
   - Name: `BP_ActionRPGPlayerCharacter`
   - Press `Enter` to confirm

5. **Don't close yet - proceed to Step 2**

---

### Step 2: Configure Player Character Blueprint

#### 2.1 Open Player Character Blueprint

1. **Double-click `BP_ActionRPGPlayerCharacter`**
   - Opens in Blueprint Editor

2. **Verify Components Panel**
   - You should see:
     - `DefaultSceneRoot` (root component)
     - `Mesh` (Skeletal Mesh Component)
     - `CharacterMovement` (Character Movement Component)
     - `CapsuleComponent` (Collision Capsule)
     - `SpringArm` (Spring Arm Component - from C++)
     - `Camera` (Camera Component - from C++)

#### 2.2 Configure Mesh Component (Optional)

**Note:** If you don't have a character mesh yet, you can skip this step and add it later.

1. **Select `Mesh` Component**
   - Click on `Mesh` in the Components panel

2. **In Details Panel:**
   - **Skeletal Mesh:**
     - If you have a character mesh:
       - Click dropdown → Select your mesh
     - If no mesh:
       - Leave as `None` (will use default or add later)

3. **Transform Settings (if mesh is assigned):**
   - **Location:** Usually (0, 0, -90) to align with capsule
   - **Rotation:** Usually (0, 0, 0)
   - **Scale:** Usually (1, 1, 1)
   - Adjust as needed for your mesh

#### 2.3 Verify Camera Components

1. **Select `SpringArm` Component**
   - Click on `SpringArm` in Components panel

2. **In Details Panel, verify these properties:**
   - **Target Arm Length:** `500.0` (can adjust if needed)
   - **Socket Offset:**
     - X: `0.0`
     - Y: `0.0`
     - Z: `500.0` (height above character)
   - **Relative Rotation:**
     - Pitch: `-65.0` (looking down)
     - Yaw: `0.0`
     - Roll: `0.0`
   - **Use Pawn Control Rotation:** `False` (unchecked)
   - **Inherit Pitch:** `False` (unchecked)
   - **Inherit Yaw:** `False` (unchecked)
   - **Inherit Roll:** `False` (unchecked)

3. **Select `Camera` Component**
   - Click on `Camera` in Components panel

4. **In Details Panel, verify:**
   - **Field of View:** `90.0` (default, can adjust)
   - **Projection Mode:** `Perspective` (default)
   - **Aspect Ratio:** Leave default

**Note:** These values are set in C++ constructor, but you can override them in Blueprint if needed.

#### 2.4 Configure Character Movement

1. **Select `CharacterMovement` Component**
   - Click on `CharacterMovement` in Components panel

2. **In Details Panel, verify/adjust these properties:**

   **Movement Settings:**
   - **Max Walk Speed:** `600.0` (default, adjust as needed)
   - **Max Acceleration:** `2048.0` (default)
   - **Braking Deceleration Walking:** `2048.0` (default)
   - **Ground Friction:** `8.0` (default)

   **Rotation Settings:**
   - **Rotation Rate:**
     - X: `0.0`
     - Y: `540.0` (degrees per second)
     - Z: `0.0`
   - **Orient Rotation to Movement:** `False` (unchecked) ⚠️ **IMPORTANT**
   - **Use Controller Desired Rotation:** `False` (unchecked) ⚠️ **IMPORTANT**

   **Why these settings:**
   - `Orient Rotation to Movement` must be FALSE because we handle rotation manually via mouse cursor
   - `Use Controller Desired Rotation` must be FALSE for the same reason

3. **Other Settings (leave defaults unless needed):**
   - **Can Crouch:** `False` (unless you want crouching)
   - **Can Jump:** `False` (unless you want jumping)
   - **Can Swim:** `True` (default, can disable if no water)

#### 2.5 Save Player Character Blueprint

1. **Click `Compile` Button**
   - Top toolbar, green checkmark icon
   - Wait for compilation to finish
   - Check for any errors (should be none)

2. **Click `Save` Button**
   - Top toolbar, disk icon
   - Or press `Ctrl+S`

3. **Close Blueprint Editor**
   - Click `X` or press `Esc`

---

### Step 3: Verify GameMode Configuration

#### 3.1 Open GameMode Blueprint

1. **Navigate to Content Browser**
   - Go to `Content/Blueprints/Core/` folder

2. **Open `BP_ActionRPGGameMode`**
   - Double-click to open

#### 3.2 Verify Default Pawn Class

1. **In Details Panel (Class Defaults):**
   - Look for `Default Pawn Class` property
   - Should be set to: `BP_ActionRPGPlayerCharacter`
   - If not set:
     - Click dropdown
     - Search for `BP_ActionRPGPlayerCharacter`
     - Select it

2. **Verify Other Default Classes:**
   - **Player Controller Class:** `BP_ActionRPGPlayerController` (or C++ class)
   - **Game State Class:** `BP_ActionRPGGameState` (or C++ class)
   - **HUD Class:** `BP_ActionRPGHUD` (or C++ class)

3. **Save and Close**
   - Click `Compile` (if needed)
   - Click `Save`
   - Close Blueprint Editor

#### 3.3 Verify Project Settings

1. **Open Project Settings**
   - Go to `Edit → Project Settings`
   - Or press `Ctrl+Alt+,`

2. **Navigate to Game Mode**
   - Left panel: `Game → Game Mode`

3. **Verify Default GameMode**
   - **Default GameMode:** Should be `BP_ActionRPGGameMode`
   - If not set:
     - Click dropdown
     - Search for `BP_ActionRPGGameMode`
     - Select it

4. **Close Project Settings**
   - Click `X` or press `Esc`

---

### Step 4: Test in Game

#### 4.1 Prepare Level

1. **Open Your Level**
   - Open the level you want to test in (e.g., `Lvl_TopDown`)

2. **Verify Player Start**
   - Look for a `Player Start` actor in the level
   - If missing:
     - Press `P` to show player start visualization
     - Or add one: `Place Actors → Game → Player Start`
     - Position it where you want the player to spawn

#### 4.2 Play in Editor (PIE)

1. **Start Play**
   - Click `Play` button (top toolbar)
   - Or press `Alt+P`

2. **Test Movement**
   - **W/S Keys:** Should move forward/backward relative to character facing
   - **A/D Keys:** Should strafe left/right relative to character facing
   - Verify movement feels responsive

3. **Test Mouse Look**
   - **Move Mouse:** Character should rotate to face mouse cursor
   - Verify rotation is smooth
   - Verify character only rotates on horizontal plane (no pitch/roll)

4. **Test Camera**
   - **Camera should:** Be positioned above character
   - **View should:** Be top-down (angled down)
   - **Camera should:** Follow character as you move
   - Verify no camera jitter or clipping

5. **Test Input Actions**
   - **E Key:** Should trigger Interact (check console for log)
   - **Left Click:** Should trigger Attack (check console for log)
   - **Space:** Should trigger Dodge (check console for log)
   - **Tab:** Should trigger Open Inventory (check console for log)
   - **1-8 Keys:** Should trigger Skill Slots (check console for logs)

6. **Stop Play**
   - Press `Esc` or click `Stop` button

#### 4.3 Check Console for Errors

1. **Open Output Log**
   - Go to `Window → Developer Tools → Output Log`
   - Or press `Ctrl+Shift+L`

2. **Review Logs**
   - Look for any errors (red text)
   - Look for any warnings (yellow text)
   - Verify input actions are logging correctly (if you added logs)

---

### Step 5: Tune Movement and Camera (Optional)

#### 5.1 Adjust Movement Speed

If movement feels too fast or too slow:

1. **Open `BP_ActionRPGPlayerCharacter`**

2. **Select `CharacterMovement` Component**

3. **Adjust `Max Walk Speed`**
   - Try values between `300.0` (slow) and `900.0` (fast)
   - Test in-game and iterate

4. **Save and Test**

#### 5.2 Adjust Camera Distance

If camera is too close or too far:

1. **Open `BP_ActionRPGPlayerCharacter`**

2. **Select `SpringArm` Component**

3. **Adjust `Target Arm Length`**
   - Increase for farther camera (e.g., `700.0`)
   - Decrease for closer camera (e.g., `300.0`)
   - Test in-game and iterate

4. **Save and Test**

#### 5.3 Adjust Camera Angle

If camera angle needs adjustment:

1. **Open `BP_ActionRPGPlayerCharacter`**

2. **Select `SpringArm` Component**

3. **Adjust `Relative Rotation` Pitch**
   - More negative = steeper angle (e.g., `-75.0`)
   - Less negative = shallower angle (e.g., `-45.0`)
   - Test in-game and iterate

4. **Save and Test**

---

### Success Checklist for Days 11-12

- ✅ `BP_ActionRPGCharacter` exists
- ✅ `BP_ActionRPGPlayerCharacter` exists and is configured
- ✅ Character Movement Component configured correctly
- ✅ Camera components configured correctly
- ✅ GameMode is set to use `BP_ActionRPGPlayerCharacter`
- ✅ Project Settings has correct Default GameMode
- ✅ Character spawns in-game
- ✅ Movement works with WASD
- ✅ Mouse rotation works
- ✅ Camera follows character
- ✅ All input actions trigger (verified with logs)
- ✅ No errors in console
- ✅ Ready for Phase 2

---

## Troubleshooting Guide

### General Issues

#### Issue: Blueprint Classes Don't Appear in Parent Class List

**Solution:**
1. Close Unreal Editor
2. Recompile C++ code in Visual Studio
3. Reopen Unreal Editor
4. The classes should now appear

#### Issue: Input Actions Don't Work

**Checklist:**
- ✅ Input Actions are created in `Content/Input/`
- ✅ Input Mapping Context has all mappings
- ✅ PlayerController Blueprint has Input Actions assigned
- ✅ PlayerController Blueprint is compiled
- ✅ GameMode is set in Project Settings
- ✅ Default Pawn Class is set correctly

#### Issue: Character Doesn't Move

**Checklist:**
- ✅ IA_Move is assigned in PlayerController
- ✅ IM_ActionRPG is assigned as Default Mapping Context
- ✅ Movement keys (W/A/S/D) are mapped in Input Mapping Context
- ✅ Character Blueprint is set as Default Pawn
- ✅ Player Start exists in the level
- ✅ Character Movement Component has `Orient Rotation to Movement` = FALSE
- ✅ Character Movement Component has `Use Controller Desired Rotation` = FALSE

#### Issue: Character Not Rotating to Mouse

**Solutions:**
1. Verify `RotateToMouseCursor()` is being called (check C++ code)
2. Check PlayerController is valid
3. Verify mouse cursor is visible (check PlayerController settings)
4. Check console for errors
5. Try restarting the editor

#### Issue: Camera Not Following Character

**Solutions:**
1. Verify `SpringArm` is attached to `RootComponent`
2. Check `Camera` is attached to `SpringArm`
3. Verify `SpringArm` properties are set correctly
4. Check for any camera constraints in level
5. Try restarting the editor

### Item System Issues

#### Issue: ItemDataAsset Doesn't Appear in Data Asset Creation

**Solution:**
1. Close and reopen Unreal Editor
2. Recompile C++ code
3. The class should appear after recompilation
4. If still not appearing, check that the class is marked with `UCLASS(BlueprintType)`

#### Issue: Asset Manager Doesn't Find Items

**Checklist:**
- ✅ Primary Asset Type `Item` is configured
- ✅ Directory `/Game/Data/Items/` is in scan list
- ✅ Item Data Assets are in `Content/Data/Items/` folder
- ✅ Item ID is set (not empty)
- ✅ Item Type (Primary Asset Type) is set to `Item`
- ✅ Editor has been restarted after configuration

#### Issue: ItemDatabase Returns NULL

**Solutions:**
1. Check that `UItemDatabase::Get()` is called after game starts
2. Verify Asset Manager is initialized
3. Check console logs for initialization errors
4. Verify items are registered in the database

### Skill System Issues

#### Issue: SkillDataAsset Doesn't Appear in Data Asset Creation

**Solution:**
1. Close and reopen Unreal Editor
2. Recompile C++ code
3. The class should appear after recompilation
4. If still not appearing, check that the class is marked with `UCLASS(BlueprintType)`

#### Issue: Asset Manager Doesn't Find Skills

**Checklist:**
- ✅ Primary Asset Type `Skill` is configured
- ✅ Directory `/Game/Data/Skills/` is in scan list
- ✅ Skill Data Assets are in `Content/Data/Skills/` folder
- ✅ Skill ID is set (not empty)
- ✅ Skill Type (Primary Asset Type) is set to `Skill`
- ✅ Editor has been restarted after configuration

#### Issue: SkillDatabase Returns NULL

**Solutions:**
1. Check that `USkillDatabase::Get()` is called after game starts
2. Verify Asset Manager is initialized
3. Check console logs for initialization errors
4. Verify skills are registered in the database

#### Issue: CanActivate Always Returns False

**Possible Causes:**
- CooldownRemaining is greater than 0
- Skill validation logic is too strict
- Check the `CanActivate()` implementation in C++

---

## Quick Reference Tables

### Input Actions Summary

| Action | Input Action | Key/Mouse | Type |
|--------|-------------|-----------|------|
| Move Forward | IA_Move | W | Vector2D (X+) |
| Move Backward | IA_Move | S | Vector2D (X-, Negate) |
| Move Left | IA_Move | A | Vector2D (Y-, Negate) |
| Move Right | IA_Move | D | Vector2D (Y+) |
| Look | IA_Look | Mouse XY | Vector2D |
| Interact | IA_Interact | E | Boolean |
| Attack | IA_Attack | Left Mouse | Boolean |
| Dodge | IA_Dodge | Space | Boolean |
| Open Inventory | IA_OpenInventory | Tab | Boolean |
| Skill Slot 1-8 | IA_SkillSlot1-8 | 1-8 | Boolean |

### Item Data Asset Properties

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

### Skill Data Asset Properties

| Property | Type | Description | Example |
|----------|------|-------------|---------|
| Skill Type | Primary Asset Type | Must be "Skill" | `Skill` |
| Skill ID | FName | Unique identifier | `Fireball` |
| Skill Name | FText | Display name | `Fireball` |
| Skill Description | FText | Tooltip description | `Launches a fireball` |
| Skill Icon | Texture2D | Skill icon image | (Optional) |
| Type | ESkillType | Skill type | `Magic` |
| Category | ESkillCategory | Skill category | `Combat` |
| Cooldown Duration | float | Cooldown in seconds | `5.0` |
| Mana Cost | float | Mana required | `25.0` |
| Stamina Cost | float | Stamina required | `0.0` |
| Cast Time | float | Cast time in seconds | `1.0` |
| Range | float | Skill range in units | `1000.0` |

---

## Final Verification Checklist

Before considering Phase 1 complete, verify:

### Core Framework
- [ ] All Blueprint classes created from C++ classes
- [ ] GameMode set in Project Settings
- [ ] All default classes configured correctly

### Input System
- [ ] All Input Actions created
- [ ] Input Mapping Context created and configured
- [ ] All Input Actions assigned to PlayerController
- [ ] Input system tested and working

### Item System
- [ ] Asset Manager configured for Items
- [ ] Test Item Data Assets created
- [ ] ItemDatabase functional
- [ ] Can create and use items

### Skill System
- [ ] Asset Manager configured for Skills
- [ ] Test Skill Data Assets created
- [ ] SkillDatabase functional
- [ ] Can create and activate skills

### Character System
- [ ] Character Blueprints configured
- [ ] Character Movement settings correct
- [ ] Camera configured correctly
- [ ] Movement and rotation working
- [ ] All systems integrated

### Testing
- [ ] No compilation errors
- [ ] No runtime errors
- [ ] All systems functional
- [ ] Ready for Phase 2

---

## Notes

- **Blueprint Compilation:** Always compile Blueprints after making changes
- **Editor Restart:** Sometimes you need to restart the editor after C++ changes
- **Hot Reload:** C++ changes can be hot-reloaded, but Blueprint changes require compilation
- **Testing:** Test frequently in Play mode to catch issues early
- **Primary Asset Types:** Must be configured in Project Settings for Asset Manager to work
- **IDs:** Must be unique and match exactly (case-sensitive)
- **Asset Manager:** Automatically scans and registers assets on startup

---

**End of Phase 1 Complete Manual Steps Guide**

**Next:** Phase 2 - Inventory System Implementation
