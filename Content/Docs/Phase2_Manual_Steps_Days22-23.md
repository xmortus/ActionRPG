# Phase 2 Manual Steps - Days 22-23
**Inventory UI Foundation**  
**Date:** 2025-01-07  
**Phase:** Phase 2 - Inventory System  
**Days:** 22-23 - Inventory UI Foundation

---

## Overview

Days 22-23 focus on creating the inventory UI system. You will create C++ widget classes, design Blueprint widgets, implement widget communication with the InventoryComponent, and add the inventory toggle functionality. This establishes the foundation for drag and drop functionality in Days 24-25.

---

## Prerequisites

- ✅ All C++ code compiled successfully
- ✅ Days 15-21 complete (InventoryComponent, ItemPickupActor, Item Usage System, Integration Testing)
- ✅ InventoryComponent functional and tested
- ✅ Items can be picked up and added to inventory
- ✅ Items can be used from inventory
- ✅ Test Item Data Assets created with icons (ItemIcon property set)
- ✅ Unreal Editor is open
- ✅ Project is loaded in the editor
- ✅ No compilation errors in Output Log
- ✅ Visual Studio (for C++ code editing)

**Before Starting:**
1. Ensure all previous days' work is complete and tested
2. Have Visual Studio ready for C++ code editing
3. Have Unreal Editor ready for Blueprint/UMG editing
4. Ensure test level has items to pick up for testing

---

## Day 22: C++ Widget Classes and Structure

### Step 1: Create Folder Structure

#### 1.1 Create Source Folders

1. **Open File Explorer**
   - Navigate to: `D:\Unreal Projects\ActionRPG\Source\ActionRPG\`
   - Or use your project path

2. **Create Public Folder**
   - Navigate to `Public/UI/`
   - If `UI` folder doesn't exist, create it
   - Create new folder: `Inventory`
   - Path should be: `Source/ActionRPG/Public/UI/Inventory/`

3. **Create Private Folder**
   - Navigate to `Private/UI/`
   - If `UI` folder doesn't exist, create it
   - Create new folder: `Inventory`
   - Path should be: `Source/ActionRPG/Private/UI/Inventory/`

#### 1.2 Create Content Folders

1. **Open Unreal Editor**
   - Go to Content Browser
   - Navigate to `Content/UI/`

2. **Create Inventory Folder**
   - Right-click in Content Browser
   - Select `New Folder`
   - Name it: `Inventory`
   - Path should be: `Content/UI/Inventory/`

**Final Structure:**
```
Source/ActionRPG/
├── Public/UI/Inventory/
│   ├── InventoryWidget.h
│   └── InventorySlotWidget.h
└── Private/UI/Inventory/
    ├── InventoryWidget.cpp
    └── InventorySlotWidget.cpp

Content/UI/Inventory/
├── WBP_InventoryWidget.uasset
└── WBP_InventorySlotWidget.uasset
```

---

### Step 2: Create InventorySlotWidget Class (C++)

#### 2.1 Create InventorySlotWidget Header

1. **Open Visual Studio**
   - Right-click on `Public/UI/Inventory/` folder in Solution Explorer
   - Select `Add` → `New Item...`
   - Select `Visual C++` → `Header File (.h)`
   - Name: `InventorySlotWidget.h`
   - Click `Add`

2. **Write Header Code**
   - Copy the InventorySlotWidget header code from implementation plan
   - Ensure includes are correct:
     ```cpp
     #include "Components/Image.h"
     #include "Components/TextBlock.h"
     #include "Components/Border.h"
     #include "Items/Core/ItemBase.h"
     ```
   - Ensure class inherits from `UUserWidget`
   - Ensure `GENERATED_BODY()` is present
   - Save file

**Key Features to Include:**
- Widget references: ItemIcon (Image), QuantityText (TextBlock), SlotBorder (Border)
- Properties: SlotIndex (int32), CurrentItem (TObjectPtr<UItemBase>)
- Methods: SetSlotData(), ClearSlot(), UpdateSlotVisuals()
- Override: NativeOnMouseButtonDown() for click detection

#### 2.2 Create InventorySlotWidget Implementation

1. **Create Implementation File**
   - Right-click on `Private/UI/Inventory/` folder in Solution Explorer
   - Select `Add` → `New Item...`
   - Select `Visual C++` → `C++ File (.cpp)`
   - Name: `InventorySlotWidget.cpp`
   - Click `Add`

2. **Write Implementation Code**
   - Include header: `#include "UI/Inventory/InventorySlotWidget.h"`
   - Include necessary headers:
     ```cpp
     #include "Components/Image.h"
     #include "Components/TextBlock.h"
     #include "Components/Border.h"
     #include "Items/Core/ItemBase.h"
     #include "Items/Core/ItemDataAsset.h"
     ```
   - Implement NativeConstruct: Initialize to empty state
   - Implement SetSlotData: Set item and quantity, update visuals
   - Implement ClearSlot: Reset to empty state
   - Implement UpdateSlotVisuals: Update icon, quantity text, border
   - Implement NativeOnMouseButtonDown: Detect clicks

3. **Compile Code**
   - Save all files (Ctrl+Shift+S)
   - Build Solution (Build → Build Solution or F7)
   - Wait for compilation to complete
   - Check for errors in Output window
   - Fix any errors before proceeding

---

### Step 3: Create InventoryWidget Class (C++)

#### 3.1 Create InventoryWidget Header

1. **Create Header File**
   - Right-click on `Public/UI/Inventory/` folder
   - Select `Add` → `New Item...`
   - Select `Header File (.h)`
   - Name: `InventoryWidget.h`
   - Click `Add`

2. **Write Header Code**
   - Include necessary headers:
     ```cpp
     #include "Components/UniformGridPanel.h"
     #include "Components/TextBlock.h"
     #include "Components/Button.h"
     #include "Components/Inventory/InventoryComponent.h"
     #include "UI/Inventory/InventorySlotWidget.h"
     ```
   - Ensure class inherits from `UUserWidget`
   - Ensure `GENERATED_BODY()` is present

**Key Features to Include:**
- Widget references: InventoryGrid (UniformGridPanel), WeightText (TextBlock), CapacityText (TextBlock), CloseButton (Button)
- Properties: SlotWidgetClass (TSubclassOf<UInventorySlotWidget>), SlotWidgets (TArray), InventoryComponent (TObjectPtr)
- Methods: UpdateInventoryDisplay(), OnInventorySlotClicked(), OnInventorySlotRightClicked()
- Overrides: NativeConstruct(), NativeDestruct()

#### 3.2 Create InventoryWidget Implementation

1. **Create Implementation File**
   - Right-click on `Private/UI/Inventory/` folder
   - Select `Add` → `New Item...`
   - Select `C++ File (.cpp)`
   - Name: `InventoryWidget.cpp`
   - Click `Add`

2. **Write Implementation Code**
   - Include header: `#include "UI/Inventory/InventoryWidget.h"`
   - Include necessary headers:
     ```cpp
     #include "Components/UniformGridPanel.h"
     #include "Components/TextBlock.h"
     #include "Components/Button.h"
     #include "Components/Inventory/InventoryComponent.h"
     #include "UI/Inventory/InventorySlotWidget.h"
     #include "Characters/ActionRPGPlayerCharacter.h"
     #include "GameFramework/PlayerController.h"
     ```
   - Implement NativeConstruct:
     - Get PlayerCharacter from PlayerController
     - Get InventoryComponent from PlayerCharacter
     - Bind to InventoryComponent events
     - Initialize slot widgets
   - Implement NativeDestruct:
     - Unbind from events
   - Implement UpdateInventoryDisplay:
     - Refresh all slots
     - Update weight/capacity text
   - Implement InitializeSlots:
     - Create 50 slot widgets (10x5 grid)
     - Add to UniformGridPanel
   - Implement RefreshSlot:
     - Update individual slot widget

3. **Compile Code**
   - Save all files
   - Build Solution
   - Wait for compilation
   - Check for errors
   - Fix any errors before proceeding

---

### Step 4: Update PlayerController for Inventory Toggle

#### 4.1 Update ActionRPGPlayerController.h

1. **Open ActionRPGPlayerController.h**
   - Locate file: `Source/ActionRPG/Public/Core/ActionRPGPlayerController.h`

2. **Add InventoryWidget Reference**
   - Add property:
     ```cpp
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
     TSubclassOf<class UUserWidget> InventoryWidgetClass;
     
     UPROPERTY()
     TObjectPtr<class UUserWidget> InventoryWidget;
     ```
   - Add include if needed: `#include "UI/Inventory/InventoryWidget.h"` (forward declare instead)

3. **Add Methods (Optional - can be done in Blueprint)**
   - OnOpenInventory() already exists - will be implemented in .cpp

#### 4.2 Update ActionRPGPlayerController.cpp

1. **Open ActionRPGPlayerController.cpp**
   - Locate OnOpenInventory() method (around line 253)

2. **Implement OnOpenInventory()**
   ```cpp
   void AActionRPGPlayerController::OnOpenInventory()
   {
       if (!InventoryWidget)
       {
           if (InventoryWidgetClass)
           {
               InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
               if (InventoryWidget)
               {
                   InventoryWidget->AddToViewport();
               }
           }
       }
       
       if (InventoryWidget)
       {
           if (InventoryWidget->IsInViewport())
           {
               // Hide inventory
               InventoryWidget->RemoveFromViewport();
               SetInputMode(FInputModeGameOnly());
               bShowMouseCursor = false;
           }
           else
           {
               // Show inventory
               InventoryWidget->AddToViewport();
               SetInputMode(FInputModeGameAndUI());
               bShowMouseCursor = true;
           }
       }
   }
   ```

3. **Add Includes**
   - Add at top of file:
     ```cpp
     #include "UI/Inventory/InventoryWidget.h"
     #include "Blueprint/UserWidget.h"
     ```

4. **Compile Code**
   - Save all files
   - Build Solution
   - Check for errors
   - Fix any errors

---

## Day 23: Blueprint Widgets and Integration

### Step 5: Create Blueprint Widget Classes

#### 5.1 Create WBP_InventorySlotWidget

1. **Open Content Browser**
   - Navigate to `Content/UI/Inventory/`

2. **Create Widget Blueprint**
   - Right-click in Content Browser
   - Select `User Interface` → `Widget Blueprint`
   - Name: `WBP_InventorySlotWidget`
   - Double-click to open in Widget Designer

3. **Set Parent Class**
   - In Widget Designer, click `Class Settings` button (top toolbar)
   - In `Details` panel, find `Parent Class`
   - Select: `Inventory Slot Widget` (your C++ class)
   - If not visible, close and reopen editor, then try again

4. **Design Slot Widget Layout**
   - **Important:** In UE 5.7, a Border widget can only have one child. Since we need three separate widgets (Border, Image, TextBlock) that must be direct children of the root for BindWidget to work, we'll use the default Canvas Panel root and make all three widgets siblings (not nested).
   - In `Palette` panel, drag widgets into `Designer` view:
   
   **a. Root Container (Canvas Panel)**
   - The root widget should already be a Canvas Panel (this is the default)
   - Select the root widget in the `Hierarchy` panel
   - In `Details` panel, set Size: 64x64 (or desired slot size)
   - **Note:** Canvas Panel allows multiple children with custom positioning
   
   **b. Border (Background)**
   - Drag `Border` from Palette onto the root Canvas Panel
   - Name: `SlotBorder` (must match BindWidget name exactly!)
   - **Critical:** Make sure it's a direct child of the Canvas Panel root (not nested inside another widget)
   - In `Details` panel → `Slot` section:
     - Set Anchors: Full Screen (or Left=0, Top=0, Right=1, Bottom=1)
     - Set Offsets: Left=0, Top=0, Right=0, Bottom=0 (fills entire parent)
   - In `Details` panel → `Appearance`:
     - Set Brush Color: Dark gray (for empty slot)
     - Set Padding: 2px all sides
   - This serves as the background/border for the slot
   
   **c. Image (Item Icon)**
   - Drag `Image` from Palette onto the root Canvas Panel (NOT inside the Border!)
   - Name: `ItemIcon` (must match BindWidget name exactly!)
   - **Critical:** Make sure it's a direct child of the Canvas Panel root (same level as SlotBorder, not inside it)
   - In `Details` panel → `Slot` section:
     - Set Anchors: Center (or Left=0.5, Top=0.5, Right=0.5, Bottom=0.5)
     - Set Offsets: Left=-30, Top=-30, Right=30, Bottom=30 (for 60x60 size, centered)
   - Set Size: 60x60 (or slightly smaller than slot size)
   - In `Details` panel → `Slot` → `Size to Content`: Unchecked (use manual size)
   - Set Visibility: Hidden (initially empty, will be shown when item is set)
   - This will display on top of the Border background
   
   **d. Text Block (Quantity)**
   - Drag `Text` from Palette onto the root Canvas Panel (NOT inside Border or Image!)
   - Name: `QuantityText` (must match BindWidget name exactly!)
   - **Critical:** Make sure it's a direct child of the Canvas Panel root (same level as SlotBorder and ItemIcon)
   - In `Details` panel → `Slot` section:
     - Set Anchors: Bottom-Right (or Left=1, Top=1, Right=1, Bottom=1)
     - Set Offsets: Right=-8, Bottom=-8 (4px padding from edges, adjust width/height as needed)
   - Set Text: "1" (placeholder)
   - In `Details` panel → `Appearance`:
     - Set Font Size: 14, Bold
     - Set Color: White with slight shadow for visibility
   - Set Visibility: Collapsed (hidden when quantity = 0 or quantity = 1)
   - Set Alignment: Right, Bottom
   - This will display on top of both Border and Image in the bottom-right corner
   
   **Widget Hierarchy Should Look Like:**
   ```
   WBP_InventorySlotWidget (Root - Canvas Panel)
   ├── SlotBorder (Border - background, fills entire slot)
   ├── ItemIcon (Image - centered, 60x60)
   └── QuantityText (TextBlock - bottom-right corner)
   ```
   
   **Critical Points:**
   - All three widgets (SlotBorder, ItemIcon, QuantityText) must be direct children of the Canvas Panel root
   - They must NOT be nested inside each other (e.g., Image should NOT be inside Border)
   - This is required for the `UPROPERTY(meta = (BindWidget))` in C++ to find them correctly
   - Widget order in Hierarchy determines draw order (Border draws first, then Image, then Text on top)

5. **Verify Widget Names Match C++**
   - All widget names must exactly match the `UPROPERTY(meta = (BindWidget))` names in C++
   - ItemIcon, QuantityText, SlotBorder must match exactly
   - Case-sensitive!

6. **Compile Widget**
   - Click `Compile` button (top toolbar)
   - Check for errors in Output Log
   - Fix any errors

7. **Save Widget**
   - Click `Save` button (Ctrl+S)
   - Close Widget Designer

#### 5.2 Create WBP_InventoryWidget

1. **Create Widget Blueprint**
   - Right-click in `Content/UI/Inventory/`
   - Select `User Interface` → `Widget Blueprint`
   - Name: `WBP_InventoryWidget`
   - Double-click to open

2. **Set Parent Class**
   - Click `Class Settings`
   - Set `Parent Class` to: `Inventory Widget` (your C++ class)

3. **Design Inventory Widget Layout**
   - **Critical:** The root Canvas Panel must have a fixed size and be centered. The widget size showing as 0.0 means the root container doesn't have proper sizing.
   - In `Palette`, drag widgets into `Designer`:

   **a. Root Container (Canvas Panel)**
   - The root widget should be a `Canvas Panel` (default)
   - Select the root widget in the `Hierarchy` panel
   - In `Details` panel → `Slot` section:
     - **Set Anchors:** Center (or Left=0.5, Top=0.5, Right=0.5, Bottom=0.5)
     - **Set Offsets:** Left=-400, Top=-300, Right=400, Bottom=300 (for 800x600 size)
     - **Alternative:** Use Left=0.4, Top=0.3, Right=0.6, Bottom=0.7 to anchor from edges
   - This centers a 800x600 widget on screen (adjust offsets if using different size)
   
   **b. Background Border**
   - Drag `Border` widget onto the root Canvas Panel
   - Set Anchors: Fill (or Left=0, Top=0, Right=0, Bottom=0)
   - Set Offsets: All 0 (fills entire root container)
   - In `Details` panel → `Appearance`:
     - Set Brush Color: Dark semi-transparent (e.g., R=20, G=20, B=30, A=230)
     - Or use a solid color with slight transparency
   - This provides the background for the inventory panel

   **c. Title Text**
   - Drag `Text` widget onto the root Canvas Panel
   - Set Anchors: Top-Center (or Left=0.5, Top=0.05, Right=0.5, Bottom=0.1)
   - Set Text: "INVENTORY"
   - Set Font Size: 24, Bold
   - Position at top center of the panel

   **d. Uniform Grid Panel (Inventory Slots)**
   - Drag `Uniform Grid Panel` from Palette onto the root Canvas Panel
   - **Name: `InventoryGrid`** (must match BindWidget exactly!)
   - Set Anchors: Top-Left (or Left=0.1, Top=0.15, Right=0.9, Bottom=0.75)
   - Set Offsets: Left=40, Top=100, Right=-40, Bottom=-100 (adjust for padding)
   - In `Details` panel → `Slot` → `Size To Content`: **Unchecked**
   - Set Slot Padding: 4px (spacing between slots in the grid)
   - The grid will be populated programmatically with slot widgets
   - Size should be enough for 10 columns x 5 rows (e.g., 640x320 pixels with 64px slots)

   **e. Weight Text**
   - Drag `Text` widget onto the root Canvas Panel
   - **Name: `WeightText`** (must match BindWidget exactly!)
   - Set Anchors: Bottom-Left (or Left=0.1, Top=0.8, Right=0.4, Bottom=0.85)
   - Set Text: "Weight: 0.0 / 100.0"
   - Set Font Size: 14
   - Position in bottom-left area of the panel

   **f. Capacity Text**
   - Drag `Text` widget onto the root Canvas Panel
   - **Name: `CapacityText`** (must match BindWidget exactly!)
   - Set Anchors: Bottom-Center (or Left=0.5, Top=0.8, Right=0.8, Bottom=0.85)
   - Set Text: "Capacity: 0 / 50"
   - Set Font Size: 14
   - Position next to WeightText or in bottom-center area

   **g. Close Button**
   - Drag `Button` widget onto the root Canvas Panel
   - **Name: `CloseButton`** (must match BindWidget exactly!)
   - Set Anchors: Top-Right (or Left=0.9, Top=0.05, Right=0.98, Bottom=0.12)
   - Set Size: 60x40 (or appropriate for button)
   - Set Text on button: "Close" or "X"
   - Position in top-right corner of the panel

   **Important Structure:**
   ```
   WBP_InventoryWidget (Root - Canvas Panel, 800x600, centered)
   ├── Background Border (fills entire root)
   ├── Title Text (top center)
   ├── InventoryGrid (UniformGridPanel - main content area)
   ├── WeightText (bottom-left)
   ├── CapacityText (bottom-center)
   └── CloseButton (top-right)
   ```
   
   **Critical Points:**
   - Root Canvas Panel MUST have explicit size via anchors/offsets (not 0x0)
   - Root Canvas Panel MUST be centered (anchors = center, not fill screen)
   - All child widgets must be direct children of the root Canvas Panel
   - Widget names (InventoryGrid, WeightText, CapacityText, CloseButton) must match C++ exactly
   - If widget size shows as 0.0 in logs, check root widget anchors/offsets are set correctly

4. **Verify Widget Names**
   - All widget names must match C++ BindWidget names exactly:
     - InventoryGrid
     - WeightText
     - CapacityText
     - CloseButton

5. **Set Slot Widget Class**
   - In `Details` panel (when root widget selected), find `Slot Widget Class`
   - Set to: `WBP_InventorySlotWidget`

6. **Connect Close Button**
   - Select `CloseButton`
   - In `Details` panel, find `Events` section
   - Click `+ OnClicked`
   - In Graph view, create event handler
   - Call `Remove From Parent` to close widget
   - Or call PlayerController's OnOpenInventory to toggle

7. **Compile Widget**
   - Click `Compile`
   - Check for errors
   - Fix any errors

8. **Save Widget**
   - Click `Save`
   - Close Widget Designer

---

### Step 6: Set Up Widget in PlayerController or HUD

#### 6.1 Option A: Set Up in PlayerController Blueprint (Recommended)

1. **Open BP_ActionRPGPlayerController**
   - Navigate to `Content/Blueprints/Core/BP_ActionRPGPlayerController`
   - Or create if doesn't exist
   - Double-click to open

2. **Set Inventory Widget Class**
   - In `Details` panel (Class Defaults view)
   - Find `Inventory Widget Class` property
   - Set to: `WBP_InventoryWidget`

3. **Compile Blueprint**
   - Click `Compile`
   - Check for errors
   - Save Blueprint

#### 6.2 Option B: Set Up in HUD Blueprint

1. **Open BP_ActionRPGHUD**
   - Navigate to `Content/UI/BP_ActionRPGHUD`
   - Double-click to open

2. **Add Inventory Widget**
   - In `Event Graph`, create `BeginPlay` event
   - Create widget: `Create Widget` node
   - Set `Class` to `WBP_InventoryWidget`
   - Store widget reference in variable
   - Initially hide widget (`Set Visibility` → Hidden)

3. **Add Toggle Function**
   - Create custom function: `ToggleInventory`
   - Show/hide widget based on current visibility
   - Update input mode and mouse cursor

4. **Compile and Save**
   - Click `Compile`
   - Save Blueprint

---

### Step 7: Test Inventory UI

#### 7.1 Test Widget Creation

1. **Open Test Level**
   - Open your test level
   - Ensure Player Start exists

2. **Start Play In Editor (PIE)**
   - Press `Alt + P` or click `Play`
   - Game should start

3. **Open Output Log**
   - Window → `Developer Tools` → `Output Log`
   - Look for any errors related to widgets

4. **Test Inventory Toggle**
   - Press the key bound to `IA_OpenInventory` (default: I key)
   - Inventory widget should appear
   - Widget should show 50 empty slots in grid
   - Weight and Capacity should display (0.0 / 100.0 and 0 / 50)

5. **Test Close Button**
   - Click `Close` button in inventory
   - Widget should disappear
   - Mouse cursor should hide
   - Game input should be restored

6. **Test Toggle Again**
   - Press inventory key again
   - Widget should appear again
   - Test multiple toggles

#### 7.2 Test Slot Display

1. **Pick Up Some Items**
   - Use IA_Interact to pick up items in the level
   - Pick up 2-3 different items
   - Items should be added to inventory

2. **Open Inventory**
   - Press inventory key
   - Widget should open

3. **Verify Slots Update**
   - Slots with items should show:
     - Item icon (if ItemIcon is set in ItemDataAsset)
     - Quantity text in bottom-right corner
   - Empty slots should show empty border
   - Weight and Capacity text should update

4. **Check Output Log**
   - Look for any errors or warnings
   - Should see logs from InventoryComponent about items added
   - Should see logs from widget updating

#### 7.3 Test Item Usage from UI

1. **Right-Click Item in Inventory**
   - Open inventory
   - Right-click on an item slot with an item
   - Item should be used (if consumable, quantity should decrease)
   - Slot should update after use
   - Health potion should heal player if health < max

2. **Verify Usage**
   - Check Output Log for usage messages
   - Verify item quantity decreased
   - Verify health increased (for health potions)

---

## Troubleshooting

### Issue: Widget Not Appearing When Opening Inventory

**Symptoms:** Pressing inventory key does nothing or causes crash

**Solutions:**
1. **Check Widget Class is Set**
   - Verify `InventoryWidgetClass` is set in PlayerController Blueprint
   - Verify it points to `WBP_InventoryWidget`

2. **Check Widget Names Match C++**
   - All BindWidget names must match exactly
   - Case-sensitive: `InventoryGrid` not `inventorygrid`
   - Check Output Log for "Failed to bind widget" errors

3. **Check Widget is Compiled**
   - Make sure WBP_InventoryWidget compiles without errors
   - Check Compiler Results in Output Log

4. **Check C++ Code Compiled**
   - Verify C++ code compiled successfully
   - Restart editor if needed

### Issue: Slots Not Showing Items

**Symptoms:** Items are in inventory but slots appear empty

**Solutions:**
1. **Check Item Icons**
   - Verify ItemDataAsset has `ItemIcon` property set
   - Verify icon texture is valid

2. **Check Widget Binding**
   - Verify `SetSlotData` is being called
   - Check Output Log for binding errors
   - Verify slot index matches inventory slot index

3. **Check UpdateInventoryDisplay**
   - Verify `UpdateInventoryDisplay` is called on inventory changes
   - Verify event bindings are working
   - Check Output Log for update messages

4. **Check Slot Widget Class**
   - Verify `SlotWidgetClass` is set to `WBP_InventorySlotWidget` in InventoryWidget
   - Verify slot widgets are being created

### Issue: Weight/Capacity Not Updating

**Symptoms:** Weight and capacity text always shows 0

**Solutions:**
1. **Check Widget Names**
   - Verify `WeightText` and `CapacityText` names match C++
   - Case-sensitive!

2. **Check UpdateInventoryDisplay**
   - Verify method is being called
   - Check code is updating text correctly
   - Add debug logs to verify values

3. **Check InventoryComponent Methods**
   - Verify `GetCurrentWeight()` and `GetTotalItemCount()` work
   - Test methods directly in Blueprint or C++

### Issue: Compilation Errors in C++

**Symptoms:** C++ code won't compile

**Solutions:**
1. **Check Includes**
   - Verify all necessary headers are included
   - Check forward declarations if needed

2. **Check Forward Declarations**
   - Use forward declarations for UClasses in headers
   - Include full headers in .cpp files

3. **Check Class Names**
   - Verify class names match exactly
   - Check ACTIONRPG_API macro is present

4. **Check GENERATED_BODY()**
   - Must be present in all UCLASS, USTRUCT, UENUM

5. **Restart Visual Studio**
   - Sometimes IntelliSense cache causes issues
   - Close and reopen Visual Studio

### Issue: Blueprint Widget Won't Compile

**Symptoms:** Widget Blueprint has compilation errors

**Solutions:**
1. **Check Parent Class**
   - Verify Parent Class is set correctly
   - Must match C++ class name exactly

2. **Check Widget Names**
   - All BindWidget names must match C++ exactly
   - No typos or case mismatches

3. **Check Required Widgets Exist**
   - All widgets referenced in C++ must exist in Blueprint
   - Check Designer view has all required widgets

4. **Check Widget Types**
   - Verify widget types match (Image vs TextBlock, etc.)
   - Check widget hierarchy is correct

---

## Verification Checklist

Before moving to Days 24-25, verify:

- [ ] InventorySlotWidget C++ class compiles without errors
- [ ] InventoryWidget C++ class compiles without errors
- [ ] WBP_InventorySlotWidget Blueprint created and compiles
- [ ] WBP_InventoryWidget Blueprint created and compiles
- [ ] All widget names match C++ BindWidget names exactly
- [ ] Inventory toggle works (key press opens/closes widget)
- [ ] Widget displays 50 slots in 10x5 grid
- [ ] Empty slots show correctly
- [ ] Items in inventory display in slots (icons and quantities)
- [ ] Weight and capacity text updates when items added/removed
- [ ] Right-click on item uses item correctly
- [ ] Close button closes inventory
- [ ] Input mode switches correctly (GameAndUI vs GameOnly)
- [ ] Mouse cursor shows/hides correctly
- [ ] No errors in Output Log during testing
- [ ] Ready for drag and drop implementation (Day 24)

---

## Next Steps

Once Days 22-23 are complete:

1. **Days 24-25** will focus on:
   - Implementing drag and drop system
   - ItemDragDropOperation class
   - Drag detection and drop logic
   - Visual feedback during drag
   - Special cases (stacking, swapping, etc.)

2. **Before Days 24-25**, you should have:
   - Working inventory UI displaying items
   - Slots clickable and responsive
   - Widget communication working
   - Ready to add drag and drop functionality

---

## Notes

- **Widget Binding:** All widget names must match C++ BindWidget names exactly (case-sensitive)
- **Slot Index:** Ensure slot indices match between inventory slots and widget slots (0-49)
- **Event Binding:** Widget must bind to InventoryComponent events in NativeConstruct
- **Performance:** Consider optimizing slot updates (only update changed slots, not all)
- **Visual Polish:** Slot appearance can be improved later; focus on functionality first

---

## Quick Reference

### Widget File Paths
- **C++ Header:** `Source/ActionRPG/Public/UI/Inventory/InventoryWidget.h`
- **C++ Implementation:** `Source/ActionRPG/Private/UI/Inventory/InventoryWidget.cpp`
- **Slot Widget Header:** `Source/ActionRPG/Public/UI/Inventory/InventorySlotWidget.h`
- **Slot Widget Implementation:** `Source/ActionRPG/Private/UI/Inventory/InventorySlotWidget.cpp`
- **Blueprint Widget:** `Content/UI/Inventory/WBP_InventoryWidget`
- **Slot Blueprint Widget:** `Content/UI/Inventory/WBP_InventorySlotWidget`

### Key Widget Names (Must Match Exactly)
- `InventoryGrid` - UniformGridPanel for slots
- `WeightText` - TextBlock for weight display
- `CapacityText` - TextBlock for capacity display
- `CloseButton` - Button to close inventory
- `ItemIcon` - Image for item icon (in slot widget)
- `QuantityText` - TextBlock for quantity (in slot widget)
- `SlotBorder` - Border for slot background (in slot widget)

### Key Methods
- `UpdateInventoryDisplay()` - Refreshes all slots and displays
- `RefreshSlot(int32 SlotIndex)` - Updates individual slot
- `SetSlotData(int32 SlotIndex, UItemBase* Item, int32 Quantity)` - Sets slot data
- `OnInventorySlotRightClicked(int32 SlotIndex)` - Handles right-click (use item)

---

**Status:** Complete these manual steps before proceeding to Days 24-25

---

**End of Phase 2 Manual Steps - Days 22-23**
