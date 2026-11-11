// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Progression/QuestComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Quests/QuestDataAsset.h"
#include "GameFramework/Actor.h"

UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();
	Inventory = GetOwner() ? GetOwner()->FindComponentByClass<UInventoryComponent>() : nullptr;
	if (Inventory) Inventory->OnInventoryChanged.AddUniqueDynamic(this, &UQuestComponent::HandleInventoryChanged);
}

bool UQuestComponent::StartQuest(UQuestDataAsset* Quest)
{
	if (!Quest || ActiveQuest || CompletedQuests.Contains(Quest) || Quest->QuestID.IsNone()
		|| !Quest->RequiredItem || Quest->RequiredItemCount < 1 || !Quest->RewardItem
		|| Quest->RewardItem->MaxStackSize < 1 || Quest->OptionalEnemyDefeatCount < 0
		|| (Quest->OptionalEnemyDefeatCount > 0 && Quest->OptionalEnemyID.IsNone())
		|| (Quest->OptionalSkillPointReward > 0 && Quest->OptionalEnemyDefeatCount < 1)
		|| Quest->OptionalSkillPointReward < 0 || !FMath::IsFinite(Quest->ExperienceReward)
		|| Quest->ExperienceReward < 0.0f || !GetOwner()) return false;
	Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>();
	if (!Inventory || !GetOwner()->FindComponentByClass<UExperienceComponent>()
		|| !GetOwner()->FindComponentByClass<USkillManagerComponent>()) return false;
	ActiveQuest = Quest;
	OptionalDefeats = 0;
	LastNotice = FText::GetEmpty();
	CreditedEnemies.Reset();
	OnQuestChanged.Broadcast(Quest);
	return true;
}

int32 UQuestComponent::GetRequiredItemCount() const
{
	return ActiveQuest && Inventory ? Inventory->CountItemData(ActiveQuest->RequiredItem) : 0;
}

bool UQuestComponent::IsQuestCompleted(const UQuestDataAsset* Quest) const
{
	return Quest && CompletedQuests.Contains(Quest);
}

bool UQuestComponent::IsQuestIDCompleted(FName QuestID) const
{
	if (QuestID.IsNone()) return false;
	return CompletedQuests.ContainsByPredicate([QuestID](const TObjectPtr<UQuestDataAsset>& Quest)
	{
		return Quest && Quest->QuestID == QuestID;
	});
}

bool UQuestComponent::ReportEnemyDefeated(FName EnemyID, AActor* Enemy)
{
	if (!ActiveQuest || !IsValid(Enemy) || EnemyID != ActiveQuest->OptionalEnemyID
		|| ActiveQuest->OptionalEnemyDefeatCount < 1 || OptionalDefeats >= ActiveQuest->OptionalEnemyDefeatCount
		|| CreditedEnemies.Contains(Enemy)) return false;
	CreditedEnemies.Add(Enemy);
	++OptionalDefeats;
	OnQuestChanged.Broadcast(ActiveQuest);
	return true;
}

bool UQuestComponent::CanPayReward() const
{
	if (!ActiveQuest || !Inventory || GetRequiredItemCount() < ActiveQuest->RequiredItemCount) return false;
	const AActor* Owner = GetOwner();
	const UExperienceComponent* XP = Owner ? Owner->FindComponentByClass<UExperienceComponent>() : nullptr;
	const USkillManagerComponent* Skills = Owner ? Owner->FindComponentByClass<USkillManagerComponent>() : nullptr;
	if (!XP || !Skills) return false;
	UItemBase* Reward = NewObject<UItemBase>(GetTransientPackage(),
		ActiveQuest->RewardItem->ItemClass ? ActiveQuest->RewardItem->ItemClass.Get() : UItemBase::StaticClass());
	if (!Reward) return false;
	Reward->ItemData = ActiveQuest->RewardItem;
	const int32 SkillPoints = OptionalDefeats >= ActiveQuest->OptionalEnemyDefeatCount
		? ActiveQuest->OptionalSkillPointReward : 0;
	return Inventory->HasSpaceFor(Reward, 1)
		&& FMath::IsFinite(XP->GetTotalExperience() + ActiveQuest->ExperienceReward)
		&& FMath::IsFinite(XP->GetUnallocatedExperience() + ActiveQuest->ExperienceReward)
		&& SkillPoints <= MAX_int32 - Skills->GetUnspentSkillPoints();
}

bool UQuestComponent::TryTurnInQuest()
{
	if (!ActiveQuest) return false;
	if (GetRequiredItemCount() < ActiveQuest->RequiredItemCount)
	{
		LastNotice = FText::FromString(TEXT("Collect the required items before returning."));
		OnQuestChanged.Broadcast(ActiveQuest);
		return false;
	}
	if (!CanPayReward())
	{
		LastNotice = FText::FromString(TEXT("Quest reward blocked. Make room for the reward and return."));
		OnQuestChanged.Broadcast(ActiveQuest);
		return false;
	}
	UQuestDataAsset* Quest = ActiveQuest;
	UItemBase* Reward = NewObject<UItemBase>(this,
		Quest->RewardItem->ItemClass ? Quest->RewardItem->ItemClass.Get() : UItemBase::StaticClass());
	if (!Reward) return false;
	Reward->ItemData = Quest->RewardItem;
	if (!Inventory->AddItem(Reward, 1))
	{
		LastNotice = FText::FromString(TEXT("Quest reward blocked. Make room for the reward and return."));
		OnQuestChanged.Broadcast(ActiveQuest);
		return false;
	}
	UExperienceComponent* XP = GetOwner()->FindComponentByClass<UExperienceComponent>();
	USkillManagerComponent* Skills = GetOwner()->FindComponentByClass<USkillManagerComponent>();
	if (Quest->ExperienceReward > 0.0f) XP->AddExperience(Quest->ExperienceReward);
	if (OptionalDefeats >= Quest->OptionalEnemyDefeatCount && Quest->OptionalSkillPointReward > 0)
	{
		Skills->AddSkillPoints(Quest->OptionalSkillPointReward);
	}
	CompletedQuests.Add(Quest);
	ActiveQuest = nullptr;
	LastNotice = FText::GetEmpty();
	CreditedEnemies.Reset();
	OnQuestCompleted.Broadcast(Quest);
	OnQuestChanged.Broadcast(nullptr);
	return true;
}

void UQuestComponent::HandleInventoryChanged(int32 SlotIndex, UItemBase* Item)
{
	if (ActiveQuest) OnQuestChanged.Broadcast(ActiveQuest);
}
