// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Progression/TrialComponent.h"

#include "Components/Progression/ClassComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/AcquiredAttributeComponent.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Skills/SkillComponent.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "GameFramework/Actor.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Trials/TrialDataAsset.h"

namespace
{
bool CanReceiveItemReward(const UInventoryComponent* Inventory, const UItemDataAsset* Item, int32 Quantity)
{
	if (!Inventory || !Item || Quantity < 1 || Item->MaxStackSize < 1
		|| !FMath::IsFinite(Item->Weight) || Item->Weight < 0.0f)
	{
		return false;
	}
	const double NewWeight = static_cast<double>(Inventory->GetCurrentWeight())
		+ static_cast<double>(Item->Weight) * Quantity;
	if (!FMath::IsFinite(NewWeight) || NewWeight > Inventory->GetMaxWeight())
	{
		return false;
	}
	int64 Available = static_cast<int64>(Inventory->GetEmptySlotCount()) * Item->MaxStackSize;
	for (const FInventorySlot& Slot : Inventory->GetInventorySlots())
	{
		if (!Slot.bIsEmpty && Slot.Item && Slot.Item->ItemData
			&& Slot.Item->ItemData->ItemID == Item->ItemID)
		{
			Available += FMath::Max(0, Slot.Item->ItemData->MaxStackSize - Slot.Quantity);
		}
	}
	return Available >= Quantity;
}
}

UTrialComponent::UTrialComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

const FTrialRecord* UTrialComponent::FindRecord(const UTrialDataAsset* Trial) const
{
	return TrialRecords.FindByPredicate([Trial](const FTrialRecord& Record) { return Record.Trial == Trial; });
}

FTrialRecord* UTrialComponent::FindRecord(UTrialDataAsset* Trial)
{
	return TrialRecords.FindByPredicate([Trial](const FTrialRecord& Record) { return Record.Trial == Trial; });
}

bool UTrialComponent::CanStartTrial(const UTrialDataAsset* Trial) const
{
	if (!Trial || ActiveTrial || Trial->TrialId.IsNone() || Trial->Objectives.IsEmpty()
		|| Trial->RequiredPlayerLevel < 1)
	{
		return false;
	}
	if (Trial->RewardChoices.IsEmpty() && (!FMath::IsFinite(Trial->ExperienceReward)
		|| Trial->ExperienceReward < 0.0f || Trial->AttributePointReward < 0
		|| Trial->SkillPointReward < 0 || Trial->EnduranceReward < 0
		|| (Trial->ItemReward.Item && (Trial->ItemReward.Item->ItemID.IsNone()
			|| Trial->ItemReward.Quantity < 1 || Trial->ItemReward.Item->MaxStackSize < 1
			|| !FMath::IsFinite(Trial->ItemReward.Item->Weight)
			|| Trial->ItemReward.Item->Weight < 0.0f))
		|| (static_cast<bool>(Trial->SkillReward.SkillClass)
			!= static_cast<bool>(Trial->SkillReward.SkillData))
		|| (Trial->SkillReward.SkillData && Trial->SkillReward.SkillData->SkillID.IsNone())))
	{
		return false;
	}

	TSet<FName> SeenObjectives;
	for (const FTrialObjectiveDefinition& Objective : Trial->Objectives)
	{
		if (Objective.ObjectiveId.IsNone() || Objective.TargetCount < 1
			|| SeenObjectives.Contains(Objective.ObjectiveId))
		{
			return false;
		}
		SeenObjectives.Add(Objective.ObjectiveId);
	}
	if (!Trial->RewardChoices.IsEmpty())
	{
		if (Trial->RewardChoices.Num() != 2) return false;
		const AActor* Owner = GetOwner();
		for (const FTrialRewardChoice& Choice : Trial->RewardChoices)
		{
			if (Choice.ChoiceName.IsEmpty() || !FMath::IsFinite(Choice.ExperienceReward)
				|| Choice.ExperienceReward < 0.0f || Choice.AttributePointReward < 0 || Choice.SkillPointReward < 0
				|| Choice.EnduranceReward < 0
				|| (Choice.ItemReward.Item && (Choice.ItemReward.Item->ItemID.IsNone()
					|| Choice.ItemReward.Quantity < 1 || Choice.ItemReward.Item->MaxStackSize < 1
					|| !FMath::IsFinite(Choice.ItemReward.Item->Weight) || Choice.ItemReward.Item->Weight < 0.0f))
				|| (static_cast<bool>(Choice.SkillReward.SkillClass) != static_cast<bool>(Choice.SkillReward.SkillData))
				|| (Choice.SkillReward.SkillData && Choice.SkillReward.SkillData->SkillID.IsNone())
				|| (Choice.ExperienceReward > 0.0f && (!Owner || !Owner->FindComponentByClass<UExperienceComponent>()))
				|| (Choice.ItemReward.Item && (!Owner || !Owner->FindComponentByClass<UInventoryComponent>()))
				|| (Choice.SkillReward.SkillClass && (!Owner || !Owner->FindComponentByClass<USkillManagerComponent>()
					|| !Owner->FindComponentByClass<USkillComponent>()))
				|| (Choice.AttributePointReward > 0 && (!Owner || !Owner->FindComponentByClass<UAttributeComponent>()))
				|| (Choice.SkillPointReward > 0 && (!Owner || !Owner->FindComponentByClass<USkillManagerComponent>()))
				|| (Choice.EnduranceReward > 0 && (!Owner || !Owner->FindComponentByClass<UAcquiredAttributeComponent>())))
			{
				return false;
			}
		}
	}

	if (const FTrialRecord* Existing = FindRecord(Trial))
	{
		if (Existing->CompletionCount > 0 && !Trial->bRepeatable)
		{
			return false;
		}
	}

	for (const UTrialDataAsset* Required : Trial->RequiredTrials)
	{
		if (!Required || GetCompletionCount(Required) < 1)
		{
			return false;
		}
	}

	if (Trial->RequiredPlayerLevel > 1)
	{
		const AActor* Owner = GetOwner();
		const UClassComponent* Classes = Owner ? Owner->FindComponentByClass<UClassComponent>() : nullptr;
		if (!Classes || Classes->GetPlayerLevel() < Trial->RequiredPlayerLevel)
		{
			return false;
		}
	}
	if (Trial->RewardChoices.IsEmpty() && Trial->ExperienceReward > 0.0f)
	{
		const AActor* Owner = GetOwner();
		if (!Owner || !Owner->FindComponentByClass<UExperienceComponent>())
		{
			return false;
		}
	}
	const AActor* Owner = GetOwner();
	if (Trial->RewardChoices.IsEmpty() && Trial->ItemReward.Item && (!Owner || !Owner->FindComponentByClass<UInventoryComponent>()))
	{
		return false;
	}
	if (Trial->RewardChoices.IsEmpty() && Trial->SkillReward.SkillClass && (!Owner
		|| !Owner->FindComponentByClass<USkillManagerComponent>()
		|| !Owner->FindComponentByClass<USkillComponent>()))
	{
		return false;
	}
	if (Trial->RewardChoices.IsEmpty() && Trial->AttributePointReward > 0 && (!Owner || !Owner->FindComponentByClass<UAttributeComponent>()))
	{
		return false;
	}
	if (Trial->RewardChoices.IsEmpty() && Trial->SkillPointReward > 0 && (!Owner || !Owner->FindComponentByClass<USkillManagerComponent>()))
	{
		return false;
	}
	if (Trial->RewardChoices.IsEmpty() && Trial->EnduranceReward > 0
		&& (!Owner || !Owner->FindComponentByClass<UAcquiredAttributeComponent>()))
	{
		return false;
	}

	return true;
}

bool UTrialComponent::StartTrial(UTrialDataAsset* Trial)
{
	if (!CanStartTrial(Trial))
	{
		return false;
	}

	FTrialRecord* Record = FindRecord(Trial);
	if (!Record)
	{
		Record = &TrialRecords.AddDefaulted_GetRef();
		Record->Trial = Trial;
	}
	Record->ObjectiveCounts.Init(0, Trial->Objectives.Num());
	Record->bActive = true;
	ActiveTrial = Trial;
	ActiveRewardChoiceIndex = INDEX_NONE;
	CreditedEnemies.Reset();
	OnTrialStarted.Broadcast(Trial);
	OnTrialProgressChanged.Broadcast(Trial, 0.0f);
	return true;
}

bool UTrialComponent::AdvanceObjective(FName ObjectiveId, int32 Amount)
{
	if (!ActiveTrial || ObjectiveId.IsNone() || Amount < 1)
	{
		return false;
	}

	FTrialRecord* Record = FindRecord(ActiveTrial);
	if (!Record || !Record->bActive || Record->ObjectiveCounts.Num() != ActiveTrial->Objectives.Num())
	{
		return false;
	}

	for (int32 Index = 0; Index < ActiveTrial->Objectives.Num(); ++Index)
	{
		const FTrialObjectiveDefinition& Objective = ActiveTrial->Objectives[Index];
		if (Objective.ObjectiveId != ObjectiveId)
		{
			continue;
		}
		const int32 Previous = Record->ObjectiveCounts[Index];
		Record->ObjectiveCounts[Index] = Previous + FMath::Min(Amount, Objective.TargetCount - Previous);
		if (Record->ObjectiveCounts[Index] == Previous)
		{
			return false;
		}

		const UTrialDataAsset* Trial = ActiveTrial;
		const float Progress = GetTrialProgress(Trial);
		OnTrialProgressChanged.Broadcast(ActiveTrial, Progress);
		if (Progress >= 1.0f)
		{
			const bool bClaimed = TryCompleteActiveTrial();
			return bClaimed || (ActiveTrial && !ActiveTrial->RewardChoices.IsEmpty()
				&& ActiveRewardChoiceIndex == INDEX_NONE);
		}
		return true;
	}
	return false;
}

bool UTrialComponent::ReportSkillActivated(FName SkillId)
{
	if (SkillId.IsNone() || !ActiveTrial || ActiveTrial->TrialType != ETrialType::Skill)
	{
		return false;
	}
	return AdvanceObjective(SkillId);
}

bool UTrialComponent::ReportEnemyDefeated(FName EnemyId, AActor* DefeatedEnemy)
{
	if (EnemyId.IsNone() || !IsValid(DefeatedEnemy) || !ActiveTrial
		|| ActiveTrial->TrialType != ETrialType::Combat
		|| CreditedEnemies.Contains(TWeakObjectPtr<AActor>(DefeatedEnemy)))
	{
		return false;
	}
	if (!AdvanceObjective(EnemyId))
	{
		return false;
	}
	CreditedEnemies.Add(TWeakObjectPtr<AActor>(DefeatedEnemy));
	return true;
}

bool UTrialComponent::ReportRecipeCrafted(FName RecipeId)
{
	if (RecipeId.IsNone() || !ActiveTrial || ActiveTrial->TrialType != ETrialType::Crafting)
	{
		return false;
	}
	return AdvanceObjective(RecipeId);
}

bool UTrialComponent::TryCompleteActiveTrial()
{
	if (!ActiveTrial)
	{
		return false;
	}
	UTrialDataAsset* CompletedTrial = ActiveTrial;
	FTrialRecord* Record = FindRecord(CompletedTrial);
	if (!Record || GetTrialProgress(CompletedTrial) < 1.0f)
	{
		return false;
	}
	if (!CompletedTrial->RewardChoices.IsEmpty() && !CompletedTrial->RewardChoices.IsValidIndex(ActiveRewardChoiceIndex))
	{
		return false;
	}
	FTrialRewardChoice Reward;
	if (CompletedTrial->RewardChoices.IsValidIndex(ActiveRewardChoiceIndex))
	{
		Reward = CompletedTrial->RewardChoices[ActiveRewardChoiceIndex];
	}
	else
	{
		Reward.ExperienceReward = CompletedTrial->ExperienceReward;
		Reward.ItemReward = CompletedTrial->ItemReward;
		Reward.SkillReward = CompletedTrial->SkillReward;
		Reward.AttributePointReward = CompletedTrial->AttributePointReward;
		Reward.SkillPointReward = CompletedTrial->SkillPointReward;
		Reward.EnduranceReward = CompletedTrial->EnduranceReward;
	}

	AActor* Owner = GetOwner();
	UExperienceComponent* Experience = Owner ? Owner->FindComponentByClass<UExperienceComponent>() : nullptr;
	UInventoryComponent* Inventory = Owner ? Owner->FindComponentByClass<UInventoryComponent>() : nullptr;
	UAttributeComponent* Attributes = Owner ? Owner->FindComponentByClass<UAttributeComponent>() : nullptr;
	UAcquiredAttributeComponent* Acquired = Owner ? Owner->FindComponentByClass<UAcquiredAttributeComponent>() : nullptr;
	USkillManagerComponent* SkillManager = Owner ? Owner->FindComponentByClass<USkillManagerComponent>() : nullptr;
	USkillComponent* SkillComponent = Owner ? Owner->FindComponentByClass<USkillComponent>() : nullptr;
	USkillBase* NewSkill = nullptr;
	bool bSkillAlreadyUnlocked = false;
	if (Reward.ExperienceReward > 0.0f
		&& (!Experience
			|| !FMath::IsFinite(Experience->GetTotalExperience() + Reward.ExperienceReward)
			|| !FMath::IsFinite(Experience->GetUnallocatedExperience() + Reward.ExperienceReward)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Trial %s reward blocked: XP pool unavailable or overflow"), *CompletedTrial->GetName());
		return false;
	}
	if (Reward.ItemReward.Item && !CanReceiveItemReward(
		Inventory, Reward.ItemReward.Item, Reward.ItemReward.Quantity))
	{
		UE_LOG(LogTemp, Warning, TEXT("Trial %s reward blocked: insufficient inventory capacity"), *CompletedTrial->GetName());
		return false;
	}
	if (Reward.AttributePointReward > 0 && (!Attributes
		|| static_cast<int64>(Attributes->GetUnspentAttributePoints())
			+ Reward.AttributePointReward > MAX_int32))
	{
		UE_LOG(LogTemp, Warning, TEXT("Trial %s reward blocked: attribute point pool unavailable or overflow"), *CompletedTrial->GetName());
		return false;
	}
	if (Reward.SkillPointReward > 0 && (!SkillManager
		|| static_cast<int64>(SkillManager->GetUnspentSkillPoints())
			+ Reward.SkillPointReward > MAX_int32))
	{
		UE_LOG(LogTemp, Warning, TEXT("Trial %s reward blocked: skill point pool unavailable or overflow"), *CompletedTrial->GetName());
		return false;
	}
	if (Reward.EnduranceReward > 0 && (!Acquired || !Acquired->CanGrantAcquiredAttribute(
		EAcquiredAttribute::Endurance, Reward.EnduranceReward)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Trial %s reward blocked: Endurance exceeds persistent Constitution cap"),
			*CompletedTrial->GetName());
		return false;
	}
	if (Reward.SkillReward.SkillClass)
	{
		if (!SkillManager || !SkillComponent || !SkillManager->SkillComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("Trial %s reward blocked: skill manager is not linked"), *CompletedTrial->GetName());
			return false;
		}
		for (USkillBase* Unlocked : SkillManager->GetUnlockedSkills())
		{
			if (Unlocked && Unlocked->GetSkillID() == Reward.SkillReward.SkillData->SkillID)
			{
				bSkillAlreadyUnlocked = true;
				break;
			}
		}
		if (!bSkillAlreadyUnlocked)
		{
			NewSkill = NewObject<USkillBase>(Owner, Reward.SkillReward.SkillClass);
			if (!NewSkill)
			{
				return false;
			}
			NewSkill->SkillData = Reward.SkillReward.SkillData;
			if (!SkillManager->CanUnlockSkill(NewSkill))
			{
				UE_LOG(LogTemp, Warning, TEXT("Trial %s reward blocked: skill requirements unmet"), *CompletedTrial->GetName());
				return false;
			}
		}
	}

	// All configured rewards are validated before any player state changes.
	if (NewSkill && !SkillManager->UnlockSkill(NewSkill))
	{
		return false;
	}
	if (Reward.ItemReward.Item)
	{
		UItemBase* RewardItem = NewObject<UItemBase>(Inventory);
		RewardItem->ItemData = Reward.ItemReward.Item;
		if (!Inventory->AddItem(RewardItem, Reward.ItemReward.Quantity))
		{
			if (NewSkill)
			{
				SkillManager->RevokeSkill(NewSkill);
			}
			return false;
		}
	}
	if (Reward.AttributePointReward > 0)
	{
		Attributes->AddAttributePoints(Reward.AttributePointReward);
	}
	if (Reward.SkillPointReward > 0)
	{
		SkillManager->AddSkillPoints(Reward.SkillPointReward);
	}
	if (Reward.EnduranceReward > 0)
	{
		Acquired->TryGrantAcquiredAttribute(EAcquiredAttribute::Endurance, Reward.EnduranceReward);
	}
	if (Reward.ExperienceReward > 0.0f)
	{
		Experience->AddExperience(Reward.ExperienceReward);
	}

	Record->LastRewardChoiceIndex = ActiveRewardChoiceIndex;
	++Record->CompletionCount;
	Record->bActive = false;
	ActiveTrial = nullptr;
	ActiveRewardChoiceIndex = INDEX_NONE;
	OnTrialCompleted.Broadcast(CompletedTrial);
	return true;
}

bool UTrialComponent::ChooseActiveTrialReward(int32 ChoiceIndex)
{
	if (!ActiveTrial || GetTrialProgress(ActiveTrial) < 1.0f
		|| !ActiveTrial->RewardChoices.IsValidIndex(ChoiceIndex)) return false;
	ActiveRewardChoiceIndex = ChoiceIndex;
	return TryCompleteActiveTrial();
}

bool UTrialComponent::FailActiveTrial()
{
	if (!ActiveTrial)
	{
		return false;
	}
	UTrialDataAsset* FailedTrial = ActiveTrial;
	if (FTrialRecord* Record = FindRecord(FailedTrial))
	{
		Record->bActive = false;
		++Record->FailureCount;
	}
	ActiveTrial = nullptr;
	ActiveRewardChoiceIndex = INDEX_NONE;
	OnTrialFailed.Broadcast(FailedTrial);
	return true;
}

float UTrialComponent::GetTrialProgress(const UTrialDataAsset* Trial) const
{
	const FTrialRecord* Record = FindRecord(Trial);
	if (!Trial || !Record || Trial->Objectives.IsEmpty()
		|| Record->ObjectiveCounts.Num() != Trial->Objectives.Num())
	{
		return 0.0f;
	}

	float Sum = 0.0f;
	for (int32 Index = 0; Index < Trial->Objectives.Num(); ++Index)
	{
		const int32 Target = Trial->Objectives[Index].TargetCount;
		if (Target < 1)
		{
			return 0.0f;
		}
		Sum += static_cast<float>(FMath::Clamp(Record->ObjectiveCounts[Index], 0, Target)) / Target;
	}
	return Sum / Trial->Objectives.Num();
}

int32 UTrialComponent::GetCompletionCount(const UTrialDataAsset* Trial) const
{
	const FTrialRecord* Record = FindRecord(Trial);
	return Record ? Record->CompletionCount : 0;
}
