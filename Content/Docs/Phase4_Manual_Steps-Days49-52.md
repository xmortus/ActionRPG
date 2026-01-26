# Phase 4 Manual Steps - Days 49-52
**Focus:** Experience + Class/Profession  
**UE Version:** 5.7  
**Last Updated:** 2026-01-22

---

## Step 1: Create Class/Profession Data Assets
1. Navigate to `Content/Data/Classes/` (create folders as needed).
2. Create data assets:
   - `DA_Class_Warrior` (ClassDataAsset)
   - `DA_Class_Mage` (ClassDataAsset)
   - `DA_Profession_Blacksmith` (ProfessionDataAsset)
3. Set:
   - Display Name, Description, Icon
   - Requirements (Level/Attributes)
   - Primary/Secondary bonus maps
   - AttributePointsPerLevel (attribute points gained per class level)
   - ClassExperienceCurve (optional per-class XP curve)
4. Save.

---

## Step 2: Register Data Assets in Asset Manager
1. Open **Project Settings** → **Asset Manager**.
2. Add Primary Asset Types:
   - `ClassDataAsset` (Base Class: `ClassDataAsset`, Scan Paths: `Content/Data/Classes`)
   - `ProfessionDataAsset` (Base Class: `ProfessionDataAsset`, Scan Paths: `Content/Data/Classes`)
3. Ensure `DA_Class_*` and `DA_Profession_*` assets are saved in the scan paths.
4. Save Project Settings.

---

## Step 3: Attach Components to Player
1. Open `BP_ActionRPGPlayerCharacter`.
2. Add components:
   - `ExperienceComponent`
   - `ClassComponent`
3. Compile and save.

---

## Step 4: Test XP Gain
1. PIE.
2. Call `AddExperience(100)` on the player (via Blueprint or console).
3. Confirm unallocated XP increases (Output Log or UI).

---

## Step 5: Test Class Selection
1. Use Blueprint to call `SelectClass(DA_Class_Warrior)` on `ClassComponent`.
2. Verify requirements pass.
3. Confirm bonuses apply to attributes and update secondary attributes.

---

## Step 6: Allocate XP to Class
1. Call `AllocateExperienceToClass(DA_Class_Warrior, 50)` on `ClassComponent`.
2. Verify class XP increases and class level increases when XP threshold is reached.
3. Confirm attribute points are awarded on class level-up.

---

## Step 7: Allocate Attribute Points
1. Call `AllocateAttributePoints(Strength, 1)` on `AttributeComponent`.
2. Verify Strength increases and unspent points decrease.

---

## Step 8: Output Log Validation
1. Open Output Log.
2. Confirm:
   - XP added and class XP changes
   - Class selection success/failure
   - Bonus application events
   - Attribute point allocation events

---

## Step 9: Create UI Widgets
1. Create `WBP_CharacterStatusPanel` (`CharacterStatusPanelWidget`).
2. Add (optional named `TextBlock`s for auto-bind):
   - `CurrentClassText`
   - `CurrentProfessionText`
   - `SelectedClassesText`
   - `SelectedProfessionsText`
   - `ClassSlotsText`
   - `ProfessionSlotsText`
   - `PlayerLevelText`
   - `UnallocatedExperienceText`
   - `TotalExperienceText`
   - `PrimaryAttributesText`
   - `UnspentAttributePointsText`
3. Create `WBP_ClassSelectionPanel` (`ClassSelectionPanelWidget`).
4. Add:
   - ScrollBox/VerticalBox for Classes
   - ScrollBox/VerticalBox for Professions
5. Create `WBP_ClassEntry` (`ClassEntryWidget`).
6. Add:
   - Name text
   - Level text
   - XP progress bar
   - Buttons: Select/Add, Remove, Allocate XP

---

## Step 10: Bind UI to Components
1. In `WBP_CharacterStatusPanel`, bind to `CharacterStatusPanelWidget` getters:
   - `GetCurrentClassName()`, `GetCurrentProfessionName()`
   - `GetSelectedClassesText()`, `GetSelectedProfessionsText()`
   - `GetClassSlotsText()`, `GetProfessionSlotsText()`
   - `GetPlayerLevel()`
   - `GetTotalExperience()`, `GetUnallocatedExperience()`
   - `GetPrimaryAttributesText()`, `GetUnspentAttributePoints()`
2. In `WBP_ClassSelectionPanel`, implement events and list building:
   - Override `OnClassListUpdated(Classes, Professions)`
   - Clear `ClassesList` container (VerticalBox/ScrollBox)
   - For each `ClassAsset`:
     - `Create Widget` → `WBP_ClassEntry`
     - Call `SetClassEntry(ClassAsset, ClassComponent)`
     - Add to `ClassesList`
   - Clear `ProfessionsList` container
   - For each `ProfessionAsset`:
     - `Create Widget` → `WBP_ClassEntry`
     - Call `SetProfessionEntry(ProfessionAsset, ClassComponent)`
     - Add to `ProfessionsList`
   - Override `OnClassProgressUpdated(ClassAsset, NewLevel, NewExperience, ExperienceToNextLevel)`
     - Refresh the matching entry (or re-run `OnClassListUpdated`)
   - Override `OnPlayerLevelUpdated(NewPlayerLevel)` to update any player-level text (optional)
3. In `WBP_ClassEntry`, bind UI fields using getters:
   - Name Text → `GetEntryName()`
   - Level Text → `GetEntryLevel()`
   - XP Progress → `GetEntryExperience()` / `GetEntryExperienceToNextLevel()` (guard divide by zero)
   - Ensure `SetClassEntry` / `SetProfessionEntry` is called after creation
4. For XP allocation UI, call `AllocateExperienceToClass(ClassAsset, Amount)` on `WBP_ClassSelectionPanel` (or use component directly).

---

## Step 11: Add Panel Toggle Input (Optional)
1. Add input actions:
   - `IA_OpenCharacterStatusPanel`
   - `IA_OpenClassPanel`
2. In PlayerController, assign:
   - `CharacterStatusPanelWidgetClass`
   - `ClassSelectionPanelWidgetClass`
3. Ensure panels use `SelfHitTestInvisible` if overlapping other UI.
