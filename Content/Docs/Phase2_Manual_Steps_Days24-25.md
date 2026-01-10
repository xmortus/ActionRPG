# Phase 2 Manual Steps - Days 24-25
**Drag and Drop System**  
**Date:** 2025-01-07  
**Phase:** Phase 2 - Inventory System  
**Days:** 24-25 - Drag and Drop System

---

## Overview

Days 24-25 focus on implementing drag and drop functionality for the inventory system. You will create the ItemDragDropOperation class, implement drag detection and drop logic in the InventorySlotWidget, add visual feedback during drag operations, and handle special cases like stacking, swapping, and partial stack splits.

---

## Prerequisites

- ✅ All C++ code compiled successfully
- ✅ Days 22-23 complete (InventoryWidget and InventorySlotWidget working)
- ✅ Inventory UI displays correctly
- ✅ Slots update when inventory changes
- ✅ Items can be picked up and added to inventory
- ✅ Right-click use item works
- ✅ Inventory toggle works correctly
- ✅ Unreal Editor is open
- ✅ Project is loaded in the editor
- ✅ No compilation errors in Output Log
- ✅ Visual Studio (for C++ code editing)

**Before Starting:**
1. Ensure Days 22-23 are complete and tested
2. Have Visual Studio ready for C++ code editing
3. Have Unreal Editor ready for Blueprint/UMG editing
4. Ensure test level has items to test drag and drop
5. Have inventory populated with test items (at least 2-3 different items)

---

## Day 24: Drag Detection and DragDropOperation

### Step 1: Create ItemDragDropOperation Class

#### 1.1 Create Header File

1. **Open Visual Studio**
   - Right-click on `Public/UI/Inventory/` folder in Solution Explorer
   - Select `Add` → `New Item...`
   - Select `Visual C++` → `Header File (.h)`
   - Name: `ItemDragDropOperation.h`
   - Click `Add`

2. **Write Header Code**
   ```cpp
   #pragma once

   #include "CoreMinimal.h"
   #include "Blueprint/DragDropOperation.h"
   #include "Items/Core/ItemBase.h"
   #include "ItemDragDropOperation.generated.h"

   /**
    * Drag and drop operation for inventory items.
    * Handles dragging items between inventory slots.
    */
   UCLASS()
   class ACTIONRPG_API UItemDragDropOperation : public UDragDropOperation
   {
       GENERATED_BODY()

   public:
       UItemDragDropOperation();

       // Source slot index where the drag started
       UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
       int32 SourceSlotIndex;

       // Item being dragged
       UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
       TObjectPtr<UItemBase> Item;

       // Quantity being dragged (for partial stack splits)
       UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
       int32 Quantity;

       // Full quantity in source slot (for reference)
       UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
       int32 SourceQuantity;

       // Is this a partial stack split (right-click drag)?
       UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
       bool bIsSplitOperation;
   };
   ```

3. **Save File**
   - Press `Ctrl + S` to save

#### 1.2 Create Implementation File

1. **Create Implementation File**
   - Right-click on `Private/UI/Inventory/` folder in Solution Explorer
   - Select `Add` → `New Item...`
   - Select `Visual C++` → `C++ File (.cpp)`
   - Name: `ItemDragDropOperation.cpp`
   - Click `Add`

2. **Write Implementation Code**
   ```cpp
   #include "UI/Inventory/ItemDragDropOperation.h"
   #include "Items/Core/ItemBase.h"
   #include "Items/Core/ItemDataAsset.h"

   UItemDragDropOperation::UItemDragDropOperation()
   {
       SourceSlotIndex = -1;
       Item = nullptr;
       Quantity = 0;
       SourceQuantity = 0;
       bIsSplitOperation = false;
       
       UE_LOG(LogTemp, Verbose, TEXT("ItemDragDropOperation::UItemDragDropOperation - Created drag operation"));
   }
   ```

3. **Compile Code**
   - Save all files (`Ctrl + Shift + S`)
   - Build Solution (`Build` → `Build Solution` or `F7`)
   - Wait for compilation to complete
   - Check for errors in Output window
   - Fix any errors before proceeding

---

### Step 2: Update InventorySlotWidget for Drag Detection

#### 2.1 Update InventorySlotWidget Header

1. **Open InventorySlotWidget.h**
   - Locate file: `Source/ActionRPG/Public/UI/Inventory/InventorySlotWidget.h`

2. **Add Includes and Forward Declarations**
   - Add at top after existing includes:
     ```cpp
     #include "Components/Image.h"
     #include "Blueprint/DragDropOperation.h"
     ```
   - Add forward declaration:
     ```cpp
     class UItemDragDropOperation;
     ```

3. **Add Drag and Drop Methods**
   - In the `protected` or `public` section, add:
     ```cpp
     // Drag and Drop
     virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
     virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
     virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
     virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
     virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
     
     /**
      * Set the parent InventoryWidget reference (needed for drag and drop).
      * @param InventoryWidget The parent InventoryWidget
      */
     UFUNCTION(BlueprintCallable, Category = "Inventory Slot")
     void SetParentInventoryWidget(UInventoryWidget* InventoryWidget);
     ```

4. **Add Parent Widget Reference Property**
   - In the `private` section, add:
     ```cpp
     // Reference to parent InventoryWidget (set when slot is created)
     UPROPERTY()
     TObjectPtr<class UInventoryWidget> ParentInventoryWidget;
     ```

4. **Add Visual Feedback Properties** (optional, for hover/drag feedback)
   - In `private` section:
     ```cpp
     // Visual feedback during drag
     FLinearColor DefaultBorderColor;
     FLinearColor HoverBorderColor;
     FLinearColor ValidDropColor;
     FLinearColor InvalidDropColor;
     
     bool bIsDragOver;
     ```

5. **Save File**

#### 2.2 Update InventorySlotWidget Implementation

1. **Open InventorySlotWidget.cpp**
   - Locate file: `Source/ActionRPG/Private/UI/Inventory/InventorySlotWidget.cpp`

2. **Add Includes**
   - Add at top:
     ```cpp
     #include "UI/Inventory/ItemDragDropOperation.h"
     #include "Blueprint/WidgetBlueprintLibrary.h"
     #include "Slate/SlateBrushAsset.h"
     #include "Engine/Texture2D.h"
     ```

3. **Update NativeConstruct**
   - Add initialization of visual feedback colors:
     ```cpp
     void UInventorySlotWidget::NativeConstruct()
     {
         Super::NativeConstruct();

         // Initialize to empty state
         SlotIndex = -1;
         CurrentItem = nullptr;
         CurrentQuantity = 0;
         ParentInventoryWidget = nullptr;
         
         // Initialize visual feedback colors
         DefaultBorderColor = FLinearColor(0.1f, 0.1f, 0.1f, 1.0f);  // Dark gray
         HoverBorderColor = FLinearColor(0.3f, 0.3f, 0.4f, 1.0f);     // Light gray-blue
         ValidDropColor = FLinearColor(0.2f, 0.6f, 0.2f, 1.0f);       // Green
         InvalidDropColor = FLinearColor(0.6f, 0.2f, 0.2f, 1.0f);     // Red
         
         bIsDragOver = false;
         
         UpdateSlotVisuals();
     }
     ```

4. **Implement NativeOnDragDetected**
   - **Important:** In UE 5.7, `NativeOnDragDetected` returns `void`, not `FReply`.
   - Add implementation for `NativeOnDragDetected`:
     ```cpp
     void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
     {
         // Only allow drag if slot has an item
         if (SlotIndex == -1 || !CurrentItem || CurrentQuantity <= 0)
         {
             UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidget::NativeOnDragDetected - Cannot drag empty slot or invalid item"));
             return;
         }

         // Create drag drop operation
         UItemDragDropOperation* DragOperation = NewObject<UItemDragDropOperation>(this, UItemDragDropOperation::StaticClass());
         if (!DragOperation)
         {
             UE_LOG(LogTemp, Error, TEXT("InventorySlotWidget::NativeOnDragDetected - Failed to create drag operation"));
             return;
         }

         // Set drag operation data
         DragOperation->SourceSlotIndex = SlotIndex;
         DragOperation->Item = CurrentItem;
         DragOperation->Quantity = CurrentQuantity;  // Default: drag entire stack
         DragOperation->SourceQuantity = CurrentQuantity;
         DragOperation->bIsSplitOperation = false;  // Left-click drag = full stack

         // Note: Drag preview is handled automatically by UE 5.7
         // If you want a custom preview, you can create a widget and set it as DefaultDragVisual
         // For now, using default preview

         OutOperation = DragOperation;

         UE_LOG(LogTemp, Log, TEXT("InventorySlotWidget::NativeOnDragDetected - Started drag from slot %d (Item: %s, Quantity: %d)"),
             SlotIndex,
             CurrentItem && CurrentItem->ItemData ? *CurrentItem->ItemData->ItemName.ToString() : TEXT("Unknown"),
             CurrentQuantity);
     }
     ```

5. **Update NativeOnMouseButtonDown**
   - Ensure it handles left-click for drag initiation:
     ```cpp
     FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
     {
         // Only handle clicks if slot has an item or is in a valid state
         if (SlotIndex != -1)
         {
             if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
             {
                 // Left click - start drag detection for drag and drop
                 // Only allow drag if slot has an item
                 if (CurrentItem && CurrentQuantity > 0)
                 {
                     // Get the Slate widget for drag detection
                     TSharedPtr<SWidget> SlateWidget = GetCachedWidget();
                     if (SlateWidget.IsValid())
                     {
                         return FReply::Handled().DetectDrag(SlateWidget.ToSharedRef(), EKeys::LeftMouseButton);
                     }
                 }
                 // Fall back to just handling the click
                 OnSlotClicked.Broadcast(SlotIndex);
                 return FReply::Handled();
             }
             else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
             {
                 // Right click - use item (only if slot has item)
                 if (CurrentItem && CurrentQuantity > 0)
                 {
                     OnSlotRightClicked.Broadcast(SlotIndex);
                     UE_LOG(LogTemp, Log, TEXT("InventorySlotWidget::NativeOnMouseButtonDown - Right clicked slot %d (use item)"), SlotIndex);
                     return FReply::Handled();
                 }
             }
         }

         return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
     }
     ```
   - **Important:** Use `GetCachedWidget().ToSharedRef()` for `DetectDrag`, not `this`. The `DetectDrag` method requires a Slate widget reference.

6. **Implement NativeOnDragEnter**
   ```cpp
     void UInventorySlotWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
     {
         Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

         UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
         if (!ItemDragOp)
         {
             return;
         }

         bIsDragOver = true;

         // Visual feedback: highlight slot based on whether drop is valid
         if (SlotBorder)
         {
             bool bCanDrop = CanDropItem(ItemDragOp);
             FLinearColor BorderColor = bCanDrop ? ValidDropColor : InvalidDropColor;
             SlotBorder->SetBrushColor(BorderColor);
         }

         UE_LOG(LogTemp, Verbose, TEXT("InventorySlotWidget::NativeOnDragEnter - Drag entered slot %d"), SlotIndex);
     }
     ```

7. **Implement NativeOnDragLeave**
   ```cpp
     void UInventorySlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
     {
         Super::NativeOnDragLeave(InDragDropEvent, InOperation);

         bIsDragOver = false;

         // Reset border color
         if (SlotBorder)
         {
             SlotBorder->SetBrushColor(DefaultBorderColor);
         }

         UE_LOG(LogTemp, Verbose, TEXT("InventorySlotWidget::NativeOnDragLeave - Drag left slot %d"), SlotIndex);
     }
     ```

8. **Add Helper Method CanDropItem** (in private section of .cpp)
   ```cpp
     bool UInventorySlotWidget::CanDropItem(UItemDragDropOperation* DragOperation) const
     {
         if (!DragOperation || !DragOperation->Item)
         {
             return false;
         }

         // Can always drop on empty slot
         if (SlotIndex == -1 || !CurrentItem || CurrentQuantity <= 0)
         {
             return true;
         }

         // Can drop if same item and can stack
         if (CurrentItem->ItemData && DragOperation->Item->ItemData)
         {
             if (CurrentItem->ItemData->ItemID == DragOperation->Item->ItemData->ItemID)
             {
                 // Check if there's room in stack
                 int32 MaxStack = CurrentItem->ItemData->MaxStackSize;
                 return (CurrentQuantity + DragOperation->Quantity) <= MaxStack;
             }
         }

         // Can drop if different item (will swap)
         return true;
     }
     ```

9. **Add CanDropItem Declaration to Header**
   - In `private` section of `.h`:
     ```cpp
     bool CanDropItem(UItemDragDropOperation* DragOperation) const;
     ```

10. **Implement SetParentInventoryWidget**
   - In `InventorySlotWidget.cpp`, add:
     ```cpp
     void UInventorySlotWidget::SetParentInventoryWidget(UInventoryWidget* InventoryWidget)
     {
         ParentInventoryWidget = InventoryWidget;
     }
     ```

11. **Update InventoryWidget to Set Parent Reference**
   - **Important:** When creating slot widgets in `InventoryWidget::InitializeSlots()`, you must call `SetParentInventoryWidget(this)` for each slot widget.
   - In `InventoryWidget.cpp`, in the `InitializeSlots()` method, after creating each slot widget, add:
     ```cpp
     // Set parent inventory widget reference (for drag and drop)
     SlotWidget->SetParentInventoryWidget(this);
     ```
   - This must be called BEFORE binding events or adding to the grid.

12. **Add Forward Declaration**
   - In `InventorySlotWidget.h`, add forward declaration:
     ```cpp
     class UInventoryWidget;
     ```

13. **Compile Code**
    - Save all files
    - Build Solution
    - Wait for compilation
    - Check for errors
    - Fix any errors before proceeding

---

## Day 25: Drop Logic and Special Cases

### Step 3: Implement Drop Logic

#### 3.1 Implement NativeOnDrop

1. **Open InventorySlotWidget.cpp**
   - Add implementation for `NativeOnDrop`:

   ```cpp
   bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
   {
       Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

       UItemDragDropOperation* ItemDragOp = Cast<UItemDragDropOperation>(InOperation);
       if (!ItemDragOp)
       {
           UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidget::NativeOnDrop - Invalid drag operation"));
           return false;
       }

       // Get parent inventory widget (stored when slot was created)
       UInventoryWidget* InventoryWidget = ParentInventoryWidget;
       if (!InventoryWidget)
       {
           UE_LOG(LogTemp, Error, TEXT("InventorySlotWidget::NativeOnDrop - ParentInventoryWidget is not set! Make sure SetParentInventoryWidget is called when creating slots."));
           return false;
       }

       int32 SourceSlot = ItemDragOp->SourceSlotIndex;
       int32 TargetSlot = SlotIndex;

       // Validate slots
       if (SourceSlot == -1 || TargetSlot == -1)
       {
           UE_LOG(LogTemp, Warning, TEXT("InventorySlotWidget::NativeOnDrop - Invalid slot indices (Source: %d, Target: %d)"), SourceSlot, TargetSlot);
           return false;
       }

       // Handle drop on same slot (do nothing)
       if (SourceSlot == TargetSlot)
       {
           UE_LOG(LogTemp, Log, TEXT("InventorySlotWidget::NativeOnDrop - Drop on same slot, ignoring"));
           return false;
       }

       // Request parent widget to handle the drop
       // The parent widget has access to InventoryComponent
       InventoryWidget->HandleItemDrop(SourceSlot, TargetSlot, ItemDragOp->Quantity);

       // Reset visual feedback
       bIsDragOver = false;
       if (SlotBorder)
       {
           SlotBorder->SetBrushColor(DefaultBorderColor);
       }

       return true;
   }
   ```

#### 3.2 Update InventoryWidget to Handle Drops

1. **Open InventoryWidget.h**
   - Add method declaration:
     ```cpp
     /**
      * Handle item drop between slots.
      * @param SourceSlotIndex The slot where the item is being dragged from
      * @param TargetSlotIndex The slot where the item is being dropped
      * @param Quantity The quantity being moved (-1 for entire stack)
      */
     UFUNCTION(BlueprintCallable, Category = "Inventory UI")
     void HandleItemDrop(int32 SourceSlotIndex, int32 TargetSlotIndex, int32 Quantity = -1);
     ```

2. **Open InventoryWidget.cpp**
   - Implement `HandleItemDrop`:
     ```cpp
     void UInventoryWidget::HandleItemDrop(int32 SourceSlotIndex, int32 TargetSlotIndex, int32 Quantity)
     {
         if (!InventoryComponent)
         {
             UE_LOG(LogTemp, Error, TEXT("InventoryWidget::HandleItemDrop - InventoryComponent is null"));
             return;
         }

         // Get source and target slots
         const TArray<FInventorySlot>& InventorySlots = InventoryComponent->GetInventorySlots();
         
         if (!InventorySlots.IsValidIndex(SourceSlotIndex))
         {
             UE_LOG(LogTemp, Error, TEXT("InventoryWidget::HandleItemDrop - Invalid source slot index: %d"), SourceSlotIndex);
             return;
         }
         
         if (!InventorySlots.IsValidIndex(TargetSlotIndex))
         {
             UE_LOG(LogTemp, Error, TEXT("InventoryWidget::HandleItemDrop - Invalid target slot index: %d"), TargetSlotIndex);
             return;
         }

         const FInventorySlot& SourceSlot = InventorySlots[SourceSlotIndex];
         const FInventorySlot& TargetSlot = InventorySlots[TargetSlotIndex];

         // Validate source slot has item
         if (SourceSlot.bIsEmpty || !SourceSlot.Item || SourceSlot.Quantity <= 0)
         {
             UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleItemDrop - Source slot %d is empty"), SourceSlotIndex);
             return;
         }

         // Determine quantity to move (default: all items in source slot)
         int32 MoveQuantity = (Quantity == -1) ? SourceSlot.Quantity : FMath::Clamp(Quantity, 1, SourceSlot.Quantity);

         // Handle different drop scenarios
         if (TargetSlot.bIsEmpty)
         {
             // Drop on empty slot - move item
             UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleItemDrop - Moving item from slot %d to empty slot %d (Quantity: %d)"), 
                 SourceSlotIndex, TargetSlotIndex, MoveQuantity);
             
             bool bMoved = InventoryComponent->MoveItem(SourceSlotIndex, TargetSlotIndex);
             if (bMoved)
             {
                 UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleItemDrop - Item moved successfully"));
             }
             else
             {
                 UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleItemDrop - Failed to move item"));
             }
         }
         else if (TargetSlot.Item && TargetSlot.Item->ItemData)
         {
             // Drop on occupied slot
             if (SourceSlot.Item && SourceSlot.Item->ItemData)
             {
                 FName SourceItemID = SourceSlot.Item->ItemData->ItemID;
                 FName TargetItemID = TargetSlot.Item->ItemData->ItemID;

                 if (SourceItemID == TargetItemID)
                 {
                     // Same item - try to stack
                     UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleItemDrop - Attempting to stack items (Source: %d, Target: %d, Quantity: %d)"), 
                         SourceSlotIndex, TargetSlotIndex, MoveQuantity);
                     
                     // Use InventoryComponent's stacking logic via AddItem
                     // First, try to add quantity to target slot
                     // If that fails, swap items
                     // Note: This requires careful handling of partial stacks
                     // For now, use MoveItem which should handle stacking
                     
                     bool bMoved = InventoryComponent->MoveItem(SourceSlotIndex, TargetSlotIndex);
                     if (!bMoved)
                     {
                         // Stacking failed, try swap
                         UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleItemDrop - Stacking failed, attempting swap"));
                         InventoryComponent->SwapItems(SourceSlotIndex, TargetSlotIndex);
                     }
                 }
                 else
                 {
                     // Different items - swap
                     UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleItemDrop - Swapping different items (Source: %d, Target: %d)"), 
                         SourceSlotIndex, TargetSlotIndex);
                     
                     bool bSwapped = InventoryComponent->SwapItems(SourceSlotIndex, TargetSlotIndex);
                     if (bSwapped)
                     {
                         UE_LOG(LogTemp, Log, TEXT("InventoryWidget::HandleItemDrop - Items swapped successfully"));
                     }
                     else
                     {
                         UE_LOG(LogTemp, Warning, TEXT("InventoryWidget::HandleItemDrop - Failed to swap items"));
                     }
                 }
             }
         }

         // Refresh display after drop
         UpdateInventoryDisplay();
     }
     ```

3. **Compile Code**
   - Save all files
   - Build Solution
   - Check for errors
   - Fix any errors

---

### Step 4: Handle Partial Stack Splits (Optional - Deferred to Days 26-27)

**Note:** This step was completed in Days 26-27 with Ctrl+drag implementation. Right-click is reserved for item usage. Full split functionality with quantity dialog can be implemented in Phase 3 if needed.

#### 4.1 Update Drag Detection for Split Mode (Days 26-27 Implementation)

1. **Add Split Mode Property to ItemDragDropOperation.h**
   - Added to `UItemDragDropOperation` class:
     ```cpp
     // Is this a partial stack split (Ctrl+drag)?
     UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
     bool bIsSplitOperation;
     ```

2. **Update InventorySlotWidget::NativeOnDragDetected**
   - Implemented to detect Ctrl+drag for split mode:
     ```cpp
     FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
     {
         if (SlotIndex != -1)
         {
             if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
             {
                 // Left click - drag entire stack
                 // Use GetCachedWidget() for proper Slate widget reference
                 TSharedPtr<SWidget> SlateWidget = GetCachedWidget();
                 if (SlateWidget.IsValid())
                 {
                     return FReply::Handled().DetectDrag(SlateWidget.ToSharedRef(), EKeys::LeftMouseButton);
                 }
                 return FReply::Handled();
             }
             else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
             {
                 // Right click - use item (not used for drag splitting)
                 // Stack splitting uses Ctrl+drag (implemented in Days 26-27)
                 OnSlotRightClicked.Broadcast(SlotIndex);
                 return FReply::Handled();
             }
         }

         return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
     }
     ```

   **Note:** Stack splitting with Ctrl+drag was implemented in Days 26-27. Right-click is reserved for item usage. Full split functionality with quantity dialog can be added in Phase 3.

---

### Step 5: Create Drag Preview Widget (Optional Enhancement)

#### 5.1 Create WBP_ItemDragPreview Blueprint

1. **Create Widget Blueprint**
   - In Content Browser, navigate to `Content/UI/Inventory/`
   - Right-click → `User Interface` → `Widget Blueprint`
   - Name: `WBP_ItemDragPreview`
   - Double-click to open

2. **Design Preview Widget**
   - Root: `Canvas Panel` or `Border`
   - Add `Image` widget for item icon
   - Add `Text` widget for quantity
   - Set size: 64x64 (same as slot size)
   - Set semi-transparent background

3. **Drag Preview in UE 5.7 (Automatic)**
   - **Important:** UE 5.7 automatically handles drag preview creation. No manual code is needed.
   - The drag preview is automatically created by UMG when `NativeOnDragDetected` sets `OutOperation`
   - If you want a custom preview widget, you would need to:
     1. Create a UMG widget (e.g., using `WBP_ItemDragPreview` created above)
     2. Convert it to a Slate widget using `TakeWidget()` or `GetCachedWidget()`
     3. Set it as `DragOperation->DefaultDragVisual = Widget->TakeWidget()`
     4. Do this in `NativeOnDragDetected` in `InventorySlotWidget.cpp`, NOT in the constructor
   - **For Phase 2:** The default automatic preview is sufficient. Custom preview can be added in Phase 3 if desired.
   - **Note:** `UDragDropOperation` does NOT have an `OnDragStarted` method. The preview must be set in `NativeOnDragDetected` when creating the operation.

---

### Step 6: Test Drag and Drop

#### 6.1 Basic Drag and Drop Test

1. **Open Test Level**
   - Open your test level in Unreal Editor
   - Ensure player character is in level
   - Ensure items are available in inventory

2. **Start Play In Editor (PIE)**
   - Press `Alt + P` or click `Play`
   - Game should start

3. **Open Inventory**
   - Press the inventory key (default: `I`)
   - Inventory should open

4. **Test Drag from Slot**
   - Left-click and hold on a slot with an item
   - Drag the mouse cursor
   - Item should follow cursor (drag preview appears)
   - Check Output Log for "Started drag" messages

5. **Test Drop on Empty Slot**
   - Drag item from slot with item to empty slot
   - Release mouse button
   - Item should move to new slot
   - Original slot should become empty
   - Check Output Log for "Item moved successfully"

6. **Test Drop on Same Item (Stacking)**
   - Have two slots with the same item (e.g., 2 health potions)
   - Drag one stack onto the other
   - Stacks should merge if within MaxStackSize
   - Or items should swap if stack is full
   - Check Output Log for stacking messages

7. **Test Drop on Different Item (Swap)**
   - Have two slots with different items
   - Drag one item onto the other
   - Items should swap positions
   - Check Output Log for "Items swapped successfully"

8. **Test Drop on Same Slot**
   - Drag item from slot to same slot
   - Nothing should happen (drop rejected)
   - Check Output Log for "Drop on same slot, ignoring"

9. **Test Visual Feedback**
   - Start dragging an item
   - Hover over valid drop target
   - Slot border should turn green
   - Hover over invalid drop target (e.g., full stack)
   - Slot border should turn red
   - Leave slot
   - Border color should reset

10. **Test Drag Cancel**
    - Start dragging an item
    - Press `Escape` or drag outside inventory
    - Drag should cancel
    - Item should remain in original slot
    - Check Output Log for "Drag cancelled" messages

#### 6.2 Edge Case Testing

1. **Test Empty Slot Drag**
   - Try to drag from empty slot
   - Should not start drag (no preview)
   - Check Output Log for warning

2. **Test Full Stack Drop**
   - Try to drop on full stack (at MaxStackSize)
   - Should swap items or reject drop
   - Check behavior is correct

3. **Test Weight Limit**
   - Fill inventory near weight limit
   - Try to drag heavy item to new slot
   - Should still work (weight is per-slot, not per-move)

4. **Test Rapid Drags**
   - Quickly drag multiple items
   - Should handle correctly without errors

---

## Troubleshooting

### Issue: Drag Doesn't Start

**Symptoms:** Clicking and dragging on item doesn't start drag operation

**Solutions:**
1. **Check NativeOnMouseButtonDown**
   - Verify it returns `FReply::Handled().DetectDrag(GetCachedWidget().ToSharedRef(), EKeys::LeftMouseButton)`
   - **Important:** Use `GetCachedWidget().ToSharedRef()`, not `this` (DetectDrag requires Slate widget reference)
   - Check slot has valid item (`CurrentItem != nullptr`)
   - Check `GetCachedWidget()` returns valid widget before calling `ToSharedRef()`

2. **Check NativeOnDragDetected**
   - Verify it's overridden and returns `void` (not `FReply` in UE 5.7)
   - Check drag operation is created correctly
   - Verify `OutOperation` is set
   - UE 5.7 handles drag start automatically when `OutOperation` is set

3. **Check Widget Focus**
   - Ensure inventory widget has input enabled
   - Check `IsFocusable` is true on slot widget

4. **Check Output Log**
   - Look for "Cannot drag empty slot" warnings
   - Check for errors in drag detection

### Issue: Drop Doesn't Work

**Symptoms:** Dropping item on slot doesn't move/swap items

**Solutions:**
1. **Check NativeOnDrop**
   - Verify it returns `true` when drop succeeds
   - Check `HandleItemDrop` is called correctly

2. **Check HandleItemDrop**
   - Verify InventoryComponent methods are called (`MoveItem`, `SwapItems`)
   - Check slots are valid indices
   - Verify source slot has item

3. **Check InventoryComponent Methods**
   - Test `MoveItem` directly in code/Blueprint
   - Test `SwapItems` directly
   - Verify these methods work independently

4. **Check Output Log**
   - Look for "Invalid slot indices" errors
   - Check for "InventoryComponent is null" errors
   - Verify move/swap success messages

### Issue: Visual Feedback Not Working

**Symptoms:** Slot borders don't change color during drag

**Solutions:**
1. **Check NativeOnDragEnter/Leave**
   - Verify they're being called
   - Check `SlotBorder` is not null
   - Verify `SetBrushColor` is called

2. **Check Colors**
   - Verify color values are set in `NativeConstruct`
   - Check colors are visible (not transparent/black)

3. **Check Border Widget**
   - In Blueprint, verify `SlotBorder` is properly bound
   - Check border has a visible brush

### Issue: Stacking Not Working

**Symptoms:** Dropping same item on another doesn't stack

**Solutions:**
1. **Check Item IDs Match**
   - Verify `SourceItemID == TargetItemID`
   - Check ItemDataAsset has correct ItemID

2. **Check MaxStackSize**
   - Verify MaxStackSize allows stacking
   - Check current quantity + moved quantity <= MaxStackSize

3. **Check MoveItem Implementation**
   - Verify MoveItem handles stacking correctly
   - Check InventoryComponent stacking logic

4. **Check HandleItemDrop Logic**
   - Verify it detects same item correctly
   - Check it calls MoveItem for stacking (not SwapItems)

### Issue: Items Disappear After Drag

**Symptoms:** Item is removed from source but not added to target

**Solutions:**
1. **Check MoveItem/SwapItems**
   - Verify these methods work correctly
   - Check they don't remove item without adding

2. **Check Event Bindings**
   - Verify `OnInventoryChanged` events fire
   - Check `RefreshSlot` is called for both slots

3. **Check UpdateInventoryDisplay**
   - Verify it's called after drop
   - Check it refreshes all affected slots

---

## Verification Checklist

Before moving to Days 26-27, verify:

- [ ] ItemDragDropOperation class compiles without errors
- [ ] Drag detection works (click and hold starts drag)
- [ ] Drag preview appears and follows cursor
- [ ] Drop on empty slot moves item correctly
- [ ] Drop on same item stacks correctly (if within MaxStackSize)
- [ ] Drop on different item swaps correctly
- [ ] Drop on same slot is rejected
- [ ] Visual feedback works (green/red borders on hover)
- [ ] Drag cancel works (Escape key)
- [ ] All slots update correctly after drop
- [ ] Weight/capacity displays update after drop
- [ ] No errors in Output Log during drag/drop
- [ ] Edge cases handled correctly
- [ ] Ready for UI polish (Days 26-27)

---

## Next Steps

Once Days 24-25 are complete:

1. **Days 26-27** will focus on:
   - Item tooltip system
   - Item details panel
   - Quick-use slots
   - Filter/sort functionality
   - UI visual polish and animations

2. **Before Days 26-27**, you should have:
   - Working drag and drop system
   - Visual feedback during drag operations
   - All special cases handled
   - Ready to add advanced UI features

---

## Notes

- **Drag Preview:** Default UMG drag preview is sufficient for Phase 2. Custom preview widget can be added in Phase 3.
- **Split Stack:** Full split stack functionality with quantity dialog can be deferred to Phase 3. Basic Ctrl+drag split (half stack) is implemented in Days 26-27.
- **Performance:** Drag operations are lightweight. No performance concerns unless dragging hundreds of items.
- **Visual Polish:** Basic visual feedback (border colors) is sufficient for Phase 2. More advanced effects can be added in Days 26-27.

---

## Quick Reference

### Key Files
- **C++ Header:** `Source/ActionRPG/Public/UI/Inventory/ItemDragDropOperation.h`
- **C++ Implementation:** `Source/ActionRPG/Private/UI/Inventory/ItemDragDropOperation.cpp`
- **Slot Widget Header:** `Source/ActionRPG/Public/UI/Inventory/InventorySlotWidget.h`
- **Slot Widget Implementation:** `Source/ActionRPG/Private/UI/Inventory/InventorySlotWidget.cpp`
- **Inventory Widget Header:** `Source/ActionRPG/Public/UI/Inventory/InventoryWidget.h`
- **Inventory Widget Implementation:** `Source/ActionRPG/Private/UI/Inventory/InventoryWidget.cpp`

### Key Methods
- `NativeOnDragDetected()` - Starts drag operation
- `NativeOnDrop()` - Handles drop on slot
- `NativeOnDragEnter()` - Visual feedback on hover
- `NativeOnDragLeave()` - Reset visual feedback
- `HandleItemDrop()` - Parent widget handles actual item move/swap
- `CanDropItem()` - Validates if drop is allowed

### Key Properties
- `SourceSlotIndex` - Slot where drag started
- `Item` - Item being dragged
- `Quantity` - Quantity being moved
- `bIsDragOver` - Visual feedback state

---

**Status:** Complete these manual steps before proceeding to Days 26-27

---

**End of Phase 2 Manual Steps - Days 24-25**