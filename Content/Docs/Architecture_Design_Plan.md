# Top-Down Action RPG - Architecture & Design Plan
**Engine Version:** Unreal Engine 5.7  
**Project:** ActionRPG  
**Document Version:** 1.1  
**Last Updated:** 2025-01-07

---

## Table of Contents
1. [Project Overview](#project-overview)
2. [Folder Structure](#folder-structure)
3. [Core Systems Architecture](#core-systems-architecture)
4. [Inventory System Design](#inventory-system-design)
5. [Skill System Design](#skill-system-design)
6. [Leveling & Progression System](#leveling--progression-system)
7. [Input System Integration](#input-system-integration)
8. [Data Assets & Configuration](#data-assets--configuration)
9. [Component Architecture](#component-architecture)
10. [Gameplay Framework Integration](#gameplay-framework-integration)
11. [Asset Organization](#asset-organization)
12. [Naming Conventions](#naming-conventions)
13. [Implementation Phases](#implementation-phases)

---

## Project Overview

### Game Type
Top-down action RPG with real-time combat, inventory management, and skill progression systems.

### Core Pillars
- **Combat:** Fast-paced, skill-based action combat
- **Progression:** Multi-layered progression system with Primary Attributes, Secondary Attributes, Skills, and Classes/Professions
- **Inventory:** Intuitive drag-and-drop inventory management
- **Interaction:** Seamless item pickup and usage
- **Character Development:** Strategic, consequence-driven growth with hard rules and meaningful choices

### Technical Requirements
- Unreal Engine 5.7
- Enhanced Input System (mandatory)
- C++ with Blueprint integration
- Modular, component-based architecture
- Data-driven design using Data Assets

---

## Folder Structure

### Source Code Structure
```
Source/ActionRPG/
├── ActionRPG.h/.cpp                    # Module entry point
├── Public/
│   ├── Core/
│   │   ├── ActionRPGPlayerController.h
│   │   ├── ActionRPGPlayerState.h
│   │   ├── ActionRPGGameMode.h
│   │   └── ActionRPGGameState.h
│   ├── Characters/
│   │   ├── ActionRPGCharacter.h         # Base character class
│   │   └── ActionRPGPlayerCharacter.h  # Player-specific character
│   ├── Components/
│   │   ├── Inventory/
│   │   │   ├── InventoryComponent.h
│   │   │   ├── ItemPickupComponent.h
│   │   │   └── EquipmentComponent.h
│   │   ├── Skills/
│   │   │   ├── SkillComponent.h
│   │   │   └── SkillManagerComponent.h
│   │   ├── Progression/
│   │   │   ├── AttributeComponent.h
│   │   │   ├── SecondaryAttributeComponent.h
│   │   │   ├── ClassComponent.h
│   │   │   ├── ExperienceComponent.h
│   │   │   └── TrialComponent.h
│   │   └── Interaction/
│   │       └── InteractionComponent.h
│   ├── Items/
│   │   ├── Core/
│   │   │   ├── ItemBase.h              # Base item class
│   │   │   ├── ItemDataAsset.h         # Item data structure
│   │   │   └── ItemTypes.h             # Enums and structs
│   │   ├── Consumables/
│   │   │   ├── ConsumableItem.h
│   │   │   ├── SkillItem.h             # Skill-granting consumable
│   │   │   ├── SkillStoneItem.h        # Stored skill knowledge
│   │   │   └── BeastCoreItem.h         # Conditional skill granting
│   │   ├── Equipment/
│   │   │   ├── EquipmentItem.h
│   │   │   └── WeaponItem.h
│   │   └── Pickups/
│   │       └── ItemPickupActor.h
│   ├── Skills/
│   │   ├── Core/
│   │   │   ├── SkillBase.h             # Base skill class
│   │   │   ├── SkillDataAsset.h        # Skill data structure
│   │   │   └── SkillTypes.h            # Enums and structs
│   │   └── Implementations/
│   │       ├── SkillMeleeAttack.h
│   │       ├── SkillRangedAttack.h
│   │       └── SkillDash.h
│   ├── Progression/
│   │   ├── Core/
│   │   │   ├── PrimaryAttributeDataAsset.h
│   │   │   ├── SecondaryAttributeDataAsset.h
│   │   │   ├── ClassDataAsset.h
│   │   │   ├── ProfessionDataAsset.h
│   │   │   └── ProgressionTypes.h      # Enums and structs
│   │   └── Trials/
│   │       ├── TrialBase.h
│   │       └── TrialDataAsset.h
│   ├── UI/
│   │   ├── Inventory/
│   │   │   ├── InventoryWidget.h
│   │   │   ├── InventorySlotWidget.h
│   │   │   └── ItemDragDropOperation.h
│   │   ├── Skills/
│   │   │   ├── SkillBarWidget.h
│   │   │   └── SkillSlotWidget.h
│   │   ├── Progression/
│   │   │   ├── AttributeWidget.h
│   │   │   ├── ClassWidget.h
│   │   │   ├── ExperienceWidget.h
│   │   │   └── TrialWidget.h
│   │   └── HUD/
│   │       └── ActionRPGHUD.h
│   └── Data/
│       ├── ItemDatabase.h              # Singleton item registry
│       ├── SkillDatabase.h             # Singleton skill registry
│       ├── ClassDatabase.h             # Singleton class/profession registry
│       └── AttributeDatabase.h         # Singleton attribute registry
└── Private/
    └── [Mirror structure of Public/]
```

### Content Structure
```
Content/
├── Docs/                                # Documentation (this folder)
├── Blueprints/
│   ├── Characters/
│   │   ├── BP_ActionRPGCharacter
│   │   └── BP_ActionRPGPlayerCharacter
│   ├── Items/
│   │   ├── Pickups/
│   │   │   └── BP_ItemPickup
│   │   └── Equipment/
│   ├── Components/
│   │   ├── Inventory/
│   │   ├── Skills/
│   │   └── Progression/
│   ├── Trials/
│   │   └── BP_Trial_Base
│   └── UI/
│       ├── Inventory/
│       │   ├── WBP_Inventory
│       │   ├── WBP_InventorySlot
│       │   └── WBP_ItemTooltip
│       └── Skills/
│           ├── WBP_SkillBar
│           └── WBP_SkillSlot
│       └── Progression/
│           ├── WBP_Attributes
│           ├── WBP_ClassSelection
│           └── WBP_Experience
├── Data/
│   ├── Items/
│   │   ├── DA_Item_HealthPotion
│   │   ├── DA_Item_Skill_Fireball
│   │   ├── DA_Item_SkillStone_Fireball
│   │   ├── DA_Item_BeastCore_Wolf
│   │   └── DA_Item_Weapon_Sword
│   ├── Skills/
│   │   ├── DA_Skill_MeleeAttack
│   │   ├── DA_Skill_RangedAttack
│   │   └── DA_Skill_Dash
│   ├── Classes/
│   │   ├── DA_Class_Warrior
│   │   ├── DA_Class_Mage
│   │   └── DA_Class_Rogue
│   ├── Professions/
│   │   ├── DA_Profession_Blacksmith
│   │   └── DA_Profession_Alchemist
│   └── Trials/
│       ├── DA_Trial_Combat_Test
│       └── DA_Trial_Survival_Challenge
├── Input/
│   ├── IA_ActionRPG                      # Input Action mappings
│   └── IM_ActionRPG                       # Input Mapping Context
├── Materials/
├── Meshes/
│   ├── Characters/
│   ├── Items/
│   └── Weapons/
├── Textures/
│   ├── UI/
│   │   ├── Inventory/
│   │   └── Skills/
│   └── Items/
└── Audio/
    ├── SFX/
    └── Music/
```

---

## Core Systems Architecture

### System Hierarchy
```
GameMode (ActionRPGGameMode)
├── GameState (ActionRPGGameState)
│   └── Manages global game state
├── PlayerController (ActionRPGPlayerController)
│   └── Handles input, UI, camera
└── PlayerCharacter (ActionRPGPlayerCharacter)
    ├── InventoryComponent
    ├── SkillComponent
    ├── SkillManagerComponent
    ├── EquipmentComponent
    ├── AttributeComponent
    ├── SecondaryAttributeComponent
    ├── ClassComponent
    ├── ExperienceComponent
    └── InteractionComponent
```

### Design Patterns
- **Component-Based Architecture:** All systems as components for modularity
- **Data-Driven Design:** Use Data Assets for all item/skill definitions
- **Observer Pattern:** Events/Delegates for inventory/skill changes
- **Factory Pattern:** Item creation from Data Assets
- **Singleton Pattern:** ItemDatabase and SkillDatabase for lookups

---

## Inventory System Design

### Core Components

#### InventoryComponent
**Purpose:** Manages player inventory state and operations

**Key Features:**
- Grid-based or list-based inventory storage
- Item stacking support
- Weight/space limitations (configurable)
- Hotkey quick-access slots
- Equipment slots (separate from inventory)

**Data Structure:**
```cpp
// Pseudo-structure
struct FInventorySlot
{
    TObjectPtr<UItemBase> Item;
    int32 Quantity;
    bool bIsEmpty;
};

class UInventoryComponent : public UActorComponent
{
    UPROPERTY()
    TArray<FInventorySlot> InventorySlots;
    
    UPROPERTY()
    int32 MaxInventorySize;
    
    UPROPERTY()
    int32 MaxStackSize;
    
    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryChanged, int32, SlotIndex, UItemBase*, Item);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, UItemBase*, Item);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, UItemBase*, Item);
};
```

**Core Methods:**
- `AddItem(UItemBase* Item, int32 Quantity)` - Add item to inventory
- `RemoveItem(int32 SlotIndex, int32 Quantity)` - Remove item from slot
- `MoveItem(int32 FromSlot, int32 ToSlot)` - Move item between slots
- `SwapItems(int32 SlotA, int32 SlotB)` - Swap two items
- `UseItem(int32 SlotIndex)` - Use item at slot
- `GetItemAt(int32 SlotIndex)` - Get item reference
- `FindItemSlot(UItemDataAsset* ItemData)` - Find slot containing item
- `HasSpaceFor(UItemBase* Item)` - Check if inventory has space

#### ItemPickupComponent
**Purpose:** Handles item pickup from world actors

**Key Features:**
- Detection of nearby item pickups
- Automatic or manual pickup
- Pickup range and interaction prompts
- Visual feedback (highlight, UI prompt)

**Core Methods:**
- `CheckForNearbyPickups()` - Scan for items in range
- `PickupItem(AItemPickupActor* Pickup)` - Execute pickup
- `CanPickupItem(AItemPickupActor* Pickup)` - Validation check

### Item System

#### ItemBase (Base Class)
**Purpose:** Base class for all items in the game

**Key Properties:**
- ItemDataAsset reference (data-driven)
- Quantity/Stack size
- Item state (equipped, in inventory, etc.)

**Core Methods:**
- `Use()` - Virtual method for item usage
- `CanUse()` - Check if item can be used
- `OnUse()` - Event fired when item is used

#### ItemDataAsset
**Purpose:** Data asset containing all item properties

**Key Properties:**
- ItemID (unique identifier)
- ItemName
- ItemDescription
- ItemIcon (texture reference)
- ItemType (enum: Consumable, Equipment, SkillItem, etc.)
- ItemRarity
- MaxStackSize
- Weight/Value
- Item-specific properties (damage, healing amount, etc.)

#### ItemPickupActor
**Purpose:** World actor representing a pickupable item

**Key Features:**
- Visual representation (mesh, particle effects)
- ItemDataAsset reference
- Quantity
- Auto-pickup or interaction required
- Respawn timer (optional)

**Core Methods:**
- `OnPlayerOverlap()` - Trigger pickup
- `SpawnPickupEffect()` - Visual feedback
- `DestroyPickup()` - Cleanup

### UI System

#### InventoryWidget
**Purpose:** Main inventory UI widget

**Key Features:**
- Grid of inventory slots
- Drag and drop support
- Item tooltips
- Filtering/sorting options
- Quick-access bar

**Components:**
- `UPanelWidget* InventoryGrid` - Container for slots
- `TArray<UInventorySlotWidget*> SlotWidgets` - Individual slot widgets

#### InventorySlotWidget
**Purpose:** Individual inventory slot UI element

**Key Features:**
- Item icon display
- Quantity text overlay
- Drag and drop source/destination
- Click to use
- Right-click context menu

**Events:**
- `OnSlotClicked` - Use item
- `OnSlotRightClicked` - Context menu
- `OnDragDetected` - Start drag operation
- `OnDrop` - Handle drop operation

#### ItemDragDropOperation
**Purpose:** Custom drag-drop operation for items

**Key Features:**
- Visual drag preview
- Source slot tracking
- Validation for drop targets
- Visual feedback (valid/invalid drop zones)

### Drag and Drop Flow
1. **Drag Start:** User clicks and drags from InventorySlotWidget
2. **Drag Operation:** ItemDragDropOperation created with item reference
3. **Drag Over:** Visual feedback on valid/invalid drop targets
4. **Drop:** Validate drop target, execute move/swap operation
5. **Update UI:** Refresh affected slot widgets

### Item Usage Flow
1. **Trigger:** Player clicks item slot or presses hotkey
2. **Validation:** Check if item can be used (CanUse())
3. **Execution:** Call item's Use() method
4. **Consumption:** Remove item from inventory if consumable
5. **Effect:** Apply item effect (healing, skill grant, etc.)
6. **UI Update:** Refresh inventory display

---

## Skill System Design

### Core Components

#### SkillComponent
**Purpose:** Manages individual skill instances on character

**Key Features:**
- Skill cooldown tracking
- Skill level/experience
- Skill activation state
- Skill effects execution

**Data Structure:**
```cpp
// Pseudo-structure
struct FSkillInstance
{
    TObjectPtr<USkillBase> Skill;
    float CooldownRemaining;
    int32 Level;
    float Experience;
    bool bIsUnlocked;
};

class USkillComponent : public UActorComponent
{
    UPROPERTY()
    TArray<FSkillInstance> ActiveSkills;
    
    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillActivated, USkillBase*, Skill, AActor*, Target);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillCooldownChanged, USkillBase*, Skill);
};
```

**Core Methods:**
- `ActivateSkill(USkillBase* Skill, AActor* Target)` - Execute skill
- `CanActivateSkill(USkillBase* Skill)` - Check cooldown/requirements
- `GrantSkill(USkillBase* Skill)` - Add skill to character
- `RemoveSkill(USkillBase* Skill)` - Remove skill
- `UpdateCooldowns(float DeltaTime)` - Tick cooldown timers
- `GetSkillLevel(USkillBase* Skill)` - Get skill level

#### SkillManagerComponent
**Purpose:** Manages skill unlocking, progression, and skill bar

**Key Features:**
- Skill unlock tracking
- Skill bar management (hotkey slots)
- Skill progression system
- Skill tree/collection management

**Core Methods:**
- `UnlockSkill(USkillBase* Skill)` - Unlock skill for use
- `AssignSkillToSlot(int32 SlotIndex, USkillBase* Skill)` - Assign to hotkey
- `GetSkillAtSlot(int32 SlotIndex)` - Get assigned skill
- `ActivateSkillFromSlot(int32 SlotIndex)` - Execute skill from hotkey

### Skill System

#### SkillBase (Base Class)
**Purpose:** Base class for all skills

**Key Properties:**
- SkillDataAsset reference
- Cooldown duration
- Mana/Energy cost
- Cast time
- Range/Duration

**Core Methods:**
- `Activate(AActor* Target)` - Virtual method for skill execution
- `CanActivate(AActor* Target)` - Validation check
- `OnActivate()` - Event fired on activation
- `OnCooldownComplete()` - Event fired when cooldown ends

#### SkillDataAsset
**Purpose:** Data asset containing all skill properties

**Key Properties:**
- SkillID (unique identifier)
- SkillName
- SkillDescription
- SkillIcon (texture reference)
- SkillType (enum: Melee, Ranged, Magic, Utility, etc.)
- CooldownDuration
- ManaCost
- CastTime
- Range
- Damage/Effect values
- RequiredLevel
- Skill-specific properties

#### SkillItem (Consumable)
**Purpose:** Special consumable item that grants skills

**Inheritance:** Inherits from ConsumableItem, which inherits from ItemBase

**Key Features:**
- References SkillDataAsset to grant
- On consumption, unlocks skill for player
- One-time use (consumed on use)
- Can stack (multiple copies = multiple uses)

**Usage Flow:**
1. Player uses SkillItem from inventory
2. SkillItem validates player can learn skill
3. SkillManagerComponent unlocks skill
4. SkillItem is consumed (removed from inventory)
5. UI updates to show new skill available

**Implementation Notes:**
- SkillItem should check if skill is already unlocked
- Option to allow re-learning for skill level increase
- Visual feedback when skill is granted

### Skill Types

#### MeleeAttack Skill
- Close-range damage
- Animation-driven
- Hit detection via collision/overlap

#### RangedAttack Skill
- Projectile-based
- Spawns projectile actor
- Range and accuracy parameters

#### Dash Skill
- Movement ability
- Invincibility frames (optional)
- Distance and speed parameters

#### Magic Skill
- Spell casting
- Particle effects
- Area of effect support

### UI System

#### SkillBarWidget
**Purpose:** Main skill bar UI (hotkey bar)

**Key Features:**
- Horizontal bar of skill slots
- Key bindings display (Q, E, R, etc.)
- Cooldown visual indicators
- Skill icons

#### SkillSlotWidget
**Purpose:** Individual skill slot in skill bar

**Key Features:**
- Skill icon display
- Cooldown overlay/progress bar
- Key binding label
- Click to activate
- Drag and drop from skill list (optional)

### Skill Acquisition Methods

#### Class Level Rewards
- Skills granted automatically when reaching certain class levels
- Defined in ClassDataAsset skill rewards table
- Unlocked immediately upon level requirement met

#### Training and Repeated Use
- Skills gain experience through use
- Experience rate modified by:
  - Primary attributes (INT for magic, DEX for precision, etc.)
  - Class affinity bonuses
  - Skill difficulty modifier
- Natural progression through gameplay

#### Skill Stones (Item)
**Purpose:** Stored skill knowledge that can be consumed to learn skills

**Inheritance:** Inherits from ConsumableItem, which inherits from ItemBase

**Key Features:**
- Contains SkillDataAsset reference
- On consumption, grants skill to player
- One-time use (consumed on use)
- Can stack (multiple copies = multiple uses)
- Works regardless of class/attribute requirements

**Usage Flow:**
1. Player uses SkillStone from inventory
2. SkillStone validates player can learn skill (optional check)
3. SkillManagerComponent unlocks skill
4. SkillStone is consumed (removed from inventory)
5. UI updates to show new skill available

#### Beast Cores (Item)
**Purpose:** Conditional skill granting based on affinity and attributes

**Inheritance:** Inherits from ConsumableItem, which inherits from ItemBase

**Key Features:**
- Contains SkillDataAsset reference
- Requires specific attribute thresholds
- May require class affinity match
- Conditional skill granting (may fail if requirements not met)
- One-time use (consumed on use)
- Can stack (multiple copies = multiple uses)

**Usage Flow:**
1. Player uses BeastCore from inventory
2. BeastCore checks attribute requirements
3. BeastCore checks class affinity (if required)
4. If requirements met: SkillManagerComponent unlocks skill
5. If requirements not met: Skill not granted, item may or may not be consumed
6. BeastCore is consumed (removed from inventory)
7. UI updates with result

**Implementation Notes:**
- BeastCore should display requirements in tooltip
- Visual/audio feedback for success/failure
- Option to allow partial consumption (keep item if failed)

#### Trials and Achievements
- Skills granted as rewards for completing trials
- Unique skills only available through specific achievements
- Handled by TrialComponent and achievement system

### Skill Progression

#### Skill Experience System
- Skills gain experience independently from class levels
- Experience sources:
  - Using the skill in combat/utility
  - Training actions (practice, meditation, etc.)
  - Skill-specific activities
- Experience rate modifiers:
  - Primary attributes (e.g., INT for magic skills)
  - Class affinity bonuses
  - Skill difficulty multiplier
  - Equipment bonuses

#### Skill Level Benefits
Higher skill levels provide:
- **Effectiveness:** Increased damage, healing, utility power
- **Efficiency:** Reduced stamina/mana cost
- **Secondary Attribute Modifiers:** 
  - Swordsmanship → Accuracy, Damage consistency, Stamina efficiency
  - Magic skills → Mana efficiency, Spell power, Mana control
  - Movement skills → Evasion, Reaction speed
- **Unlock Requirements:** Some skills require minimum skill levels in prerequisites

#### Skill Proficiency
- Skills affect secondary attributes directly
- Example: Higher Swordsmanship increases:
  - Accuracy (derived from DEX + Swordsmanship level)
  - Damage consistency (derived from STR + Swordsmanship level)
  - Stamina efficiency (reduced cost per use)

---

## Leveling & Progression System

### System Overview

The progression system is built on four interlocking layers that work together to create strategic, meaningful character development:

1. **Primary Attributes** – Raw potential and growth ceilings
2. **Secondary Attributes** – Derived combat & utility stats (real-time performance)
3. **Skills** – Learned techniques and proficiencies
4. **Classes / Professions** – Structured growth paths

None of these work in isolation; optimization comes from how they reinforce one another.

### Core Design Philosophy

- **Hard Rules:** Growth is constrained by prerequisites and consequences
- **Strategic Choices:** Poor specialization choices have long-term consequences
- **Nonlinear Growth:** Avoids raw level inflation through attribute/skill interaction
- **Meaningful Progression:** Slower but more meaningful than typical LitRPGs
- **Consequence-Driven:** Skills can fail or cause harm if misused

---

## Experience & Leveling System

### Experience (XP) System

#### XP Sources
XP is earned through multiple activities:
- **Combat:** Kills scale by difficulty (enemy level, rarity, etc.)
- **Trials:** Major source of growth (high-risk, high-reward)
- **Relevant Actions:** 
  - Crafting grants XP to crafting professions
  - Healing grants XP to healing skills/professions
  - Training activities grant skill-specific XP

#### XP Storage
- XP is stored **globally** until applied to a class or profession
- Player can choose which class/profession to allocate XP to
- Allows strategic XP management and planning

### Class & Profession Levels

#### Level Structure
- Each class or profession has its **own independent level**
- Total "character level" is the **sum of all active class/profession levels**
- XP must be **assigned to a class** to increase its level
- Classes can be switched/activated, but levels persist

#### Class Prerequisites
Higher-tier classes require:
- **Minimum attribute thresholds** (e.g., STR 50, INT 30)
- **Relevant skills at sufficient levels** (e.g., Swordsmanship Level 25)
- **Sometimes specific achievements** (e.g., "Defeat 100 enemies")
- **Previous class levels** (e.g., Warrior Level 20 to unlock Paladin)

#### Specialization System
- Basic classes can **evolve into advanced classes**
- **Specializing resets the class level to 1**
- **Attributes, skills, and affinities persist** (not lost)
- Allows character refinement without complete reset
- Example: Warrior → Paladin (resets class level, keeps attributes/skills)

### ExperienceComponent

**Purpose:** Manages global XP pool and class/profession leveling

**Key Features:**
- Global XP storage
- XP allocation to classes/professions
- Level tracking per class/profession
- Total level calculation
- XP gain from various sources

**Data Structure:**
```cpp
// Pseudo-structure
struct FClassLevel
{
    TObjectPtr<UClassDataAsset> Class;
    int32 Level;
    float CurrentXP;
    float XPToNextLevel;
    bool bIsActive;
};

class UExperienceComponent : public UActorComponent
{
    UPROPERTY()
    float GlobalXP;
    
    UPROPERTY()
    TArray<FClassLevel> ClassLevels;
    
    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXPGained, float, Amount, FString, Source);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClassLevelUp, UClassDataAsset*, Class, int32, NewLevel);
};
```

**Core Methods:**
- `AddXP(float Amount, FString Source)` - Add XP to global pool
- `AllocateXPToClass(UClassDataAsset* Class, float Amount)` - Assign XP to class
- `GetClassLevel(UClassDataAsset* Class)` - Get class level
- `GetTotalLevel()` - Calculate sum of all class levels
- `CanLevelUpClass(UClassDataAsset* Class)` - Check if class can level
- `LevelUpClass(UClassDataAsset* Class)` - Execute class level up
- `UnlockClass(UClassDataAsset* Class)` - Unlock new class if prerequisites met

---

## Primary Attributes System

### The Six Primary Attributes

Primary Attributes define growth ceilings and learning speed:

1. **Strength (STR)** – Physical power
   - Influences: Melee damage, carrying capacity, physical skills
   - Affects: Attack Power (melee), Stamina pool

2. **Agility (AGI)** – Movement, balance
   - Influences: Movement speed, evasion, reaction time
   - Affects: Attack Speed, Evasion, Reaction Speed

3. **Constitution (CON)** – Endurance, survivability
   - Influences: Health, stamina, resistance to damage
   - Affects: Health/Vitality, Stamina, Regeneration Rates

4. **Dexterity (DEX)** – Precision, control
   - Influences: Accuracy, fine motor skills, ranged combat
   - Affects: Accuracy, Attack Power (ranged/precision)

5. **Intelligence (INT)** – Learning, mana capacity
   - Influences: Magic power, learning speed, mana pool
   - Affects: Mana Pool, Magic skill learning rate

6. **Wisdom (WIS)** – Efficiency, insight, control
   - Influences: Mana efficiency, skill efficiency, perception
   - Affects: Mana Regeneration, Mana Control/Efficiency, Perception, Skill Efficiency

### Attribute Growth

#### Natural Growth Methods
Primary attributes increase through:
- **Use:** Using attribute-related actions (STR from combat, INT from casting)
- **Training:** Specific training activities (weight training, meditation, etc.)
- **Leveling:** Class level ups grant attribute points
- **Trials:** Attribute boosts as trial rewards
- **Items:** Temporary or permanent attribute bonuses

#### Attribute Gating
- Attributes **gate access** to certain skills and classes
- Example: Fireball requires INT 25, Paladin requires STR 50 + CON 40
- Prevents power-leveling without proper foundation

### AttributeComponent

**Purpose:** Manages primary attributes and their growth

**Key Features:**
- Six primary attribute values
- Attribute growth tracking
- Attribute modification (temporary/permanent)
- Attribute requirement validation

**Data Structure:**
```cpp
// Pseudo-structure
struct FPrimaryAttributes
{
    int32 Strength;
    int32 Agility;
    int32 Constitution;
    int32 Dexterity;
    int32 Intelligence;
    int32 Wisdom;
};

class UAttributeComponent : public UActorComponent
{
    UPROPERTY()
    FPrimaryAttributes BaseAttributes;
    
    UPROPERTY()
    FPrimaryAttributes BonusAttributes; // Temporary bonuses
    
    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, EPrimaryAttribute, Attribute, int32, NewValue);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeIncreased, EPrimaryAttribute, Attribute);
};
```

**Core Methods:**
- `GetAttributeValue(EPrimaryAttribute Attribute)` - Get current attribute value
- `IncreaseAttribute(EPrimaryAttribute Attribute, int32 Amount)` - Increase attribute
- `AddTemporaryBonus(EPrimaryAttribute Attribute, int32 Amount, float Duration)` - Temporary boost
- `MeetsRequirement(FPrimaryAttributes Requirements)` - Check if requirements met
- `GetTotalAttributeValue(EPrimaryAttribute Attribute)` - Base + bonuses

---

## Secondary Attributes System

### Overview

Secondary attributes are **calculated values** derived from:
- Primary attributes
- Skill levels
- Class bonuses
- Equipment bonuses
- Temporary effects

These directly affect **moment-to-moment performance** in combat and gameplay.

### Secondary Attribute Categories

#### Health & Survival

**Health / Vitality**
- Derived mainly from **Constitution**
- Formula: `BaseHealth = 100 + (CON * 10) + (ClassBonus)`
- Modified by: Equipment, skills, temporary effects

**Stamina**
- Driven by **Constitution and Strength**
- Formula: `BaseStamina = 50 + (CON * 5) + (STR * 2)`
- Used for: Physical actions, combat abilities

**Regeneration Rates**
- Influenced by **Constitution and class traits**
- Health Regen: `CON * 0.1 + ClassBonus per second`
- Stamina Regen: `CON * 0.2 + ClassBonus per second`

#### Mana & Control

**Mana Pool**
- Primarily based on **Intelligence**
- Formula: `BaseMana = 50 + (INT * 8) + (ClassBonus)`
- Modified by: Equipment, skills, class bonuses

**Mana Regeneration**
- Strongly influenced by **Wisdom**
- Formula: `ManaRegen = (WIS * 0.15) + (ClassBonus) per second`
- Modified by: Skills, equipment, temporary effects

**Mana Control / Efficiency**
- **Wisdom + relevant skills** (e.g., Mana Control skill)
- Reduces mana cost of spells
- Formula: `ManaCostReduction = (WIS * 0.01) + (ManaControlSkillLevel * 0.02)`

#### Combat Effectiveness

**Attack Power**
- **Strength** (melee), **Dexterity** (precision/ranged)
- Formula: `MeleeAttack = STR * 2 + WeaponDamage + SkillBonus`
- Formula: `RangedAttack = DEX * 2 + WeaponDamage + SkillBonus`

**Accuracy**
- **Dexterity + weapon skills**
- Formula: `Accuracy = 50 + (DEX * 1.5) + (WeaponSkillLevel * 2)`
- Affects: Hit chance, critical hit chance

**Attack Speed**
- **Agility + skill proficiency**
- Formula: `AttackSpeed = 1.0 + (AGI * 0.01) + (CombatSkillLevel * 0.005)`
- Modified by: Equipment, skills, temporary effects

**Defense / Damage Reduction**
- **Constitution, armor skills, class bonuses**
- Formula: `Defense = (CON * 0.5) + (ArmorSkillLevel * 1) + EquipmentDefense`
- Reduces incoming damage

**Evasion**
- **Agility + movement skills**
- Formula: `Evasion = (AGI * 1.2) + (MovementSkillLevel * 1.5)`
- Affects: Dodge chance, movement speed

#### Utility & Awareness

**Perception**
- **Wisdom + Dexterity**
- Formula: `Perception = (WIS * 1.5) + (DEX * 0.5)`
- Affects: Detection range, hidden object discovery

**Reaction Speed**
- **Agility + experience**
- Formula: `ReactionSpeed = (AGI * 2) + (CombatExperience * 0.1)`
- Affects: Response time to threats, parry timing

**Skill Efficiency**
- **Wisdom reduces cost and waste** when using skills
- Formula: `EfficiencyBonus = (WIS * 0.02) + (SkillLevel * 0.01)`
- Reduces: Mana cost, stamina cost, cooldown time

### SecondaryAttributeComponent

**Purpose:** Calculates and manages all secondary attributes

**Key Features:**
- Real-time calculation of secondary attributes
- Caching for performance (recalculate on attribute/skill change)
- Event broadcasting when attributes change
- Support for temporary modifiers

**Data Structure:**
```cpp
// Pseudo-structure
struct FSecondaryAttributes
{
    // Health & Survival
    float MaxHealth;
    float CurrentHealth;
    float MaxStamina;
    float CurrentStamina;
    float HealthRegenRate;
    float StaminaRegenRate;
    
    // Mana & Control
    float MaxMana;
    float CurrentMana;
    float ManaRegenRate;
    float ManaEfficiency;
    
    // Combat
    float MeleeAttackPower;
    float RangedAttackPower;
    float Accuracy;
    float AttackSpeed;
    float Defense;
    float Evasion;
    
    // Utility
    float Perception;
    float ReactionSpeed;
    float SkillEfficiency;
};

class USecondaryAttributeComponent : public UActorComponent
{
    UPROPERTY()
    FSecondaryAttributes CalculatedAttributes;
    
    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSecondaryAttributesChanged, FSecondaryAttributes, NewAttributes);
};
```

**Core Methods:**
- `RecalculateAllAttributes()` - Recalculate all secondary attributes
- `GetSecondaryAttribute(ESecondaryAttribute Attribute)` - Get specific attribute
- `ApplyTemporaryModifier(ESecondaryAttribute Attribute, float Modifier, float Duration)` - Temporary boost
- `GetHealthPercentage()` - Get current health as percentage
- `GetManaPercentage()` - Get current mana as percentage

**Calculation Flow:**
1. **Primary Attributes** → Base values
2. **Skill Levels** → Add skill-based bonuses
3. **Class Bonuses** → Add class-specific modifiers
4. **Equipment Bonuses** → Add equipment modifiers
5. **Temporary Effects** → Apply temporary modifiers
6. **Final Values** → Cached and broadcast to systems

---

## Class & Profession System

### Class System

#### Class Types
- **Basic Classes:** Starting classes (Warrior, Mage, Rogue, etc.)
- **Advanced Classes:** Evolved from basic classes (Paladin, Archmage, Assassin, etc.)
- **Hybrid Classes:** Combine multiple class paths
- **Unique Classes:** Special classes with unique requirements

#### Class Features
- **Level Progression:** Independent level per class
- **Skill Unlocks:** Skills granted at specific class levels
- **Attribute Bonuses:** Passive bonuses to primary/secondary attributes
- **Class Affinity:** Affects skill learning speed and effectiveness
- **Specialization:** Can evolve into advanced classes

#### ClassDataAsset
**Purpose:** Data asset containing all class properties

**Key Properties:**
- ClassID (unique identifier)
- ClassName
- ClassDescription
- ClassTier (Basic, Advanced, Hybrid, Unique)
- Prerequisites (attributes, skills, achievements)
- Evolution Classes (classes this can evolve into)
- Level Rewards (skills, attributes, bonuses at each level)
- Attribute Bonuses (passive bonuses)
- Class Affinity Multipliers (skill learning speed modifiers)

### Profession System

#### Profession Types
- **Crafting Professions:** Blacksmith, Alchemist, Enchanter, etc.
- **Gathering Professions:** Miner, Herbalist, Hunter, etc.
- **Support Professions:** Healer, Scholar, Merchant, etc.

#### Profession Features
- **Independent Leveling:** Separate from combat classes
- **Recipe Unlocks:** Recipes unlocked at profession levels
- **Efficiency Bonuses:** Better results at higher levels
- **Resource Management:** Specialized resource gathering/usage

#### ProfessionDataAsset
**Purpose:** Data asset containing all profession properties

**Key Properties:**
- ProfessionID (unique identifier)
- ProfessionName
- ProfessionDescription
- ProfessionType (Crafting, Gathering, Support)
- Level Rewards (recipes, bonuses at each level)
- Efficiency Formulas (how level affects output quality)

### ClassComponent

**Purpose:** Manages class/profession selection, switching, and bonuses

**Key Features:**
- Active class/profession tracking
- Class switching (with restrictions)
- Class bonus application
- Specialization handling
- Class unlock validation

**Data Structure:**
```cpp
// Pseudo-structure
struct FActiveClass
{
    TObjectPtr<UClassDataAsset> Class;
    bool bIsActive;
    int32 Level;
    TArray<FString> UnlockedFeatures; // Skills, bonuses unlocked at this level
};

class UClassComponent : public UActorComponent
{
    UPROPERTY()
    TArray<FActiveClass> UnlockedClasses;
    
    UPROPERTY()
    TArray<FActiveClass> UnlockedProfessions;
    
    UPROPERTY()
    TObjectPtr<UClassDataAsset> ActiveClass;
    
    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClassChanged, UClassDataAsset*, OldClass, UClassDataAsset*, NewClass);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClassUnlocked, UClassDataAsset*, Class);
};
```

**Core Methods:**
- `UnlockClass(UClassDataAsset* Class)` - Unlock class if prerequisites met
- `SetActiveClass(UClassDataAsset* Class)` - Switch active class
- `CanUnlockClass(UClassDataAsset* Class)` - Check prerequisites
- `GetClassBonus(EPrimaryAttribute Attribute)` - Get class attribute bonus
- `GetClassAffinityMultiplier(USkillDataAsset* Skill)` - Get skill learning speed modifier
- `SpecializeClass(UClassDataAsset* NewClass)` - Evolve to advanced class (resets level)

---

## Trials System

### Trial Overview

Trials are **high-risk, high-reward challenges** that can grant:
- **Large XP infusions** (bypass slow natural growth)
- **Unique skills or traits** (not available elsewhere)
- **Attribute boosts** (permanent or temporary)
- **Rare items** (powerful equipment or consumables)

### Trial Types

#### Combat Trials
- Defeat waves of enemies
- Boss battles
- Survival challenges
- Time-limited combat

#### Skill Trials
- Demonstrate mastery of specific skills
- Complete skill-based challenges
- Precision/accuracy tests

#### Exploration Trials
- Discover hidden locations
- Solve puzzles
- Navigate dangerous areas

#### Crafting Trials
- Create specific items
- Achieve quality thresholds
- Resource management challenges

### Trial Features

- **High Risk:** Failure may result in penalties (death, item loss, etc.)
- **High Reward:** Significant progression boosts
- **Reshape Secondary Attributes:** Rapid attribute changes
- **Unique Rewards:** Skills/items only available through trials
- **One-Time or Repeatable:** Some trials can be repeated, others are one-time

### TrialComponent

**Purpose:** Manages trial activation, completion, and rewards

**Key Features:**
- Trial activation and tracking
- Completion validation
- Reward distribution
- Trial state management

**Data Structure:**
```cpp
// Pseudo-structure
struct FTrialProgress
{
    TObjectPtr<UTrialDataAsset> Trial;
    bool bIsCompleted;
    bool bIsActive;
    float Progress; // 0.0 to 1.0
    TArray<FString> CompletedObjectives;
};

class UTrialComponent : public UActorComponent
{
    UPROPERTY()
    TArray<FTrialProgress> TrialProgress;
    
    UPROPERTY()
    TObjectPtr<UTrialDataAsset> ActiveTrial;
    
    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrialStarted, UTrialDataAsset*, Trial);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrialCompleted, UTrialDataAsset*, Trial);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTrialFailed, UTrialDataAsset*, Trial);
};
```

**Core Methods:**
- `StartTrial(UTrialDataAsset* Trial)` - Begin trial
- `CompleteTrial(UTrialDataAsset* Trial)` - Mark trial as complete
- `FailTrial(UTrialDataAsset* Trial)` - Handle trial failure
- `GetTrialProgress(UTrialDataAsset* Trial)` - Get current progress
- `CanStartTrial(UTrialDataAsset* Trial)` - Check requirements
- `GrantTrialRewards(UTrialDataAsset* Trial)` - Distribute rewards

### TrialDataAsset

**Purpose:** Data asset containing trial configuration

**Key Properties:**
- TrialID (unique identifier)
- TrialName
- TrialDescription
- TrialType (Combat, Skill, Exploration, Crafting)
- Prerequisites (attributes, skills, previous trials)
- Objectives (what must be completed)
- Rewards (XP, skills, attributes, items)
- Failure Penalties (optional)
- IsRepeatable (boolean)

---

## System Interactions

### How Systems Reinforce Each Other

#### Example Growth Loop
1. **Increase Strength** → Higher attack power (secondary attribute)
2. **Train weapon skill** → Better accuracy and stamina use (skill affects secondary attributes)
3. **Level class** → Unlock passive bonuses and new skills (class provides structure)
4. **Secondary attributes rise** → Combat becomes more efficient (real-time performance)
5. **Complete trial** → Large XP boost and unique skill (trial bypasses slow growth)

This creates **nonlinear growth**, avoiding raw level inflation.

### Attribute → Skill → Secondary Attribute Flow

1. **Primary Attributes** determine:
   - Learning speed (INT for magic, DEX for precision)
   - Maximum potential (attribute gates)
   - Base secondary attribute values

2. **Skills** refine how attributes are expressed:
   - Swordsmanship increases accuracy (DEX + skill level)
   - Magic skills increase mana efficiency (INT + WIS + skill level)
   - Movement skills increase evasion (AGI + skill level)

3. **Secondary Attributes** are the real-time result:
   - Calculated from primary attributes + skills + class bonuses
   - Directly affect gameplay performance
   - Update dynamically as components change

4. **Classes** provide structured bonuses:
   - Passive attribute bonuses
   - Skill unlocks at specific levels
   - Affinity multipliers for skill learning

### Hard Rules & Consequences

#### Growth Constraints
- **Prerequisites:** Cannot unlock class/skill without meeting requirements
- **Attribute Gates:** Skills fail or are unavailable if attributes too low
- **Secondary Attribute Limits:** Fatigue, mana collapse impose real limits
- **Specialization Consequences:** Poor choices have long-term impact

#### Skill Failure System
- Skills can **fail or cause harm** if misused
- Low attribute + low skill level = higher failure chance
- Example: Low INT + low Mana Control = mana backlash on spell failure

#### Strategic Depth
- **Consequence-driven:** Choices matter long-term
- **Strategic:** Must plan attribute/skill/class progression
- **Meaningful:** Slower but more impactful than typical LitRPGs

---

## UI System for Progression

### AttributeWidget
**Purpose:** Display primary and secondary attributes

**Key Features:**
- Primary attribute display with values
- Secondary attribute display with calculated values
- Visual bars for health/stamina/mana
- Attribute growth indicators
- Tooltips explaining attribute effects

### ClassWidget
**Purpose:** Class selection and management UI

**Key Features:**
- Available classes list
- Class prerequisites display
- Active class indicator
- Class level and XP display
- Specialization options
- Class bonus display

### ExperienceWidget
**Purpose:** XP management and allocation UI

**Key Features:**
- Global XP pool display
- XP allocation interface
- Class level progress bars
- XP source history
- Level-up notifications

### TrialWidget
**Purpose:** Trial selection and progress UI

**Key Features:**
- Available trials list
- Trial prerequisites display
- Active trial progress
- Trial rewards preview
- Completion status

---

## Input System Integration

### Enhanced Input Setup

#### Input Actions (IA_ActionRPG)
**Required Actions:**
- `IA_Move` - 2D vector for movement (WASD/Gamepad stick)
- `IA_Look` - 2D vector for camera look (Mouse/Gamepad stick)
- `IA_Interact` - Action for item pickup/interaction
- `IA_UseItem` - Action for using items (with slot parameter)
- `IA_OpenInventory` - Toggle inventory UI
- `IA_SkillSlot1` through `IA_SkillSlot8` - Skill activation hotkeys
- `IA_Attack` - Primary attack action
- `IA_Dodge` - Dodge/roll action

#### Input Mapping Context (IM_ActionRPG)
**Keyboard/Mouse Mappings:**
- W/A/S/D → IA_Move
- Mouse XY → IA_Look
- E → IA_Interact
- Tab → IA_OpenInventory
- 1-8 → IA_SkillSlot1-8
- Left Click → IA_Attack
- Space → IA_Dodge

**Gamepad Mappings:**
- Left Stick → IA_Move
- Right Stick → IA_Look
- X/A → IA_Interact
- Y/Triangle → IA_OpenInventory
- D-Pad/Shoulder Buttons → IA_SkillSlot1-8
- Right Trigger → IA_Attack
- A/X → IA_Dodge

### Input Handling Architecture

#### ActionRPGPlayerController
**Responsibilities:**
- Enhanced Input setup and context switching
- Input action bindings to gameplay functions
- UI input handling (inventory, menus)
- Camera control (top-down camera)

**Key Methods:**
- `SetupInputComponent()` - Initialize Enhanced Input
- `OnMove(const FInputActionValue& Value)` - Handle movement
- `OnInteract()` - Handle interaction
- `OnSkillSlotPressed(int32 SlotIndex)` - Activate skill from hotkey
- `OnInventoryToggle()` - Toggle inventory UI

#### Input Flow
1. **Input Action Triggered** → Enhanced Input System
2. **PlayerController Receives** → Routes to appropriate handler
3. **Character/Component Executes** → Gameplay action performed
4. **Feedback** → Visual/audio feedback to player

---

## Data Assets & Configuration

### ItemDatabase (Singleton)
**Purpose:** Central registry for all item Data Assets

**Features:**
- Fast lookup by ItemID
- Validation of item references
- Runtime item creation from Data Assets
- Item category organization

**Implementation:**
- UObject-based singleton pattern
- Loaded at game start
- Accessible via `UItemDatabase::Get()`

### SkillDatabase (Singleton)
**Purpose:** Central registry for all skill Data Assets

**Features:**
- Fast lookup by SkillID
- Validation of skill references
- Runtime skill creation from Data Assets
- Skill category organization

**Implementation:**
- UObject-based singleton pattern
- Loaded at game start
- Accessible via `USkillDatabase::Get()`

### ClassDatabase (Singleton)
**Purpose:** Central registry for all class and profession Data Assets

**Features:**
- Fast lookup by ClassID/ProfessionID
- Validation of class prerequisites
- Class evolution tree management
- Class category organization

**Implementation:**
- UObject-based singleton pattern
- Loaded at game start
- Accessible via `UClassDatabase::Get()`

### AttributeDatabase (Singleton)
**Purpose:** Central registry for attribute calculations and formulas

**Features:**
- Secondary attribute calculation formulas
- Attribute requirement validation
- Attribute growth rate definitions
- Attribute interaction rules

**Implementation:**
- UObject-based singleton pattern
- Loaded at game start
- Accessible via `UAttributeDatabase::Get()`

### Data Asset Types

#### Item Data Assets
- `ItemDataAsset` - Base item properties
- `SkillItemDataAsset` - Skill-granting consumables
- `SkillStoneDataAsset` - Stored skill knowledge items
- `BeastCoreDataAsset` - Conditional skill-granting items

#### Skill Data Assets
- `SkillDataAsset` - Base skill properties
- Includes: Skill requirements, cooldowns, costs, effects

#### Class Data Assets
- `ClassDataAsset` - Class properties and progression
- `ProfessionDataAsset` - Profession properties and recipes

#### Attribute Data Assets
- `PrimaryAttributeDataAsset` - Primary attribute definitions
- `SecondaryAttributeDataAsset` - Secondary attribute calculation formulas

#### Trial Data Assets
- `TrialDataAsset` - Trial configuration and rewards

### Data Asset Best Practices
- All items, skills, classes, and attributes must have Data Assets
- Data Assets should be validated on load
- Use meaningful naming: `DA_Item_[Category]_[Name]`, `DA_Class_[Name]`, etc.
- Group related Data Assets in folders
- Version control all Data Assets
- Use inheritance for shared properties (e.g., base item properties)

---

## Component Architecture

### Component Communication

#### Event System
- Use `DECLARE_DYNAMIC_MULTICAST_DELEGATE` for component events
- Inventory changes broadcast to UI
- Skill activation broadcasts to effects system
- Item usage broadcasts to relevant systems

#### Dependency Injection
- Components reference each other via GetComponent calls
- Avoid circular dependencies
- Use interfaces for loose coupling where appropriate

### Component Lifecycle
1. **Construction:** Components created in constructor
2. **BeginPlay:** Initialize component state
3. **Tick:** Update component logic (if needed)
4. **EndPlay:** Cleanup component state

### Component Organization
- **Core Components:** Inventory, Skills, Equipment
- **Progression Components:** Attributes, Secondary Attributes, Class, Experience, Trials
- **Interaction Components:** Pickup, Interaction
- **Combat Components:** Health, Damage, Combat (future)
- **UI Components:** UI-related components (if needed)

---

## Gameplay Framework Integration

### ActionRPGCharacter (Base)
**Purpose:** Base character class for all characters

**Components:**
- Movement component (custom or CharacterMovement)
- Health component (future)
- Inventory component (player only)
- Skill component
- Equipment component

### ActionRPGPlayerCharacter
**Purpose:** Player-specific character class

**Additional Components:**
- InventoryComponent
- SkillManagerComponent
- InteractionComponent
- AttributeComponent
- SecondaryAttributeComponent
- ClassComponent
- ExperienceComponent
- TrialComponent
- Camera component (top-down)

**Features:**
- Player input handling
- UI interaction
- Camera control
- Character progression management

### ActionRPGGameMode
**Purpose:** Game mode configuration

**Settings:**
- Default player class
- HUD class
- Player controller class
- Game state class

### ActionRPGPlayerController
**Purpose:** Player controller for input and UI

**Responsibilities:**
- Enhanced Input setup
- UI management
- Camera control
- Game state access

### ActionRPGGameState
**Purpose:** Replicated game state (for multiplayer future)

**Features:**
- Global game state
- Time management
- Score/statistics (future)

---

## Asset Organization

### Naming Conventions

#### Blueprints
- `BP_[Category]_[Name]` - e.g., `BP_Item_HealthPotion`
- `BP_Character_[Name]` - e.g., `BP_Character_Player`
- `BP_Component_[Name]` - e.g., `BP_Component_Inventory`

#### Widgets
- `WBP_[Category]_[Name]` - e.g., `WBP_Inventory_Main`
- `WBP_[Category]_[Element]` - e.g., `WBP_Inventory_Slot`

#### Data Assets
- `DA_Item_[Category]_[Name]` - e.g., `DA_Item_Consumable_HealthPotion`
- `DA_Skill_[Type]_[Name]` - e.g., `DA_Skill_Combat_MeleeAttack`
- `DA_Class_[Name]` - e.g., `DA_Class_Warrior`
- `DA_Profession_[Name]` - e.g., `DA_Profession_Blacksmith`
- `DA_Trial_[Type]_[Name]` - e.g., `DA_Trial_Combat_Test`

#### Materials
- `M_[Category]_[Name]` - e.g., `M_Item_Glow`

#### Textures
- `T_[Category]_[Name]` - e.g., `T_UI_Inventory_Background`
- `T_Item_[Name]_Icon` - e.g., `T_Item_HealthPotion_Icon`

#### Meshes
- `SM_[Category]_[Name]` - e.g., `SM_Item_Pickup`

### Folder Organization
- Group by system (Inventory, Skills, Items)
- Group by type (Blueprints, Data, Materials)
- Use consistent naming across folders
- Avoid deep nesting (max 3-4 levels)

---

## Naming Conventions

### C++ Classes
- **Base Classes:** `[System]Base` - e.g., `ItemBase`, `SkillBase`
- **Components:** `[System]Component` - e.g., `InventoryComponent`
- **Actors:** `[Type]Actor` - e.g., `ItemPickupActor`
- **Widgets:** `[System]Widget` - e.g., `InventoryWidget`
- **Data Assets:** `[Type]DataAsset` - e.g., `ItemDataAsset`

### Variables
- **Member Variables:** `b[Name]` for bool, `[Name]` for others
- **Public Properties:** Use `UPROPERTY()` with appropriate specifiers
- **Private Members:** Prefix with `[System]` if needed for clarity

### Functions
- **Public Functions:** PascalCase - e.g., `AddItem()`, `UseSkill()`
- **Event Handlers:** `On[Event]` - e.g., `OnItemAdded()`, `OnSkillActivated()`
- **Validation:** `Can[Action]` - e.g., `CanUseItem()`, `CanActivateSkill()`

### Enums
- **Enum Names:** `E[System][Type]` - e.g., `EItemType`, `ESkillType`
- **Enum Values:** `[Type]_[Value]` - e.g., `ItemType_Consumable`, `SkillType_Combat`

### Structs
- **Struct Names:** `F[System][Type]` - e.g., `FInventorySlot`, `FSkillInstance`

---

## Implementation Phases

### Phase 1: Foundation (Week 1-2)
**Goals:**
- Set up folder structure
- Create base classes (ItemBase, SkillBase)
- Implement Data Asset structures
- Set up Enhanced Input System
- Create basic character and player controller

**Deliverables:**
- ItemDataAsset and SkillDataAsset classes
- Base item and skill classes
- Input actions and mapping context
- Basic player character setup

### Phase 2: Inventory System (Week 3-4)
**Goals:**
- Implement InventoryComponent
- Create ItemPickupActor
- Build inventory UI (InventoryWidget, InventorySlotWidget)
- Implement drag and drop
- Create sample item Data Assets

**Deliverables:**
- Fully functional inventory system
- Item pickup in world
- Drag and drop inventory UI
- Item usage system

### Phase 3: Skill System (Week 5-6)
**Goals:**
- Implement SkillComponent and SkillManagerComponent
- Create skill base classes and implementations
- Build skill bar UI
- Implement skill activation
- Create SkillItem, SkillStone, and BeastCore consumables
- Implement skill experience and leveling

**Deliverables:**
- Skill system with cooldowns
- Skill bar UI with hotkeys
- Skill-granting items (SkillItem, SkillStone, BeastCore)
- Skill experience and progression system
- Sample skill implementations

### Phase 4: Attribute & Progression System (Week 7-9)
**Goals:**
- Implement AttributeComponent (Primary Attributes)
- Implement SecondaryAttributeComponent with calculation formulas
- Create attribute Data Assets
- Implement ExperienceComponent (XP system)
- Implement ClassComponent (Class/Profession system)
- Create class and profession Data Assets
- Build attribute and class UI widgets

**Deliverables:**
- Primary attribute system (STR, AGI, CON, DEX, INT, WIS)
- Secondary attribute calculation system
- XP management and allocation system
- Class/Profession leveling system
- Class specialization system
- Attribute and class UI

### Phase 5: Trials System (Week 10-11)
**Goals:**
- Implement TrialComponent
- Create TrialDataAsset structure
- Build trial activation and completion system
- Implement trial reward distribution
- Create sample trials (Combat, Skill, Exploration)
- Build trial UI widget

**Deliverables:**
- Trial system with multiple trial types
- Trial progress tracking
- Trial reward system
- Sample trial implementations
- Trial UI

### Phase 6: Integration & Polish (Week 12-14)
**Goals:**
- Integrate all systems (Inventory, Skills, Attributes, Classes, Trials)
- Implement system interactions and reinforcement
- Polish UI/UX for all systems
- Add visual/audio feedback
- Create comprehensive Data Assets (items, skills, classes, trials)
- Testing and bug fixes
- Balance attribute calculations and progression rates

**Deliverables:**
- Complete integrated progression system
- Polished UI for all systems
- Comprehensive sample content
- Balanced progression rates
- Documentation updates

---

## Technical Considerations

### Performance
- **Inventory:** Use object pooling for item instances if needed
- **Skills:** Cache skill references, avoid repeated lookups
- **Attributes:** Cache secondary attribute calculations, recalculate only on change
- **UI:** Use widget pooling for inventory slots and attribute displays
- **Data Assets:** Load on demand or at game start
- **Secondary Attributes:** Batch recalculation, avoid per-frame calculations

### Memory Management
- Use `TObjectPtr<>` for Unreal Engine 5.7
- Proper garbage collection (avoid UPROPERTY issues)
- Clean up event bindings in EndPlay

### Networking (Future)
- Design components with replication in mind
- Use RPCs for skill activation
- Replicate inventory changes
- Consider server-authoritative design

### Extensibility
- Use interfaces for cross-system communication
- Design for easy addition of new item/skill types
- Modular component architecture
- Data-driven design allows content creation without code

---

## Testing Strategy

### Unit Testing
- Test inventory operations (add, remove, move, swap)
- Test skill activation and cooldowns
- Test item usage and consumption
- Test SkillItem, SkillStone, and BeastCore skill granting
- Test primary attribute growth and modifications
- Test secondary attribute calculations
- Test XP allocation and class leveling
- Test class prerequisites and unlocking
- Test trial activation and completion

### Integration Testing
- Test inventory + skill system interaction
- Test drag and drop across different scenarios
- Test item pickup and inventory integration
- Test skill activation from hotkeys
- Test attribute → skill → secondary attribute flow
- Test class bonuses affecting attributes and skills
- Test XP allocation affecting class levels
- Test trial rewards affecting progression
- Test system reinforcement loops

### UI Testing
- Test inventory UI responsiveness
- Test drag and drop visual feedback
- Test skill bar cooldown displays
- Test tooltip displays
- Test attribute widget updates
- Test class selection UI
- Test XP allocation UI
- Test trial progress UI

---

## Documentation Requirements

### Code Documentation
- All public functions must have comments
- Complex algorithms should have inline comments
- Use Unreal Engine's documentation style
- Document all events and delegates

### Design Documentation
- Update this document as design evolves
- Document any design decisions and rationale
- Keep implementation notes for future reference

### User Documentation (Future)
- Player guide for inventory system
- Skill system explanation
- Attribute and progression system guide
- Class and profession guide
- Trial system guide
- Controls reference

---

## References

### Unreal Engine 5.7 Documentation
- Enhanced Input System: https://docs.unrealengine.com/5.7/en-US/enhanced-input-in-unreal-engine/
- Component System: https://docs.unrealengine.com/5.7/en-US/components-in-unreal-engine/
- Data Assets: https://docs.unrealengine.com/5.7/en-US/data-assets-in-unreal-engine/
- UI System: https://docs.unrealengine.com/5.7/en-US/umg-ui-designer-for-unreal-engine/
- Drag and Drop: https://docs.unrealengine.com/5.7/en-US/drag-and-drop-in-umg-for-unreal-engine/

### Best Practices
- Unreal Engine Coding Standards: https://docs.unrealengine.com/5.7/en-US/epic-cplusplus-coding-standard-for-unreal-engine/
- Gameplay Framework: https://docs.unrealengine.com/5.7/en-US/gameplay-framework-in-unreal-engine/

---

## Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-01-07 | Initial | Created initial architecture and design plan |
| 1.1 | 2025-01-07 | Update | Added comprehensive Leveling & Progression System including Primary/Secondary Attributes, Class/Profession system, Trials, and enhanced Skill acquisition methods (SkillStone, BeastCore) |

---

**End of Document**

