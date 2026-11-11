// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Progression/ClassEntryWidget.h"
#include "Components/Progression/ClassComponent.h"
#include "Progression/Core/ClassDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "UI/Progression/ClassSelectionPanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void UClassEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!WidgetTree || WidgetTree->RootWidget) return;
	Button_SelectClass = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Button_SelectClass"));
	WidgetTree->RootWidget = Button_SelectClass;
	USizeBox* RowSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("EntrySize"));
	RowSize->SetMinDesiredHeight(62.0f);
	Button_SelectClass->SetContent(RowSize);
	RowText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EntryText"));
	RowText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	RowText->SetAutoWrapText(true);
	FSlateFontInfo Font = RowText->GetFont();
	Font.Size = 16;
	RowText->SetFont(Font);
	RowSize->SetContent(RowText);
	RefreshRow();
}

void UClassEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Button_SelectClass)
	{
		Button_SelectClass->OnClicked.AddUniqueDynamic(this, &UClassEntryWidget::HandleSelectionClicked);
	}
}

void UClassEntryWidget::NativeDestruct()
{
	if (Button_SelectClass)
	{
		Button_SelectClass->OnClicked.RemoveDynamic(this, &UClassEntryWidget::HandleSelectionClicked);
	}
	Super::NativeDestruct();
}

void UClassEntryWidget::HandleSelectionClicked()
{
	if (!ParentPanel)
	{
		return;
	}

	if (ProfessionAsset)
	{
		ParentPanel->SetSelectedProfessionForXp(ProfessionAsset);
	}
	else if (ClassAsset)
	{
		ParentPanel->SetSelectedClassForXp(ClassAsset);
	}
}

void UClassEntryWidget::SetClassEntry(UClassDataAsset* InClassAsset, UClassComponent* InClassComponent)
{
	ClassAsset = InClassAsset;
	ClassComponent = InClassComponent;
	ProfessionAsset = nullptr;
	bIsProfessionEntry = false;
	RefreshRow();
}

void UClassEntryWidget::SetProfessionEntry(UProfessionDataAsset* InProfessionAsset, UClassComponent* InClassComponent)
{
	ProfessionAsset = InProfessionAsset;
	ClassComponent = InClassComponent;
	ClassAsset = nullptr;
	bIsProfessionEntry = true;
	RefreshRow();
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
	if (ProfessionAsset && ClassComponent)
	{
		return ClassComponent->GetProfessionLevel(ProfessionAsset);
	}
	if (ClassAsset && ClassComponent)
	{
		return ClassComponent->GetClassLevel(ClassAsset);
	}

	return 0;
}

float UClassEntryWidget::GetEntryExperience() const
{
	if (ProfessionAsset && ClassComponent)
	{
		return ClassComponent->GetProfessionExperience(ProfessionAsset);
	}
	if (ClassAsset && ClassComponent)
	{
		return ClassComponent->GetClassExperience(ClassAsset);
	}

	return 0.0f;
}

float UClassEntryWidget::GetEntryExperienceToNextLevel() const
{
	if (ProfessionAsset && ClassComponent)
	{
		return ClassComponent->GetProfessionExperienceToNextLevel(ProfessionAsset);
	}
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

void UClassEntryWidget::SetSelectedForXp(bool bSelected)
{
	bIsSelectedForXp = bSelected;
	RefreshRow();
}

void UClassEntryWidget::RefreshRow()
{
	if (!RowText || !Button_SelectClass) return;
	const int32 Level = GetEntryLevel();
	const float Experience = GetEntryExperience();
	const float Required = GetEntryExperienceToNextLevel();
	const bool bActive = IsSelected();
	const TCHAR* State = bActive ? TEXT("ACTIVE") : Level > 0 ? TEXT("UNLOCKED") : TEXT("LOCKED");
	RowText->SetText(FText::FromString(FString::Printf(TEXT("%s    Lv %d    %.0f / %.0f XP\n%s"),
		*GetEntryName().ToString(), Level, Experience, Required, State)));
	Button_SelectClass->SetBackgroundColor(bIsSelectedForXp
		? FLinearColor(0.16f, 0.39f, 0.58f, 1.0f)
		: bActive ? FLinearColor(0.12f, 0.31f, 0.25f, 1.0f)
		: FLinearColor(0.11f, 0.16f, 0.23f, 1.0f));
}
