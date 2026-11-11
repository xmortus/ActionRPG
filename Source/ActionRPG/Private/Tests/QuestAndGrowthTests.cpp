// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Components/Progression/QuestComponent.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Engine/World.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Equipment/WeaponItem.h"
#include "Quests/QuestDataAsset.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FQuestAndGrowingSwordTest, "ActionRPG.Quests.GrowingSword",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FQuestAndGrowingSwordTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 6000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	USkillManagerComponent* SkillManager = NewObject<USkillManagerComponent>(Player);
	Player->AddInstanceComponent(SkillManager);
	SkillManager->RegisterComponent();
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	UItemDataAsset* Ore = NewObject<UItemDataAsset>(Player);
	Ore->ItemID = TEXT("TEST_QUEST_ORE");
	Ore->ItemName = FText::FromString(TEXT("Quest Ore"));
	Ore->MaxStackSize = 99;
	Ore->Weight = 1.0f;
	UItemDataAsset* Sword = NewObject<UItemDataAsset>(Player);
	Sword->ItemID = TEXT("TEST_TRIALBOUND_SWORD");
	Sword->ItemName = FText::FromString(TEXT("Trialbound Sword"));
	Sword->Type = EItemType::Equipment;
	Sword->EquipmentSlot = EEquipmentSlot::MainHand;
	Sword->ItemClass = UWeaponItem::StaticClass();
	Sword->MaxStackSize = 1;
	Sword->Weight = 3.0f;
	Sword->WeaponDamage = 10.0f;
	Sword->bSoulboundGrowth = true;
	Sword->DefeatsPerGrowthRank = 3;
	Sword->MaxGrowthRank = 2;
	Sword->DamagePerGrowthRank = 2.0f;
	UQuestDataAsset* Quest = NewObject<UQuestDataAsset>(Player);
	Quest->QuestID = TEXT("TEST_ORE_QUEST");
	Quest->QuestName = FText::FromString(TEXT("Supply the Forge"));
	Quest->RequiredItem = Ore;
	Quest->RequiredItemCount = 2;
	Quest->OptionalEnemyID = TEXT("TRAINING_ENEMY");
	Quest->OptionalEnemyDefeatCount = 1;
	Quest->ExperienceReward = 40.0f;
	Quest->RewardItem = Sword;
	Quest->OptionalSkillPointReward = 1;
	UQuestComponent* Quests = Player->QuestComponent;
	const int32 StartingSkillPoints = SkillManager->GetUnspentSkillPoints();
	TestTrue(TEXT("Quest starts without starting a Trial"), Quests->StartQuest(Quest));
	TestFalse(TEXT("Cannot turn in without ore"), Quests->TryTurnInQuest());
	TestTrue(TEXT("Missing-item turn-in has visible feedback"), Quests->GetLastNotice().ToString().Contains(TEXT("required items")));
	TestEqual(TEXT("Blocked turn-in pays no XP"), Player->ExperienceComponent->GetTotalExperience(), 0.0f);
	UItemBase* OreItem = NewObject<UItemBase>(Player);
	OreItem->ItemData = Ore;
	TestTrue(TEXT("Two required ore enter inventory"), Player->InventoryComponent->AddItem(OreItem, 2));
	TestEqual(TEXT("Quest sees existing inventory quantity"), Quests->GetRequiredItemCount(), 2);
	AActor* Enemy = World->SpawnActor<AActor>(FVector(100000.0f, 6500.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Test enemy spawned"), Enemy)) { Player->Destroy(); return false; }
	TestFalse(TEXT("Unrelated enemy does not advance optional objective"), Quests->ReportEnemyDefeated(TEXT("OTHER"), Enemy));
	TestTrue(TEXT("Matching enemy advances optional objective"), Quests->ReportEnemyDefeated(TEXT("TRAINING_ENEMY"), Enemy));
	TestFalse(TEXT("Same enemy cannot pay twice"), Quests->ReportEnemyDefeated(TEXT("TRAINING_ENEMY"), Enemy));
	TestTrue(TEXT("Required and optional rewards pay at turn-in"), Quests->TryTurnInQuest());
	TestEqual(TEXT("Quest XP paid once"), Player->ExperienceComponent->GetTotalExperience(), 40.0f);
	TestEqual(TEXT("Optional skill point paid"), SkillManager->GetUnspentSkillPoints(), StartingSkillPoints + 1);
	TestTrue(TEXT("Quest recorded independently"), Quests->IsQuestCompleted(Quest));
	TestFalse(TEXT("Completed quest cannot be claimed again"), Quests->TryTurnInQuest());
	TestFalse(TEXT("Completed quest cannot restart"), Quests->StartQuest(Quest));
	const int32 SwordSlot = Player->InventoryComponent->FindItemSlot(Sword->ItemID);
	TestTrue(TEXT("Reward sword appears in inventory"), SwordSlot != INDEX_NONE);
	UWeaponItem* RewardSword = Cast<UWeaponItem>(Player->InventoryComponent->GetItemAt(SwordSlot));
	if (TestNotNull(TEXT("Reward uses weapon item instance"), RewardSword))
	{
		TestEqual(TEXT("Sword begins at base damage"), RewardSword->GetWeaponDamage(), 10.0f);
		TestFalse(TEXT("Soulbound sword cannot be dropped"), Player->InventoryComponent->DropItemToWorld(SwordSlot, 1, Player->GetActorLocation()));
		TestTrue(TEXT("Sword equips"), Player->InventoryComponent->EquipItemFromSlot(SwordSlot));
		int32 GrowthNotifications = 0;
		const FDelegateHandle GrowthHandle = Player->EquipmentComponent->OnEquipmentChangedNative.AddLambda(
			[&GrowthNotifications](EEquipmentSlot ChangedSlot, UEquipmentItem*)
			{
				if (ChangedSlot == EEquipmentSlot::MainHand) ++GrowthNotifications;
			});
		Player->EquipmentComponent->ReportCreditedEnemyDefeat();
		Player->EquipmentComponent->ReportCreditedEnemyDefeat();
		TestEqual(TEXT("Two defeats are below threshold"), RewardSword->GetGrowthRank(), 0);
		TestEqual(TEXT("Each partial defeat notifies growth UI"), GrowthNotifications, 2);
		Player->EquipmentComponent->ReportCreditedEnemyDefeat();
		TestEqual(TEXT("Third defeat earns one rank"), RewardSword->GetGrowthRank(), 1);
		TestEqual(TEXT("Rank-up notifies growth UI"), GrowthNotifications, 3);
		TestEqual(TEXT("Growth raises instance weapon damage"), RewardSword->GetWeaponDamage(), 12.0f);
		TestTrue(TEXT("Tooltip exposes growth"), RewardSword->GetItemTooltip().ToString().Contains(TEXT("Soulbound rank: 1 / 2")));
		for (int32 Index = 0; Index < 5; ++Index) Player->EquipmentComponent->ReportCreditedEnemyDefeat();
		TestEqual(TEXT("Second rank reached"), RewardSword->GetGrowthRank(), 2);
		TestEqual(TEXT("Growth caps at configured maximum"), RewardSword->GetWeaponDamage(), 14.0f);
		TestEqual(TEXT("Capped defeats do not notify growth UI"), GrowthNotifications, 6);
		Player->EquipmentComponent->OnEquipmentChangedNative.Remove(GrowthHandle);
		TestTrue(TEXT("Soulbound sword can be unequipped"), Player->InventoryComponent->UnequipItemToInventory(EEquipmentSlot::MainHand));
		const int32 ReturnedSlot = Player->InventoryComponent->FindItemSlot(Sword->ItemID);
		TestEqual(TEXT("Unequip preserves the exact sword instance"), Player->InventoryComponent->GetItemAt(ReturnedSlot), static_cast<UItemBase*>(RewardSword));
		TestEqual(TEXT("Unequip preserves earned rank"), RewardSword->GetGrowthRank(), 2);
		TestTrue(TEXT("Grown sword can be equipped again"), Player->InventoryComponent->EquipItemFromSlot(ReturnedSlot));
		TestEqual(TEXT("Re-equipped sword keeps earned damage"), RewardSword->GetWeaponDamage(), 14.0f);
	}
	UQuestDataAsset* BlockedQuest = NewObject<UQuestDataAsset>(Player);
	BlockedQuest->QuestID = TEXT("TEST_BLOCKED_REWARD");
	BlockedQuest->RequiredItem = Ore;
	BlockedQuest->RequiredItemCount = 2;
	UItemDataAsset* HeavyItem = NewObject<UItemDataAsset>(BlockedQuest);
	HeavyItem->ItemID = TEXT("TEST_HEAVY_REWARD");
	HeavyItem->MaxStackSize = 1;
	HeavyItem->Weight = Player->InventoryComponent->GetMaxWeight() + 1.0f;
	BlockedQuest->RewardItem = HeavyItem;
	BlockedQuest->ExperienceReward = 25.0f;
	TestTrue(TEXT("Another quest can start after completion"), Quests->StartQuest(BlockedQuest));
	TestFalse(TEXT("Overweight reward stays pending"), Quests->TryTurnInQuest());
	TestEqual(TEXT("Blocked quest pays no XP"), Player->ExperienceComponent->GetTotalExperience(), 40.0f);
	TestEqual(TEXT("Blocked quest stays active"), Quests->GetActiveQuest(), BlockedQuest);
	TestTrue(TEXT("Blocked reward has visible feedback"), Quests->GetLastNotice().ToString().Contains(TEXT("Make room")));
	HeavyItem->Weight = 1.0f;
	TestTrue(TEXT("Pending reward succeeds after capacity is available"), Quests->TryTurnInQuest());
	TestEqual(TEXT("Second quest XP pays once"), Player->ExperienceComponent->GetTotalExperience(), 65.0f);
	Enemy->Destroy();
	Player->Destroy();
	return true;
}

#endif
