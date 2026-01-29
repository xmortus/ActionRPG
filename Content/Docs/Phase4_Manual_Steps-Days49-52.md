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
   - RequiredClassLevelToUnlock (class level needed before auto-add)
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

## Step 5: Pick a Class
1. Call `SelectClass(DA_Class_Warrior)` on `ClassComponent`.
2. Verify requirements pass.
3. Confirm bonuses apply to attributes and update secondary attributes.
4. From UI:
   - Open the Class Selection panel.
   - Click your class entry (wire button to `AddClass` or `SelectClass`).
   - Confirm the class appears in the selected list.

---

## Step 6: Add XP to a Class
1. Call `AllocateExperienceToClass(DA_Class_Warrior, 50)` on `ClassComponent`.
2. Verify class XP increases and class level increases when XP threshold is reached.
3. Confirm attribute points are awarded on class level-up.
4. From UI:
   - Enter XP amount (use a numeric input or a preset button).
   - Click Allocate on the class entry (wire to `AllocateExperienceToClass`).
   - Verify the row updates (level/XP/next level).

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
   - `AllClassesList` (VerticalBox or ScrollBox, **Is Variable** checked)
   - `ClassesList` (VerticalBox or ScrollBox, **Is Variable** checked)
   - `ProfessionsList` (VerticalBox or ScrollBox, **Is Variable** checked)
5. Create `WBP_ClassEntry` (`ClassEntryWidget`).
6. Add:
   - Name text
   - Level text
   - XP progress bar
   - Button: Select/Add (optional)
7. Add UI controls for class selection and XP allocation in `WBP_ClassSelectionPanel`:
   - `Input_XPAmount` (numeric `EditableTextBox`) or preset buttons (e.g., +10, +50)
   - `Button_AllocateXP`
   - (Optional) Text showing the currently selected class for XP

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
   - Override `OnAllClassesListUpdated(Classes)`:
     - Clear `AllClassesList`
     - For each `ClassAsset`:
       - `Create Widget` → `WBP_ClassEntry`
       - Get `ClassComponent` from owning pawn
       - Call `SetClassEntry(ClassAsset, ClassComponent)`
       - Add to `AllClassesList`
   - Override `OnClassListUpdated(Classes, Professions)`
   - Clear `ClassesList` container
   - For each `ClassAsset`:
     - `Create Widget` → `WBP_ClassEntry`
     - Get `ClassComponent` from owning pawn
     - Call `SetClassEntry(ClassAsset, ClassComponent)`
     - Add to `ClassesList`
   - Clear `ProfessionsList` container
   - For each `ProfessionAsset`:
     - `Create Widget` → `WBP_ClassEntry`
     - Get `ClassComponent` from owning pawn
     - Call `SetProfessionEntry(ProfessionAsset, ClassComponent)`
     - Add to `ProfessionsList`
   - Override `OnClassProgressUpdated(ClassAsset, NewLevel, NewExperience, ExperienceToNextLevel)`
     - Rebuild list or refresh matching entry
   - Override `OnPlayerLevelUpdated(NewPlayerLevel)` to update any player-level text (optional)
   - Override `OnSelectedClassForXpChanged(ClassAsset)` to update selection highlight (optional)
3. In `WBP_ClassEntry`, bind UI fields using getters:
   - Name Text → `GetEntryName()`
   - Level Text → `GetEntryLevel()`
   - XP Progress → `GetEntryExperience()` / `GetEntryExperienceToNextLevel()` (guard divide by zero)
   - Ensure `SetClassEntry` / `SetProfessionEntry` is called after creation
4. For class unlocking via XP (single panel controls):
   - On class entry click, call `SetSelectedClassForXp(ClassAsset)`
   - On `Button_AllocateXP`, call `AllocateExperienceToSelectedClass(Amount)`
   - Set `RequiredClassLevelToUnlock` in the class data asset to control when it gets added

---

## Step 11: Add Panel Toggle Input (Optional)
1. Add input actions:
   - `IA_OpenCharacterStatusPanel`
   - `IA_OpenClassPanel`
2. In PlayerController, assign:
   - `CharacterStatusPanelWidgetClass`
   - `ClassSelectionPanelWidgetClass`
3. Ensure panels use `SelfHitTestInvisible` if overlapping other UI.
