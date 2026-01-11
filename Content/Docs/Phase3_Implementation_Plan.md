# Phase 3: Skill System - Implementation Plan
**Duration:** Week 5-6 (14 days)  
**Based on:** Architecture & Design Plan v1.3  
**Last Updated:** 2025-01-07

---

## Table of Contents
1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Week 1 Breakdown](#week-1-breakdown)
4. [Week 2 Breakdown](#week-2-breakdown)
5. [Detailed Implementation Steps](#detailed-implementation-steps)
6. [Testing Checklist](#testing-checklist)
7. [Success Criteria](#success-criteria)
8. [Common Issues & Solutions](#common-issues--solutions)

---

## Overview

### Phase 3 Goals
- Implement SkillComponent for managing individual skill instances
- Implement SkillManagerComponent for skill unlocking, progression, and skill bar management
- Create SkillBase class and skill implementations (MeleeAttack, RangedAttack, Dash, Magic)
- Create SkillDataAsset for data-driven skill definitions
- Build skill bar UI system (SkillBarWidget, SkillSlotWidget)
- Implement skill activation with cooldowns and resource costs
- Create skill-granting consumables (SkillItem, SkillStone, BeastCore)
- Implement skill experience and leveling system
- Integrate skill system with inventory (skill-granting items)
- Test complete skill flow (acquire → unlock → assign → activate → level up)

### Phase 3 Deliverables
- ✅ SkillComponent class (C++ and Blueprint-ready)
- ✅ SkillManagerComponent class (C++ and Blueprint-ready)
- ✅ SkillBase class and skill implementations
- ✅ SkillDataAsset class
- ✅ SkillBarWidget and SkillSlotWidget (UMG widgets)
- ✅ Skill activation system with cooldowns
- ✅ Skill hotkey support (1-8 keys for skills)
- ✅ Skill experience and leveling system
- ✅ SkillItem, SkillStone, and BeastCore consumable classes
- ✅ Skill-granting item integration with inventory
- ✅ Sample skill Data Assets (MeleeAttack, RangedAttack, Dash, Fireball)
- ✅ Complete skill UI with visual feedback
- ✅ Skill cooldown visual indicators
- ✅ SkillDatabase singleton for skill lookup

### Success Metrics
- SkillComponent compiles and attaches to player character
- Skills can be unlocked and assigned to skill bar slots
- Skills can be activated via hotkeys (1-8)
- Skill cooldowns function correctly
- Skill experience gains through use
- Skill levels increase with experience
- Skill-granting items (SkillItem, SkillStone, BeastCore) work correctly
- Skill bar UI displays correctly and responds to input
- Skill icons and cooldown overlays display properly
- Multiple skill types (Melee, Ranged, Dash, Magic) function correctly
- Skill requirements (mana/stamina costs) are enforced
- Skill effects execute properly (damage, movement, etc.)

---

## Prerequisites

### Required Knowledge
- Unreal Engine 5.7 component system
- UMG Widget system
- Enhanced Input System
- Actor spawning and collision detection
- Animation system basics (for melee/ranged skills)
- Particle system basics (for magic skills)

### Required Tools
- Unreal Engine 5.7 installed
- Visual Studio 2022 (or compatible IDE)
- Phase 1 and Phase 2 complete (all deliverables verified)

### Pre-Phase Setup
- ✅ Phase 1 complete (ItemBase, ItemDataAsset, ItemDatabase)
- ✅ Phase 2 complete (InventoryComponent, ItemPickupActor, Inventory UI)
- ✅ PlayerCharacter class ready for component attachment
- ✅ Input system working (IA_Attack, IA_Dodge, IA_SkillSlot1-8)
- ✅ Project compiles and runs in editor

### Phase 1 & 2 Foundations Available
- ✅ `UItemBase` - Base item class with Use() and CanUse()
- ✅ `UItemDataAsset` - Item data definitions
- ✅ `UItemDatabase` - Singleton for item lookup
- ✅ `UInventoryComponent` - Inventory management
- ✅ `AActionRPGPlayerCharacter` - Character ready for components
- ✅ `AActionRPGPlayerController` - Input handling ready
- ✅ Enhanced Input System with skill slot actions (IA_SkillSlot1-8)
- ✅ Inventory UI system for skill-granting items

---

## Week 1 Breakdown

### Day 29-30: Skill System Foundation

#### Tasks
1. **Create Folder Structure**
   - Create `Source/ActionRPG/Public/Skills/Core/`
   - Create `Source/ActionRPG/Private/Skills/Core/`
   - Create `Source/ActionRPG/Public/Skills/Implementations/`
   - Create `Source/ActionRPG/Private/Skills/Implementations/`
   - Create `Content/Blueprints/Skills/`
   - Create `Content/Data/Skills/`

2. **Create SkillDataAsset Header**
   - Inherit from `UPrimaryDataAsset`
   - Add skill properties (SkillID, SkillName, SkillDescription, SkillIcon)
   - Add skill type enum (ESkillType: Melee, Ranged, Magic, Utility)
   - Add skill category enum (ESkillCategory: Combat, Support, Movement, Passive)
   - Add cooldown, mana cost, stamina cost properties
   - Add cast time, range, required level
   - Add damage properties (BaseDamage, DamageType enum: Physical, Fire, Cold, Lightning, Poison, Magic, Dark, Holy)
   - Add area of effect radius
   - Add projectile properties (ProjectileSpeed, ProjectileClass)
   - Add movement properties (AbilityMoveSpeed, InvincibilityFrames)

3. **Create SkillBase Header**
   - Inherit from `UObject` (or appropriate base class)
   - Add SkillDataAsset reference
   - Add cooldown tracking
   - Add virtual methods: `Activate()`, `CanActivate()`, `OnActivate()`, `OnCooldownComplete()`
   - Add skill level and experience properties
   - Add owner actor reference

4. **Implement SkillBase Core Methods**
   - `Activate(AActor* Target)` - Virtual method for skill execution
   - `CanActivate(AActor* Target)` - Validation check (cooldown, resources, requirements)
   - `OnActivate()` - Event fired on activation
   - `OnCooldownComplete()` - Event fired when cooldown ends
   - `UpdateCooldown(float DeltaTime)` - Update cooldown timer
   - `GetCooldownRemaining()` - Get remaining cooldown time
   - `GetManaCost()` - Get current mana cost (may vary by level)
   - `GetStaminaCost()` - Get current stamina cost (may vary by level)

5. **Create SkillDatabase Singleton**
   - Create `Source/ActionRPG/Public/Data/SkillDatabase.h`
   - Implement singleton pattern (similar to ItemDatabase)
   - Add skill lookup by SkillID
   - Add skill registration and validation
   - Load skills at game start

#### Deliverables
- SkillDataAsset class created
- SkillBase class created with core methods
- SkillDatabase singleton created
- Folder structure established
- Basic skill system foundation ready

---

### Day 31-32: SkillComponent Implementation

#### Tasks
1. **Create SkillComponent Header**
   - Inherit from `UActorComponent`
   - Add FSkillInstance structure (Skill, CooldownRemaining, Level, Experience, bIsUnlocked)
   - Add TArray<FSkillInstance> ActiveSkills
   - Add events/delegates for skill activation and cooldown changes

2. **Implement SkillComponent Core Methods**
   - `ActivateSkill(USkillBase* Skill, AActor* Target)` - Execute skill
   - `CanActivateSkill(USkillBase* Skill)` - Check cooldown/requirements
   - `GrantSkill(USkillBase* Skill)` - Add skill to character
   - `RemoveSkill(USkillBase* Skill)` - Remove skill
   - `UpdateCooldowns(float DeltaTime)` - Tick cooldown timers (in TickComponent)
   - `GetSkillLevel(USkillBase* Skill)` - Get skill level
   - `GetSkillExperience(USkillBase* Skill)` - Get skill experience
   - `AddSkillExperience(USkillBase* Skill, float Amount)` - Add experience to skill
   - `IsSkillUnlocked(USkillBase* Skill)` - Check if skill is unlocked
   - `GetSkillInstance(USkillBase* Skill)` - Get skill instance data

3. **Add Skill Experience System**
   - Calculate experience gain based on skill use
   - Apply experience rate modifiers (attributes, class affinity, difficulty)
   - Handle skill level ups
   - Update skill effectiveness based on level

4. **Add Resource Cost Validation**
   - Check mana/stamina availability before activation
   - Deduct resources on successful activation
   - Handle insufficient resources gracefully

5. **Attach to Player Character**
   - Add component in PlayerCharacter constructor or Blueprint
   - Test component attachment
   - Verify TickComponent is enabled for cooldown updates

#### Deliverables
- SkillComponent class created
- Core methods implemented
- Skill experience system functional
- Resource cost validation working
- Component attached to PlayerCharacter
- Blueprint class created

---

### Day 33-34: SkillManagerComponent Implementation

#### Tasks
1. **Create SkillManagerComponent Header**
   - Inherit from `UActorComponent`
   - Add skill unlock tracking (TArray<USkillBase*>)
   - Add skill bar management (TMap<int32, USkillBase*> for slot assignments)
   - Add events/delegates for skill unlock and slot assignment

2. **Implement SkillManagerComponent Core Methods**
   - `UnlockSkill(USkillBase* Skill)` - Unlock skill for use
   - `AssignSkillToSlot(int32 SlotIndex, USkillBase* Skill)` - Assign to hotkey
   - `GetSkillAtSlot(int32 SlotIndex)` - Get assigned skill
   - `ActivateSkillFromSlot(int32 SlotIndex)` - Execute skill from hotkey
   - `RemoveSkillFromSlot(int32 SlotIndex)` - Remove skill from slot
   - `ClearSlot(int32 SlotIndex)` - Clear slot assignment
   - `IsSkillUnlocked(USkillBase* Skill)` - Check unlock status
   - `GetUnlockedSkills()` - Get all unlocked skills

3. **Add Skill Bar Management**
   - Support 8 skill slots (slots 1-8)
   - Validate slot indices (0-7)
   - Handle slot swapping (assigning to occupied slot)
   - Persist skill bar assignments (save/load support)

4. **Integrate with SkillComponent**
   - SkillManagerComponent references SkillComponent
   - On skill unlock, grant skill to SkillComponent
   - On skill activation, call SkillComponent::ActivateSkill()
   - Handle skill cooldown queries from UI

5. **Add Skill Prerequisites Validation**
   - Check attribute requirements
   - Check level requirements
   - Check prerequisite skill requirements
   - Validate class requirements (if applicable)

6. **Attach to Player Character**
   - Add component in PlayerCharacter constructor or Blueprint
   - Test component attachment
   - Verify integration with SkillComponent

#### Deliverables
- SkillManagerComponent class created
- Core methods implemented
- Skill bar management functional
- Integration with SkillComponent working
- Component attached to PlayerCharacter
- Blueprint class created

---

### Day 35: Skill Implementations - MeleeAttack & RangedAttack

#### Tasks
1. **Create SkillMeleeAttack Header**
   - Inherit from `USkillBase`
   - Add melee-specific properties (damage, range, hit detection type)
   - Override `Activate()` method

2. **Implement SkillMeleeAttack**
   - `Activate(AActor* Target)` - Execute melee attack
   - Spawn hit detection (sphere trace or collision)
   - Apply damage to hit targets
   - Play attack animation (via character)
   - Apply knockback/effects if applicable
   - Grant skill experience on successful hit

3. **Create SkillRangedAttack Header**
   - Inherit from `USkillBase`
   - Add ranged-specific properties (projectile class, range, accuracy)
   - Override `Activate()` method

4. **Implement SkillRangedAttack**
   - `Activate(AActor* Target)` - Execute ranged attack
   - Spawn projectile actor
   - Set projectile direction and velocity
   - Apply damage on projectile hit
   - Grant skill experience on successful hit

5. **Create Sample Skill Data Assets**
   - Create `DA_Skill_MeleeAttack` Data Asset
   - Create `DA_Skill_RangedAttack` Data Asset
   - Configure skill properties (cooldown, cost, damage, range)
   - Set skill icons and descriptions

6. **Test Skill Implementations**
   - Test melee attack activation
   - Test ranged attack activation
   - Verify damage application
   - Verify cooldown functionality
   - Verify resource cost deduction

#### Deliverables
- SkillMeleeAttack class created and implemented
- SkillRangedAttack class created and implemented
- Sample skill Data Assets created
- Skills tested and functional

---

## Week 2 Breakdown

### Day 36-37: Skill Implementations - Dash & Magic

#### Tasks
1. **Create SkillDash Header**
   - Inherit from `USkillBase`
   - Add dash-specific properties (distance, speed, invincibility frames)
   - Override `Activate()` method

2. **Implement SkillDash**
   - `Activate(AActor* Target)` - Execute dash movement
   - Calculate dash direction (from input or target direction)
   - Apply movement impulse to character
   - Enable invincibility frames (optional)
   - Play dash animation/effects
   - Grant skill experience on use

3. **Create SkillMagic Header**
   - Inherit from `USkillBase`
   - Add magic-specific properties (spell effect, area of effect, particle system)
   - Override `Activate()` method

4. **Implement SkillMagic (Fireball Example)**
   - `Activate(AActor* Target)` - Execute magic spell
   - Spawn projectile with particle effects
   - Apply area of effect damage on impact
   - Play spell casting animation
   - Grant skill experience on successful cast

5. **Create Sample Skill Data Assets**
   - Create `DA_Skill_Dash` Data Asset
   - Create `DA_Skill_Fireball` Data Asset
   - Configure skill properties
   - Set skill icons and descriptions

6. **Test Skill Implementations**
   - Test dash activation and movement
   - Test fireball casting and damage
   - Verify particle effects
   - Verify cooldown and resource costs

#### Deliverables
- SkillDash class created and implemented
- SkillMagic (Fireball) class created and implemented
- Sample skill Data Assets created
- Skills tested and functional

---

### Day 38-39: Skill-Granting Items (SkillItem, SkillStone, BeastCore)

#### Tasks
1. **Create SkillItem Header**
   - Inherit from `ConsumableItem` (which inherits from `ItemBase`)
   - Add SkillDataAsset reference
   - Override `Use()` method

2. **Implement SkillItem**
   - `Use()` - Grant skill to player
   - Check if skill is already unlocked (optional validation)
   - Call SkillManagerComponent::UnlockSkill()
   - Consume item (remove from inventory)
   - Provide visual/audio feedback
   - Handle skill already unlocked scenario

3. **Create SkillStoneItem Header**
   - Inherit from `ConsumableItem`
   - Add SkillDataAsset reference
   - Override `Use()` method
   - Works regardless of class/attribute requirements

4. **Implement SkillStoneItem**
   - `Use()` - Grant skill to player (no requirements check)
   - Call SkillManagerComponent::UnlockSkill()
   - Consume item
   - Provide feedback

5. **Create BeastCoreItem Header**
   - Inherit from `ConsumableItem`
   - Add SkillDataAsset reference
   - Add attribute requirements (FPrimaryAttributes)
   - Add class affinity requirement (optional)
   - Override `Use()` method

6. **Implement BeastCoreItem**
   - `Use()` - Conditionally grant skill based on requirements
   - Check attribute requirements (via AttributeComponent)
   - Check class affinity (if required)
   - If requirements met: Unlock skill
   - If requirements not met: Optionally keep item or consume with failure message
   - Provide success/failure feedback

7. **Create Sample Item Data Assets**
   - Create `DA_Item_Skill_Fireball` (SkillItem)
   - Create `DA_Item_SkillStone_Fireball` (SkillStoneItem)
   - Create `DA_Item_BeastCore_Wolf` (BeastCoreItem)
   - Configure item properties and skill references

8. **Integrate with Inventory System**
   - Test skill-granting items in inventory
   - Verify item usage grants skills
   - Verify items are consumed on use
   - Test BeastCore requirement validation

#### Deliverables
- SkillItem class created and implemented
- SkillStoneItem class created and implemented
- BeastCoreItem class created and implemented
- Sample item Data Assets created
- Integration with inventory tested

---

### Day 40-41: Skill Bar UI (SkillBarWidget, SkillSlotWidget)

#### Tasks
1. **Create SkillBarWidget Header**
   - Inherit from `UUserWidget`
   - Add 8 skill slot widgets (TArray<USkillSlotWidget*>)
   - Add reference to SkillManagerComponent
   - Add methods for updating skill slots

2. **Implement SkillBarWidget**
   - `NativeConstruct()` - Initialize widget and bind to SkillManagerComponent
   - `UpdateSkillSlot(int32 SlotIndex, USkillBase* Skill)` - Update slot display
   - `RefreshAllSlots()` - Refresh all skill slots
   - `OnSkillUnlocked(USkillBase* Skill)` - Handle skill unlock event
   - `OnSkillSlotChanged(int32 SlotIndex, USkillBase* Skill)` - Handle slot assignment

3. **Create SkillSlotWidget Header**
   - Inherit from `UUserWidget`
   - Add skill icon (UImage)
   - Add cooldown overlay (UProgressBar or UImage)
   - Add key binding label (UTextBlock)
   - Add click handler for activation
   - Add reference to parent SkillBarWidget

4. **Implement SkillSlotWidget**
   - `SetSkillData(USkillBase* Skill, int32 SlotIndex)` - Set skill and slot data
   - `UpdateCooldown(float CooldownRemaining, float CooldownDuration)` - Update cooldown display
   - `UpdateKeyBinding(FText KeyText)` - Update key binding label
   - `OnSlotClicked()` - Activate skill from slot
   - `NativeTick()` - Update cooldown overlay each frame

5. **Add Visual Feedback**
   - Cooldown progress bar overlay
   - Skill icon display
   - Key binding text (1-8)
   - Empty slot indicator
   - Skill ready indicator (when cooldown complete)

6. **Create Blueprint Widgets**
   - Create `WBP_SkillBar` Blueprint
   - Create `WBP_SkillSlot` Blueprint
   - Set up widget layout (horizontal bar with 8 slots)
   - Configure visual styling

7. **Integrate with PlayerController**
   - Add SkillBarWidget to HUD
   - Create widget in BeginPlay
   - Add to viewport with appropriate Z-order
   - Test widget display

#### Deliverables
- SkillBarWidget class created
- SkillSlotWidget class created
- Blueprint widgets created and configured
- Visual feedback implemented
- Integration with HUD complete

---

### Day 42: Skill Hotkey Integration

#### Tasks
1. **Update PlayerController**
   - Verify IA_SkillSlot1-8 Input Actions exist
   - Add handlers for skill slot activation
   - `OnSkillSlot1Pressed()` through `OnSkillSlot8Pressed()`
   - Call SkillManagerComponent::ActivateSkillFromSlot()

2. **Add Input Action Bindings**
   - Bind IA_SkillSlot1-8 to corresponding handlers
   - Map keys 1-8 to skill slots
   - Test input handling

3. **Update SkillBarWidget**
   - Query PlayerController for bound keys
   - Display actual bound keys in skill slots (similar to quick-use bar)
   - Update key binding labels dynamically

4. **Test Hotkey Activation**
   - Test each hotkey (1-8) activates correct skill
   - Verify cooldown prevents activation
   - Verify resource costs are deducted
   - Verify skills execute correctly

5. **Add Visual Feedback for Activation**
   - Flash skill slot on activation
   - Show "on cooldown" visual state
   - Show "insufficient resources" feedback

#### Deliverables
- Hotkey integration complete
- Input actions bound correctly
- Dynamic key binding display working
- Skill activation via hotkeys functional

---

### Day 43: Integration Testing & Skill Experience System

#### Tasks
1. **Test Complete Skill Flow**
   - Acquire skill-granting item (SkillItem, SkillStone, BeastCore)
   - Use item to unlock skill
   - Assign skill to skill bar slot
   - Activate skill via hotkey
   - Verify skill executes correctly
   - Verify skill gains experience
   - Verify skill level increases

2. **Test Skill Experience System**
   - Use skills multiple times
   - Verify experience gains
   - Verify experience rate modifiers (attributes, class affinity)
   - Verify skill level ups
   - Verify skill effectiveness increases with level

3. **Test Skill Cooldowns**
   - Activate skill
   - Verify cooldown starts
   - Verify skill cannot be activated during cooldown
   - Verify cooldown completes correctly
   - Verify UI cooldown display updates

4. **Test Resource Costs**
   - Activate skill with sufficient resources
   - Verify resources are deducted
   - Try to activate skill with insufficient resources
   - Verify activation is prevented
   - Verify feedback is provided

5. **Test Multiple Skill Types**
   - Test MeleeAttack skill
   - Test RangedAttack skill
   - Test Dash skill
   - Test Magic skill (Fireball)
   - Verify all types function correctly

6. **Test Skill Bar Management**
   - Assign skills to slots
   - Swap skills between slots
   - Remove skills from slots
   - Verify slot assignments persist
   - Test with all 8 slots

7. **Test Edge Cases**
   - Unlock skill that's already unlocked
   - Assign skill to invalid slot index
   - Activate skill from empty slot
   - Activate skill during cooldown
   - Activate skill with insufficient resources
   - Use BeastCore with unmet requirements

8. **Performance Testing**
   - Test with multiple skills active
   - Test rapid skill activation
   - Check for frame drops during skill execution
   - Verify no memory leaks during extended play

#### Deliverables
- Complete skill flow tested
- Skill experience system verified
- All skill types tested
- Edge cases handled
- Performance validated

---

### Day 44: Polish, Bug Fixes, and Documentation

#### Tasks
1. **UI Polish**
   - Refine skill bar visual design
   - Improve cooldown overlay appearance
   - Add skill tooltips (skill name, description, cooldown, cost)
   - Add skill level display
   - Add skill experience progress bar (optional)

2. **Visual/Audio Feedback**
   - Add particle effects for skill activation
   - Add sound effects for skill activation
   - Add visual feedback for skill unlocks
   - Add feedback for skill level ups
   - Add feedback for insufficient resources

3. **Bug Fixes**
   - Fix any issues found during testing
   - Verify all edge cases are handled
   - Check for memory leaks
   - Verify event bindings are cleaned up properly

4. **Code Cleanup**
   - Review code for UE 5.7 best practices
   - Add missing comments
   - Remove debug logging (or make it conditional)
   - Optimize performance-critical code

5. **Documentation**
   - Update Architecture Design Plan if needed
   - Document any deviations from plan
   - Create manual steps document for Blueprint setup
   - Document skill creation process for designers

6. **Final Testing**
   - Run complete test suite
   - Verify all success criteria met
   - Test in packaged build (optional)

#### Deliverables
- Polished UI and visual feedback
- All bugs fixed
- Code cleaned and optimized
- Documentation updated
- Phase 3 complete

---

## Detailed Implementation Steps

### SkillComponent Implementation Details

#### FSkillInstance Structure
```cpp
USTRUCT(BlueprintType)
struct FSkillInstance
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<USkillBase> Skill;

    UPROPERTY(BlueprintReadOnly)
    float CooldownRemaining;

    UPROPERTY(BlueprintReadOnly)
    int32 Level;

    UPROPERTY(BlueprintReadOnly)
    float Experience;

    UPROPERTY(BlueprintReadOnly)
    bool bIsUnlocked;

    FSkillInstance()
        : Skill(nullptr)
        , CooldownRemaining(0.0f)
        , Level(1)
        , Experience(0.0f)
        , bIsUnlocked(false)
    {}
};
```

#### Key Methods Implementation Notes
- `ActivateSkill()`: Check cooldown, validate resources, call skill's Activate(), start cooldown, deduct resources
- `UpdateCooldowns()`: Called in TickComponent, decrement cooldown timers, fire events when cooldowns complete
- `AddSkillExperience()`: Calculate experience gain with modifiers, add to skill, check for level up, update skill effectiveness

### SkillManagerComponent Implementation Details

#### Skill Bar Slot Management
- Use TMap<int32, USkillBase*> for slot assignments (0-7 for slots 1-8)
- Validate slot indices before assignment
- Handle slot swapping (if slot occupied, swap or clear)
- Persist assignments (consider save/load system)

#### Skill Unlock Flow
1. Check prerequisites (attributes, level, skills)
2. Validate skill can be unlocked
3. Add to unlocked skills list
4. Grant skill to SkillComponent
5. Fire unlock event
6. Update UI

### Skill Implementations Details

#### MeleeAttack Implementation
- Use sphere trace or collision detection for hit detection
- Apply damage to hit targets
- Play attack animation (via character's animation system)
- Grant experience on successful hit
- Consider hit detection timing (animation-driven)

#### RangedAttack Implementation
- Spawn projectile actor (create ProjectileActor class if needed)
- Set projectile properties (speed, damage, range)
- Handle projectile hit events
- Grant experience on hit

#### Dash Implementation
- Calculate dash direction from input or target
- Apply movement impulse via CharacterMovementComponent
- Enable invincibility frames (set collision response temporarily)
- Play dash animation/effects
- Grant experience on use

#### Magic (Fireball) Implementation
- Spawn projectile with particle effects
- Handle area of effect on impact
- Apply damage to targets in AoE
- Play casting animation
- Grant experience on successful cast

### Skill-Granting Items Implementation Details

#### SkillItem
- Check if skill already unlocked (optional - may allow re-learning)
- Call SkillManagerComponent::UnlockSkill()
- Consume item (remove from inventory via InventoryComponent)
- Provide feedback (UI message, sound, particles)

#### SkillStoneItem
- Similar to SkillItem but no requirement checks
- Always grants skill if used
- Useful for guaranteed skill acquisition

#### BeastCoreItem
- Check AttributeComponent for attribute requirements
- Check ClassComponent for class affinity (if required)
- If requirements met: Unlock skill, consume item, success feedback
- If requirements not met: Optionally keep item or consume with failure message, failure feedback

### Skill Bar UI Implementation Details

#### SkillBarWidget
- Create 8 SkillSlotWidget instances in NativeConstruct
- Bind to SkillManagerComponent events
- Update slots when skills are assigned/unassigned
- Query PlayerController for bound keys
- Update key binding labels dynamically

#### SkillSlotWidget
- Display skill icon (or empty slot indicator)
- Show cooldown overlay (progress bar or image with mask)
- Display key binding text (1-8)
- Update cooldown in NativeTick
- Handle click events for activation

#### Cooldown Visual Feedback
- Use UProgressBar or UImage with material mask for cooldown
- Update fill percentage based on CooldownRemaining / CooldownDuration
- Show different visual state when on cooldown vs ready
- Consider color coding (red = on cooldown, green = ready)

---

## Testing Checklist

### Component Testing
- [ ] SkillComponent attaches to player character
- [ ] Skills can be granted to SkillComponent
- [ ] Skills can be activated via SkillComponent
- [ ] Cooldowns update correctly
- [ ] Resource costs are deducted correctly
- [ ] Skill experience gains correctly
- [ ] Skill levels increase correctly
- [ ] SkillManagerComponent attaches to player character
- [ ] Skills can be unlocked via SkillManagerComponent
- [ ] Skills can be assigned to skill bar slots
- [ ] Skills can be activated from slots

### Skill Implementation Testing
- [ ] MeleeAttack skill activates and deals damage
- [ ] RangedAttack skill spawns projectile and deals damage
- [ ] Dash skill moves character correctly
- [ ] Magic skill (Fireball) casts and deals damage
- [ ] All skills respect cooldowns
- [ ] All skills respect resource costs
- [ ] All skills grant experience on use

### Skill-Granting Items Testing
- [ ] SkillItem grants skill and consumes item
- [ ] SkillStoneItem grants skill and consumes item
- [ ] BeastCoreItem grants skill when requirements met
- [ ] BeastCoreItem fails when requirements not met
- [ ] Items are removed from inventory after use
- [ ] Visual/audio feedback plays on skill grant

### UI Testing
- [ ] SkillBarWidget displays correctly
- [ ] SkillSlotWidget displays skill icons
- [ ] Cooldown overlays update correctly
- [ ] Key binding labels display correctly
- [ ] Skill slots update when skills assigned/unassigned
- [ ] Clicking skill slot activates skill
- [ ] Tooltips display skill information

### Hotkey Testing
- [ ] Keys 1-8 activate corresponding skill slots
- [ ] Hotkeys respect cooldowns
- [ ] Hotkeys respect resource costs
- [ ] Hotkeys work when inventory is open/closed
- [ ] Hotkeys work during combat

### Integration Testing
- [ ] Complete flow: Acquire item → Use item → Unlock skill → Assign to slot → Activate skill
- [ ] Skill experience system works end-to-end
- [ ] Skill level ups increase effectiveness
- [ ] Multiple skills can be active simultaneously
- [ ] Skill bar persists across inventory open/close

### Edge Case Testing
- [ ] Unlocking already unlocked skill
- [ ] Assigning skill to invalid slot
- [ ] Activating skill from empty slot
- [ ] Activating skill during cooldown
- [ ] Activating skill with insufficient resources
- [ ] Using BeastCore with unmet requirements
- [ ] Rapid skill activation
- [ ] Skill activation during other actions

---

## Success Criteria

### Functional Requirements
- ✅ All skill components compile and attach to player character
- ✅ Skills can be unlocked via skill-granting items
- ✅ Skills can be assigned to skill bar slots (1-8)
- ✅ Skills can be activated via hotkeys (1-8)
- ✅ Skill cooldowns function correctly
- ✅ Skill resource costs (mana/stamina) are enforced
- ✅ Skill experience gains through use
- ✅ Skill levels increase with experience
- ✅ Skill effectiveness scales with level
- ✅ Multiple skill types (Melee, Ranged, Dash, Magic) function correctly

### UI Requirements
- ✅ Skill bar displays with 8 slots
- ✅ Skill icons display correctly
- ✅ Cooldown overlays update in real-time
- ✅ Key binding labels display actual bound keys
- ✅ Skill tooltips show skill information
- ✅ Visual feedback for skill activation
- ✅ Visual feedback for skill unlocks and level ups

### Integration Requirements
- ✅ Skill-granting items integrate with inventory system
- ✅ Skills integrate with character resource system (mana/stamina)
- ✅ Skill bar integrates with HUD
- ✅ Hotkeys integrate with Enhanced Input System
- ✅ Skill system ready for Phase 4 (attribute integration)

### Performance Requirements
- ✅ No frame drops during skill activation
- ✅ Cooldown updates don't cause performance issues
- ✅ No memory leaks during extended play
- ✅ Skill system performs well with multiple active skills

---

## Common Issues & Solutions

### Issue: Skills Not Activating
**Symptoms:** Pressing hotkey doesn't activate skill
**Solutions:**
- Verify Input Actions (IA_SkillSlot1-8) are bound in PlayerController
- Check SkillManagerComponent has skill assigned to slot
- Verify skill cooldown is complete
- Check resource costs (mana/stamina) are sufficient
- Verify SkillComponent is attached to character

### Issue: Cooldowns Not Updating
**Symptoms:** Cooldown overlay doesn't update or skills stay on cooldown forever
**Solutions:**
- Verify SkillComponent::TickComponent is enabled
- Check UpdateCooldowns() is called in TickComponent
- Verify cooldown timer is decrementing correctly
- Check UI update is called when cooldown changes

### Issue: Skills Not Gaining Experience
**Symptoms:** Skills don't level up after use
**Solutions:**
- Verify AddSkillExperience() is called on skill use
- Check experience calculation with modifiers
- Verify skill level up logic is working
- Check experience values in Data Asset

### Issue: Skill-Granting Items Not Working
**Symptoms:** Using item doesn't unlock skill
**Solutions:**
- Verify SkillDataAsset reference is set in item Data Asset
- Check SkillManagerComponent is accessible from item
- Verify UnlockSkill() is being called
- Check if skill prerequisites are met (for BeastCore)

### Issue: UI Not Updating
**Symptoms:** Skill bar doesn't reflect skill assignments or cooldowns
**Solutions:**
- Verify event bindings in SkillBarWidget
- Check UpdateSkillSlot() is called when skills change
- Verify NativeTick is enabled for cooldown updates
- Check widget is added to viewport correctly

### Issue: Resource Costs Not Deducted
**Symptoms:** Skills activate without deducting mana/stamina
**Solutions:**
- Verify resource system (mana/stamina) is implemented
- Check resource deduction in SkillComponent::ActivateSkill()
- Verify resource validation before activation
- Check resource component is accessible

### Issue: Skills Not Persisting
**Symptoms:** Skills disappear after game restart
**Solutions:**
- Implement save/load system for skill unlocks (Phase 4+)
- Verify skill assignments are stored correctly
- Check skill instances are preserved

---

## Notes & Deviations

### Design Decisions
- **Skill Slots:** Using 8 slots (1-8) for skills, separate from quick-use slots (9-10 for consumables)
- **Skill Experience:** Experience gains through use, with modifiers from attributes and class affinity
- **Skill Leveling:** Skills level independently, affecting effectiveness and efficiency
- **Skill-Granting Items:** Three types (SkillItem, SkillStone, BeastCore) for different acquisition methods

### Implementation Notes
- SkillComponent handles individual skill instances and cooldowns
- SkillManagerComponent handles skill unlocking and skill bar management
- Skills are data-driven via SkillDataAsset
- Skill experience system prepares for Phase 4 attribute integration
- Skill bar UI uses dynamic key binding display (similar to quick-use bar)

### Future Enhancements (Post-Phase 3)
- Skill trees and prerequisites
- Skill upgrades and modifications
- Skill combos and synergies
- Save/load system for skill unlocks
- Skill mastery bonuses
- Skill customization

---

**End of Phase 3 Implementation Plan**
