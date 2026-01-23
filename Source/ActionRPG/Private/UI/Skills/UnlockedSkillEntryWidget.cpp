// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Skills/UnlockedSkillEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Skills/Core/SkillBase.h"
#include "UI/Skills/UnlockedSkillsPanelWidget.h"

void UUnlockedSkillEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AssignButton)
	{
		AssignButton->OnClicked.RemoveDynamic(this, &UUnlockedSkillEntryWidget::OnAssignClicked);
		AssignButton->OnClicked.AddDynamic(this, &UUnlockedSkillEntryWidget::OnAssignClicked);
	}

	if (AssignMainHandButton)
	{
		AssignMainHandButton->OnClicked.RemoveDynamic(this, &UUnlockedSkillEntryWidget::OnAssignMainHandClicked);
		AssignMainHandButton->OnClicked.AddDynamic(this, &UUnlockedSkillEntryWidget::OnAssignMainHandClicked);
	}

	if (AssignOffhandButton)
	{
		AssignOffhandButton->OnClicked.RemoveDynamic(this, &UUnlockedSkillEntryWidget::OnAssignOffhandClicked);
		AssignOffhandButton->OnClicked.AddDynamic(this, &UUnlockedSkillEntryWidget::OnAssignOffhandClicked);
	}
}

void UUnlockedSkillEntryWidget::SetSkillData(USkillBase* InSkill, UUnlockedSkillsPanelWidget* InParentPanel)
{
	CurrentSkill = InSkill;
	ParentPanel = InParentPanel;

	if (SkillIcon)
	{
		if (CurrentSkill && CurrentSkill->SkillData && CurrentSkill->SkillData->SkillIcon)
		{
			SkillIcon->SetBrushFromTexture(CurrentSkill->SkillData->SkillIcon);
			SkillIcon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			SkillIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (SkillNameText)
	{
		SkillNameText->SetText(CurrentSkill && CurrentSkill->SkillData ? CurrentSkill->SkillData->SkillName : FText::GetEmpty());
	}
}

void UUnlockedSkillEntryWidget::OnAssignClicked()
{
	if (!CurrentSkill || !ParentPanel)
	{
		return;
	}

	ParentPanel->AssignSkillToPendingSlot(CurrentSkill);
}

void UUnlockedSkillEntryWidget::OnAssignMainHandClicked()
{
	if (!CurrentSkill || !ParentPanel)
	{
		return;
	}

	ParentPanel->AssignSkillToMainHand(CurrentSkill);
}

void UUnlockedSkillEntryWidget::OnAssignOffhandClicked()
{
	if (!CurrentSkill || !ParentPanel)
	{
		return;
	}

	ParentPanel->AssignSkillToOffhand(CurrentSkill);
}
