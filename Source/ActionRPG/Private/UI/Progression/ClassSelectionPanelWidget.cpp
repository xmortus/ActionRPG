// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Progression/ClassSelectionPanelWidget.h"
#include "Components/Progression/ClassComponent.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Progression/Core/ClassDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Data/ClassDatabase.h"
#include "GameFramework/Pawn.h"

void UClassSelectionPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindComponents();
	RefreshAllClassesList();
	RefreshClassList();

	if (ClassComponent)
	{
		OnPlayerLevelUpdated(ClassComponent->GetPlayerLevel());
	}
}

void UClassSelectionPanelWidget::NativeDestruct()
{
	if (ClassComponent)
	{
		ClassComponent->OnClassListChanged.RemoveDynamic(this, &UClassSelectionPanelWidget::HandleClassListChanged);
		ClassComponent->OnProfessionListChanged.RemoveDynamic(this, &UClassSelectionPanelWidget::HandleProfessionListChanged);
		ClassComponent->OnClassExperienceChanged.RemoveDynamic(this, &UClassSelectionPanelWidget::HandleClassExperienceChanged);
		ClassComponent->OnClassLevelChanged.RemoveDynamic(this, &UClassSelectionPanelWidget::HandleClassLevelChanged);
	}

	Super::NativeDestruct();
}

void UClassSelectionPanelWidget::BindComponents()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassSelectionPanelWidget - No owning pawn found."));
		return;
	}

	ClassComponent = OwningPawn->FindComponentByClass<UClassComponent>();
	ExperienceComponent = OwningPawn->FindComponentByClass<UExperienceComponent>();

	if (!ClassComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassSelectionPanelWidget - ClassComponent not found on owning pawn."));
		return;
	}

	ClassComponent->OnClassListChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleClassListChanged);
	ClassComponent->OnProfessionListChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleProfessionListChanged);
	ClassComponent->OnClassExperienceChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleClassExperienceChanged);
	ClassComponent->OnClassLevelChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleClassLevelChanged);
}

void UClassSelectionPanelWidget::RefreshClassList()
{
	if (!ClassComponent)
	{
		return;
	}

	OnClassListUpdated(ClassComponent->GetSelectedClasses(), ClassComponent->GetSelectedProfessions());
}

void UClassSelectionPanelWidget::RefreshAllClassesList()
{
	OnAllClassesListUpdated(GetAllClasses());
}

void UClassSelectionPanelWidget::SetSelectedClassForXp(UClassDataAsset* ClassAsset)
{
	SelectedClassForXp = ClassAsset;
	OnSelectedClassForXpChanged(SelectedClassForXp);
}

bool UClassSelectionPanelWidget::AllocateExperienceToSelectedClass(float Amount)
{
	return AllocateExperienceToClassAllowUnselected(SelectedClassForXp, Amount);
}

bool UClassSelectionPanelWidget::AllocateExperienceToClass(UClassDataAsset* ClassAsset, float Amount)
{
	return ClassComponent ? ClassComponent->AllocateExperienceToClass(ClassAsset, Amount) : false;
}

bool UClassSelectionPanelWidget::AllocateExperienceToClassAllowUnselected(UClassDataAsset* ClassAsset, float Amount)
{
	return ClassComponent ? ClassComponent->AllocateExperienceToClassAllowUnselected(ClassAsset, Amount) : false;
}

bool UClassSelectionPanelWidget::AddClass(UClassDataAsset* ClassAsset)
{
	return ClassComponent ? ClassComponent->AddClass(ClassAsset) : false;
}

bool UClassSelectionPanelWidget::AddProfession(UProfessionDataAsset* ProfessionAsset)
{
	return ClassComponent ? ClassComponent->AddProfession(ProfessionAsset) : false;
}

bool UClassSelectionPanelWidget::RemoveClass(UClassDataAsset* ClassAsset)
{
	return ClassComponent ? ClassComponent->RemoveClass(ClassAsset) : false;
}

bool UClassSelectionPanelWidget::RemoveProfession(UProfessionDataAsset* ProfessionAsset)
{
	return ClassComponent ? ClassComponent->RemoveProfession(ProfessionAsset) : false;
}

int32 UClassSelectionPanelWidget::GetPlayerLevel() const
{
	return ClassComponent ? ClassComponent->GetPlayerLevel() : 0;
}

float UClassSelectionPanelWidget::GetUnallocatedExperience() const
{
	return ExperienceComponent ? ExperienceComponent->GetUnallocatedExperience() : 0.0f;
}

TArray<UClassDataAsset*> UClassSelectionPanelWidget::GetSelectedClasses() const
{
	return ClassComponent ? ClassComponent->GetSelectedClasses() : TArray<UClassDataAsset*>();
}

FText UClassSelectionPanelWidget::GetSelectedClassesText() const
{
	if (!ClassComponent)
	{
		return FText::FromString(TEXT("None"));
	}

	return ClassComponent->GetSelectedClassesText();
}

TArray<UClassDataAsset*> UClassSelectionPanelWidget::GetAllClasses() const
{
	if (UClassDatabase* Database = UClassDatabase::Get())
	{
		return Database->GetAllClasses();
	}

	return TArray<UClassDataAsset*>();
}

TArray<UProfessionDataAsset*> UClassSelectionPanelWidget::GetSelectedProfessions() const
{
	return ClassComponent ? ClassComponent->GetSelectedProfessions() : TArray<UProfessionDataAsset*>();
}

FText UClassSelectionPanelWidget::GetSelectedProfessionsText() const
{
	if (!ClassComponent)
	{
		return FText::FromString(TEXT("None"));
	}

	return ClassComponent->GetSelectedProfessionsText();
}

void UClassSelectionPanelWidget::HandleClassListChanged(UClassDataAsset* ChangedClass, bool bAdded)
{
	RefreshClassList();

	if (ClassComponent)
	{
		OnPlayerLevelUpdated(ClassComponent->GetPlayerLevel());
	}
}

void UClassSelectionPanelWidget::HandleProfessionListChanged(UProfessionDataAsset* ChangedProfession, bool bAdded)
{
	RefreshClassList();
}

void UClassSelectionPanelWidget::HandleClassExperienceChanged(UClassDataAsset* ClassAsset, float NewExperience)
{
	if (!ClassComponent || !ClassAsset)
	{
		return;
	}

	const int32 CurrentLevel = ClassComponent->GetClassLevel(ClassAsset);
	const float ExperienceToNext = ClassComponent->GetClassExperienceToNextLevel(ClassAsset);
	OnClassProgressUpdated(ClassAsset, CurrentLevel, NewExperience, ExperienceToNext);
}

void UClassSelectionPanelWidget::HandleClassLevelChanged(UClassDataAsset* ClassAsset, int32 NewLevel)
{
	if (!ClassComponent || !ClassAsset)
	{
		return;
	}

	const float CurrentExperience = ClassComponent->GetClassExperience(ClassAsset);
	const float ExperienceToNext = ClassComponent->GetClassExperienceToNextLevel(ClassAsset);
	OnClassProgressUpdated(ClassAsset, NewLevel, CurrentExperience, ExperienceToNext);
	OnPlayerLevelUpdated(ClassComponent->GetPlayerLevel());
}
