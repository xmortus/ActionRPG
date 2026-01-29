// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Progression/ClassComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Progression/Core/ClassDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Progression/Core/ProgressionTypes.h"

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
}

bool UClassComponent::SelectClass(UClassDataAsset* NewClass)
{
	ClearClass();
	return AddClass(NewClass);
}

bool UClassComponent::SelectProfession(UProfessionDataAsset* NewProfession)
{
	ClearProfession();
	return AddProfession(NewProfession);
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
		return true;
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

	SelectedClasses.Add(NewClass);
	if (!ClassProgress.Contains(NewClass))
	{
		ClassProgress.Add(NewClass, FClassProgress());
	}
	CurrentClass = NewClass;
	UpdateAppliedBonuses();
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

	SelectedProfessions.Add(NewProfession);
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

	for (const TObjectPtr<UClassDataAsset>& ClassAsset : SelectedClasses)
	{
		if (ClassAsset)
		{
			OnClassListChanged.Broadcast(ClassAsset, false);
		}
	}

	SelectedClasses.Reset();
	CurrentClass = nullptr;
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

	return FMath::Max(1, TotalLevel);
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
	const int32 Level = GetClassLevel(ClassAsset);
	if (Level <= 0)
	{
		return 0.0f;
	}

	return GetExperienceRequiredForLevel(ClassAsset, Level);
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
	if (!ClassAsset || Amount <= 0.0f)
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
	OnClassExperienceChanged.Broadcast(ClassAsset, Progress.Experience);

	float Required = GetExperienceRequiredForLevel(ClassAsset, FMath::Max(1, Progress.Level));
	while (Progress.Experience >= Required)
	{
		Progress.Experience -= Required;
		Progress.Level = FMath::Max(1, Progress.Level + 1);
		OnClassLevelChanged.Broadcast(ClassAsset, Progress.Level);
		const int32 PointsPerLevel = ClassAsset ? ClassAsset->AttributePointsPerLevel : 0;
		if (AttributeComponent && PointsPerLevel > 0)
		{
			AttributeComponent->AddAttributePoints(PointsPerLevel);
		}
		Required = GetExperienceRequiredForLevel(ClassAsset, FMath::Max(1, Progress.Level));
	}

	if (!bIsSelected && ClassAsset && Progress.Level >= ClassAsset->RequiredClassLevelToUnlock)
	{
		AddClass(ClassAsset);
	}

	return true;
}

bool UClassComponent::MeetsRequirements(int32 RequiredLevel, const TMap<EPrimaryAttribute, float>& RequiredAttributes) const
{
	const int32 CurrentLevel = GetPlayerLevel();
	if (CurrentLevel < RequiredLevel)
	{
		return false;
	}

	if (AttributeComponent)
	{
		for (const TPair<EPrimaryAttribute, float>& Requirement : RequiredAttributes)
		{
			if (AttributeComponent->GetAttribute(Requirement.Key) < Requirement.Value)
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

void UClassComponent::UpdateAppliedBonuses()
{
	if (AttributeComponent)
	{
		TMap<EPrimaryAttribute, float> PrimaryBonuses;
		for (const TObjectPtr<UClassDataAsset>& ClassAsset : SelectedClasses)
		{
			if (!ClassAsset)
			{
				continue;
			}
			for (const TPair<EPrimaryAttribute, float>& Bonus : ClassAsset->PrimaryAttributeBonuses)
			{
				PrimaryBonuses.FindOrAdd(Bonus.Key) += Bonus.Value;
			}
		}
		for (const TObjectPtr<UProfessionDataAsset>& ProfessionAsset : SelectedProfessions)
		{
			if (!ProfessionAsset)
			{
				continue;
			}
			for (const TPair<EPrimaryAttribute, float>& Bonus : ProfessionAsset->PrimaryAttributeBonuses)
			{
				PrimaryBonuses.FindOrAdd(Bonus.Key) += Bonus.Value;
			}
		}

		AttributeComponent->SetClassBonuses(PrimaryBonuses);
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
		for (const TObjectPtr<UProfessionDataAsset>& ProfessionAsset : SelectedProfessions)
		{
			if (!ProfessionAsset)
			{
				continue;
			}
			for (const TPair<ESecondaryAttribute, float>& Bonus : ProfessionAsset->SecondaryAttributeBonuses)
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
		return FMath::Max(0.0f, CurveValue);
	}

	return BaseExperienceToLevel + (ExperiencePerLevel * FMath::Max(0, Level - 1));
}
