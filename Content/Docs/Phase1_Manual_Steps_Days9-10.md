# Phase 1 Manual Steps - Days 9-10: Skill System Foundation
**After Code Implementation**  
**Date:** 2025-01-07

---

## Overview

Days 9-10 focus on setting up the Skill System Foundation. The C++ code should be complete, but you need to configure the Asset Manager for skills and create test Skill Data Assets in the Unreal Editor.

**Status:** ⚠️ Complete C++ code first, then follow these manual steps

**Prerequisites:**
- ✅ Days 7-8 complete (Item System working)
- ✅ C++ code compiled successfully
- ✅ Unreal Editor is open
- ✅ Project is loaded

---

## Step 1: Configure Asset Manager for Skills

### 1.1 Open Project Settings

1. **Open Project Settings**
   - Go to `Edit` → `Project Settings`
   - Or press `Ctrl + ,`

2. **Navigate to Game → Asset Manager**
   - In the left panel, expand `Game` section
   - Click on `Asset Manager`

### 1.2 Add Primary Asset Type for Skills

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

## Step 2: Create Test Skill Data Assets

### 2.1 Navigate to Data Folder

1. **Go to Content Browser**
   - Navigate to `Content/Data/Skills/` folder
   - (Create the folder if it doesn't exist)
   - Right-click in Content Browser → `New Folder` → Name it `Skills`

### 2.2 Create Test Skill: Fireball (Magic/Combat)

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
     - **Cast Time:** `1.0` (seconds)
     - **Range:** `1000.0` (units)

4. **Save and Close** the Data Asset

### 2.3 Create Test Skill: Heal (Magic/Support)

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
   - **Cast Time:** `2.0`
   - **Range:** `500.0`

4. **Save and Close**

### 2.4 Create Test Skill: Dash (Utility/Movement)

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
   - **Cast Time:** `0.0` (instant)
   - **Range:** `500.0` (dash distance)

3. **Save and Close**

### 2.5 Create Test Skill: Power Strike (Melee/Combat)

1. **Create Skill Data Asset:** `DA_Skill_Melee_PowerStrike`

2. **Configure:**
   - **Skill Type:** `Skill`
   - **Skill ID:** `PowerStrike` (or `Skill_PowerStrike`)
   - **Skill Name:** `Power Strike`
   - **Skill Description:** `A powerful melee attack that deals extra damage`
   - **Type:** `Melee`
   - **Category:** `Combat`
   - **Cooldown Duration:** `8.0`
   - **Mana Cost:** `15.0`
   - **Cast Time:** `0.5`
   - **Range:** `200.0` (melee range)

3. **Save and Close**

---

## Step 3: Verify Asset Manager Configuration

### 3.1 Check Primary Asset Types

1. **Open Project Settings** → `Game` → `Asset Manager`

2. **Verify Configuration:**
   - Primary Asset Type `Item` exists (from Days 7-8)
   - Primary Asset Type `Skill` exists (newly added)
   - Directory `/Game/Data/Skills/` is in the scan list for `Skill` type
   - Base class is set correctly

### 3.2 Scan for Assets

1. **In Asset Manager Settings:**
   - Click `Scan Primary Asset Types` button (if available)
   - Or restart the editor to trigger automatic scan

2. **Verify Skills are Found:**
   - The Asset Manager should detect your Skill Data Assets
   - Check the Output Log for scan results
   - You should see messages about skills being registered

---

## Step 4: Test Skill System in Blueprint

### 4.1 Test Skill Database

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

**Example Node Setup:**
```
Event BeginPlay
→ Get Skill Database
→ Get Skill Data Asset
   ├─ Target: (Skill Database output)
   └─ Skill ID: Make Literal Name ("Fireball")
→ Break Skill Data Asset
   └─ Skill Data Asset: (from Get Skill Data Asset)
→ Print String
   └─ In String: Skill Name (from Break node)
```

**Visual Guide:**
- `Event BeginPlay` node (white)
- `Get Skill Database` node (blue - static function)
- `Get Skill Data Asset` node (blue - member function)
- `Make Literal Name` node (green - set to "Fireball")
- `Break Skill Data Asset` node (yellow - breaks struct)
- `Print String` node (white - prints to screen/log)

**Option B: Test in C++ (if you add test code)**

Add this to `ActionRPGPlayerCharacter.cpp` in `BeginPlay()`:
```cpp
// Test Skill Database
if (USkillDatabase* SkillDB = USkillDatabase::Get())
{
    if (USkillDataAsset* Fireball = SkillDB->GetSkillDataAsset(FName("Fireball")))
    {
        UE_LOG(LogTemp, Log, TEXT("Found skill: %s"), *Fireball->SkillName.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Fireball not found in database"));
    }
}
```

**Expected Results:**
- If skill exists: Print String should show "Fireball" (or your skill name)
- If skill doesn't exist: Skill Data Asset will be NULL, Print String may show empty or error
- Check Output Log for "SkillDatabase: Registered skill..." messages on game start
- Check Output Log for "SkillDatabase: Skill not found..." if lookup fails

### 4.2 Test Skill Creation

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

   **Example Node Setup:**
   ```
   Event BeginPlay
   → Get Skill Database
   → Create Skill
      ├─ Target: (Skill Database)
      └─ Skill ID: Make Literal Name ("Fireball")
   → Get Skill Name (from Skill output)
   → Print String (Skill Name)
   ```

### 4.3 Test Skill Activation

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

   **Example Node Setup:**
   ```
   Event BeginPlay
   → Get Skill Database
   → Create Skill (Fireball)
   → Can Activate
      ├─ Target: (Skill)
      └─ Target: (null or actor)
   → Branch (if Can Activate is true)
   → Activate
      ├─ Target: (Skill)
      └─ Target: (null or actor)
   → Get Cooldown Remaining
   → Print String (Cooldown value)
   ```

   **Expected Results:**
   - Console should show: "SkillBase::Activate - Skill activated: Fireball"
   - `OnSkillActivated` event should fire (if you bind to it)
   - Check Output Log for activation messages
   - CooldownRemaining should be set to CooldownDuration after activation

---

## Step 5: Verify Skill Database Initialization

### 5.1 Check Console Logs

1. **Open Output Log**
   - Window → Developer Tools → `Output Log`
   - Or press `Ctrl + Shift + L`

2. **Look for Skill Database Logs:**
   - When game starts, you should see:
     - `"SkillDatabase: Registered skill Fireball"`
     - `"SkillDatabase: Registered skill Heal"`
     - `"SkillDatabase: Registered skill Dash"`
     - `"SkillDatabase: Registered skill PowerStrike"`

3. **If Skills Not Found:**
   - Check Asset Manager configuration
   - Verify Skill Data Assets are in correct folder
   - Check Skill ID is set correctly
   - Verify Primary Asset Type matches

---

## Troubleshooting

### Issue: SkillDataAsset Doesn't Appear in Data Asset Creation

**Solution:**
1. Close and reopen Unreal Editor
2. Recompile C++ code
3. The class should appear after recompilation
4. If still not appearing, check that the class is marked with `UCLASS(BlueprintType)`

### Issue: Asset Manager Doesn't Find Skills

**Checklist:**
- ✅ Primary Asset Type `Skill` is configured
- ✅ Directory `/Game/Data/Skills/` is in scan list
- ✅ Skill Data Assets are in `Content/Data/Skills/` folder
- ✅ Skill ID is set (not empty)
- ✅ Skill Type (Primary Asset Type) is set to `Skill`
- ✅ Editor has been restarted after configuration

### Issue: SkillDatabase Returns NULL

**Solutions:**
1. Check that `USkillDatabase::Get()` is called after game starts
2. Verify Asset Manager is initialized
3. Check console logs for initialization errors
4. Verify skills are registered in the database

### Issue: CreateSkill Returns NULL

**Checklist:**
- ✅ Skill ID matches exactly (case-sensitive)
- ✅ Skill exists in SkillDatabase registry
- ✅ SkillDataAsset is valid
- ✅ Check console for "Skill not found" warnings

### Issue: "Get Skill Database" Node Not Found in Blueprint

**Solution:**
1. Make sure C++ code is compiled
2. Close and reopen the Blueprint
3. Try searching for "Skill Database" (without "Get")
4. The static function should appear as a blue node
5. If still not found, restart the editor

### Issue: "Get Skill Data Asset" Node Not Found

**Solution:**
1. Make sure you have the Skill Database reference first
2. Drag from the Skill Database output pin
3. Search for "Get Skill Data Asset" in the context menu
4. The function should appear as a member function of SkillDatabase

### Issue: Skill ID Input Doesn't Accept String

**Solution:**
- Skill ID is an FName, not FString
- Use `Make Literal Name` node instead of `Make Literal String`
- Or right-click on the pin and select "Set Value" to type directly
- The value should be: `Fireball` (no quotes needed when typing directly)

### Issue: CanActivate Always Returns False

**Possible Causes:**
- CooldownRemaining is greater than 0
- Skill validation logic is too strict
- Check the `CanActivate()` implementation in C++

### Issue: Activate Doesn't Execute

**Checklist:**
- ✅ Skill is valid (not NULL)
- ✅ `CanActivate()` returns true
- ✅ Check console for error messages
- ✅ Verify `Activate()` function is implemented correctly

---

## Verification Checklist

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

## Quick Reference: Skill Data Asset Properties

When creating Skill Data Assets, configure these properties:

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
| Cast Time | float | Cast time in seconds | `1.0` |
| Range | float | Skill range in units | `1000.0` |

---

## Code Reference

### Key Files Created (After C++ Implementation)
- `Source/ActionRPG/Public/Skills/Core/SkillTypes.h` - Enums and types
- `Source/ActionRPG/Public/Skills/Core/SkillDataAsset.h` - Data asset class
- `Source/ActionRPG/Private/Skills/Core/SkillDataAsset.cpp` - Data asset implementation
- `Source/ActionRPG/Public/Skills/Core/SkillBase.h` - Base skill class
- `Source/ActionRPG/Private/Skills/Core/SkillBase.cpp` - Base skill implementation
- `Source/ActionRPG/Public/Data/SkillDatabase.h` - Database singleton
- `Source/ActionRPG/Private/Data/SkillDatabase.cpp` - Database implementation

### Key Functions
- `USkillDatabase::Get()` - Get singleton instance
- `GetSkillDataAsset(SkillID)` - Lookup skill by ID
- `CreateSkill(SkillID)` - Create skill instance
- `USkillBase::Activate(Target)` - Activate the skill
- `USkillBase::CanActivate(Target)` - Check if skill can be activated

---

## Next Steps

After completing Days 9-10:

1. **Days 11-12** will focus on:
   - Character Classes (if not already complete)
   - Integration of Skills with Character
   - Skill activation from character
   - Skill cooldown management

2. **Before Days 11-12**, you should have:
   - ✅ Skill system working
   - ✅ SkillDatabase functional
   - ✅ Can create and activate skills
   - ✅ Ready to integrate skills with character

---

## Notes

- **Primary Asset Types:** Must be configured in Project Settings for Asset Manager to work
- **Skill IDs:** Must be unique and match exactly (case-sensitive)
- **Asset Manager:** Automatically scans and registers skills on startup
- **SkillDatabase:** Singleton pattern - only one instance exists
- **SkillBase:** Can be created from SkillDataAsset using SkillDatabase
- **Cooldown System:** Basic implementation in Phase 1, can be expanded later
- **Activation Target:** Optional parameter - can be null for self-targeted skills
- **Skill Types vs Categories:** Type is the execution method (Melee, Magic, etc.), Category is the purpose (Combat, Support, etc.)

---

**Status:** Complete these manual steps to finish Days 9-10
