// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Progression/ClassComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Progression/Core/ClassDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Progression/Core/ProgressionTypes.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Components/Skills/SkillComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Core/SkillDataAsset.h"

UClassComponent::UClassComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UClassComponent::BeginPlay()
{
	Super::BeginPlay();

	AttributeComponent = GetOwner() ? GetOwner()->FindComponentByClass<UAttributeComponent>() : nullptr;
	SecondaryAttributeComponent = GetOwner() ? GetOwner()->FindComponentByClass<USecondaryAttributeComponent>() : nullptr;
	ExperienceComponent = GetOwner() ? GetOwner()->FindComponentByClass<UExperienceComponent>() : nullptr;
	SkillComponent = GetOwner() ? GetOwner()->FindComponentByClass<USkillComponent>() : nullptr;
	if (SkillComponent)
	{
		SkillComponent->OnSkillLevelUp.AddUniqueDynamic(this, &UClassComponent::HandleSkillLevelUp);
	}
	if (GetOwner())
	{
		if (USkillManagerComponent* Manager = GetOwner()->FindComponentByClass<USkillManagerComponent>())
		{
			Manager->OnSkillRevoked.AddUniqueDynamic(this, &UClassComponent::HandleSkillRevoked);
		}
	}
	for (const TObjectPtr<UClassDataAsset>& ClassAsset : SelectedClasses)
	{
		GrantClassSkills(ClassAsset);
	}
	for (const TObjectPtr<UProfessionDataAsset>& Profession : SelectedProfessions)
	{
		if (Profession)
		{
			GrantProfessionSkill(Profession);
		}
	}
}

bool UClassComponent::SelectClass(UClassDataAsset* NewClass)
{
	if (!NewClass || !HasValidClassSkillGrants(NewClass)
		|| !MeetsRequirements(NewClass->RequiredLevel, NewClass->RequiredAttributes)
		|| (GetClassLevel(NewClass) < 1 && !MeetsClassSkillRequirements(NewClass, 1)))
	{
		return false;
	}

	if (SelectedClasses.Num() == 1 && SelectedClasses.Contains(NewClass))
	{
		GrantAttributePointsForClassLevels(NewClass, GetClassLevel(NewClass));
		GrantClassSkills(NewClass);
		return true;
	}

	ClearClass();
	SelectedClasses.Add(NewClass);
	ClassProgress.FindOrAdd(NewClass);
	CurrentClass = NewClass;
	UpdateAppliedBonuses();
	GrantAttributePointsForClassLevels(NewClass, GetClassLevel(NewClass));
	GrantClassSkills(NewClass);
	OnClassChanged.Broadcast(CurrentClass);
	OnClassListChanged.Broadcast(NewClass, true);
	return true;
}

bool UClassComponent::SelectProfession(UProfessionDataAsset* NewProfession)
{
	if (!NewProfession || !MeetsRequirements(NewProfession->RequiredLevel, NewProfession->RequiredAttributes))
	{
		return false;
	}
	if (GetProfessionLevel(NewProfession) < 1)
	{
		return false;
	}

	if (SelectedProfessions.Num() == 1 && SelectedProfessions.Contains(NewProfession))
	{
		return true;
	}
	if (!GrantProfessionSkill(NewProfession))
	{
		return false;
	}

	ClearProfession();
	SelectedProfessions.Add(NewProfession);
	ProfessionProgress.FindOrAdd(NewProfession);
	CurrentProfession = NewProfession;
	UpdateAppliedBonuses();
	OnProfessionChanged.Broadcast(CurrentProfession);
	OnProfessionListChanged.Broadcast(NewProfession, true);
	return true;
}

bool UClassComponent::AddClass(UClassDataAsset* NewClass)
{
	if (!NewClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent::AddClass - Invalid class"));
		return false;
	}

	if (SelectedClasses.Contains(NewClass))
	{
		GrantAttributePointsForClassLevels(NewClass, GetClassLevel(NewClass));
		GrantClassSkills(NewClass);
		return true;
	}
	if (!HasValidClassSkillGrants(NewClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent::AddClass - Invalid class skill grants"));
		return false;
	}

	if (!CanAddClass())
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent::AddClass - Class cap reached"));
		return false;
	}

	if (!MeetsRequirements(NewClass->RequiredLevel, NewClass->RequiredAttributes))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent::AddClass - Requirements not met"));
		return false;
	}
	if (GetClassLevel(NewClass) < 1 && !MeetsClassSkillRequirements(NewClass, 1))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent::AddClass - Skill requirement not met"));
		return false;
	}

	SelectedClasses.Add(NewClass);
	if (!ClassProgress.Contains(NewClass))
	{
		ClassProgress.Add(NewClass, FClassProgress());
	}
	CurrentClass = NewClass;
	UpdateAppliedBonuses();
	GrantAttributePointsForClassLevels(NewClass, GetClassLevel(NewClass));
	GrantClassSkills(NewClass);
	OnClassChanged.Broadcast(CurrentClass);
	OnClassListChanged.Broadcast(NewClass, true);
	return true;
}

bool UClassComponent::AddProfession(UProfessionDataAsset* NewProfession)
{
	if (!NewProfession)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent::AddProfession - Invalid profession"));
		return false;
	}

	if (SelectedProfessions.Contains(NewProfession))
	{
		return true;
	}

	if (!CanAddProfession())
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent::AddProfession - Profession cap reached"));
		return false;
	}

	if (!MeetsRequirements(NewProfession->RequiredLevel, NewProfession->RequiredAttributes))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent::AddProfession - Requirements not met"));
		return false;
	}
	if (GetProfessionLevel(NewProfession) < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent::AddProfession - Profession needs its first XP level"));
		return false;
	}
	if (!GrantProfessionSkill(NewProfession))
	{
		return false;
	}

	SelectedProfessions.Add(NewProfession);
	ProfessionProgress.FindOrAdd(NewProfession);
	CurrentProfession = NewProfession;
	UpdateAppliedBonuses();
	OnProfessionChanged.Broadcast(CurrentProfession);
	OnProfessionListChanged.Broadcast(NewProfession, true);
	return true;
}

bool UClassComponent::RemoveClass(UClassDataAsset* ClassToRemove)
{
	if (!ClassToRemove)
	{
		return false;
	}

	if (!SelectedClasses.Remove(ClassToRemove))
	{
		return false;
	}
	RevokeClassSkills(ClassToRemove);

	if (CurrentClass == ClassToRemove)
	{
		CurrentClass = SelectedClasses.Num() > 0 ? SelectedClasses.Last() : nullptr;
		OnClassChanged.Broadcast(CurrentClass);
	}

	UpdateAppliedBonuses();
	OnClassListChanged.Broadcast(ClassToRemove, false);
	return true;
}

bool UClassComponent::RemoveProfession(UProfessionDataAsset* ProfessionToRemove)
{
	if (!ProfessionToRemove)
	{
		return false;
	}

	if (!SelectedProfessions.Remove(ProfessionToRemove))
	{
		return false;
	}
	RevokeProfessionSkill(ProfessionToRemove);

	if (CurrentProfession == ProfessionToRemove)
	{
		CurrentProfession = SelectedProfessions.Num() > 0 ? SelectedProfessions.Last() : nullptr;
		OnProfessionChanged.Broadcast(CurrentProfession);
	}

	UpdateAppliedBonuses();
	OnProfessionListChanged.Broadcast(ProfessionToRemove, false);
	return true;
}

void UClassComponent::ClearClass()
{
	if (SelectedClasses.Num() == 0 && !CurrentClass)
	{
		return;
	}

	const TArray<TObjectPtr<UClassDataAsset>> RemovedClasses = SelectedClasses;
	SelectedClasses.Reset();
	CurrentClass = nullptr;
	for (const TObjectPtr<UClassDataAsset>& ClassAsset : RemovedClasses)
	{
		if (ClassAsset)
		{
			RevokeClassSkills(ClassAsset);
			OnClassListChanged.Broadcast(ClassAsset, false);
		}
	}
	UpdateAppliedBonuses();
	OnClassChanged.Broadcast(nullptr);
}

void UClassComponent::ClearProfession()
{
	if (SelectedProfessions.Num() == 0 && !CurrentProfession)
	{
		return;
	}

	for (const TObjectPtr<UProfessionDataAsset>& ProfessionAsset : SelectedProfessions)
	{
		if (ProfessionAsset)
		{
			RevokeProfessionSkill(ProfessionAsset);
			OnProfessionListChanged.Broadcast(ProfessionAsset, false);
		}
	}

	SelectedProfessions.Reset();
	CurrentProfession = nullptr;
	UpdateAppliedBonuses();
	OnProfessionChanged.Broadcast(nullptr);
}

TArray<UClassDataAsset*> UClassComponent::GetSelectedClasses() const
{
	TArray<UClassDataAsset*> Result;
	Result.Reserve(SelectedClasses.Num());
	for (const TObjectPtr<UClassDataAsset>& ClassAsset : SelectedClasses)
	{
		if (ClassAsset)
		{
			Result.Add(ClassAsset);
		}
	}
	return Result;
}

TArray<UProfessionDataAsset*> UClassComponent::GetSelectedProfessions() const
{
	TArray<UProfessionDataAsset*> Result;
	Result.Reserve(SelectedProfessions.Num());
	for (const TObjectPtr<UProfessionDataAsset>& ProfessionAsset : SelectedProfessions)
	{
		if (ProfessionAsset)
		{
			Result.Add(ProfessionAsset);
		}
	}
	return Result;
}

FText UClassComponent::GetSelectedProfessionsText() const
{
	if (SelectedProfessions.Num() == 0)
	{
		return FText::FromString(TEXT("None"));
	}

	TArray<FString> Names;
	for (const TObjectPtr<UProfessionDataAsset>& ProfessionAsset : SelectedProfessions)
	{
		if (ProfessionAsset)
		{
			Names.Add(FString::Printf(TEXT("%s: %d"), *ProfessionAsset->ProfessionName.ToString(), GetProfessionLevel(ProfessionAsset)));
		}
	}

	if (Names.Num() == 0)
	{
		return FText::FromString(TEXT("None"));
	}

	return FText::FromString(FString::Join(Names, TEXT("\n")));
}

FText UClassComponent::GetSelectedClassesText() const
{
	if (SelectedClasses.Num() == 0)
	{
		return FText::FromString(TEXT("None"));
	}

	TArray<FString> Lines;
	for (const TObjectPtr<UClassDataAsset>& ClassAsset : SelectedClasses)
	{
		if (ClassAsset)
		{
			const int32 ClassLevel = GetClassLevel(ClassAsset);
			Lines.Add(FString::Printf(TEXT("%s: %d"), *ClassAsset->ClassName.ToString(), ClassLevel));
		}
	}

	if (Lines.Num() == 0)
	{
		return FText::FromString(TEXT("None"));
	}

	return FText::FromString(FString::Join(Lines, TEXT("\n")));
}

int32 UClassComponent::GetPlayerLevel() const
{
	int32 TotalLevel = 0;
	for (const TObjectPtr<UClassDataAsset>& ClassAsset : SelectedClasses)
	{
		if (!ClassAsset)
		{
			continue;
		}
		if (const FClassProgress* Progress = ClassProgress.Find(ClassAsset))
		{
			TotalLevel += FMath::Max(1, Progress->Level);
		}
		else
		{
			TotalLevel += 1;
		}
	}
	for (const TObjectPtr<UProfessionDataAsset>& ProfessionAsset : SelectedProfessions)
	{
		if (ProfessionAsset)
		{
			TotalLevel += FMath::Max(1, GetProfessionLevel(ProfessionAsset));
		}
	}

	return TotalLevel;
}

int32 UClassComponent::GetClassLevel(UClassDataAsset* ClassAsset) const
{
	if (!ClassAsset)
	{
		return 0;
	}

	if (const FClassProgress* Progress = ClassProgress.Find(ClassAsset))
	{
		return Progress->Level;
	}

	return 0;
}

float UClassComponent::GetClassExperience(UClassDataAsset* ClassAsset) const
{
	if (!ClassAsset)
	{
		return 0.0f;
	}

	if (const FClassProgress* Progress = ClassProgress.Find(ClassAsset))
	{
		return Progress->Experience;
	}

	return 0.0f;
}

float UClassComponent::GetClassExperienceToNextLevel(UClassDataAsset* ClassAsset) const
{
	if (!ClassAsset)
	{
		return 0.0f;
	}

	const int32 Level = GetClassLevel(ClassAsset);
	return GetExperienceRequiredForLevel(ClassAsset, FMath::Max(1, Level));
}

int32 UClassComponent::GetProfessionLevel(UProfessionDataAsset* ProfessionAsset) const
{
	if (!ProfessionAsset) return 0;
	if (const FClassProgress* Progress = ProfessionProgress.Find(ProfessionAsset)) return Progress->Level;
	return SelectedProfessions.Contains(ProfessionAsset) ? 1 : 0;
}

float UClassComponent::GetProfessionExperience(UProfessionDataAsset* ProfessionAsset) const
{
	if (!ProfessionAsset) return 0.0f;
	if (const FClassProgress* Progress = ProfessionProgress.Find(ProfessionAsset)) return Progress->Experience;
	return 0.0f;
}

float UClassComponent::GetProfessionExperienceToNextLevel(UProfessionDataAsset* ProfessionAsset) const
{
	if (!ProfessionAsset) return 0.0f;
	return BaseExperienceToLevel + ExperiencePerLevel * FMath::Max(0, GetProfessionLevel(ProfessionAsset) - 1);
}

bool UClassComponent::AllocateExperienceToProfession(UProfessionDataAsset* ProfessionAsset, float Amount)
{
	if (!ProfessionAsset || !FMath::IsFinite(Amount) || Amount <= 0.0f)
	{
		return false;
	}
	if (!ExperienceComponent && GetOwner())
	{
		ExperienceComponent = GetOwner()->FindComponentByClass<UExperienceComponent>();
	}
	const FClassProgress* ExistingProgress = ProfessionProgress.Find(ProfessionAsset);
	const bool bHadProgress = ExistingProgress != nullptr;
	const float CurrentExperience = ExistingProgress ? ExistingProgress->Experience : 0.0f;
	if (!FMath::IsFinite(CurrentExperience + Amount) || !ExperienceComponent || !ExperienceComponent->SpendExperience(Amount))
	{
		return false;
	}
	FClassProgress& Progress = ProfessionProgress.FindOrAdd(ProfessionAsset);
	if (!bHadProgress && !SelectedProfessions.Contains(ProfessionAsset))
	{
		Progress.Level = 0;
	}
	Progress.Experience += Amount;
	float Required = GetProfessionExperienceToNextLevel(ProfessionAsset);
	while (FMath::IsFinite(Required) && Required > 0.0f && Progress.Experience >= Required && Progress.Level < MAX_int32)
	{
		Progress.Experience -= Required;
		++Progress.Level;
		UpdateAppliedBonuses();
		OnProfessionLevelChanged.Broadcast(ProfessionAsset, Progress.Level);
		Required = GetProfessionExperienceToNextLevel(ProfessionAsset);
	}
	OnProfessionExperienceChanged.Broadcast(ProfessionAsset, Progress.Experience);
	if (!SelectedProfessions.Contains(ProfessionAsset) && Progress.Level >= 1)
	{
		AddProfession(ProfessionAsset);
	}
	return true;
}

bool UClassComponent::AllocateExperienceToClass(UClassDataAsset* ClassAsset, float Amount)
{
	return AllocateExperienceInternal(ClassAsset, Amount, false);
}

bool UClassComponent::AllocateExperienceToClassAllowUnselected(UClassDataAsset* ClassAsset, float Amount)
{
	return AllocateExperienceInternal(ClassAsset, Amount, true);
}

bool UClassComponent::AllocateExperienceInternal(UClassDataAsset* ClassAsset, float Amount, bool bAllowUnselected)
{
	if (!ClassAsset || !FMath::IsFinite(Amount) || Amount <= 0.0f)
	{
		return false;
	}

	const bool bIsSelected = SelectedClasses.Contains(ClassAsset);
	if (!bIsSelected && !bAllowUnselected)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent::AllocateExperienceToClass - Class not selected"));
		return false;
	}

	if (!ExperienceComponent || !ExperienceComponent->SpendExperience(Amount))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent::AllocateExperienceToClass - Not enough unallocated XP"));
		return false;
	}

	const bool bWasInProgress = ClassProgress.Contains(ClassAsset);
	FClassProgress& Progress = ClassProgress.FindOrAdd(ClassAsset);
	if (!bWasInProgress && !bIsSelected && bAllowUnselected)
	{
		Progress.Level = 0;
	}
	Progress.Experience += Amount;

	AdvanceClassIfReady(ClassAsset, Progress);
	OnClassExperienceChanged.Broadcast(ClassAsset, Progress.Experience);

	if (!bIsSelected && ClassAsset && Progress.Level >= ClassAsset->RequiredClassLevelToUnlock)
	{
		AddClass(ClassAsset);
	}

	return true;
}

bool UClassComponent::GetUnmetClassSkillRequirement(UClassDataAsset* ClassAsset, FName& OutSkillID, int32& OutRequiredLevel) const
{
	OutSkillID = NAME_None;
	OutRequiredLevel = 0;
	if (!ClassAsset) return false;
	const int32 TargetLevel = GetClassLevel(ClassAsset) + 1;
	for (const FClassSkillLevelRequirement& Requirement : ClassAsset->SkillLevelRequirements)
	{
		if (Requirement.TargetClassLevel != TargetLevel || Requirement.SkillID.IsNone() || Requirement.RequiredSkillLevel < 1)
		{
			continue;
		}
		bool bMet = false;
		if (SkillComponent)
		{
			for (USkillBase* Skill : SkillComponent->GetUnlockedSkills())
			{
				if (Skill && Skill->GetSkillID() == Requirement.SkillID &&
					SkillComponent->GetSkillLevel(Skill) >= Requirement.RequiredSkillLevel)
				{
					bMet = true;
					break;
				}
			}
		}
		if (!bMet)
		{
			OutSkillID = Requirement.SkillID;
			OutRequiredLevel = Requirement.RequiredSkillLevel;
			return true;
		}
	}
	return false;
}

bool UClassComponent::MeetsClassSkillRequirements(UClassDataAsset* ClassAsset, int32 TargetLevel) const
{
	FName SkillID;
	int32 RequiredLevel = 0;
	return TargetLevel != GetClassLevel(ClassAsset) + 1 ||
		!GetUnmetClassSkillRequirement(ClassAsset, SkillID, RequiredLevel);
}

bool UClassComponent::AdvanceClassIfReady(UClassDataAsset* ClassAsset, FClassProgress& Progress)
{
	bool bAdvanced = false;
	float Required = GetExperienceRequiredForLevel(ClassAsset, FMath::Max(1, Progress.Level));
	while (FMath::IsFinite(Required) && Required > 0.0f && Progress.Experience >= Required &&
		Progress.Level < MAX_int32 && MeetsClassSkillRequirements(ClassAsset, Progress.Level + 1))
	{
		Progress.Experience -= Required;
		Progress.Level = FMath::Max(1, Progress.Level + 1);
		UpdateAppliedBonuses();
		GrantAttributePointsForClassLevels(ClassAsset, Progress.Level);
		if (SelectedClasses.Contains(ClassAsset))
		{
			GrantClassSkills(ClassAsset);
		}
		OnClassLevelChanged.Broadcast(ClassAsset, Progress.Level);
		bAdvanced = true;
		Required = GetExperienceRequiredForLevel(ClassAsset, FMath::Max(1, Progress.Level));
	}
	return bAdvanced;
}

void UClassComponent::HandleSkillLevelUp(USkillBase* Skill, int32 NewLevel)
{
	if (!Skill || NewLevel < 1) return;
	TArray<TObjectPtr<UClassDataAsset>> ClassesToUnlock;
	for (TPair<TObjectPtr<UClassDataAsset>, FClassProgress>& Entry : ClassProgress)
	{
		UClassDataAsset* ClassAsset = Entry.Key.Get();
		if (!ClassAsset || !AdvanceClassIfReady(ClassAsset, Entry.Value)) continue;
		OnClassExperienceChanged.Broadcast(ClassAsset, Entry.Value.Experience);
		if (!SelectedClasses.Contains(ClassAsset) && Entry.Value.Level >= ClassAsset->RequiredClassLevelToUnlock)
		{
			ClassesToUnlock.Add(ClassAsset);
		}
	}
	for (UClassDataAsset* ClassAsset : ClassesToUnlock)
	{
		AddClass(ClassAsset);
	}
}

void UClassComponent::GrantAttributePointsForClassLevels(UClassDataAsset* ClassAsset, int32 NewLevel)
{
	if (!ClassAsset || NewLevel <= 0) return;
	if (!AttributeComponent && GetOwner())
		AttributeComponent = GetOwner()->FindComponentByClass<UAttributeComponent>();
	if (!AttributeComponent) return;
	int32& AwardedLevel = AwardedAttributePointLevels.FindOrAdd(ClassAsset);
	if (NewLevel <= AwardedLevel) return;
	const int32 PointsPerLevel = ClassAsset->GetFreeAttributePointsPerLevel();
	const int64 PointsToGrant = static_cast<int64>(NewLevel - AwardedLevel) * PointsPerLevel;
	if (PointsToGrant > MAX_int32 - static_cast<int64>(AttributeComponent->GetUnspentAttributePoints()))
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent - Attribute point award would overflow for %s"), *GetNameSafe(ClassAsset));
		return;
	}
	AttributeComponent->AddAttributePoints(static_cast<int32>(PointsToGrant));
	AwardedLevel = NewLevel;
}

bool UClassComponent::MeetsRequirements(int32 RequiredLevel, const TMap<EPrimaryAttribute, float>& RequiredAttributes) const
{
	const int32 CurrentLevel = GetPlayerLevel();
	if (RequiredLevel > 1 && CurrentLevel < RequiredLevel)
	{
		return false;
	}

	if (AttributeComponent)
	{
		for (const TPair<EPrimaryAttribute, float>& Requirement : RequiredAttributes)
		{
			if (AttributeComponent->GetPersistentAttribute(Requirement.Key) < Requirement.Value)
			{
				return false;
			}
		}
	}
	else if (RequiredAttributes.Num() > 0)
	{
		return false;
	}

	return true;
}

bool UClassComponent::HasValidClassSkillGrants(const UClassDataAsset* ClassAsset) const
{
	if (!ClassAsset) return false;
	if (ClassAsset->GrantedSkills.IsEmpty()) return true;
	if (!GetOwner() || !GetOwner()->FindComponentByClass<USkillManagerComponent>()
		|| !GetOwner()->FindComponentByClass<USkillComponent>()) return false;
	TSet<FName> SeenSkillIDs;
	for (const FClassGrantedSkill& Grant : ClassAsset->GrantedSkills)
	{
		if (Grant.TargetClassLevel < 1 || !Grant.SkillClass || !Grant.SkillData
			|| Grant.SkillData->SkillID.IsNone() || SeenSkillIDs.Contains(Grant.SkillData->SkillID))
		{
			return false;
		}
		SeenSkillIDs.Add(Grant.SkillData->SkillID);
	}
	return true;
}

bool UClassComponent::GrantClassSkills(UClassDataAsset* ClassAsset)
{
	if (!HasValidClassSkillGrants(ClassAsset)) return false;
	if (ClassAsset->GrantedSkills.IsEmpty()) return true;
	USkillManagerComponent* Manager = GetOwner()->FindComponentByClass<USkillManagerComponent>();
	USkillComponent* Skills = GetOwner()->FindComponentByClass<USkillComponent>();
	Manager->SkillComponent = Skills;
	FClassGrantedSkillState& State = GrantedClassSkills.FindOrAdd(ClassAsset);
	State.Instances.SetNum(ClassAsset->GrantedSkills.Num());
	bool bAllGranted = true;
	for (int32 Index = 0; Index < ClassAsset->GrantedSkills.Num(); ++Index)
	{
		const FClassGrantedSkill& Grant = ClassAsset->GrantedSkills[Index];
		if (GetClassLevel(ClassAsset) < Grant.TargetClassLevel
			|| Manager->IsSkillIDUnlocked(Grant.SkillData->SkillID)) continue;
		USkillBase* Skill = State.Instances[Index];
		if (!Skill)
		{
			Skill = NewObject<USkillBase>(this, Grant.SkillClass);
			if (Skill)
			{
				Skill->SkillData = Grant.SkillData;
				State.Instances[Index] = Skill;
			}
		}
		if (!Skill || !Manager->UnlockSkill(Skill))
		{
			UE_LOG(LogTemp, Warning, TEXT("ClassComponent - Could not grant %s from %s"),
				*Grant.SkillData->SkillID.ToString(), *ClassAsset->ClassName.ToString());
			bAllGranted = false;
		}
	}
	return bAllGranted;
}

void UClassComponent::RevokeClassSkills(UClassDataAsset* ClassAsset)
{
	FClassGrantedSkillState* State = GrantedClassSkills.Find(ClassAsset);
	USkillManagerComponent* Manager = GetOwner() ? GetOwner()->FindComponentByClass<USkillManagerComponent>() : nullptr;
	if (!State || !Manager) return;
	for (USkillBase* Skill : State->Instances)
	{
		if (Skill && Manager->IsSkillUnlocked(Skill))
		{
			Manager->RevokeSkill(Skill);
		}
	}
}

void UClassComponent::HandleSkillRevoked(USkillBase* Skill)
{
	for (UClassDataAsset* ClassAsset : GetSelectedClasses())
	{
		GrantClassSkills(ClassAsset);
	}
}

bool UClassComponent::GrantProfessionSkill(UProfessionDataAsset* Profession)
{
	if (!Profession)
	{
		return false;
	}
	if (!Profession->GrantedSkillClass && !Profession->GrantedSkillData)
	{
		return true; // Legacy professions without a skill remain selectable.
	}
	if (!Profession->GrantedSkillClass || !Profession->GrantedSkillData || !GetOwner())
	{
		return false;
	}
	if (GrantedProfessionSkills.Contains(Profession))
	{
		return true;
	}
	USkillManagerComponent* Manager = GetOwner()->FindComponentByClass<USkillManagerComponent>();
	USkillComponent* Skills = GetOwner()->FindComponentByClass<USkillComponent>();
	if (!Manager || !Skills)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassComponent - Cannot grant profession skill without skill components"));
		return false;
	}
	Manager->SkillComponent = Skills;
	USkillBase* Skill = NewObject<USkillBase>(this, Profession->GrantedSkillClass);
	if (!Skill)
	{
		return false;
	}
	Skill->SkillData = Profession->GrantedSkillData;
	if (!Manager->UnlockSkill(Skill))
	{
		return false;
	}
	GrantedProfessionSkills.Add(Profession, Skill);
	return true;
}

void UClassComponent::RevokeProfessionSkill(UProfessionDataAsset* Profession)
{
	TObjectPtr<USkillBase> Skill;
	if (!Profession || !GrantedProfessionSkills.RemoveAndCopyValue(Profession, Skill))
	{
		return;
	}
	if (GetOwner())
	{
		if (USkillManagerComponent* Manager = GetOwner()->FindComponentByClass<USkillManagerComponent>())
		{
			Manager->RevokeSkill(Skill);
		}
	}
}

void UClassComponent::UpdateAppliedBonuses()
{
	if (GetOwner())
	{
		if (!AttributeComponent)
		{
			AttributeComponent = GetOwner()->FindComponentByClass<UAttributeComponent>();
		}
		if (!SecondaryAttributeComponent)
		{
			SecondaryAttributeComponent = GetOwner()->FindComponentByClass<USecondaryAttributeComponent>();
		}
	}
	if (AttributeComponent)
	{
		TMap<EPrimaryAttribute, float> SelectedPrimaryBonuses;
		TMap<EPrimaryAttribute, float> EarnedPrimaryBonuses;
		for (const TObjectPtr<UClassDataAsset>& ClassAsset : SelectedClasses)
		{
			if (!ClassAsset)
			{
				continue;
			}
			for (const TPair<EPrimaryAttribute, float>& Bonus : ClassAsset->PrimaryAttributeBonuses)
			{
				SelectedPrimaryBonuses.FindOrAdd(Bonus.Key) += Bonus.Value;
			}
		}
		for (const auto& ProgressPair : ClassProgress)
		{
			const UClassDataAsset* ClassAsset = ProgressPair.Key.Get();
			if (!ClassAsset || ProgressPair.Value.Level <= 0)
			{
				continue;
			}
			for (const TPair<EPrimaryAttribute, float>& Gain : ClassAsset->AttributeGainsPerLevel)
			{
				if (FMath::IsFinite(Gain.Value) && Gain.Value >= 0.0f)
				{
					EarnedPrimaryBonuses.FindOrAdd(Gain.Key) += Gain.Value * ProgressPair.Value.Level;
				}
			}
		}
		for (const auto& ProgressPair : ProfessionProgress)
		{
			const UProfessionDataAsset* ProfessionAsset = ProgressPair.Key.Get();
			if (!ProfessionAsset || ProgressPair.Value.Level <= 0)
			{
				continue;
			}
			for (const TPair<EPrimaryAttribute, float>& Gain : ProfessionAsset->AttributeGainsPerLevel)
			{
				if (FMath::IsFinite(Gain.Value) && Gain.Value >= 0.0f)
				{
					EarnedPrimaryBonuses.FindOrAdd(Gain.Key) += Gain.Value * ProgressPair.Value.Level;
				}
			}
		}
		AttributeComponent->SetClassProgressionBonuses(SelectedPrimaryBonuses, EarnedPrimaryBonuses);
	}

	if (SecondaryAttributeComponent)
	{
		TMap<ESecondaryAttribute, float> SecondaryBonuses;
		for (const TObjectPtr<UClassDataAsset>& ClassAsset : SelectedClasses)
		{
			if (!ClassAsset)
			{
				continue;
			}
			for (const TPair<ESecondaryAttribute, float>& Bonus : ClassAsset->SecondaryAttributeBonuses)
			{
				SecondaryBonuses.FindOrAdd(Bonus.Key) += Bonus.Value;
			}
		}
		SecondaryAttributeComponent->SetClassBonuses(SecondaryBonuses);
	}
}

bool UClassComponent::CanAddClass() const
{
	return MaxClasses <= 0 || SelectedClasses.Num() < MaxClasses;
}

bool UClassComponent::CanAddProfession() const
{
	return MaxProfessions <= 0 || SelectedProfessions.Num() < MaxProfessions;
}

float UClassComponent::GetExperienceRequiredForLevel(UClassDataAsset* ClassAsset, int32 Level) const
{
	if (ClassAsset && ClassAsset->ClassExperienceCurve)
	{
		const float CurveValue = ClassAsset->ClassExperienceCurve->GetFloatValue(static_cast<float>(Level));
		if (FMath::IsFinite(CurveValue) && CurveValue > 0.0f)
		{
			return CurveValue;
		}

		UE_LOG(LogTemp, Warning, TEXT("ClassComponent - Invalid XP curve value for %s at level %d; using default progression"), *ClassAsset->GetName(), Level);
	}

	return BaseExperienceToLevel + (ExperiencePerLevel * FMath::Max(0, Level - 1));
}
