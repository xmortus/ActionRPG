// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Skills/SkillComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Core/SkillDataAsset.h"
#include "GameFramework/Actor.h"

USkillComponent::USkillComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bTickEvenWhenPaused = false;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;

	bShouldTickForCooldowns = true;
	ExperienceMultiplier = 1.0f;
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("SkillComponent::BeginPlay - Initializing skill component for actor: %s"), 
		GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"));

	// Validate owner
	if (!GetOwner())
	{
		UE_LOG(LogTemp, Error, TEXT("SkillComponent::BeginPlay - Component has no owner actor!"));
		return;
	}

	// Log active skills count
	UE_LOG(LogTemp, Log, TEXT("SkillComponent: Initialized with %d active skills"), ActiveSkills.Num());
	UE_LOG(LogTemp, Log, TEXT("  This SkillComponent is UNIQUE to this player/actor."));
	UE_LOG(LogTemp, Log, TEXT("  Skill instances stored here are separate from other players."));
}

void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update cooldowns if enabled
	if (bShouldTickForCooldowns)
	{
		UpdateCooldowns(DeltaTime);
	}
}

bool USkillComponent::GrantSkill(USkillBase* Skill)
{
	if (!Skill)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillComponent::GrantSkill - Skill is null"));
		return false;
	}

	// Check if skill already exists
	if (FindSkillInstanceIndex(Skill) != INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillComponent::GrantSkill - Skill already exists: %s"), 
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	// Set owner actor on skill
	Skill->OwnerActor = GetOwner();

	// Create new skill instance
	FSkillInstance NewInstance;
	NewInstance.Skill = Skill;
	NewInstance.CooldownRemaining = 0.0f;
	NewInstance.Level = Skill->SkillLevel > 0 ? Skill->SkillLevel : 1;
	NewInstance.Experience = Skill->Experience;
	NewInstance.bIsUnlocked = true;

	// Add to active skills
	ActiveSkills.Add(NewInstance);

	UE_LOG(LogTemp, Log, TEXT("SkillComponent::GrantSkill - Granted skill: %s (Level: %d, Experience: %.2f)"),
		Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"),
		NewInstance.Level,
		NewInstance.Experience);

	return true;
}

bool USkillComponent::RemoveSkill(USkillBase* Skill)
{
	if (!Skill)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillComponent::RemoveSkill - Skill is null"));
		return false;
	}

	int32 Index = FindSkillInstanceIndex(Skill);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillComponent::RemoveSkill - Skill not found: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("SkillComponent::RemoveSkill - Removing skill: %s"),
		Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));

	ActiveSkills.RemoveAt(Index);
	return true;
}

bool USkillComponent::CanActivateSkill(USkillBase* Skill) const
{
	if (!Skill)
	{
		return false;
	}

	// Check if skill is unlocked
	const FSkillInstance* Instance = FindSkillInstance(Skill);
	if (!Instance || !Instance->bIsUnlocked)
	{
		UE_LOG(LogTemp, Verbose, TEXT("SkillComponent::CanActivateSkill - Skill not unlocked: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	// Check cooldown
	if (Instance->CooldownRemaining > 0.0f)
	{
		UE_LOG(LogTemp, Verbose, TEXT("SkillComponent::CanActivateSkill - Skill on cooldown: %s (%.2f remaining)"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"),
			Instance->CooldownRemaining);
		return false;
	}

	// Check resource costs
	if (!ValidateResourceCosts(Skill))
	{
		UE_LOG(LogTemp, Verbose, TEXT("SkillComponent::CanActivateSkill - Insufficient resources for skill: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	// Check skill's own CanActivate
	if (!Skill->CanActivate())
	{
		UE_LOG(LogTemp, Verbose, TEXT("SkillComponent::CanActivateSkill - Skill cannot be activated: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	return true;
}

bool USkillComponent::ActivateSkill(USkillBase* Skill, AActor* Target)
{
	if (!CanActivateSkill(Skill))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillComponent::ActivateSkill - Cannot activate skill: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	FSkillInstance* Instance = FindSkillInstance(Skill);
	if (!Instance)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillComponent::ActivateSkill - Skill instance not found: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	// Deduct resource costs
	DeductResourceCosts(Skill);

	// Activate the skill
	USkillBase* ActivatedSkill = Skill->Activate(Target);
	if (!ActivatedSkill)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillComponent::ActivateSkill - Skill activation failed: %s"),
			Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
		return false;
	}

	// Update cooldown in instance
	if (Skill->SkillData)
	{
		Instance->CooldownRemaining = Skill->SkillData->CooldownDuration;
		// Sync cooldown with skill's own cooldown
		Skill->CooldownRemaining = Instance->CooldownRemaining;
	}

	// Grant experience (base amount for now, will be enhanced in Phase 4)
	// Base experience: 10.0 per use (can be customized per skill later)
	float BaseExperience = 10.0f;
	AddSkillExperience(Skill, BaseExperience);

	// Broadcast event
	OnSkillActivated.Broadcast(Skill, Target);

	UE_LOG(LogTemp, Log, TEXT("SkillComponent::ActivateSkill - Activated skill: %s (Target: %s, Cooldown: %.2f)"),
		Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"),
		Target ? *Target->GetName() : TEXT("None"),
		Instance->CooldownRemaining);

	return true;
}

void USkillComponent::UpdateCooldowns(float DeltaTime)
{
	for (FSkillInstance& Instance : ActiveSkills)
	{
		if (Instance.Skill && Instance.CooldownRemaining > 0.0f)
		{
			float PreviousCooldown = Instance.CooldownRemaining;
			Instance.CooldownRemaining = FMath::Max(0.0f, Instance.CooldownRemaining - DeltaTime);

			// Sync with skill's cooldown
			Instance.Skill->CooldownRemaining = Instance.CooldownRemaining;

			// Broadcast cooldown changed event
			OnSkillCooldownChanged.Broadcast(Instance.Skill);

			// Check if cooldown just completed
			if (PreviousCooldown > 0.0f && Instance.CooldownRemaining <= 0.0f)
			{
				UE_LOG(LogTemp, Verbose, TEXT("SkillComponent::UpdateCooldowns - Cooldown complete: %s"),
					Instance.Skill->SkillData ? *Instance.Skill->SkillData->SkillName.ToString() : TEXT("NULL"));
			}
		}
	}
}

int32 USkillComponent::GetSkillLevel(USkillBase* Skill) const
{
	const FSkillInstance* Instance = FindSkillInstance(Skill);
	if (!Instance)
	{
		return 0;
	}
	return Instance->Level;
}

float USkillComponent::GetSkillExperience(USkillBase* Skill) const
{
	const FSkillInstance* Instance = FindSkillInstance(Skill);
	if (!Instance)
	{
		return 0.0f;
	}
	return Instance->Experience;
}

bool USkillComponent::IsSkillUnlocked(USkillBase* Skill) const
{
	const FSkillInstance* Instance = FindSkillInstance(Skill);
	if (!Instance)
	{
		return false;
	}
	return Instance->bIsUnlocked;
}

float USkillComponent::GetSkillCooldownRemaining(USkillBase* Skill) const
{
	const FSkillInstance* Instance = FindSkillInstance(Skill);
	if (!Instance)
	{
		return 0.0f;
	}
	return Instance->CooldownRemaining;
}

FSkillInstance* USkillComponent::GetSkillInstance(USkillBase* Skill)
{
	return FindSkillInstance(Skill);
}

const FSkillInstance* USkillComponent::GetSkillInstance(USkillBase* Skill) const
{
	return FindSkillInstance(Skill);
}

TArray<USkillBase*> USkillComponent::GetSkills() const
{
	TArray<USkillBase*> Skills;
	Skills.Reserve(ActiveSkills.Num());

	for (const FSkillInstance& Instance : ActiveSkills)
	{
		if (Instance.Skill)
		{
			Skills.Add(Instance.Skill);
		}
	}

	return Skills;
}

TArray<USkillBase*> USkillComponent::GetUnlockedSkills() const
{
	TArray<USkillBase*> UnlockedSkills;
	UnlockedSkills.Reserve(ActiveSkills.Num());

	for (const FSkillInstance& Instance : ActiveSkills)
	{
		if (Instance.Skill && Instance.bIsUnlocked)
		{
			UnlockedSkills.Add(Instance.Skill);
		}
	}

	return UnlockedSkills;
}

FSkillInstance USkillComponent::GetSkillInstanceData(USkillBase* Skill) const
{
	const FSkillInstance* Instance = FindSkillInstance(Skill);
	if (Instance)
	{
		return *Instance;
	}
	
	// Return empty instance if not found
	return FSkillInstance();
}

void USkillComponent::AddSkillExperience(USkillBase* Skill, float Amount)
{
	if (!Skill || Amount <= 0.0f)
	{
		return;
	}

	FSkillInstance* Instance = FindSkillInstance(Skill);
	if (!Instance || !Instance->bIsUnlocked)
	{
		return;
	}

	// Calculate experience gain with modifiers
	float ExperienceGain = CalculateExperienceGain(Skill, Amount);
	int32 PreviousLevel = Instance->Level;

	// Add experience
	Instance->Experience += ExperienceGain;
	
	// Sync with skill's experience
	Instance->Skill->Experience = Instance->Experience;

	// Check for level up
	CheckSkillLevelUp(*Instance, ExperienceGain);

	// Broadcast event
	OnSkillExperienceGained.Broadcast(Skill, ExperienceGain, Instance->Level);

	UE_LOG(LogTemp, Verbose, TEXT("SkillComponent::AddSkillExperience - Added %.2f XP to skill: %s (Level: %d, Total XP: %.2f)"),
		ExperienceGain,
		Skill->SkillData ? *Skill->SkillData->SkillName.ToString() : TEXT("NULL"),
		Instance->Level,
		Instance->Experience);
}

float USkillComponent::CalculateExperienceGain(USkillBase* Skill, float BaseAmount) const
{
	if (!Skill)
	{
		return 0.0f;
	}

	// Apply global experience multiplier
	float ExperienceGain = BaseAmount * ExperienceMultiplier;

	// Phase 4 will add:
	// - Attribute modifiers (INT for magic, DEX for precision, etc.)
	// - Class affinity bonuses
	// - Skill difficulty multiplier

	return ExperienceGain;
}

void USkillComponent::CheckSkillLevelUp(FSkillInstance& Instance, float ExperienceAdded)
{
	if (!Instance.Skill)
	{
		return;
	}

	int32 PreviousLevel = Instance.Level;
	float ExperienceRequired = GetExperienceRequiredForLevel(Instance.Level);

	// Check if enough experience for level up
	while (Instance.Experience >= ExperienceRequired)
	{
		Instance.Experience -= ExperienceRequired;
		Instance.Level++;
		
		// Sync with skill
		Instance.Skill->SkillLevel = Instance.Level;
		Instance.Skill->Experience = Instance.Experience;

		// Calculate next level requirement
		ExperienceRequired = GetExperienceRequiredForLevel(Instance.Level);

		UE_LOG(LogTemp, Log, TEXT("SkillComponent::CheckSkillLevelUp - Skill leveled up: %s (Level %d -> %d)"),
			Instance.Skill->SkillData ? *Instance.Skill->SkillData->SkillName.ToString() : TEXT("NULL"),
			PreviousLevel,
			Instance.Level);

		// Broadcast level up event
		OnSkillLevelUp.Broadcast(Instance.Skill, Instance.Level);

		PreviousLevel = Instance.Level;
	}
}

float USkillComponent::GetExperienceRequiredForLevel(int32 CurrentLevel) const
{
	// Simple formula: Level * 100
	// Phase 4 may add more complex formulas
	return static_cast<float>(CurrentLevel * 100);
}

bool USkillComponent::ValidateResourceCosts(USkillBase* Skill) const
{
	if (!Skill || !Skill->SkillData)
	{
		return false;
	}

	// Phase 4: Query resource component for mana/stamina
	// For now, log warning if costs are required but resource system not available
	float ManaCost = Skill->GetManaCost();
	float StaminaCost = Skill->GetStaminaCost();

	if (ManaCost > 0.0f || StaminaCost > 0.0f)
	{
		// TODO: Phase 4 - Check resource component for available resources
		UE_LOG(LogTemp, Verbose, TEXT("SkillComponent::ValidateResourceCosts - Resource system not yet implemented (Mana: %.2f, Stamina: %.2f)"),
			ManaCost, StaminaCost);
		// For now, allow activation (will be properly validated in Phase 4)
		return true;
	}

	return true;
}

void USkillComponent::DeductResourceCosts(USkillBase* Skill)
{
	if (!Skill || !Skill->SkillData)
	{
		return;
	}

	float ManaCost = Skill->GetManaCost();
	float StaminaCost = Skill->GetStaminaCost();

	if (ManaCost > 0.0f || StaminaCost > 0.0f)
	{
		// Phase 4: Call resource component to deduct costs
		UE_LOG(LogTemp, Verbose, TEXT("SkillComponent::DeductResourceCosts - Resource system not yet implemented (Mana: %.2f, Stamina: %.2f)"),
			ManaCost, StaminaCost);
		// TODO: Phase 4 - Deduct resources from resource component
	}
}

int32 USkillComponent::FindSkillInstanceIndex(USkillBase* Skill) const
{
	if (!Skill)
	{
		return INDEX_NONE;
	}

	for (int32 i = 0; i < ActiveSkills.Num(); i++)
	{
		if (ActiveSkills[i].Skill == Skill)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

FSkillInstance* USkillComponent::FindSkillInstance(USkillBase* Skill)
{
	if (!Skill)
	{
		return nullptr;
	}

	for (FSkillInstance& Instance : ActiveSkills)
	{
		if (Instance.Skill == Skill)
		{
			return &Instance;
		}
	}

	return nullptr;
}

const FSkillInstance* USkillComponent::FindSkillInstance(USkillBase* Skill) const
{
	if (!Skill)
	{
		return nullptr;
	}

	for (const FSkillInstance& Instance : ActiveSkills)
	{
		if (Instance.Skill == Skill)
		{
			return &Instance;
		}
	}

	return nullptr;
}
