// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Components/Progression/ClassComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/AcquiredAttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Combat/StatusEffectComponent.h"
#include "Combat/StatusEffectDataAsset.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Components/Progression/TrialComponent.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Progression/ExperiencePickupActor.h"
#include "Progression/Core/ClassDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Progression/Core/PrimaryAttributeDataAsset.h"
#include "Progression/Core/SecondaryAttributeDataAsset.h"
#include "Trials/TrialDataAsset.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Skills/SkillComponent.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Crafting/CraftingRecipeDataAsset.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/SkillStoneItem.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Items/Pickups/ItemPickupActor.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Implementations/SkillBlacksmithCrafting.h"
#include "Skills/Implementations/SkillRegeneration.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"

#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSkillStoneUseTransactionTest, "ActionRPG.Inventory.SkillStoneUseTransaction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSkillStoneUseTransactionTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Owner = World->SpawnActor<AActor>(FVector(108000.0f, 108000.0f, 100.0f),
		FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Item owner spawned"), Owner)) return false;
	UInventoryComponent* Inventory = NewObject<UInventoryComponent>(Owner);
	USkillManagerComponent* Manager = NewObject<USkillManagerComponent>(Owner);
	Owner->AddInstanceComponent(Inventory);
	Owner->AddInstanceComponent(Manager);
	Inventory->RegisterComponent();
	Manager->RegisterComponent();
	if (!Owner->HasActorBegunPlay()) Owner->DispatchBeginPlay();

	UItemDataAsset* ItemData = NewObject<UItemDataAsset>(Owner);
	ItemData->ItemID = TEXT("TestSkillStone");
	ItemData->Type = EItemType::SkillStone;
	ItemData->MaxStackSize = 1;
	ItemData->Weight = 0.0f;
	ItemData->SkillClass = USkillBase::StaticClass();
	ItemData->SkillData = NewObject<USkillDataAsset>(ItemData);
	ItemData->SkillData->SkillID = TEXT("TEST_STONE_GRANT");
	USkillStoneItem* Stone = NewObject<USkillStoneItem>(Owner);
	Stone->ItemData = ItemData;
	TestTrue(TEXT("Skill stone added"), Inventory->AddItem(Stone, 1));
	TestFalse(TEXT("Missing runtime skill component rejects use"), Inventory->UseItem(0));
	TestEqual(TEXT("Failed use preserves the stone"), Inventory->CountItemData(ItemData), 1);
	TestFalse(TEXT("Failed use does not claim the skill"), Manager->IsSkillIDUnlocked(ItemData->SkillData->SkillID));

	USkillComponent* Skills = NewObject<USkillComponent>(Owner);
	Owner->AddInstanceComponent(Skills);
	Skills->RegisterComponent();
	Manager->SkillComponent = Skills;
	TestTrue(TEXT("Valid grant uses the stone"), Inventory->UseItem(0));
	TestEqual(TEXT("Successful use consumes exactly one stone"), Inventory->CountItemData(ItemData), 0);
	TestTrue(TEXT("Successful use unlocks the skill"), Manager->IsSkillIDUnlocked(ItemData->SkillData->SkillID));
	Owner->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryCapacityPreflightTest, "ActionRPG.Inventory.CapacityPreflight",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FInventoryCapacityPreflightTest::RunTest(const FString& Parameters)
{
	UInventoryComponent* Inventory = NewObject<UInventoryComponent>(GetTransientPackage());
	UItemDataAsset* Data = NewObject<UItemDataAsset>(GetTransientPackage());
	Data->ItemID = TEXT("CapacityTestItem");
	Data->MaxStackSize = 2;
	Data->Weight = 0.0f;
	UItemBase* Item = NewObject<UItemBase>(GetTransientPackage());
	Item->ItemData = Data;
	const int32 TooManyItems = Inventory->GetMaxCapacity() * Data->MaxStackSize + 1;
	TestFalse(TEXT("Capacity preflight rejects quantity needing more slots than exist"),
		Inventory->HasSpaceFor(Item, TooManyItems));
	TestFalse(TEXT("Oversized pickup cannot partially add"), Inventory->AddItem(Item, TooManyItems));
	TestEqual(TEXT("Rejected add leaves inventory empty"), Inventory->GetTotalItemCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryDropValidationTest, "ActionRPG.Inventory.DropValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FInventoryDropValidationTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(106000.0f, 106000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	UItemDataAsset* Data = NewObject<UItemDataAsset>(Player);
	Data->ItemID = TEXT("DropValidationItem");
	Data->Type = EItemType::Consumable;
	Data->MaxStackSize = 2;
	Data->Weight = 0.0f;
	UItemBase* Item = NewObject<UItemBase>(Player);
	Item->ItemData = Data;
	UInventoryComponent* Inventory = Player->InventoryComponent;
	TestTrue(TEXT("Stack added"), Inventory->AddItem(Item, 2));
	const FVector DropLocation(106100.0f, 106000.0f, 100.0f);
	TestFalse(TEXT("Zero quantity cannot drop entire stack"), Inventory->DropItemToWorld(0, 0, DropLocation));
	TestFalse(TEXT("Quantity above stack cannot drop entire stack"), Inventory->DropItemToWorld(0, 3, DropLocation));
	TestEqual(TEXT("Invalid drops preserve the stack"), Inventory->GetTotalItemCount(), 2);
	TestTrue(TEXT("Stack assigned to quick-use"), Inventory->AssignItemToQuickUseSlot(0, 8));
	TestTrue(TEXT("One item drops to the world"), Inventory->DropItemToWorld(0, 1, DropLocation));
	TestEqual(TEXT("Partial drop leaves one item carried"), Inventory->GetTotalItemCount(), 1);
	TestEqual(TEXT("Partial drop keeps quick-use assigned"), Inventory->GetQuickUseSlot(8).InventorySlotIndex, 0);
	for (TActorIterator<AItemPickupActor> It(World); It; ++It)
	{
		if (It->GetItemData() == Data) It->Destroy();
	}
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FQuickUseFollowsMovesTest, "ActionRPG.Inventory.QuickUseFollowsMoves",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FQuickUseFollowsMovesTest::RunTest(const FString& Parameters)
{
	UInventoryComponent* Inventory = NewObject<UInventoryComponent>(GetTransientPackage());
	UItemDataAsset* Data = NewObject<UItemDataAsset>(GetTransientPackage());
	Data->ItemID = TEXT("QuickMovePotion");
	Data->Type = EItemType::Consumable;
	Data->MaxStackSize = 1;
	Data->Weight = 0.0f;
	UItemBase* Item = NewObject<UItemBase>(GetTransientPackage());
	Item->ItemData = Data;
	TestTrue(TEXT("Potion added"), Inventory->AddItem(Item, 1));
	TestTrue(TEXT("Potion assigned to quick-use"), Inventory->AssignItemToQuickUseSlot(0, 8));
	UItemBase* AssignedItem = Inventory->GetQuickUseSlot(8).Item;
	TestTrue(TEXT("Item moves to empty slot"), Inventory->MoveItem(0, 1));
	TestEqual(TEXT("Quick-use follows the moved item"), Inventory->GetQuickUseSlot(8).InventorySlotIndex, 1);
	TestTrue(TEXT("Quick-use keeps the same item"), Inventory->GetQuickUseSlot(8).Item == AssignedItem);
	UItemDataAsset* OtherData = NewObject<UItemDataAsset>(GetTransientPackage());
	OtherData->ItemID = TEXT("OtherQuickMovePotion");
	OtherData->Type = EItemType::Consumable;
	OtherData->MaxStackSize = 1;
	OtherData->Weight = 0.0f;
	UItemBase* OtherItem = NewObject<UItemBase>(GetTransientPackage());
	OtherItem->ItemData = OtherData;
	TestTrue(TEXT("Second potion added"), Inventory->AddItem(OtherItem, 1));
	TestTrue(TEXT("Occupied slots swap"), Inventory->SwapItems(0, 1));
	TestEqual(TEXT("Quick-use follows the swapped item"), Inventory->GetQuickUseSlot(8).InventorySlotIndex, 0);
	TestTrue(TEXT("Quick-use still points to the assigned item"), Inventory->GetQuickUseSlot(8).Item == AssignedItem);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FQuickUseStackMergeTest, "ActionRPG.Inventory.QuickUseStackMerge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FQuickUseStackMergeTest::RunTest(const FString& Parameters)
{
	UInventoryComponent* Inventory = NewObject<UInventoryComponent>(GetTransientPackage());
	UItemDataAsset* Data = NewObject<UItemDataAsset>(GetTransientPackage());
	Data->ItemID = TEXT("QuickMergePotion");
	Data->Type = EItemType::Consumable;
	Data->MaxStackSize = 2;
	Data->Weight = 0.0f;
	UItemBase* Item = NewObject<UItemBase>(GetTransientPackage());
	Item->ItemData = Data;
	TestTrue(TEXT("Potion stack added"), Inventory->AddItem(Item, 2));
	TestTrue(TEXT("Potion stack split"), Inventory->SplitStackToSlot(0, 1, 1));
	TestTrue(TEXT("Source stack assigned to quick-use"), Inventory->AssignItemToQuickUseSlot(1, 8));
	TestTrue(TEXT("Source stack fully merges"), Inventory->MoveItem(1, 0));
	TestEqual(TEXT("Quick-use follows the surviving stack"), Inventory->GetQuickUseSlot(8).InventorySlotIndex, 0);
	TestTrue(TEXT("Quick-use points to the surviving item"), Inventory->GetQuickUseSlot(8).Item == Inventory->GetItemAt(0));
	TestEqual(TEXT("Surviving item quantity matches its slot"), Inventory->GetItemAt(0)->Quantity, 2);
	TestTrue(TEXT("Stack splits again"), Inventory->SplitStackToSlot(0, 1, 1));
	TestTrue(TEXT("Second stack assigned to another quick-use slot"), Inventory->AssignItemToQuickUseSlot(1, 9));
	TestTrue(TEXT("Second stack merges into already bound destination"), Inventory->MoveItem(1, 0));
	TestEqual(TEXT("Destination binding remains"), Inventory->GetQuickUseSlot(8).InventorySlotIndex, 0);
	TestEqual(TEXT("Redundant source binding clears"), Inventory->GetQuickUseSlot(9).InventorySlotIndex, -1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExperiencePoolTest, "ActionRPG.Progression.ExperiencePool",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FExperiencePoolTest::RunTest(const FString& Parameters)
{
	UExperienceComponent* Experience = NewObject<UExperienceComponent>(GetTransientPackage());
	Experience->AddExperience(100.0f);
	TestEqual(TEXT("Granted XP is available"), Experience->GetUnallocatedExperience(), 100.0f);
	TestEqual(TEXT("Total XP tracks grants"), Experience->GetTotalExperience(), 100.0f);
	TestTrue(TEXT("Available XP can be spent"), Experience->SpendExperience(40.0f));
	TestEqual(TEXT("Spending reduces only the available pool"), Experience->GetUnallocatedExperience(), 60.0f);
	TestEqual(TEXT("Spending does not reduce lifetime XP"), Experience->GetTotalExperience(), 100.0f);
	TestFalse(TEXT("Overspending is rejected"), Experience->SpendExperience(61.0f));
	TestFalse(TEXT("Zero spending is rejected"), Experience->SpendExperience(0.0f));
	Experience->AddExperience(std::numeric_limits<float>::infinity());
	TestFalse(TEXT("Non-finite spending is rejected"), Experience->SpendExperience(std::numeric_limits<float>::infinity()));
	TestEqual(TEXT("Invalid XP leaves the pool unchanged"), Experience->GetUnallocatedExperience(), 60.0f);
	UExperienceComponent* OverflowExperience = NewObject<UExperienceComponent>(GetTransientPackage());
	OverflowExperience->AddExperience(std::numeric_limits<float>::max());
	OverflowExperience->AddExperience(std::numeric_limits<float>::max());
	TestEqual(TEXT("Overflowing XP grant is rejected"), OverflowExperience->GetUnallocatedExperience(), std::numeric_limits<float>::max());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FResourceRegenerationTest, "ActionRPG.Progression.ResourceRegeneration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FResourceRegenerationTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 100000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;

	USecondaryAttributeDataAsset* Rules = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula HealthFormula;
	HealthFormula.BaseValue = 100.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxHealth, HealthFormula);
	FSecondaryAttributeFormula StaminaFormula;
	StaminaFormula.BaseValue = 100.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxStamina, StaminaFormula);
	FSecondaryAttributeFormula ManaFormula;
	ManaFormula.BaseValue = 10.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxMana, ManaFormula);
	Rules->HealthRegenerationPerSecond = 10.0f;
	Rules->StaminaRegenerationPerSecond = 20.0f;
	Rules->StaminaRegenerationPerEndurance = 5.0f;
	Rules->ManaRegenerationPerSecond = 2.0f;
	Rules->HealthRegenerationDelay = 0.2f;
	Rules->StaminaRegenerationDelay = 0.2f;
	Rules->ManaRegenerationDelay = 0.2f;

	USecondaryAttributeComponent* Resources = Player->SecondaryAttributeComponent;
	Resources->SecondaryAttributeDataAsset = Rules;
	Resources->RecalculateSecondaryAttributes();
	TestEqual(TEXT("Health starts full"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth), 100.0f);
	Resources->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth, 50.0f);
	Resources->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina, 20.0f);
	Resources->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxMana, 5.0f);
	TestFalse(TEXT("Atomic spend rejects insufficient mana"), Resources->TrySpendResources(6.0f, 10.0f));
	TestEqual(TEXT("Failed atomic spend preserves stamina"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina), 20.0f);
	TestTrue(TEXT("Atomic spend deducts both resources"), Resources->TrySpendResources(2.0f, 3.0f));
	TestEqual(TEXT("Mana cost applied"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxMana), 3.0f);
	TestEqual(TEXT("Stamina cost applied"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina), 17.0f);
	TestFalse(TEXT("Negative cost rejected"), Resources->TrySpendResources(-1.0f, 0.0f));
	TestTrue(TEXT("Resource restoration succeeds"), Resources->RestoreResource(ESecondaryAttribute::MaxMana, 2.0f));
	TestEqual(TEXT("Mana restored"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxMana), 5.0f);
	Resources->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina, 20.0f);
	Resources->RecalculateSecondaryAttributes();
	TestEqual(TEXT("Recalculation preserves current health"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth), 50.0f);
	FTimerManager& Timers = World->GetTimerManager();
	TestTrue(TEXT("Health recovery timer is scheduled"), Timers.TimerExists(*Resources->GetRegenerationTimer(ESecondaryAttribute::MaxHealth)));
	TestTrue(TEXT("Stamina recovery timer is scheduled"), Timers.TimerExists(*Resources->GetRegenerationTimer(ESecondaryAttribute::MaxStamina)));
	TestTrue(TEXT("Mana recovery timer is scheduled"), Timers.TimerExists(*Resources->GetRegenerationTimer(ESecondaryAttribute::MaxMana)));
	Resources->RegenerateResource(ESecondaryAttribute::MaxHealth);
	Resources->RegenerateResource(ESecondaryAttribute::MaxStamina);
	Resources->RegenerateResource(ESecondaryAttribute::MaxMana);
	TestTrue(TEXT("Health regenerates"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth) > 50.0f);
	TestTrue(TEXT("Base Stamina regeneration restores two per tick"),
		FMath::IsNearlyEqual(Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina), 22.0f));
	TestTrue(TEXT("Mana regenerates"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxMana) > 5.0f);
	Player->AttributeComponent->SetAttribute(EPrimaryAttribute::Constitution, 1.0f);
	TestTrue(TEXT("Endurance reward is accepted"),
		Player->AcquiredAttributeComponent->TryGrantAcquiredAttribute(EAcquiredAttribute::Endurance, 1));
	TestTrue(TEXT("Endurance raises Stamina regeneration to 25 per second"),
		FMath::IsNearlyEqual(Resources->GetRegenerationRate(ESecondaryAttribute::MaxStamina), 25.0f));
	Resources->RegenerateResource(ESecondaryAttribute::MaxStamina);
	TestTrue(TEXT("Endurance restores 2.5 Stamina per tick"),
		FMath::IsNearlyEqual(Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina), 24.5f));
	Resources->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth, 0.0f);
	TestFalse(TEXT("Zero health stops passive recovery"), Timers.TimerExists(*Resources->GetRegenerationTimer(ESecondaryAttribute::MaxHealth)));
	Timers.ClearTimer(*Resources->GetRegenerationTimer(ESecondaryAttribute::MaxStamina));
	Timers.ClearTimer(*Resources->GetRegenerationTimer(ESecondaryAttribute::MaxMana));
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPrimaryModifierSourcesTest, "ActionRPG.Progression.PrimaryModifierSources",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPrimaryModifierSourcesTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* PlayerCharacter = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(107000.0f, 107000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawned"), PlayerCharacter)) return false;
	UAttributeComponent* Attributes = PlayerCharacter->AttributeComponent;
	UPrimaryAttributeDataAsset* Rules = NewObject<UPrimaryAttributeDataAsset>(PlayerCharacter);
	Rules->BaseValues.Add(EPrimaryAttribute::Strength, 10.0f);
	Attributes->AttributeDataAsset = Rules;
	Attributes->InitializeFromDataAsset();
	Attributes->AddAttributePoints(2);
	TestTrue(TEXT("Special reward points can be allocated"), Attributes->AllocateAttributePoints(EPrimaryAttribute::Strength, 2));
	TestEqual(TEXT("Allocated points are permanent"), Attributes->GetPersistentAttribute(EPrimaryAttribute::Strength), 12.0f);

	UClassDataAsset* ClassAsset = NewObject<UClassDataAsset>(PlayerCharacter);
	ClassAsset->PrimaryAttributeBonuses.Add(EPrimaryAttribute::Strength, 4.0f);
	ClassAsset->AttributeGainsPerLevel.Add(EPrimaryAttribute::Strength, 3.0f);
	TestTrue(TEXT("Class can be selected"), PlayerCharacter->ClassComponent->SelectClass(ClassAsset));
	TestEqual(TEXT("Selected and earned class gains both affect combat"), Attributes->GetAttribute(EPrimaryAttribute::Strength), 19.0f);
	TestEqual(TEXT("Only earned class gain counts as persistent"), Attributes->GetPersistentAttribute(EPrimaryAttribute::Strength), 15.0f);
	UClassDataAsset* RestrictedClass = NewObject<UClassDataAsset>(PlayerCharacter);
	RestrictedClass->RequiredAttributes.Add(EPrimaryAttribute::Strength, 16.0f);
	TestFalse(TEXT("Temporary class selection cannot satisfy persistent requirement"), PlayerCharacter->ClassComponent->SelectClass(RestrictedClass));

	UItemDataAsset* ItemData = NewObject<UItemDataAsset>(PlayerCharacter);
	ItemData->Type = EItemType::Equipment;
	ItemData->EquipmentSlot = EEquipmentSlot::Ring;
	ItemData->EquipmentPrimaryBonuses.Add(EPrimaryAttribute::Strength, 5.0f);
	UEquipmentItem* Ring = NewObject<UEquipmentItem>(PlayerCharacter);
	Ring->ItemData = ItemData;
	TestTrue(TEXT("Ring equips"), PlayerCharacter->EquipmentComponent->EquipItem(Ring));
	TestEqual(TEXT("Equipment source is applied"), Attributes->GetAttribute(EPrimaryAttribute::Strength), 24.0f);
	TestEqual(TEXT("Equipment is excluded from persistent value"), Attributes->GetPersistentAttribute(EPrimaryAttribute::Strength), 15.0f);
	TestTrue(TEXT("Ring unequips"), PlayerCharacter->EquipmentComponent->UnequipSlot(EEquipmentSlot::Ring) == Ring);
	TestEqual(TEXT("Unequip removes only equipment source"), Attributes->GetAttribute(EPrimaryAttribute::Strength), 19.0f);

	TMap<EPrimaryAttribute, float> TemporaryBonus;
	TemporaryBonus.Add(EPrimaryAttribute::Strength, 2.0f);
	TestTrue(TEXT("Named source applies"), Attributes->SetModifierSource(TEXT("Test.Temporary"), TemporaryBonus, false));
	TemporaryBonus.Add(EPrimaryAttribute::Strength, 5.0f);
	TestTrue(TEXT("Repeated source replaces its magnitude"), Attributes->SetModifierSource(TEXT("Test.Temporary"), TemporaryBonus, false));
	TestEqual(TEXT("Repeated source does not accumulate"), Attributes->GetAttribute(EPrimaryAttribute::Strength), 24.0f);
	TestTrue(TEXT("Named source removes"), Attributes->RemoveModifierSource(TEXT("Test.Temporary")));
	TestTrue(TEXT("Class can be removed"), PlayerCharacter->ClassComponent->RemoveClass(ClassAsset));
	TestEqual(TEXT("Selected class bonus is removed but earned growth remains"), Attributes->GetAttribute(EPrimaryAttribute::Strength), 15.0f);
	TestEqual(TEXT("Earned growth still satisfies persistent reading"), Attributes->GetPersistentAttribute(EPrimaryAttribute::Strength), 15.0f);
	PlayerCharacter->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEquipmentLevelRequirementTest, "ActionRPG.Progression.EquipmentLevelRequirement",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEquipmentLevelRequirementTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(107100.0f, 107100.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	UItemDataAsset* Data = NewObject<UItemDataAsset>(Player);
	Data->Type = EItemType::Equipment;
	Data->EquipmentSlot = EEquipmentSlot::Ring;
	Data->EquipmentRequiredLevel = 2;
	UEquipmentItem* Ring = NewObject<UEquipmentItem>(Player);
	Ring->ItemData = Data;
	TestTrue(TEXT("Ring cannot be dropped onto Head slot"),
		Player->EquipmentComponent->GetEquipFailureReason(Ring, EEquipmentSlot::Head).ToString().Contains(TEXT("Ring slot")));
	TestTrue(TEXT("Level failure explains the requirement"),
		Player->EquipmentComponent->GetEquipFailureReason(Ring).ToString().Contains(TEXT("level 2")));
	TestFalse(TEXT("Level-one player cannot equip level-two item"), Player->EquipmentComponent->EquipItem(Ring));
	TestFalse(TEXT("Rejected item does not occupy its slot"), Player->EquipmentComponent->IsSlotOccupied(EEquipmentSlot::Ring));
	UClassComponent* Classes = Player->ClassComponent;
	Classes->MaxClasses = 2;
	TestTrue(TEXT("First class selected"), Classes->SelectClass(NewObject<UClassDataAsset>(Player)));
	TestTrue(TEXT("Second class selected"), Classes->AddClass(NewObject<UClassDataAsset>(Player)));
	TestEqual(TEXT("Current player level is two"), Classes->GetPlayerLevel(), 2);
	TestTrue(TEXT("Level requirement is now satisfied"), Player->EquipmentComponent->GetEquipFailureReason(Ring).IsEmpty());
	TestTrue(TEXT("Level-two player can equip item"), Player->EquipmentComponent->EquipItem(Ring));
	TestTrue(TEXT("Equipped item occupies its slot"), Player->EquipmentComponent->IsSlotOccupied(EEquipmentSlot::Ring));
	UEquipmentItem* ReplacementRing = NewObject<UEquipmentItem>(Player);
	ReplacementRing->ItemData = Data;
	TestTrue(TEXT("Occupied slot explains why another item cannot be equipped"),
		Player->EquipmentComponent->GetEquipFailureReason(ReplacementRing).ToString().Contains(TEXT("occupied")));
	TestFalse(TEXT("Equipping into an occupied slot is rejected"), Player->EquipmentComponent->EquipItem(ReplacementRing));
	TestTrue(TEXT("Rejected replacement preserves the original item"),
		Player->EquipmentComponent->GetEquippedItem(EEquipmentSlot::Ring) == Ring);
	Data->Weight = Player->InventoryComponent->GetMaxWeight() + 1.0f;
	TestFalse(TEXT("Unequip is rejected when inventory cannot carry the item"),
		Player->InventoryComponent->UnequipItemToInventory(EEquipmentSlot::Ring));
	TestTrue(TEXT("Failed unequip preserves the equipped item"),
		Player->EquipmentComponent->GetEquippedItem(EEquipmentSlot::Ring) == Ring);
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEquipmentItemTooltipTest, "ActionRPG.Progression.EquipmentItemTooltip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEquipmentItemTooltipTest::RunTest(const FString& Parameters)
{
	UItemDataAsset* Data = NewObject<UItemDataAsset>(GetTransientPackage());
	Data->ItemName = FText::FromString(TEXT("Flame Ward Ring"));
	Data->ItemDescription = FText::FromString(TEXT("A warm band"));
	Data->Type = EItemType::Equipment;
	Data->EquipmentRequiredLevel = 2;
	Data->EquipmentRequiredAttributes.Add(EPrimaryAttribute::Wisdom, 3.0f);
	Data->EquipmentDamageResistanceBonuses.Add(EDamageType::Fire, 25.0f);
	Data->EquipmentPrimaryBonuses.Add(EPrimaryAttribute::Wisdom, 2.0f);
	Data->EquipmentBonuses.Add(ESecondaryAttribute::Armor, 5.0f);
	const FString Tooltip = Data->GetItemTooltip().ToString();
	TestTrue(TEXT("Tooltip names the item"), Tooltip.Contains(TEXT("Flame Ward Ring")));
	TestTrue(TEXT("Tooltip shows required level"), Tooltip.Contains(TEXT("Requires player level 2")));
	TestTrue(TEXT("Tooltip shows required attribute"), Tooltip.Contains(TEXT("Wisdom 3")));
	TestTrue(TEXT("Tooltip shows resistance bonus"), Tooltip.Contains(TEXT("Fire Resistance: +25%")));
	TestTrue(TEXT("Tooltip shows primary bonus"), Tooltip.Contains(TEXT("Wisdom: +2")));
	TestTrue(TEXT("Tooltip shows Armor bonus"), Tooltip.Contains(TEXT("Armor: +5")));
	UItemDataAsset* Sword = NewObject<UItemDataAsset>(GetTransientPackage());
	Sword->ItemName = FText::FromString(TEXT("Flame Sword"));
	Sword->Type = EItemType::Equipment;
	Sword->WeaponDamage = 25.0f;
	Sword->WeaponDamageType = EDamageType::Fire;
	TestTrue(TEXT("Weapon tooltip identifies the additive bonus and type"),
		Sword->GetItemTooltip().ToString().Contains(TEXT("Weapon Bonus: +25 Fire")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FStatusEffectTest, "ActionRPG.Progression.StatusEffects",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStatusEffectTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 100000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	USecondaryAttributeDataAsset* Rules = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula Health;
	Health.BaseValue = 100.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxHealth, Health);
	Player->SecondaryAttributeComponent->SecondaryAttributeDataAsset = Rules;
	Player->SecondaryAttributeComponent->RecalculateSecondaryAttributes();
	Player->SecondaryAttributeComponent->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth, 50.0f);
	UStatusEffectDataAsset* Effect = NewObject<UStatusEffectDataAsset>(Player);
	Effect->EffectID = TEXT("TestRegeneration");
	Effect->DurationSeconds = 0.2f;
	Effect->TickIntervalSeconds = 0.1f;
	Effect->HealthPerTick = 5.0f;
	Effect->StackPolicy = EStatusEffectStackPolicy::Stack;
	Effect->MaxStacks = 2;
	Effect->PrimaryAttributeBonuses.Add(EPrimaryAttribute::Strength, 3.0f);
	UStatusEffectComponent* Effects = Player->StatusEffectComponent;
	const float BaseStrength = Player->AttributeComponent->GetAttribute(EPrimaryAttribute::Strength);
	TestTrue(TEXT("Effect applies"), Effects->ApplyEffect(Effect, Player));
	TestEqual(TEXT("First effect stack adds its own primary bonus"), Player->AttributeComponent->GetAttribute(EPrimaryAttribute::Strength), BaseStrength + 3.0f);
	TestEqual(TEXT("Temporary effect does not meet persistent requirements"), Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Strength), BaseStrength);
	TestTrue(TEXT("Effect stacks"), Effects->ApplyEffect(Effect, Player));
	TestEqual(TEXT("Stack cap is respected"), Effects->GetActiveEffects()[0].Stacks, 2);
	TestEqual(TEXT("Second stack replaces the effect source magnitude"), Player->AttributeComponent->GetAttribute(EPrimaryAttribute::Strength), BaseStrength + 6.0f);
	Effects->AdvanceEffects();
	TestEqual(TEXT("Stacked effect restores health"), Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth), 60.0f);
	Effects->AdvanceEffects();
	TestTrue(TEXT("Effect expires"), Effects->GetActiveEffects().IsEmpty());
	TestEqual(TEXT("Expiration removes only the effect's primary bonus"), Player->AttributeComponent->GetAttribute(EPrimaryAttribute::Strength), BaseStrength);
	TestTrue(TEXT("Effect can be reapplied"), Effects->ApplyEffect(Effect, Player));
	Effects->RemoveEffectsOnDeath();
	TestTrue(TEXT("Death removes temporary effects"), Effects->GetActiveEffects().IsEmpty());
	TestEqual(TEXT("Death cleanup removes the effect's primary bonus"), Player->AttributeComponent->GetAttribute(EPrimaryAttribute::Strength), BaseStrength);
	Player->SecondaryAttributeComponent->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth, 5.0f);
	Effect->HealthPerTick = -10.0f;
	TestTrue(TEXT("Lethal periodic effect applies"), Effects->ApplyEffect(Effect, Player));
	Effects->AdvanceEffects();
	TestTrue(TEXT("Lethal periodic damage kills the player"), Player->IsDead());
	TestTrue(TEXT("Death cleanup is safe during effect processing"), Effects->GetActiveEffects().IsEmpty());
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWisdomHealingScalingTest, "ActionRPG.Progression.WisdomHealingScaling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWisdomHealingScalingTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100300.0f, 100300.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	USecondaryAttributeDataAsset* Rules = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula Health;
	Health.BaseValue = 100.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxHealth, Health);
	Player->SecondaryAttributeComponent->SecondaryAttributeDataAsset = Rules;
	Player->SecondaryAttributeComponent->RecalculateSecondaryAttributes();
	Player->SecondaryAttributeComponent->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth, 50.0f);
	UStatusEffectDataAsset* Effect = NewObject<UStatusEffectDataAsset>(Player);
	Effect->EffectID = TEXT("TestWisdomHealing");
	Effect->DurationSeconds = 0.3f;
	Effect->TickIntervalSeconds = 0.1f;
	Effect->HealthPerTick = 3.0f;
	Effect->HealthPerTickPerWisdom = 0.5f;
	UStatusEffectComponent* Effects = Player->StatusEffectComponent;
	TestTrue(TEXT("Wisdom-scaled healing effect applies"), Effects->ApplyEffect(Effect, Player));
	const float InitialTick = Effects->GetEffectiveHealthPerTick(Effects->GetActiveEffects()[0]);
	TMap<EPrimaryAttribute, float> WisdomBonus;
	WisdomBonus.Add(EPrimaryAttribute::Wisdom, 2.0f);
	TestTrue(TEXT("Wisdom bonus applies"), Player->AttributeComponent->SetModifierSource(TEXT("Test.WisdomHealing"), WisdomBonus, true));
	const float BoostedTick = Effects->GetEffectiveHealthPerTick(Effects->GetActiveEffects()[0]);
	TestTrue(TEXT("Two Wisdom points raise each healing tick by one"), FMath::IsNearlyEqual(BoostedTick, InitialTick + 1.0f));
	Effects->AdvanceEffects();
	TestTrue(TEXT("Healing spends the current source Wisdom value"), FMath::IsNearlyEqual(
		Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth), 50.0f + BoostedTick));
	TestTrue(TEXT("Temporary Wisdom bonus removes"), Player->AttributeComponent->RemoveModifierSource(TEXT("Test.WisdomHealing")));
	Effects->AdvanceEffects();
	TestTrue(TEXT("Subsequent healing returns to the lower value"), FMath::IsNearlyEqual(
		Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth), 50.0f + BoostedTick + InitialTick));
	TestTrue(TEXT("Healing effect removes"), Effects->RemoveEffect(Effect->EffectID));
	Effect->HealthPerTick = -3.0f;
	TestTrue(TEXT("Periodic damage effect applies"), Effects->ApplyEffect(Effect, Player));
	TestTrue(TEXT("Wisdom does not amplify periodic damage"), FMath::IsNearlyEqual(
		Effects->GetEffectiveHealthPerTick(Effects->GetActiveEffects()[0]), -3.0f));
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRegenerationSkillTest, "ActionRPG.Progression.RegenerationSkill",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRegenerationSkillTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world exists"), World)) return false;
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(106000.0f, 106000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawns"), Player)) return false;
	USecondaryAttributeDataAsset* Rules = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula Health;
	Health.BaseValue = 100.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxHealth, Health);
	FSecondaryAttributeFormula Mana;
	Mana.BaseValue = 10.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxMana, Mana);
	FSecondaryAttributeFormula Stamina;
	Stamina.BaseValue = 30.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxStamina, Stamina);
	USecondaryAttributeComponent* Resources = Player->SecondaryAttributeComponent;
	Resources->SecondaryAttributeDataAsset = Rules;
	Resources->RecalculateSecondaryAttributes();
	Resources->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth, 50.0f);
	USkillComponent* Skills = NewObject<USkillComponent>(Player);
	Player->AddInstanceComponent(Skills);
	Skills->RegisterComponent();
	UStatusEffectDataAsset* Effect = NewObject<UStatusEffectDataAsset>(Player);
	Effect->EffectID = TEXT("TestSkillRegeneration");
	Effect->DurationSeconds = 2.0f;
	Effect->TickIntervalSeconds = 1.0f;
	Effect->HealthPerTick = 5.0f;
	USkillRegeneration* Skill = NewObject<USkillRegeneration>(Player);
	Skill->SkillData = NewObject<USkillDataAsset>(Skill);
	Skill->SkillData->ManaCost = 8.0f;
	Skill->SkillData->CooldownDuration = 5.0f;
	Skill->EffectDefinition = Effect;
	TestTrue(TEXT("Regeneration skill is granted"), Skills->GrantSkill(Skill));
	TestTrue(TEXT("Regeneration skill activates"), Skills->ActivateSkill(Skill));
	TestEqual(TEXT("Regeneration skill spends mana once"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxMana), 2.0f);
	TestEqual(TEXT("Regeneration effect is active"), Player->StatusEffectComponent->GetActiveEffects().Num(), 1);
	TestFalse(TEXT("Cooldown prevents another cast"), Skills->ActivateSkill(Skill));
	TestEqual(TEXT("Blocked cast does not spend mana"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxMana), 2.0f);
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FClassSelectionTest, "ActionRPG.Progression.ClassSelection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FClassSelectionTest::RunTest(const FString& Parameters)
{
	UClassComponent* Classes = NewObject<UClassComponent>(GetTransientPackage());
	TestEqual(TEXT("No selected paths give combined level zero"), Classes->GetPlayerLevel(), 0);
	UClassDataAsset* Warrior = NewObject<UClassDataAsset>(GetTransientPackage());
	UClassDataAsset* Mage = NewObject<UClassDataAsset>(GetTransientPackage());
	UClassDataAsset* LevelTwoClass = NewObject<UClassDataAsset>(GetTransientPackage());
	LevelTwoClass->RequiredLevel = 2;
	TestFalse(TEXT("Level two class remains locked at combined level zero"), Classes->SelectClass(LevelTwoClass));
	TestEqual(TEXT("Unselected class displays its first XP threshold"), Classes->GetClassExperienceToNextLevel(Mage), 100.0f);
	UClassDataAsset* LockedClass = NewObject<UClassDataAsset>(GetTransientPackage());
	LockedClass->RequiredLevel = 10;
	UProfessionDataAsset* Blacksmith = NewObject<UProfessionDataAsset>(GetTransientPackage());
	UProfessionDataAsset* LockedProfession = NewObject<UProfessionDataAsset>(GetTransientPackage());
	LockedProfession->RequiredLevel = 10;

	TestTrue(TEXT("Initial class can be selected"), Classes->SelectClass(Warrior));
	TestFalse(TEXT("Profession cannot be selected before earning XP"), Classes->SelectProfession(Blacksmith));
	TestFalse(TEXT("Null class selection is rejected"), Classes->SelectClass(nullptr));
	TestFalse(TEXT("Locked class selection is rejected"), Classes->SelectClass(LockedClass));
	TestTrue(TEXT("Rejected class selection preserves current class"), Classes->GetCurrentClass() == Warrior);
	TestEqual(TEXT("Rejected class selection preserves class count"), Classes->GetSelectedClasses().Num(), 1);
	TestFalse(TEXT("Null profession selection is rejected"), Classes->SelectProfession(nullptr));
	TestFalse(TEXT("Locked profession selection is rejected"), Classes->SelectProfession(LockedProfession));
	TestTrue(TEXT("Rejected profession selection leaves current profession empty"), Classes->GetCurrentProfession() == nullptr);
	TestEqual(TEXT("Rejected profession selection leaves profession count empty"), Classes->GetSelectedProfessions().Num(), 0);
	TestTrue(TEXT("Eligible replacement class can be selected"), Classes->SelectClass(Mage));
	TestTrue(TEXT("Replacement becomes current class"), Classes->GetCurrentClass() == Mage);
	TestFalse(TEXT("Previous class is no longer selected"), Classes->GetSelectedClasses().Contains(Warrior));
	Classes->MaxClasses = 2;
	TestTrue(TEXT("Second class fits available slot"), Classes->AddClass(Warrior));
	TestEqual(TEXT("Player level sums two classes"), Classes->GetPlayerLevel(), 2);
	UClassDataAsset* ThirdClass = NewObject<UClassDataAsset>(GetTransientPackage());
	TestFalse(TEXT("Class cap rejects third class"), Classes->AddClass(ThirdClass));
	TestEqual(TEXT("Rejected class leaves level unchanged"), Classes->GetPlayerLevel(), 2);
	Classes->ClearClass();
	TestEqual(TEXT("Removing all classes returns combined level to zero"), Classes->GetPlayerLevel(), 0);
	TestTrue(TEXT("Starter class remains available at combined level zero"), Classes->SelectClass(Warrior));
	TestEqual(TEXT("Reselected class contributes its level"), Classes->GetPlayerLevel(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProfessionExperienceTest, "ActionRPG.Progression.ProfessionExperience",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FProfessionExperienceTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 100000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;

	UProfessionDataAsset* Blacksmith = NewObject<UProfessionDataAsset>(Player);
	UClassDataAsset* Warrior = NewObject<UClassDataAsset>(Player);
	Blacksmith->AttributeGainsPerLevel.Add(EPrimaryAttribute::Dexterity, 2.0f);
	Warrior->AttributeGainsPerLevel.Add(EPrimaryAttribute::Strength, 3.0f);
	Warrior->AttributePointsPerLevel = 5; // Legacy asset setting must not override the tier rate.
	UClassComponent* Classes = Player->ClassComponent;
	UExperienceComponent* Experience = Player->ExperienceComponent;
	const int32 StartingPoints = Player->AttributeComponent->GetUnspentAttributePoints();
	TestFalse(TEXT("Blacksmith cannot be selected without XP"), Classes->SelectProfession(Blacksmith));
	TestFalse(TEXT("Blacksmith cannot be added without XP"), Classes->AddProfession(Blacksmith));
	TestFalse(TEXT("Blacksmith cannot be allocated unavailable XP"), Classes->AllocateExperienceToProfession(Blacksmith, 10.0f));
	Experience->AddExperience(250.0f);
	TestTrue(TEXT("Partial XP can be assigned to locked Blacksmith"), Classes->AllocateExperienceToProfession(Blacksmith, 50.0f));
	TestEqual(TEXT("Locked Blacksmith remains at level zero"), Classes->GetProfessionLevel(Blacksmith), 0);
	TestFalse(TEXT("Partial XP does not unlock Blacksmith"), Classes->GetSelectedProfessions().Contains(Blacksmith));
	TestFalse(TEXT("Direct selection cannot bypass partial XP"), Classes->SelectProfession(Blacksmith));
	TestEqual(TEXT("Partial profession XP is retained"), Classes->GetProfessionExperience(Blacksmith), 50.0f);
	TestTrue(TEXT("Reaching 100 XP unlocks Blacksmith"), Classes->AllocateExperienceToProfession(Blacksmith, 50.0f));
	TestTrue(TEXT("Unlocked Blacksmith is selected"), Classes->GetSelectedProfessions().Contains(Blacksmith));
	TestEqual(TEXT("Selected profession starts at level one"), Classes->GetProfessionLevel(Blacksmith), 1);
	TestEqual(TEXT("Profession alone contributes combined level one"), Classes->GetPlayerLevel(), 1);
	TestEqual(TEXT("First profession level grants fixed Dexterity"), Player->AttributeComponent->GetAttribute(EPrimaryAttribute::Dexterity), 2.0f);
	TestEqual(TEXT("Unlock XP has been consumed"), Classes->GetProfessionExperience(Blacksmith), 0.0f);
	TestEqual(TEXT("First profession threshold"), Classes->GetProfessionExperienceToNextLevel(Blacksmith), 100.0f);
	TestTrue(TEXT("XP can be allocated to Blacksmith"), Classes->AllocateExperienceToProfession(Blacksmith, 125.0f));
	TestEqual(TEXT("Blacksmith advances one level"), Classes->GetProfessionLevel(Blacksmith), 2);
	TestEqual(TEXT("Second profession level grants another fixed Dexterity gain"), Player->AttributeComponent->GetAttribute(EPrimaryAttribute::Dexterity), 4.0f);
	TestEqual(TEXT("Overflow XP remains on profession"), Classes->GetProfessionExperience(Blacksmith), 25.0f);
	TestEqual(TEXT("Next profession threshold increases"), Classes->GetProfessionExperienceToNextLevel(Blacksmith), 125.0f);
	TestEqual(TEXT("Unallocated XP decreases"), Experience->GetUnallocatedExperience(), 25.0f);
	TestEqual(TEXT("Profession awards no attribute points"), Player->AttributeComponent->GetUnspentAttributePoints(), StartingPoints);
	TestFalse(TEXT("Cannot overspend on profession"), Classes->AllocateExperienceToProfession(Blacksmith, 26.0f));
	TestFalse(TEXT("Cannot allocate invalid XP"), Classes->AllocateExperienceToProfession(Blacksmith, std::numeric_limits<float>::infinity()));
	TestTrue(TEXT("Class can be selected too"), Classes->SelectClass(Warrior));
	TestEqual(TEXT("First class level grants fixed Strength"), Player->AttributeComponent->GetAttribute(EPrimaryAttribute::Strength), 3.0f);
	TestEqual(TEXT("Basic class level one grants four free attribute points"), Player->AttributeComponent->GetUnspentAttributePoints(), StartingPoints + 4);
	TestEqual(TEXT("Player level includes profession level"), Classes->GetPlayerLevel(), 3);
	TestTrue(TEXT("Profession can be removed"), Classes->RemoveProfession(Blacksmith));
	TestEqual(TEXT("Earned profession attributes persist when deselected"), Player->AttributeComponent->GetAttribute(EPrimaryAttribute::Dexterity), 4.0f);
	TestEqual(TEXT("Inactive profession no longer contributes level"), Classes->GetPlayerLevel(), 1);
	TestTrue(TEXT("Profession can be selected again"), Classes->SelectProfession(Blacksmith));
	TestEqual(TEXT("Reselecting unlocked profession costs no XP"), Experience->GetUnallocatedExperience(), 25.0f);
	TestEqual(TEXT("Blacksmith level persists across selection"), Classes->GetProfessionLevel(Blacksmith), 2);
	TestEqual(TEXT("Blacksmith XP persists across selection"), Classes->GetProfessionExperience(Blacksmith), 25.0f);
	Experience->AddExperience(75.0f);
	TestTrue(TEXT("Class XP advances Warrior"), Classes->AllocateExperienceToClass(Warrior, 100.0f));
	TestEqual(TEXT("Warrior reaches level two"), Classes->GetClassLevel(Warrior), 2);
	TestEqual(TEXT("Second class level grants another fixed Strength gain"), Player->AttributeComponent->GetAttribute(EPrimaryAttribute::Strength), 6.0f);
	TestEqual(TEXT("Basic class level two grants four more points, independent of legacy setting"), Player->AttributeComponent->GetUnspentAttributePoints(), StartingPoints + 8);
	UClassDataAsset* AdvancedClass = NewObject<UClassDataAsset>(Player);
	AdvancedClass->ClassTier = ECharacterClassTier::Advanced;
	TestTrue(TEXT("Advanced class can be selected"), Classes->SelectClass(AdvancedClass));
	TestEqual(TEXT("Advanced class first level grants five points"), Player->AttributeComponent->GetUnspentAttributePoints(), StartingPoints + 13);
	TestTrue(TEXT("Reselecting the same advanced class is valid"), Classes->SelectClass(AdvancedClass));
	TestEqual(TEXT("Reselection cannot duplicate free points"), Player->AttributeComponent->GetUnspentAttributePoints(), StartingPoints + 13);
	Experience->AddExperience(100.0f);
	TestTrue(TEXT("Advanced class can receive XP"), Classes->AllocateExperienceToClass(AdvancedClass, 100.0f));
	TestEqual(TEXT("Advanced class level two grants five more points"), Player->AttributeComponent->GetUnspentAttributePoints(), StartingPoints + 18);
	UClassDataAsset* SpecializedClass = NewObject<UClassDataAsset>(Player);
	SpecializedClass->ClassTier = ECharacterClassTier::Specialized;
	TestTrue(TEXT("Specialized class can be selected"), Classes->SelectClass(SpecializedClass));
	TestEqual(TEXT("Specialized class first level grants four points"), Player->AttributeComponent->GetUnspentAttributePoints(), StartingPoints + 22);
	TestTrue(TEXT("Previously earned Warrior class can be reselected"), Classes->SelectClass(Warrior));
	TestEqual(TEXT("Returning to a class does not duplicate past level grants"), Player->AttributeComponent->GetUnspentAttributePoints(), StartingPoints + 22);
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSkillAttributeMilestoneTest, "ActionRPG.Progression.SkillAttributeMilestone",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSkillAttributeMilestoneTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 2500.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	USkillComponent* Skills = NewObject<USkillComponent>(Player);
	Skills->RegisterComponent();
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	UAttributeComponent* Attributes = Player->AttributeComponent;
	const float InitialAgility = Attributes->GetAttribute(EPrimaryAttribute::Agility);
	USkillDataAsset* DashData = NewObject<USkillDataAsset>(Player);
	DashData->SkillID = TEXT("MilestoneDash");
	FSkillAttributeMilestone Reward;
	Reward.SkillLevel = 2;
	Reward.Attribute = EPrimaryAttribute::Agility;
	Reward.Amount = 1.0f;
	DashData->AttributeMilestones.Add(Reward);
	USkillBase* Dash = NewObject<USkillBase>(Player);
	Dash->SkillData = DashData;
	TestTrue(TEXT("Milestone skill grants at level one"), Skills->GrantSkill(Dash));
	TestEqual(TEXT("No attribute reward before milestone"), Attributes->GetAttribute(EPrimaryAttribute::Agility), InitialAgility);
	Skills->AddSkillExperience(Dash, 100.0f);
	TestEqual(TEXT("Use-based level two awards permanent Agility"), Attributes->GetAttribute(EPrimaryAttribute::Agility), InitialAgility + 1.0f);
	TestEqual(TEXT("Milestone contributes to persistent requirement value"),
		Attributes->GetPersistentAttribute(EPrimaryAttribute::Agility), InitialAgility + 1.0f);
	TestTrue(TEXT("Skill can be removed"), Skills->RemoveSkill(Dash));
	USkillBase* RegrantedDash = NewObject<USkillBase>(Player);
	RegrantedDash->SkillData = DashData;
	RegrantedDash->SkillLevel = 2;
	TestTrue(TEXT("Skill can be granted again at earned level"), Skills->GrantSkill(RegrantedDash));
	TestEqual(TEXT("Regranting cannot duplicate milestone reward"), Attributes->GetAttribute(EPrimaryAttribute::Agility), InitialAgility + 1.0f);
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FClassSkillMilestoneTest, "ActionRPG.Progression.ClassSkillMilestone",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FClassSkillMilestoneTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 2000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	USkillComponent* Skills = NewObject<USkillComponent>(Player);
	Skills->RegisterComponent();
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();

	UClassDataAsset* Survivalist = NewObject<UClassDataAsset>(Player);
	Survivalist->ClassName = FText::FromString(TEXT("Survivalist"));
	FClassSkillLevelRequirement Requirement;
	Requirement.TargetClassLevel = 2;
	Requirement.SkillID = TEXT("CreateTraps");
	Requirement.RequiredSkillLevel = 2;
	Survivalist->SkillLevelRequirements.Add(Requirement);
	UClassComponent* Classes = Player->ClassComponent;
	const int32 StartingPoints = Player->AttributeComponent->GetUnspentAttributePoints();
	TestTrue(TEXT("Survivalist can be selected"), Classes->SelectClass(Survivalist));
	Player->ExperienceComponent->AddExperience(100.0f);
	TestTrue(TEXT("XP can be assigned before skill requirement is met"), Classes->AllocateExperienceToClass(Survivalist, 100.0f));
	TestEqual(TEXT("Class stays at level one with XP banked"), Classes->GetClassLevel(Survivalist), 1);
	TestEqual(TEXT("Banked XP remains intact"), Classes->GetClassExperience(Survivalist), 100.0f);
	TestEqual(TEXT("Blocked level grants no points"), Player->AttributeComponent->GetUnspentAttributePoints(), StartingPoints + 4);
	FName UnmetSkillID;
	int32 UnmetLevel = 0;
	TestTrue(TEXT("Unmet milestone is exposed for UI"), Classes->GetUnmetClassSkillRequirement(Survivalist, UnmetSkillID, UnmetLevel));
	TestEqual(TEXT("UI receives required skill"), UnmetSkillID, FName(TEXT("CreateTraps")));
	TestEqual(TEXT("UI receives required level"), UnmetLevel, 2);

	USkillBase* Traps = NewObject<USkillBase>(Player);
	Traps->SkillData = NewObject<USkillDataAsset>(Traps);
	Traps->SkillData->SkillID = TEXT("CreateTraps");
	TestTrue(TEXT("Required skill can be granted"), Skills->GrantSkill(Traps));
	TestEqual(TEXT("Merely learning the skill does not advance the class"), Classes->GetClassLevel(Survivalist), 1);
	Skills->AddSkillExperience(Traps, 100.0f);
	TestEqual(TEXT("Using the skill to level two advances the class automatically"), Classes->GetClassLevel(Survivalist), 2);
	TestEqual(TEXT("Class consumes banked XP once"), Classes->GetClassExperience(Survivalist), 0.0f);
	TestEqual(TEXT("Earned level grants its attribute points once"), Player->AttributeComponent->GetUnspentAttributePoints(), StartingPoints + 8);
	TestFalse(TEXT("Milestone is no longer unmet"), Classes->GetUnmetClassSkillRequirement(Survivalist, UnmetSkillID, UnmetLevel));
	UClassDataAsset* Pathfinder = NewObject<UClassDataAsset>(Player);
	FClassSkillLevelRequirement EntryRequirement;
	EntryRequirement.TargetClassLevel = 1;
	EntryRequirement.SkillID = TEXT("Pathfinding");
	EntryRequirement.RequiredSkillLevel = 1;
	Pathfinder->SkillLevelRequirements.Add(EntryRequirement);
	Classes->MaxClasses = 2;
	TestFalse(TEXT("First-level skill gate blocks class selection"), Classes->SelectClass(Pathfinder));
	TestFalse(TEXT("First-level skill gate blocks adding a class"), Classes->AddClass(Pathfinder));
	TestEqual(TEXT("Rejected entry preserves the selected class"), Classes->GetCurrentClass(), Survivalist);
	USkillBase* Pathfinding = NewObject<USkillBase>(Player);
	Pathfinding->SkillData = NewObject<USkillDataAsset>(Pathfinding);
	Pathfinding->SkillData->SkillID = TEXT("Pathfinding");
	TestTrue(TEXT("Entry skill can be learned"), Skills->GrantSkill(Pathfinding));
	TestTrue(TEXT("Entry skill unlocks adding the class"), Classes->AddClass(Pathfinder));
	TestTrue(TEXT("Entry skill unlocks selecting the class"), Classes->SelectClass(Pathfinder));
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FClassGrantedSkillTest, "ActionRPG.Progression.ClassGrantedSkill",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FClassGrantedSkillTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 2200.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	USkillComponent* Skills = NewObject<USkillComponent>(Player);
	Skills->RegisterComponent();
	USkillManagerComponent* Manager = NewObject<USkillManagerComponent>(Player);
	Manager->RegisterComponent();
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	UClassComponent* Classes = Player->ClassComponent;
	if (!TestNotNull(TEXT("Skill manager exists"), Manager) || !TestNotNull(TEXT("Skill component exists"), Skills))
	{
		Player->Destroy();
		return false;
	}
	UClassDataAsset* ClassAsset = NewObject<UClassDataAsset>(Player);
	ClassAsset->ClassName = FText::FromString(TEXT("Test Warrior"));
	USkillDataAsset* SkillData = NewObject<USkillDataAsset>(ClassAsset);
	SkillData->SkillID = TEXT("TEST_CLASS_GRANTED_SKILL");
	SkillData->SkillName = FText::FromString(TEXT("Class Skill"));
	FClassGrantedSkill Grant;
	Grant.TargetClassLevel = 1;
	Grant.SkillClass = USkillBase::StaticClass();
	Grant.SkillData = SkillData;
	ClassAsset->GrantedSkills.Add(Grant);
	const int32 FreeSkillPoints = Manager->GetUnspentSkillPoints();
	TestTrue(TEXT("Class can be selected"), Classes->SelectClass(ClassAsset));
	TestTrue(TEXT("Class selection grants the skill without spending points"), Manager->IsSkillIDUnlocked(SkillData->SkillID));
	TestEqual(TEXT("Free skill point balance is unchanged"), Manager->GetUnspentSkillPoints(), FreeSkillPoints);
	USkillBase* GrantedSkill = nullptr;
	for (USkillBase* Skill : Manager->GetUnlockedSkills())
	{
		if (Skill && Skill->GetSkillID() == SkillData->SkillID) GrantedSkill = Skill;
	}
	if (!TestNotNull(TEXT("Granted skill instance exists"), GrantedSkill))
	{
		Player->Destroy();
		return false;
	}
	Skills->AddSkillExperience(GrantedSkill, 100.0f);
	TestEqual(TEXT("Use levels the class-granted skill"), Skills->GetSkillLevel(GrantedSkill), 2);
	TestTrue(TEXT("Class can be removed"), Classes->RemoveClass(ClassAsset));
	TestFalse(TEXT("Removing the class revokes its skill"), Manager->IsSkillIDUnlocked(SkillData->SkillID));
	TestTrue(TEXT("Class can be reselected"), Classes->AddClass(ClassAsset));
	TestTrue(TEXT("Reselection reuses the earned skill instance"), Manager->IsSkillUnlocked(GrantedSkill));
	TestEqual(TEXT("Reselection preserves use-earned rank"), Skills->GetSkillLevel(GrantedSkill), 2);
	Classes->RemoveClass(ClassAsset);
	USkillBase* IndependentSkill = NewObject<USkillBase>(Player);
	IndependentSkill->SkillData = SkillData;
	TestTrue(TEXT("Independent source can grant the same skill ID"), Manager->UnlockSkill(IndependentSkill));
	TestTrue(TEXT("Class can be selected with overlapping skill source"), Classes->AddClass(ClassAsset));
	TestEqual(TEXT("Overlapping source does not duplicate the known skill"), Manager->GetUnlockedSkills().Num(), 1);
	TestTrue(TEXT("Removing the class leaves the independent skill"), Classes->RemoveClass(ClassAsset));
	TestTrue(TEXT("Independent skill remains unlocked"), Manager->IsSkillUnlocked(IndependentSkill));
	TestTrue(TEXT("Class can be selected again"), Classes->AddClass(ClassAsset));
	TestTrue(TEXT("Independent skill can be revoked"), Manager->RevokeSkill(IndependentSkill));
	TestTrue(TEXT("Selected class restores its own grant when the other source ends"), Manager->IsSkillUnlocked(GrantedSkill));
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAcquiredEnduranceTest, "ActionRPG.Progression.AcquiredEndurance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAcquiredEnduranceTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 2400.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	UAcquiredAttributeComponent* Acquired = Player->AcquiredAttributeComponent;
	UAttributeComponent* Primary = Player->AttributeComponent;
	USecondaryAttributeComponent* Resources = Player->SecondaryAttributeComponent;
	if (!TestNotNull(TEXT("Acquired attributes exist"), Acquired))
	{
		Player->Destroy();
		return false;
	}
	Primary->SetAttribute(EPrimaryAttribute::Constitution, 1.0f);
	USecondaryAttributeDataAsset* Rules = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula Health;
	Health.BaseValue = 100.0f;
	Health.AcquiredAttributeScaling.Add(EAcquiredAttribute::Endurance, 5.0f);
	Rules->Formulas.Add(ESecondaryAttribute::MaxHealth, Health);
	FSecondaryAttributeFormula Stamina;
	Stamina.BaseValue = 100.0f;
	Stamina.AcquiredAttributeScaling.Add(EAcquiredAttribute::Endurance, 2.0f);
	Rules->Formulas.Add(ESecondaryAttribute::MaxStamina, Stamina);
	Resources->SecondaryAttributeDataAsset = Rules;
	Resources->RecalculateSecondaryAttributes();
	Resources->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth, 50.0f);
	Resources->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina, 60.0f);
	TestEqual(TEXT("Endurance starts at zero"), Acquired->GetAcquiredAttribute(EAcquiredAttribute::Endurance), 0);
	TestEqual(TEXT("Persistent Constitution sets the cap"), Acquired->GetAcquiredAttributeCap(EAcquiredAttribute::Endurance), 1);
	TestTrue(TEXT("Special reward grants one Endurance"), Acquired->TryGrantAcquiredAttribute(EAcquiredAttribute::Endurance, 1));
	TestEqual(TEXT("Endurance adds configured maximum Health"), Resources->GetSecondaryAttribute(ESecondaryAttribute::MaxHealth), 105.0f);
	TestEqual(TEXT("Endurance adds configured maximum Stamina"), Resources->GetSecondaryAttribute(ESecondaryAttribute::MaxStamina), 102.0f);
	TestEqual(TEXT("Maximum Health growth does not heal"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth), 50.0f);
	TestEqual(TEXT("Maximum Stamina growth does not refill"), Resources->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina), 60.0f);
	TestFalse(TEXT("Endurance cannot exceed Constitution"), Acquired->TryGrantAcquiredAttribute(EAcquiredAttribute::Endurance, 1));
	Primary->AddAttributePoints(1);
	TestTrue(TEXT("Constitution can be raised"), Primary->AllocateAttributePoints(EPrimaryAttribute::Constitution, 1));
	TestEqual(TEXT("Raising persistent Constitution raises Endurance cap"), Acquired->GetAcquiredAttributeCap(EAcquiredAttribute::Endurance), 2);
	UTrialDataAsset* Trial = NewObject<UTrialDataAsset>(Player);
	Trial->TrialId = TEXT("TEST_ENDURANCE_TRIAL");
	Trial->TrialName = FText::FromString(TEXT("Endurance Trial"));
	Trial->EnduranceReward = 1;
	FTrialObjectiveDefinition Objective;
	Objective.ObjectiveId = TEXT("ENDURANCE_OBJECTIVE");
	Objective.TargetCount = 1;
	Trial->Objectives.Add(Objective);
	TestTrue(TEXT("Trial with Endurance reward can start"), Player->TrialComponent->StartTrial(Trial));
	TestTrue(TEXT("Trial completion grants Endurance"), Player->TrialComponent->AdvanceObjective(Objective.ObjectiveId, 1));
	TestEqual(TEXT("Trial reward respects Constitution cap"), Acquired->GetAcquiredAttribute(EAcquiredAttribute::Endurance), 2);
	TestEqual(TEXT("Trial reward updates Health maximum"), Resources->GetSecondaryAttribute(ESecondaryAttribute::MaxHealth), 110.0f);
	TestEqual(TEXT("Trial reward updates Stamina maximum"), Resources->GetSecondaryAttribute(ESecondaryAttribute::MaxStamina), 104.0f);
	UTrialDataAsset* CappedTrial = NewObject<UTrialDataAsset>(Player);
	CappedTrial->TrialId = TEXT("TEST_CAPPED_ENDURANCE_TRIAL");
	CappedTrial->EnduranceReward = 1;
	CappedTrial->Objectives.Add(Objective);
	TestTrue(TEXT("A capped Endurance trial can start"), Player->TrialComponent->StartTrial(CappedTrial));
	TestFalse(TEXT("Capped reward does not complete or pay early"), Player->TrialComponent->AdvanceObjective(Objective.ObjectiveId, 1));
	TestEqual(TEXT("Blocked trial leaves Endurance unchanged"), Acquired->GetAcquiredAttribute(EAcquiredAttribute::Endurance), 2);
	Primary->AddAttributePoints(1);
	TestTrue(TEXT("Constitution can raise the cap again"), Primary->AllocateAttributePoints(EPrimaryAttribute::Constitution, 1));
	TestTrue(TEXT("Pending trial completes once Endurance fits the cap"), Player->TrialComponent->TryCompleteActiveTrial());
	TestEqual(TEXT("Pending trial grants its one Endurance"), Acquired->GetAcquiredAttribute(EAcquiredAttribute::Endurance), 3);
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FExperiencePickupTest, "ActionRPG.Progression.ExperiencePickup",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FExperiencePickupTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World))
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	AExperiencePickupActor* Pickup = World->SpawnActor<AExperiencePickupActor>(
		FVector(101000.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player) || !TestNotNull(TEXT("XP pickup spawned"), Pickup))
	{
		if (IsValid(Pickup)) Pickup->Destroy();
		if (IsValid(Player)) Player->Destroy();
		return false;
	}

	TestFalse(TEXT("Invalid player cannot collect XP"), Pickup->TryCollect(nullptr));
	TestTrue(TEXT("Player collects XP"), Pickup->TryCollect(Player));
	TestEqual(TEXT("Pickup awards unallocated XP"), Player->ExperienceComponent->GetUnallocatedExperience(), 25.0f);
	TestEqual(TEXT("Pickup awards lifetime XP"), Player->ExperienceComponent->GetTotalExperience(), 25.0f);
	TestFalse(TEXT("Pickup cannot award twice"), Pickup->TryCollect(Player));
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlacksmithCraftingTest, "ActionRPG.Progression.BlacksmithCrafting",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBlacksmithCraftingTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World))
	{
		return false;
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player))
	{
		return false;
	}
	USkillComponent* Skills = NewObject<USkillComponent>(Player);
	USkillManagerComponent* Manager = NewObject<USkillManagerComponent>(Player);
	Player->AddInstanceComponent(Skills);
	Player->AddInstanceComponent(Manager);
	Skills->RegisterComponent();
	Manager->RegisterComponent();

	UItemDataAsset* Ore = NewObject<UItemDataAsset>(GetTransientPackage());
	Ore->ItemID = TEXT("IronOre");
	Ore->MaxStackSize = 99;
	Ore->Weight = 1.0f;
	UItemDataAsset* Sword = NewObject<UItemDataAsset>(GetTransientPackage());
	Sword->ItemID = TEXT("IronSword");
	Sword->MaxStackSize = 1;
	Sword->Weight = 3.0f;
	UCraftingRecipeDataAsset* Recipe = NewObject<UCraftingRecipeDataAsset>(GetTransientPackage());
	FCraftingIngredient Ingredient;
	Ingredient.Item = Ore;
	Ingredient.Quantity = 2;
	Recipe->Ingredients.Add(Ingredient);
	Recipe->OutputItem = Sword;
	Recipe->OutputQuantity = 1;
	UProfessionDataAsset* Blacksmith = NewObject<UProfessionDataAsset>(GetTransientPackage());
	Blacksmith->GrantedSkillClass = USkillBlacksmithCrafting::StaticClass();
	Blacksmith->GrantedSkillData = NewObject<USkillDataAsset>(Blacksmith);
	Blacksmith->Recipes.Add(Recipe);
	Player->ExperienceComponent->AddExperience(100.0f);
	UItemBase* OreItem = NewObject<UItemBase>(Player);
	OreItem->ItemData = Ore;
	TestTrue(TEXT("Ore can be added"), Player->InventoryComponent->AddItem(OreItem, 2));
	TestFalse(TEXT("Recipe is locked before profession selection"), Player->InventoryComponent->CountItemData(Sword) > 0);
	TestTrue(TEXT("XP unlocks Blacksmith"), Player->ClassComponent->AllocateExperienceToProfession(Blacksmith, 100.0f));
	const TArray<USkillBase*> Unlocked = Manager->GetUnlockedSkills();
	TestEqual(TEXT("Blacksmith grants one skill"), Unlocked.Num(), 1);
	USkillBlacksmithCrafting* Crafting = Unlocked.Num() == 1 ? Cast<USkillBlacksmithCrafting>(Unlocked[0]) : nullptr;
	if (!TestNotNull(TEXT("Granted skill is Blacksmith Crafting"), Crafting))
	{
		Player->Destroy();
		return false;
	}
	TestTrue(TEXT("Ingredients enable crafting"), Crafting->CanCraftRecipe(Recipe));
	TestTrue(TEXT("Crafting skill can activate"), Crafting->CanActivate());
	TestTrue(TEXT("Crafting skill can be assigned to the hotbar"), Manager->AssignSkillToSlot(0, Crafting));
	TestTrue(TEXT("Iron Sword can be crafted from the hotbar"), Manager->ActivateSkillFromSlot(0));
	TestEqual(TEXT("Ore is consumed"), Player->InventoryComponent->CountItemData(Ore), 0);
	TestEqual(TEXT("Sword is added"), Player->InventoryComponent->CountItemData(Sword), 1);
	TestFalse(TEXT("Cannot craft without ore"), Crafting->CraftRecipe(Recipe));
	TestTrue(TEXT("Removing Blacksmith succeeds"), Player->ClassComponent->RemoveProfession(Blacksmith));
	TestFalse(TEXT("Profession skill is revoked"), Manager->IsSkillUnlocked(Crafting));
	TestFalse(TEXT("Revoked skill is removed from active skills"), Skills->IsSkillUnlocked(Crafting));
	TestTrue(TEXT("Revoked skill is removed from the hotbar"), Manager->GetSkillAtSlot(0) == nullptr);
	TestFalse(TEXT("Revoked skill cannot craft"), Crafting->CraftRecipe(Recipe));
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlacksmithContentTest, "ActionRPG.Progression.BlacksmithContent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBlacksmithContentTest::RunTest(const FString& Parameters)
{
	const UProfessionDataAsset* Blacksmith = LoadObject<UProfessionDataAsset>(nullptr,
		TEXT("/Game/Data/Professions/DA_Profession_Blacksmith.DA_Profession_Blacksmith"));
	const UCraftingRecipeDataAsset* Recipe = LoadObject<UCraftingRecipeDataAsset>(nullptr,
		TEXT("/Game/Data/Recipes/DA_Recipe_IronSword.DA_Recipe_IronSword"));
	const UItemDataAsset* Ore = LoadObject<UItemDataAsset>(nullptr,
		TEXT("/Game/Data/Items/Data/Materials/DA_Item_IronOre.DA_Item_IronOre"));
	const UItemDataAsset* Sword = LoadObject<UItemDataAsset>(nullptr,
		TEXT("/Game/Data/Items/Data/Equipment/DA_Item_Weapon_Sword.DA_Item_Weapon_Sword"));
	if (!TestNotNull(TEXT("Blacksmith asset"), Blacksmith) || !TestNotNull(TEXT("Recipe asset"), Recipe) ||
		!TestNotNull(TEXT("Iron Ore asset"), Ore) || !TestNotNull(TEXT("Iron Sword asset"), Sword))
	{
		return false;
	}
	TestEqual(TEXT("Blacksmith grants native crafting skill"), Blacksmith->GrantedSkillClass.Get(), USkillBlacksmithCrafting::StaticClass());
	TestTrue(TEXT("Blacksmith has crafting skill data"), Blacksmith->GrantedSkillData != nullptr);
	TestTrue(TEXT("Blacksmith knows the Iron Sword recipe"), Blacksmith->Recipes.Contains(Recipe));
	TestEqual(TEXT("Recipe has one ingredient"), Recipe->Ingredients.Num(), 1);
	if (Recipe->Ingredients.Num() == 1)
	{
		TestTrue(TEXT("Ingredient is Iron Ore"), Recipe->Ingredients[0].Item == Ore);
		TestEqual(TEXT("Recipe consumes two ore"), Recipe->Ingredients[0].Quantity, 2);
	}
	TestTrue(TEXT("Recipe outputs Iron Sword"), Recipe->OutputItem == Sword);
	TestEqual(TEXT("Recipe outputs one sword"), Recipe->OutputQuantity, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSkillPointLearningTest, "ActionRPG.Progression.SkillPointLearning",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSkillPointLearningTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Owner = World->SpawnActor<AActor>(FVector(100000.0f, 1000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Skill owner spawned"), Owner)) return false;

	USkillComponent* Skills = NewObject<USkillComponent>(Owner);
	USkillManagerComponent* Manager = NewObject<USkillManagerComponent>(Owner);
	Owner->AddInstanceComponent(Skills);
	Owner->AddInstanceComponent(Manager);
	Skills->RegisterComponent();
	Manager->RegisterComponent();
	Manager->SkillComponent = Skills;

	USkillDataAsset* LearnData = NewObject<USkillDataAsset>(Owner);
	LearnData->SkillID = TEXT("LearnTest");
	LearnData->SkillName = FText::FromString(TEXT("Learn Test"));
	LearnData->SkillPointCost = 2;
	FSkillCatalogEntry Offer;
	Offer.SkillClass = USkillBase::StaticClass();
	Offer.SkillData = LearnData;
	Manager->SkillCatalog.Add(Offer);
	TestFalse(TEXT("Insufficient points block learning"), Manager->LearnCatalogSkill(0));
	TestTrue(TEXT("Two free skill points granted"), Manager->AddSkillPoints(2));
	LearnData->RequiredLevel = 2;
	TestFalse(TEXT("Player level requirement blocks learning"), Manager->CanLearnCatalogSkill(0));
	LearnData->RequiredLevel = 1;
	LearnData->RequiredAttributes.Add(EPrimaryAttribute::Strength, 999.0f);
	TestFalse(TEXT("Persistent attribute requirement blocks learning"), Manager->CanLearnCatalogSkill(0));
	LearnData->RequiredAttributes.Empty();
	LearnData->RequiredClass = NewObject<UClassDataAsset>(Owner);
	TestFalse(TEXT("Selected class requirement blocks learning"), Manager->CanLearnCatalogSkill(0));
	LearnData->RequiredClass = nullptr;
	LearnData->PrerequisiteSkillIDs.Add(TEXT("StarterTest"));
	TestFalse(TEXT("Prerequisite skill blocks learning"), Manager->CanLearnCatalogSkill(0));
	USkillBase* Starter = NewObject<USkillBase>(Owner);
	Starter->SkillData = NewObject<USkillDataAsset>(Starter);
	Starter->SkillData->SkillID = TEXT("StarterTest");
	TestTrue(TEXT("Starter skill granted without spending points"), Manager->UnlockSkill(Starter));
	TestEqual(TEXT("Grant leaves points intact"), Manager->GetUnspentSkillPoints(), 2);
	TestTrue(TEXT("Requirements now permit learning"), Manager->CanLearnCatalogSkill(0));
	TestTrue(TEXT("Catalog purchase learns skill"), Manager->LearnCatalogSkill(0));
	TestEqual(TEXT("Purchase consumes exactly two points"), Manager->GetUnspentSkillPoints(), 0);
	TestTrue(TEXT("Catalog entry is now learned"), Manager->IsCatalogSkillUnlocked(0));
	TestFalse(TEXT("Duplicate purchase is blocked"), Manager->LearnCatalogSkill(0));
	USkillBase* Learned = Manager->GetUnlockedSkills().Last();
	TestEqual(TEXT("Learned skill starts at use-based level one"), Skills->GetSkillLevel(Learned), 1);
	TestEqual(TEXT("Learning grants no use XP"), Skills->GetSkillExperience(Learned), 0.0f);
	TestTrue(TEXT("Learned skill can activate"), Skills->ActivateSkill(Learned));
	TestEqual(TEXT("Successful use grants skill XP"), Skills->GetSkillExperience(Learned), 10.0f);
	TestEqual(TEXT("Next skill level requires 100 XP"), Skills->GetSkillExperienceRequiredForNextLevel(Learned), 100.0f);
	Skills->AddSkillExperience(Learned, 90.0f);
	TestEqual(TEXT("Using skill XP raises its level"), Skills->GetSkillLevel(Learned), 2);
	TestEqual(TEXT("Level-up carries remaining XP correctly"), Skills->GetSkillExperience(Learned), 0.0f);
	Skills->AddSkillExperience(Learned, std::numeric_limits<float>::infinity());
	Skills->AddSkillExperience(Learned, std::numeric_limits<float>::quiet_NaN());
	TestEqual(TEXT("Nonfinite grants cannot change skill level"), Skills->GetSkillLevel(Learned), 2);
	TestEqual(TEXT("Nonfinite grants cannot change skill XP"), Skills->GetSkillExperience(Learned), 0.0f);
	Skills->ExperienceMultiplier = std::numeric_limits<float>::infinity();
	Skills->AddSkillExperience(Learned, 10.0f);
	TestEqual(TEXT("Nonfinite multiplier cannot change skill XP"), Skills->GetSkillExperience(Learned), 0.0f);
	TestEqual(TEXT("Skill use does not spend free points"), Manager->GetUnspentSkillPoints(), 0);
	Owner->Destroy();
	return true;
}

#endif
