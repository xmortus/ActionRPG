# Phase 2 Execution Plan - Days 17-18: Item Pickup Actor
**Date:** 2025-01-07  
**Phase:** Phase 2 - Inventory System  
**Days:** 17-18 - Item Pickup Actor Implementation

---

## Overview

Days 17-18 focus on creating the ItemPickupActor class that allows items to be spawned in the world and collected by the player. This actor will integrate with the InventoryComponent created in Days 15-16.

---

## Prerequisites

### Required Completion
- ✅ Days 15-16 complete (InventoryComponent functional)
- ✅ InventoryComponent attached to PlayerCharacter
- ✅ ItemDatabase working and initialized
- ✅ ItemBase and ItemDataAsset classes functional
- ✅ Test Item Data Assets created (HealthPotion, etc.)

### Required Knowledge
- Unreal Engine Actor class system
- Component creation and attachment
- Overlap events and collision detection
- Blueprint actor creation and configuration
- Integration with existing components

---

## Implementation Breakdown

### Day 17: C++ Implementation

#### Task 1: Create Folder Structure

**Location:** `Source/ActionRPG/`

**Folders to Create:**
```
Source/ActionRPG/
├── Public/
│   └── Items/
│       └── Pickups/
│           └── ItemPickupActor.h
└── Private/
    └── Items/
        └── Pickups/
            └── ItemPickupActor.cpp
```

**Steps:**
1. Create `Public/Items/Pickups/` folder
2. Create `Private/Items/Pickups/` folder
3. Verify folder structure matches above

---

#### Task 2: Create ItemPickupActor Header

**File:** `Source/ActionRPG/Public/Items/Pickups/ItemPickupActor.h`

**Key Components:**
- Inherit from `AActor`
- Add `ItemDataAsset` reference property
- Add `Quantity` property
- Add `StaticMeshComponent` for visual representation
- Add `USphereComponent` for collision/overlap detection
- Add pickup logic methods
- Add visual feedback methods

**Header Structure:**
```cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/Core/ItemDataAsset.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "ItemPickupActor.generated.h"

class AActionRPGPlayerCharacter;

/**
 * Actor that represents an item pickup in the world.
 * When player overlaps, item is added to their inventory.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API AItemPickupActor : public AActor
{
    GENERATED_BODY()

public:
    AItemPickupActor();

protected:
    virtual void BeginPlay() override;

    // Item Data
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    TObjectPtr<UItemDataAsset> ItemData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    int32 Quantity = 1;

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USphereComponent> CollisionComponent;

    // Pickup Logic
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                        bool bFromSweep, const FHitResult& SweepResult);

    void PickupItem(AActionRPGPlayerCharacter* Player);
    bool CanPickup(AActionRPGPlayerCharacter* Player) const;
    void SpawnPickupEffect();
    void DestroyPickup();

    // Visual Effects
    void SetupVisuals();
    void UpdateMaterialBasedOnRarity();
};
```

**Key Points:**
- Use `USphereComponent` for overlap detection (simpler than box collision)
- `ItemData` property allows setting item in Blueprint
- `Quantity` defaults to 1 but can be overridden
- `OnOverlapBegin` handles player overlap events
- `CanPickup` validates pickup conditions (inventory space, etc.)

---

#### Task 3: Implement ItemPickupActor

**File:** `Source/ActionRPG/Private/Items/Pickups/ItemPickupActor.cpp`

**Key Implementation Details:**

**Constructor:**
- Create `RootComponent` (SceneComponent)
- Create `CollisionComponent` (USphereComponent)
- Create `MeshComponent` (UStaticMeshComponent)
- Set collision settings (OverlapAllDynamic)
- Set sphere radius (default: 50.0f)
- Attach components in hierarchy

**BeginPlay:**
- Bind overlap event: `OnComponentBeginOverlap`
- Setup visual representation
- Update material based on item rarity (if ItemData is set)

**OnOverlapBegin:**
- Cast `OtherActor` to `AActionRPGPlayerCharacter`
- Validate player is valid
- Call `CanPickup()` to check conditions
- If valid, call `PickupItem()`

**CanPickup:**
- Check if player has InventoryComponent
- Check if inventory has space for item
- Check if item can be added (weight/capacity)
- Return true if all conditions met

**PickupItem:**
- Get InventoryComponent from player
- Create ItemBase instance from ItemDataAsset using ItemDatabase
- Call `InventoryComponent::AddItem()`
- If successful:
  - Call `SpawnPickupEffect()`
  - Call `DestroyPickup()`
- If failed:
  - Log warning
  - Optionally show UI message (inventory full)

**SpawnPickupEffect:**
- Spawn particle effect (optional)
- Play sound effect (optional)
- Log pickup to console

**DestroyPickup:**
- Call `Destroy()` on actor
- Cleanup any effects

**Implementation Notes:**
- Include necessary headers:
  - `Characters/ActionRPGPlayerCharacter.h`
  - `Components/Inventory/InventoryComponent.h`
  - `Data/ItemDatabase.h`
  - `Items/Core/ItemBase.h`
- Use `UE_LOG` for debugging
- Handle NULL checks for all pointers
- Use `ItemDatabase::Get()->CreateItem()` to create item instance

---

#### Task 4: Add Visual Representation

**Visual Setup:**
- Use `UStaticMeshComponent` for item mesh
- Set default mesh (can be overridden in Blueprint)
- Create material instance based on item rarity
- Add rotation animation (optional, in Blueprint or C++)

**Material Setup:**
- Create material parameter for rarity color
- Set color based on `EItemRarity` enum:
  - Common: Gray
  - Uncommon: Green
  - Rare: Blue
  - Epic: Purple
  - Legendary: Gold
- Apply glow effect for higher rarities

**Rotation Animation:**
- Can be done in Blueprint with Timeline
- Or in C++ with Tick() function
- Rotate around Z-axis (Yaw)

---

### Day 18: Blueprint Setup & Integration

#### Task 1: Create Blueprint Class

**Steps:**
1. Navigate to `Content/Blueprints/Items/` (create if needed)
2. Create Blueprint from `ItemPickupActor` C++ class
3. Name it: `BP_ItemPickup`
4. Configure components and properties

---

#### Task 2: Configure Blueprint Components

**MeshComponent Setup:**
- Assign a static mesh (cube, sphere, or item-specific mesh)
- Set scale appropriate for item size
- Position relative to root

**CollisionComponent Setup:**
- Set sphere radius (default: 50.0f, adjust as needed)
- Set collision response:
  - Collision Enabled: Query Only
  - Object Type: WorldDynamic
  - Response to Pawn: Overlap
- Verify overlap detection is enabled

**Visual Setup:**
- Assign material instance
- Set up rotation animation (Timeline node)
- Add particle effect (optional)

---

#### Task 3: Create Item-Specific Pickup Blueprints

**Create Variants:**
- `BP_ItemPickup_HealthPotion` - Pre-configured with HealthPotion ItemData
- `BP_ItemPickup_Generic` - Generic pickup (set ItemData in instance)

**Configuration:**
- Set `ItemData` property to specific Item Data Asset
- Set `Quantity` if needed
- Customize mesh and materials per item type
- Set appropriate scale and visual effects

---

#### Task 4: Integrate with Inventory Component

**Verification:**
- Ensure PlayerCharacter has InventoryComponent attached
- Test pickup in-game
- Verify item is added to inventory
- Check inventory UI updates (if created)
- Test full inventory scenario

**Testing Scenarios:**
1. **Normal Pickup:**
   - Player overlaps pickup
   - Item added to inventory
   - Pickup destroyed
   - Inventory updates

2. **Full Inventory:**
   - Fill inventory to capacity
   - Attempt pickup
   - Pickup should remain (not destroyed)
   - Show message or log warning

3. **Stacking:**
   - Pickup item that already exists in inventory
   - Item should stack if possible
   - Quantity should increase

4. **Weight Limit:**
   - Fill inventory to weight limit
   - Attempt pickup of heavy item
   - Should fail if over weight limit

---

#### Task 5: Add to Test Level

**Steps:**
1. Open test level (e.g., `Lvl_TopDown`)
2. Place `BP_ItemPickup` actors in level
3. Configure each instance:
   - Set `ItemData` property
   - Set `Quantity` if needed
   - Position in world
4. Test pickup in Play mode
5. Verify items appear in inventory

---

## Code Structure Reference

### Required Includes

**ItemPickupActor.h:**
```cpp
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/Core/ItemDataAsset.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "ItemPickupActor.generated.h"
```

**ItemPickupActor.cpp:**
```cpp
#include "Items/Pickups/ItemPickupActor.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Data/ItemDatabase.h"
#include "Items/Core/ItemBase.h"
#include "Engine/Engine.h"
```

### Component Hierarchy

```
ItemPickupActor (Root)
├── SceneComponent (RootComponent)
├── SphereComponent (CollisionComponent)
│   └── Attached to RootComponent
└── StaticMeshComponent (MeshComponent)
    └── Attached to CollisionComponent (or RootComponent)
```

---

## Testing Checklist

### C++ Code Testing
- [ ] ItemPickupActor class compiles without errors
- [ ] No linter warnings
- [ ] Components created correctly in constructor
- [ ] Overlap event binds correctly
- [ ] CanPickup validation works
- [ ] PickupItem creates item correctly
- [ ] ItemDatabase integration works
- [ ] InventoryComponent integration works

### Blueprint Testing
- [ ] Blueprint class can be created
- [ ] Components visible in Blueprint editor
- [ ] Properties editable in Details panel
- [ ] Blueprint compiles without errors

### In-Game Testing
- [ ] Pickup spawns in world correctly
- [ ] Visual representation displays
- [ ] Collision sphere detects player overlap
- [ ] Item added to inventory on pickup
- [ ] Pickup destroyed after successful pickup
- [ ] Full inventory handled correctly
- [ ] Stacking works correctly
- [ ] Weight limit enforced
- [ ] Multiple pickups work independently

### Integration Testing
- [ ] Pickup → Inventory flow works
- [ ] Inventory UI updates (if created)
- [ ] Console logs appear correctly
- [ ] No crashes or errors
- [ ] Performance acceptable

---

## Common Issues & Solutions

### Issue: Pickup Not Detecting Overlap

**Symptoms:** Player walks through pickup, nothing happens

**Solutions:**
1. Check `CollisionComponent` settings:
   - Collision Enabled: Query Only
   - Object Type: WorldDynamic
   - Response to Pawn: Overlap
2. Verify `OnComponentBeginOverlap` is bound in `BeginPlay()`
3. Check `CollisionComponent` is registered
4. Verify player has correct collision settings
5. Check sphere radius is appropriate size

### Issue: Item Not Added to Inventory

**Symptoms:** Overlap detected but item not added

**Solutions:**
1. Verify PlayerCharacter has InventoryComponent attached
2. Check `CanPickup()` validation logic
3. Verify `ItemDatabase::CreateItem()` succeeds
4. Check `InventoryComponent::AddItem()` returns true
5. Add debug logs to trace flow
6. Verify ItemData is set in Blueprint

### Issue: Pickup Not Destroying

**Symptoms:** Item added but pickup remains

**Solutions:**
1. Check `DestroyPickup()` is called after successful pickup
2. Verify `Destroy()` is called on actor
3. Check for any errors preventing destruction
4. Verify pickup is not set to "Don't Destroy on Overlap"

### Issue: Visual Representation Not Showing

**Symptoms:** Pickup invisible or wrong mesh

**Solutions:**
1. Verify `MeshComponent` has mesh assigned
2. Check mesh is not scaled to 0
3. Verify material is assigned
4. Check component visibility settings
5. Verify mesh is attached to correct parent

### Issue: Full Inventory Not Handled

**Symptoms:** Item added even when inventory is full

**Solutions:**
1. Check `CanPickup()` calls `HasSpaceFor()`
2. Verify `InventoryComponent::HasSpaceFor()` works correctly
3. Check weight/capacity limits are enforced
4. Add validation in `PickupItem()` before adding

---

## Success Criteria

### Must Have (Days 17-18 Complete)
- ✅ ItemPickupActor class created and compiles
- ✅ Pickup logic implemented correctly
- ✅ Visual representation working
- ✅ Blueprint class created and configured
- ✅ Integration with InventoryComponent tested
- ✅ Pickup → Inventory flow works
- ✅ Full inventory scenario handled

### Nice to Have (Can Defer)
- Visual effects (particles, glow)
- Rotation animation
- Sound effects
- Item-specific meshes
- Advanced visual feedback

---

## Next Steps (Day 19-20)

After completing Days 17-18:

1. **Day 19-20** will focus on:
   - Item Usage System
   - UseItem implementation in InventoryComponent
   - Item consumption logic
   - Input integration for item usage

2. **Before Day 19-20**, verify:
   - ✅ ItemPickupActor working
   - ✅ Items can be picked up
   - ✅ Items appear in inventory
   - ✅ Ready to implement usage system

---

## Code Quality Checklist

Before considering Days 17-18 complete:

- [ ] All code compiles without errors
- [ ] No linter warnings
- [ ] Proper includes and forward declarations
- [ ] NULL checks for all pointers
- [ ] Error handling for edge cases
- [ ] Debug logging added (can be removed later)
- [ ] Code comments added
- [ ] Follows UE 5.7 naming conventions
- [ ] Blueprint-ready (BlueprintType, Blueprintable)
- [ ] Proper use of UPROPERTY/UFUNCTION macros

---

**End of Phase 2 Execution Plan - Days 17-18**
