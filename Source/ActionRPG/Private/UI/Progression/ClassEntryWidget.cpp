// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Progression/ClassEntryWidget.h"
#include "Components/Progression/ClassComponent.h"
#include "Progression/Core/ClassDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"

void UClassEntryWidget::SetClassEntry(UClassDataAsset* InClassAsset, UClassComponent* InClassComponent)
{
	ClassAsset = InClassAsset;
	ClassComponent = InClassComponent;
	ProfessionAsset = nullptr;
	bIsProfessionEntry = false;
}

void UClassEntryWidget::SetProfessionEntry(UProfessionDataAsset* InProfessionAsset, UClassComponent* InClassComponent)
{
	ProfessionAsset = InProfessionAsset;
	ClassComponent = InClassComponent;
	ClassAsset = nullptr;
	bIsProfessionEntry = true;
}

FText UClassEntryWidget::GetEntryName() const
{
	if (ClassAsset)
	{
		return ClassAsset->ClassName;
	}

	if (ProfessionAsset)
	{
		return ProfessionAsset->ProfessionName;
	}

	return FText::GetEmpty();
}

FText UClassEntryWidget::GetEntryDescription() const
{
	if (ClassAsset)
	{
		return ClassAsset->ClassDescription;
	}

	if (ProfessionAsset)
	{
		return ProfessionAsset->ProfessionDescription;
	}

	return FText::GetEmpty();
}

UTexture2D* UClassEntryWidget::GetEntryIcon() const
{
	if (ClassAsset)
	{
		return ClassAsset->ClassIcon;
	}

	if (ProfessionAsset)
	{
		return ProfessionAsset->ProfessionIcon;
	}

	return nullptr;
}

int32 UClassEntryWidget::GetEntryLevel() const
{
	if (ClassAsset && ClassComponent)
	{
		return ClassComponent->GetClassLevel(ClassAsset);
	}

	return 0;
}

float UClassEntryWidget::GetEntryExperience() const
{
	if (ClassAsset && ClassComponent)
	{
		return ClassComponent->GetClassExperience(ClassAsset);
	}

	return 0.0f;
}

float UClassEntryWidget::GetEntryExperienceToNextLevel() const
{
	if (ClassAsset && ClassComponent)
	{
		return ClassComponent->GetClassExperienceToNextLevel(ClassAsset);
	}

	return 0.0f;
}

bool UClassEntryWidget::IsSelected() const
{
	if (!ClassComponent)
	{
		return false;
	}

	if (ClassAsset)
	{
		return ClassComponent->GetSelectedClasses().Contains(ClassAsset);
	}

	if (ProfessionAsset)
	{
		return ClassComponent->GetSelectedProfessions().Contains(ProfessionAsset);
	}

	return false;
}
