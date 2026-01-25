// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Progression/CharacterStatusPanelWidget.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Components/Progression/ClassComponent.h"
#include "Progression/Core/ClassDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"

namespace
{
	FText JoinNames(const TArray<FString>& Names)
	{
		if (Names.Num() == 0)
		{
			return FText::FromString(TEXT("None"));
		}

		return FText::FromString(FString::Join(Names, TEXT(", ")));
	}
}

void UCharacterStatusPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindComponents();
	UpdateStatusDisplay();
}

void UCharacterStatusPanelWidget::NativeDestruct()
{
	if (ExperienceComponent)
	{
		ExperienceComponent->OnExperienceChanged.RemoveDynamic(this, &UCharacterStatusPanelWidget::HandleExperienceChanged);
	}

	if (ClassComponent)
	{
		ClassComponent->OnClassListChanged.RemoveDynamic(this, &UCharacterStatusPanelWidget::HandleClassListChanged);
		ClassComponent->OnProfessionListChanged.RemoveDynamic(this, &UCharacterStatusPanelWidget::HandleProfessionListChanged);
		ClassComponent->OnClassLevelChanged.RemoveDynamic(this, &UCharacterStatusPanelWidget::HandleClassLevelChanged);
	}

	Super::NativeDestruct();
}

void UCharacterStatusPanelWidget::BindComponents()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterStatusPanelWidget - No owning pawn found."));
		return;
	}

	ExperienceComponent = OwningPawn->FindComponentByClass<UExperienceComponent>();
	ClassComponent = OwningPawn->FindComponentByClass<UClassComponent>();

	if (ExperienceComponent)
	{
		ExperienceComponent->OnExperienceChanged.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleExperienceChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterStatusPanelWidget - ExperienceComponent not found on owning pawn."));
	}

	if (ClassComponent)
	{
		ClassComponent->OnClassListChanged.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleClassListChanged);
		ClassComponent->OnProfessionListChanged.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleProfessionListChanged);
		ClassComponent->OnClassLevelChanged.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleClassLevelChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterStatusPanelWidget - ClassComponent not found on owning pawn."));
	}
}

void UCharacterStatusPanelWidget::HandleExperienceChanged(float NewUnallocatedExperience, float Delta)
{
	UpdateStatusDisplay();
}

void UCharacterStatusPanelWidget::HandleClassListChanged(UClassDataAsset* ChangedClass, bool bAdded)
{
	UpdateStatusDisplay();
}

void UCharacterStatusPanelWidget::HandleProfessionListChanged(UProfessionDataAsset* ChangedProfession, bool bAdded)
{
	UpdateStatusDisplay();
}

void UCharacterStatusPanelWidget::HandleClassLevelChanged(UClassDataAsset* ClassAsset, int32 NewLevel)
{
	UpdateStatusDisplay();
}

void UCharacterStatusPanelWidget::UpdateStatusDisplay()
{
	if (CurrentClassText)
	{
		CurrentClassText->SetText(GetCurrentClassName());
	}

	if (CurrentProfessionText)
	{
		CurrentProfessionText->SetText(GetCurrentProfessionName());
	}

	if (SelectedClassesText)
	{
		SelectedClassesText->SetText(GetSelectedClassesText());
	}

	if (SelectedProfessionsText)
	{
		SelectedProfessionsText->SetText(GetSelectedProfessionsText());
	}

	if (ClassSlotsText)
	{
		ClassSlotsText->SetText(GetClassSlotsText());
	}

	if (ProfessionSlotsText)
	{
		ProfessionSlotsText->SetText(GetProfessionSlotsText());
	}

	if (PlayerLevelText)
	{
		PlayerLevelText->SetText(FText::AsNumber(GetPlayerLevel()));
	}

	if (UnallocatedExperienceText)
	{
		UnallocatedExperienceText->SetText(FText::AsNumber(FMath::RoundToInt(GetUnallocatedExperience())));
	}

	if (TotalExperienceText)
	{
		TotalExperienceText->SetText(FText::AsNumber(FMath::RoundToInt(GetTotalExperience())));
	}
}

float UCharacterStatusPanelWidget::GetUnallocatedExperience() const
{
	return ExperienceComponent ? ExperienceComponent->GetUnallocatedExperience() : 0.0f;
}

float UCharacterStatusPanelWidget::GetTotalExperience() const
{
	return ExperienceComponent ? ExperienceComponent->GetTotalExperience() : 0.0f;
}

int32 UCharacterStatusPanelWidget::GetPlayerLevel() const
{
	return ClassComponent ? ClassComponent->GetPlayerLevel() : 0;
}

FText UCharacterStatusPanelWidget::GetCurrentClassName() const
{
	if (ClassComponent)
	{
		if (UClassDataAsset* CurrentClass = ClassComponent->GetCurrentClass())
		{
			return CurrentClass->ClassName;
		}
	}

	return FText::FromString(TEXT("None"));
}

FText UCharacterStatusPanelWidget::GetCurrentProfessionName() const
{
	if (ClassComponent)
	{
		if (UProfessionDataAsset* CurrentProfession = ClassComponent->GetCurrentProfession())
		{
			return CurrentProfession->ProfessionName;
		}
	}

	return FText::FromString(TEXT("None"));
}

FText UCharacterStatusPanelWidget::GetSelectedClassesText() const
{
	return BuildClassListText();
}

FText UCharacterStatusPanelWidget::GetSelectedProfessionsText() const
{
	return BuildProfessionListText();
}

FText UCharacterStatusPanelWidget::GetClassSlotsText() const
{
	if (!ClassComponent)
	{
		return FText::FromString(TEXT("0 / 0"));
	}

	const int32 CurrentCount = ClassComponent->GetSelectedClasses().Num();
	const int32 MaxCount = ClassComponent->MaxClasses;
	return FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCount, MaxCount));
}

FText UCharacterStatusPanelWidget::GetProfessionSlotsText() const
{
	if (!ClassComponent)
	{
		return FText::FromString(TEXT("0 / 0"));
	}

	const int32 CurrentCount = ClassComponent->GetSelectedProfessions().Num();
	const int32 MaxCount = ClassComponent->MaxProfessions;
	return FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCount, MaxCount));
}

FText UCharacterStatusPanelWidget::BuildClassListText() const
{
	if (!ClassComponent)
	{
		return FText::FromString(TEXT("None"));
	}

	TArray<FString> Names;
	for (UClassDataAsset* ClassAsset : ClassComponent->GetSelectedClasses())
	{
		if (ClassAsset)
		{
			Names.Add(ClassAsset->ClassName.ToString());
		}
	}

	return JoinNames(Names);
}

FText UCharacterStatusPanelWidget::BuildProfessionListText() const
{
	if (!ClassComponent)
	{
		return FText::FromString(TEXT("None"));
	}

	TArray<FString> Names;
	for (UProfessionDataAsset* ProfessionAsset : ClassComponent->GetSelectedProfessions())
	{
		if (ProfessionAsset)
		{
			Names.Add(ProfessionAsset->ProfessionName.ToString());
		}
	}

	return JoinNames(Names);
}
