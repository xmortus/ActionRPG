// Copyright Epic Games, Inc. All Rights Reserved.

#include "Items/SkillItem.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Core/SkillDataAsset.h"

void USkillItem::Use()
{
	if (!CanUse())
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::Use - Cannot use skill item"));
		return;
	}

	AActor* OwnerActor = nullptr;
	USkillManagerComponent* SkillManager = FindSkillManager(OwnerActor);
	if (!SkillManager || !OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::Use - SkillManager or OwnerActor missing"));
		return;
	}

	USkillDataAsset* SkillData = nullptr;
	TSubclassOf<USkillBase> SkillClass;
	if (!ResolveSkillDefinition(SkillData, SkillClass))
	{
		return;
	}

	if (IsSkillAlreadyUnlocked(SkillManager, SkillData, SkillClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::Use - Skill already unlocked"));
		return;
	}

	USkillBase* SkillInstance = CreateSkillInstance(OwnerActor, SkillClass, SkillData);
	if (!SkillInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::Use - Failed to create skill instance"));
		return;
	}

	if (!SkillManager->UnlockSkill(SkillInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::Use - UnlockSkill failed"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("SkillItem::Use - Unlocked skill: %s"),
		SkillData ? *SkillData->SkillName.ToString() : TEXT("NULL"));
}

bool USkillItem::CanUse() const
{
	if (!Super::CanUse())
	{
		return false;
	}

	AActor* OwnerActor = nullptr;
	USkillManagerComponent* SkillManager = FindSkillManager(OwnerActor);
	if (!SkillManager || !OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::CanUse - SkillManager or OwnerActor missing"));
		return false;
	}

	USkillDataAsset* SkillData = nullptr;
	TSubclassOf<USkillBase> SkillClass;
	if (!ResolveSkillDefinition(SkillData, SkillClass))
	{
		return false;
	}

	if (IsSkillAlreadyUnlocked(SkillManager, SkillData, SkillClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::CanUse - Skill already unlocked"));
		return false;
	}

	USkillBase* TempSkill = CreateSkillInstance(GetTransientPackage(), SkillClass, SkillData);
	if (!TempSkill)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::CanUse - Failed to create temp skill instance"));
		return false;
	}

	if (!SkillManager->CanUnlockSkill(TempSkill))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::CanUse - Skill prerequisites not met"));
		return false;
	}

	return true;
}

bool USkillItem::ResolveSkillDefinition(USkillDataAsset*& OutSkillData, TSubclassOf<USkillBase>& OutSkillClass) const
{
	OutSkillData = ItemData ? ItemData->SkillData : nullptr;
	OutSkillClass = ItemData ? ItemData->SkillClass : nullptr;

	if (!OutSkillData && !OutSkillClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::ResolveSkillDefinition - ItemData missing SkillData and SkillClass"));
		return false;
	}

	if (!OutSkillClass)
	{
		OutSkillClass = USkillBase::StaticClass();
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::ResolveSkillDefinition - SkillClass missing, using SkillBase"));
	}

	return true;
}

USkillManagerComponent* USkillItem::FindSkillManager(AActor*& OutOwnerActor) const
{
	OutOwnerActor = nullptr;

	UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(GetOuter());
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::FindSkillManager - Outer is not InventoryComponent"));
		return nullptr;
	}

	OutOwnerActor = InventoryComponent->GetOwner();
	if (!OutOwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::FindSkillManager - InventoryComponent has no owner"));
		return nullptr;
	}

	USkillManagerComponent* SkillManager = OutOwnerActor->FindComponentByClass<USkillManagerComponent>();
	if (!SkillManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillItem::FindSkillManager - SkillManagerComponent not found on owner"));
	}

	return SkillManager;
}

bool USkillItem::IsSkillAlreadyUnlocked(USkillManagerComponent* SkillManager, const USkillDataAsset* SkillData, TSubclassOf<USkillBase> SkillClass) const
{
	if (!SkillManager)
	{
		return false;
	}

	const TArray<USkillBase*> UnlockedSkills = SkillManager->GetUnlockedSkills();
	for (USkillBase* Skill : UnlockedSkills)
	{
		if (!Skill)
		{
			continue;
		}

		if (SkillData && Skill->SkillData && Skill->SkillData->SkillID == SkillData->SkillID)
		{
			return true;
		}

		if (SkillClass && Skill->GetClass() == SkillClass)
		{
			return true;
		}
	}

	return false;
}

USkillBase* USkillItem::CreateSkillInstance(UObject* Outer, TSubclassOf<USkillBase> SkillClass, USkillDataAsset* SkillData) const
{
	if (!Outer || !SkillClass)
	{
		return nullptr;
	}

	USkillBase* NewSkill = NewObject<USkillBase>(Outer, SkillClass);
	if (!NewSkill)
	{
		return nullptr;
	}

	if (SkillData)
	{
		NewSkill->SkillData = SkillData;
	}

	return NewSkill;
}
