# Phase 4 Manual Steps - Days 42-44
**Focus:** Primary Attributes + Secondary Attributes  
**UE Version:** 5.7  
**Last Updated:** 2026-01-22

---

## Step 1: Create Attribute Data Assets
1. Navigate to `Content/Data/Attributes/` (create folder if missing).
2. Create Data Assets:
   - `DA_PrimaryAttributes_Base` (PrimaryAttributeDataAsset)
   - `DA_SecondaryAttributes_Base` (SecondaryAttributeDataAsset)
3. Populate defaults (example):
   - STR = 10, AGI = 10, CON = 10, DEX = 10, INT = 10, WIS = 10
4. Save.

---

## Step 2: Attach Components to Player
1. Open `BP_ActionRPGPlayerCharacter`.
2. Add components:
   - `AttributeComponent`
   - `SecondaryAttributeComponent`
3. Assign Data Assets to component fields (if exposed).
4. Compile and save.

---

## Step 3: Basic Validation
1. PIE.
2. Open Output Log and confirm:
   - AttributeComponent initializes with default values
   - SecondaryAttributeComponent recalculates on BeginPlay
3. Modify a primary attribute in the component details (PIE) and confirm secondary attributes update.
