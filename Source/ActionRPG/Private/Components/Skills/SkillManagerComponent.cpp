// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Skills/SkillManagerComponent.h"
#include "Components/Skills/SkillComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/ClassComponent.h"
#include "Progression/Core/ClassDataAsset.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"

USkillManagerComponent::USkillManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	for (int32 Index = 0; Index < 8; ++Index)
	{
		SkillBarSlots.Add(Index, nullptr);
	}
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

bool USkillManagerComponent::AddSkillPoints(int32 Points)
{
	if (Points <= 0 || Points > MAX_int32 - UnspentSkillPoints)
	{
		return false;
	}
	UnspentSkillPoints += Points;
	OnSkillPointsChanged.Broadcast(UnspentSkillPoints, Points);
	return true;
}

bool USkillManagerComponent::SpendSkillPoints(int32 Points)
{
	if (Points <= 0 || Points > UnspentSkillPoints)
	{
		return false;
	}
	UnspentSkillPoints -= Points;
	OnSkillPointsChanged.Broadcast(UnspentSkillPoints, -Points);
	return true;
}

bool USkillManagerComponent::IsSkillIDUnlocked(FName SkillID) const
{
	if (SkillID.IsNone()) return false;
	for (const USkillBase* Skill : UnlockedSkills)
	{
		if (Skill && Skill->GetSkillID() == SkillID) return true;
	}
	return false;
}

bool USkillManagerComponent::IsCatalogSkillUnlocked(int32 CatalogIndex) const
{
	return SkillCatalog.IsValidIndex(CatalogIndex) && SkillCatalog[CatalogIndex].SkillData &&
		IsSkillIDUnlocked(SkillCatalog[CatalogIndex].SkillData->SkillID);
}

bool USkillManagerComponent::CanLearnCatalogSkill(int32 CatalogIndex) const
{
	if (!SkillCatalog.IsValidIndex(CatalogIndex) || !SkillComponent) return false;
	const FSkillCatalogEntry& Entry = SkillCatalog[CatalogIndex];
	return Entry.SkillClass && Entry.SkillData && !Entry.SkillData->SkillID.IsNone() &&
		!IsSkillIDUnlocked(Entry.SkillData->SkillID) && Entry.SkillData->SkillPointCost > 0 &&
		UnspentSkillPoints >= Entry.SkillData->SkillPointCost && GetRequirementFailure(Entry.SkillData).IsEmpty();
}

FText USkillManagerComponent::GetCatalogSkillStatus(int32 CatalogIndex) const
{
	if (!SkillCatalog.IsValidIndex(CatalogIndex)) return FText::FromString(TEXT("Unavailable"));
	const FSkillCatalogEntry& Entry = SkillCatalog[CatalogIndex];
	if (!Entry.SkillClass || !Entry.SkillData || Entry.SkillData->SkillID.IsNone() || Entry.SkillData->SkillPointCost <= 0)
	{
		return FText::FromString(TEXT("Skill definition missing"));
	}
	if (IsSkillIDUnlocked(Entry.SkillData->SkillID)) return FText::FromString(TEXT("Learned"));
	const FString Failure = GetRequirementFailure(Entry.SkillData);
	if (!Failure.IsEmpty()) return FText::FromString(Failure);
	if (UnspentSkillPoints < Entry.SkillData->SkillPointCost)
	{
		return FText::FromString(FString::Printf(TEXT("Need %d free skill point%s"),
			Entry.SkillData->SkillPointCost, Entry.SkillData->SkillPointCost == 1 ? TEXT("") : TEXT("s")));
	}
	return FText::FromString(TEXT("Available"));
}

bool USkillManagerComponent::LearnCatalogSkill(int32 CatalogIndex)
{
	if (!CanLearnCatalogSkill(CatalogIndex)) return false;
	const FSkillCatalogEntry& Entry = SkillCatalog[CatalogIndex];
	USkillBase* NewSkill = NewObject<USkillBase>(GetOwner(), Entry.SkillClass);
	if (!NewSkill) return false;
	NewSkill->SkillData = Entry.SkillData;
	if (!CanUnlockSkill(NewSkill) || !SkillComponent->GrantSkill(NewSkill)) return false;
	const int32 Cost = Entry.SkillData->SkillPointCost;
	UnlockedSkills.Add(NewSkill);
	UnspentSkillPoints -= Cost;
	OnSkillPointsChanged.Broadcast(UnspentSkillPoints, -Cost);
	OnSkillUnlocked.Broadcast(NewSkill);
	return true;
}

bool USkillManagerComponent::UnlockSkill(USkillBase* Skill)
{
	return UnlockSkillInternal(Skill, true);
}

bool USkillManagerComponent::UnlockSkillFromStone(USkillBase* Skill)
{
	return UnlockSkillInternal(Skill, false);
}

bool USkillManagerComponent::UnlockSkillInternal(USkillBase* Skill, bool bCheckPrerequisites)
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
	if (bCheckPrerequisites && !CanUnlockSkill(Skill))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::UnlockSkill - Prerequisites not met for skill: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	// A failed runtime grant must not appear unlocked or consume its source item.
	if (!SkillComponent || !SkillComponent->GrantSkill(Skill))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillManagerComponent::UnlockSkill - SkillComponent could not grant skill"));
		return false;
	}
	UnlockedSkills.Add(Skill);
	UE_LOG(LogTemp, Log, TEXT("SkillManagerComponent::UnlockSkill - Granted skill to SkillComponent: %s"),
		Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));

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

bool USkillManagerComponent::RevokeSkill(USkillBase* Skill)
{
	if (!Skill || !UnlockedSkills.Contains(Skill))
	{
		return false;
	}
	for (int32 Index = 0; Index < 8; ++Index)
	{
		if (GetSkillAtSlot(Index) == Skill)
		{
			ClearSlot(Index);
		}
	}
	if (MainHandSkill == Skill)
	{
		ClearMainHandSkill();
	}
	if (OffhandSkill == Skill)
	{
		ClearOffhandSkill();
	}
	if (SkillComponent)
	{
		SkillComponent->RemoveSkill(Skill);
	}
	UnlockedSkills.Remove(Skill);
	OnSkillRevoked.Broadcast(Skill);
	return true;
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
	return Skill && Skill->SkillData && GetRequirementFailure(Skill->SkillData).IsEmpty();
}

FString USkillManagerComponent::GetRequirementFailure(const USkillDataAsset* SkillData) const
{
	if (!SkillData) return TEXT("Skill data missing");
	const UClassComponent* Classes = GetOwner() ? GetOwner()->FindComponentByClass<UClassComponent>() : nullptr;
	if (SkillData->RequiredLevel > 1 && (!Classes || Classes->GetPlayerLevel() < SkillData->RequiredLevel))
	{
		return FString::Printf(TEXT("Requires player level %d"), SkillData->RequiredLevel);
	}
	if (SkillData->RequiredAttributes.Num() > 0)
	{
		const UAttributeComponent* Attributes = GetOwner() ? GetOwner()->FindComponentByClass<UAttributeComponent>() : nullptr;
		for (const TPair<EPrimaryAttribute, float>& Requirement : SkillData->RequiredAttributes)
		{
			if (!Attributes || Attributes->GetPersistentAttribute(Requirement.Key) < Requirement.Value)
			{
				const UEnum* AttributeEnum = StaticEnum<EPrimaryAttribute>();
				const FString Name = AttributeEnum ? AttributeEnum->GetDisplayNameTextByValue(static_cast<int64>(Requirement.Key)).ToString() : TEXT("Attribute");
				return FString::Printf(TEXT("Requires %s %.0f"), *Name, Requirement.Value);
			}
		}
	}
	if (SkillData->RequiredClass)
	{
		if (!Classes || !Classes->GetSelectedClasses().Contains(SkillData->RequiredClass.Get()) ||
			Classes->GetClassLevel(SkillData->RequiredClass) < SkillData->RequiredClassLevel)
		{
			return FString::Printf(TEXT("Requires %s level %d"), *SkillData->RequiredClass->ClassName.ToString(),
				SkillData->RequiredClassLevel);
		}
	}
	for (const FName PrerequisiteID : SkillData->PrerequisiteSkillIDs)
	{
		if (!IsSkillIDUnlocked(PrerequisiteID))
		{
			return FString::Printf(TEXT("Requires skill %s"), *PrerequisiteID.ToString());
		}
	}
	return FString();
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
