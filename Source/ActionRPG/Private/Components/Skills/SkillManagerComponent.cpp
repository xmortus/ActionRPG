// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Skills/SkillManagerComponent.h"
#include "Components/Skills/SkillComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Core/SkillDataAsset.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"

USkillManagerComponent::USkillManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USkillManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::BeginPlay - Initializing skill manager for actor: %s"),
		GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"));

	// Find SkillComponent on owner
	SkillComponent = FindSkillComponent();
	if (!SkillComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::BeginPlay - SkillComponent not found on owner! Skill activation will fail."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::BeginPlay - SkillComponent found and linked."));
	}

	// Initialize skill bar slots (0-7 for slots 1-8)
	SkillBarSlots.Empty();
	for (int32 i = 0; i < 8; ++i)
	{
		SkillBarSlots.Add(i, nullptr);
	}

	// Initialize main/offhand slots
	MainHandSkill = nullptr;
	OffhandSkill = nullptr;

	UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent: Initialized with %d unlocked skills, %d skill bar slots"),
		UnlockedSkills.Num(), SkillBarSlots.Num());
}

bool USkillManagerComponent::UnlockSkill(USkillBase* Skill)
{
	if (!Skill)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::UnlockSkill - Skill is null"));
		return false;
	}

	// Check if already unlocked
	if (IsSkillUnlocked(Skill))
	{
		UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::UnlockSkill - Skill already unlocked: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return true; // Already unlocked, return true
	}

	// Check prerequisites
	if (!CanUnlockSkill(Skill))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::UnlockSkill - Prerequisites not met for skill: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	// Add to unlocked skills
	UnlockedSkills.Add(Skill);

	// Grant skill to SkillComponent
	if (SkillComponent)
	{
		// Create a new skill instance if needed (SkillComponent will handle duplicates)
		SkillComponent->GrantSkill(Skill);
		UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::UnlockSkill - Granted skill to SkillComponent: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::UnlockSkill - SkillComponent not found, skill unlocked but not granted!"));
	}

	// Broadcast unlock event
	OnSkillUnlocked.Broadcast(Skill);

	UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::UnlockSkill - Successfully unlocked skill: %s"),
		Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));

	return true;
}

bool USkillManagerComponent::IsSkillUnlocked(USkillBase* Skill) const
{
	if (!Skill)
	{
		return false;
	}

	return UnlockedSkills.Contains(Skill);
}

TArray<USkillBase*> USkillManagerComponent::GetUnlockedSkills() const
{
	TArray<USkillBase*> Result;
	Result.Reserve(UnlockedSkills.Num());

	for (const TObjectPtr<USkillBase>& Skill : UnlockedSkills)
	{
		if (Skill)
		{
			Result.Add(Skill);
		}
	}

	return Result;
}

bool USkillManagerComponent::AssignSkillToSlot(int32 SlotIndex, USkillBase* Skill)
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::AssignSkillToSlot - Invalid slot index: %d (must be 0-7)"), SlotIndex);
		return false;
	}

	// If skill is null, clear the slot
	if (!Skill)
	{
		ClearSlot(SlotIndex);
		return true;
	}

	// Check if skill is unlocked
	if (!IsSkillUnlocked(Skill))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::AssignSkillToSlot - Cannot assign unlocked skill to slot %d: %s"),
			SlotIndex, Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	// Check if slot is already occupied
	USkillBase* ExistingSkill = SkillBarSlots.FindRef(SlotIndex);
	if (ExistingSkill && ExistingSkill == Skill)
	{
		UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::AssignSkillToSlot - Skill already assigned to slot %d"), SlotIndex);
		return true; // Already assigned, return true
	}

	// Handle slot swapping: if slot is occupied, clear it first
	if (ExistingSkill)
	{
		UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::AssignSkillToSlot - Slot %d was occupied, clearing old skill"), SlotIndex);
	}

	// Assign skill to slot
	SkillBarSlots[SlotIndex] = Skill;

	// Broadcast slot changed event
	OnSkillSlotChanged.Broadcast(SlotIndex, Skill);

	UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::AssignSkillToSlot - Assigned skill %s to slot %d (hotkey %d)"),
		Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"), SlotIndex, SlotIndex + 1);

	return true;
}

bool USkillManagerComponent::RemoveSkillFromSlot(int32 SlotIndex)
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::RemoveSkillFromSlot - Invalid slot index: %d"), SlotIndex);
		return false;
	}

	if (!IsSlotOccupied(SlotIndex))
	{
		UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::RemoveSkillFromSlot - Slot %d is already empty"), SlotIndex);
		return false;
	}

	ClearSlot(SlotIndex);
	return true;
}

void USkillManagerComponent::ClearSlot(int32 SlotIndex)
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::ClearSlot - Invalid slot index: %d"), SlotIndex);
		return;
	}

	USkillBase* RemovedSkill = SkillBarSlots.FindRef(SlotIndex);
	SkillBarSlots[SlotIndex] = nullptr;

	// Broadcast slot cleared event
	OnSkillSlotCleared.Broadcast(SlotIndex);

	if (RemovedSkill)
	{
		UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::ClearSlot - Cleared slot %d (hotkey %d)"),
			SlotIndex, SlotIndex + 1);
	}
}

USkillBase* USkillManagerComponent::GetSkillAtSlot(int32 SlotIndex) const
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		return nullptr;
	}

	return SkillBarSlots.FindRef(SlotIndex);
}

bool USkillManagerComponent::IsSlotOccupied(int32 SlotIndex) const
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		return false;
	}

	USkillBase* Skill = SkillBarSlots.FindRef(SlotIndex);
	return Skill != nullptr;
}

bool USkillManagerComponent::ActivateSkillFromSlot(int32 SlotIndex, AActor* Target)
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::ActivateSkillFromSlot - Invalid slot index: %d"), SlotIndex);
		return false;
	}

	USkillBase* Skill = GetSkillAtSlot(SlotIndex);
	if (!Skill)
	{
		UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::ActivateSkillFromSlot - No skill assigned to slot %d"), SlotIndex);
		return false;
	}

	// Check if skill can be activated
	if (!CanActivateSkillFromSlot(SlotIndex))
	{
		UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::ActivateSkillFromSlot - Cannot activate skill from slot %d (cooldown or insufficient resources)"), SlotIndex);
		return false;
	}

	// Activate skill via SkillComponent
	if (!SkillComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillManagerComponent::ActivateSkillFromSlot - SkillComponent not found!"));
		return false;
	}

	bool bSuccess = SkillComponent->ActivateSkill(Skill, Target);
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::ActivateSkillFromSlot - Successfully activated skill from slot %d (hotkey %d)"),
			SlotIndex, SlotIndex + 1);
	}

	return bSuccess;
}

bool USkillManagerComponent::CanActivateSkillFromSlot(int32 SlotIndex) const
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		return false;
	}

	USkillBase* Skill = GetSkillAtSlot(SlotIndex);
	if (!Skill)
	{
		return false;
	}

	// Check via SkillComponent
	if (!SkillComponent)
	{
		return false;
	}

	return SkillComponent->CanActivateSkill(Skill);
}

bool USkillManagerComponent::CanUnlockSkill(USkillBase* Skill) const
{
	if (!Skill)
	{
		return false;
	}

	// Check all prerequisites
	return CheckSkillPrerequisites(Skill);
}

USkillComponent* USkillManagerComponent::FindSkillComponent() const
{
	if (!GetOwner())
	{
		return nullptr;
	}

	return GetOwner()->FindComponentByClass<USkillComponent>();
}

bool USkillManagerComponent::IsValidSlotIndex(int32 SlotIndex) const
{
	return SlotIndex >= 0 && SlotIndex < 8;
}

bool USkillManagerComponent::CheckSkillPrerequisites(USkillBase* Skill) const
{
	if (!Skill || !Skill->SkillData)
	{
		return false;
	}

	// Check level requirements
	if (!CheckLevelRequirements(Skill))
	{
		return false;
	}

	// Check attribute requirements (stub for Phase 4)
	if (!CheckAttributeRequirements(Skill))
	{
		return false;
	}

	// Check prerequisite skills (stub for now)
	if (!CheckPrerequisiteSkills(Skill))
	{
		return false;
	}

	// Check class requirements (stub for Phase 4)
	if (!CheckClassRequirements(Skill))
	{
		return false;
	}

	return true;
}

bool USkillManagerComponent::CheckAttributeRequirements(USkillBase* Skill) const
{
	if (!Skill || !Skill->SkillData)
	{
		return false;
	}

	// TODO: Phase 4 - Implement attribute requirement checking
	// For now, always return true (no attribute requirements enforced)
	// In Phase 4, query AttributeComponent and check against SkillDataAsset requirements

	return true;
}

bool USkillManagerComponent::CheckLevelRequirements(USkillBase* Skill) const
{
	if (!Skill || !Skill->SkillData)
	{
		return false;
	}

	// Check required level
	int32 RequiredLevel = Skill->SkillData->RequiredLevel;
	if (RequiredLevel <= 1)
	{
		return true; // No level requirement or level 1 requirement
	}

	// TODO: Phase 4 - Query character level from AttributeComponent or LevelComponent
	// For now, always return true (no level requirements enforced)
	// In Phase 4, check character level against RequiredLevel

	UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::CheckLevelRequirements - Skill requires level %d (checking will be implemented in Phase 4)"),
		RequiredLevel);

	return true;
}

bool USkillManagerComponent::CheckPrerequisiteSkills(USkillBase* Skill) const
{
	if (!Skill || !Skill->SkillData)
	{
		return false;
	}

	// TODO: Add prerequisite skill checking
	// This would require adding a PrerequisiteSkillIDs array to SkillDataAsset
	// For now, always return true (no prerequisite skills required)

	return true;
}

bool USkillManagerComponent::CheckClassRequirements(USkillBase* Skill) const
{
	if (!Skill || !Skill->SkillData)
	{
		return false;
	}

	// TODO: Phase 4 - Implement class requirement checking
	// This would require ClassComponent and class affinity system
	// For now, always return true (no class requirements enforced)

	return true;
}

TMap<int32, USkillBase*> USkillManagerComponent::GetAllSlotAssignments() const
{
	TMap<int32, USkillBase*> Result;
	Result.Reserve(SkillBarSlots.Num());

	for (const auto& Pair : SkillBarSlots)
	{
		Result.Add(Pair.Key, Pair.Value);
	}

	return Result;
}

bool USkillManagerComponent::AssignMainHandSkill(USkillBase* Skill)
{
	if (!Skill)
	{
		ClearMainHandSkill();
		return true;
	}

	if (!IsSkillUnlocked(Skill))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::AssignMainHandSkill - Skill is not unlocked: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	if (MainHandSkill == Skill)
	{
		return true;
	}

	MainHandSkill = Skill;
	OnMainHandSkillChanged.Broadcast(MainHandSkill);
	return true;
}

bool USkillManagerComponent::AssignOffhandSkill(USkillBase* Skill)
{
	if (!Skill)
	{
		ClearOffhandSkill();
		return true;
	}

	if (!IsSkillUnlocked(Skill))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::AssignOffhandSkill - Skill is not unlocked: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	if (OffhandSkill == Skill)
	{
		return true;
	}

	OffhandSkill = Skill;
	OnOffhandSkillChanged.Broadcast(OffhandSkill);
	return true;
}

void USkillManagerComponent::ClearMainHandSkill()
{
	if (MainHandSkill)
	{
		MainHandSkill = nullptr;
		OnMainHandSkillChanged.Broadcast(nullptr);
	}
}

void USkillManagerComponent::ClearOffhandSkill()
{
	if (OffhandSkill)
	{
		OffhandSkill = nullptr;
		OnOffhandSkillChanged.Broadcast(nullptr);
	}
}

bool USkillManagerComponent::ActivateMainHandSkill(AActor* Target)
{
	if (!SkillComponent || !MainHandSkill)
	{
		return false;
	}

	if (!SkillComponent->CanActivateSkill(MainHandSkill))
	{
		return false;
	}

	return SkillComponent->ActivateSkill(MainHandSkill, Target);
}

bool USkillManagerComponent::ActivateOffhandSkill(AActor* Target)
{
	if (!SkillComponent || !OffhandSkill)
	{
		return false;
	}

	if (!SkillComponent->CanActivateSkill(OffhandSkill))
	{
		return false;
	}

	return SkillComponent->ActivateSkill(OffhandSkill, Target);
}
