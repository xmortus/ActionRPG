// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Skills/UnlockedSkillsPanelWidget.h"
#include "Components/UniformGridPanel.h"
#include "UI/Skills/UnlockedSkillEntryWidget.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "GameFramework/PlayerController.h"

void UUnlockedSkillsPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SkillManagerComponent = GetSkillManagerComponent();
	if (!SkillManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnlockedSkillsPanelWidget::NativeConstruct - SkillManagerComponent not found"));
	}
	else
	{
		SkillManagerComponent->OnSkillUnlocked.AddDynamic(this, &UUnlockedSkillsPanelWidget::OnSkillUnlockedInternal);
	}

	RefreshUnlockedSkills();
}

void UUnlockedSkillsPanelWidget::NativeDestruct()
{
	if (SkillManagerComponent)
	{
		SkillManagerComponent->OnSkillUnlocked.RemoveAll(this);
	}

	EntryWidgets.Empty();

	Super::NativeDestruct();
}

void UUnlockedSkillsPanelWidget::RefreshUnlockedSkills()
{
	if (!SkillGrid)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnlockedSkillsPanelWidget::RefreshUnlockedSkills - SkillGrid not bound"));
		return;
	}

	if (!EntryWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnlockedSkillsPanelWidget::RefreshUnlockedSkills - EntryWidgetClass not set"));
		return;
	}

	if (!SkillManagerComponent)
	{
		SkillManagerComponent = GetSkillManagerComponent();
	}

	SkillGrid->ClearChildren();
	EntryWidgets.Empty();

	const TArray<USkillBase*> UnlockedSkills = SkillManagerComponent ? SkillManagerComponent->GetUnlockedSkills() : TArray<USkillBase*>();

	const int32 Columns = 4;
	int32 Row = 0;
	int32 Col = 0;

	for (USkillBase* Skill : UnlockedSkills)
	{
		if (!Skill)
		{
			continue;
		}

		UUserWidget* UserWidget = CreateWidget<UUserWidget>(this, EntryWidgetClass);
		UUnlockedSkillEntryWidget* EntryWidget = Cast<UUnlockedSkillEntryWidget>(UserWidget);
		if (!EntryWidget)
		{
			continue;
		}

		EntryWidget->SetSkillData(Skill, this);
		SkillGrid->AddChildToUniformGrid(EntryWidget, Row, Col);
		EntryWidgets.Add(EntryWidget);

		Col++;
		if (Col >= Columns)
		{
			Col = 0;
			Row++;
		}
	}
}

void UUnlockedSkillsPanelWidget::SetPendingAssignSlotIndex(int32 SlotIndex)
{
	PendingAssignSlotIndex = SlotIndex;
}

bool UUnlockedSkillsPanelWidget::AssignSkillToSlot(USkillBase* Skill, int32 SlotIndex)
{
	if (!SkillManagerComponent || !Skill)
	{
		return false;
	}

	if (SlotIndex < 0 || SlotIndex > 7)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnlockedSkillsPanelWidget::AssignSkillToSlot - Invalid slot index %d"), SlotIndex);
		return false;
	}

	return SkillManagerComponent->AssignSkillToSlot(SlotIndex, Skill);
}

bool UUnlockedSkillsPanelWidget::AssignSkillToPendingSlot(USkillBase* Skill)
{
	if (!Skill)
	{
		return false;
	}

	int32 TargetSlot = PendingAssignSlotIndex;
	if (TargetSlot < 0 || TargetSlot > 7)
	{
		TargetSlot = FindFirstEmptySkillSlot();
	}

	if (TargetSlot < 0 || TargetSlot > 7)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnlockedSkillsPanelWidget::AssignSkillToPendingSlot - No valid slot available"));
		return false;
	}

	return AssignSkillToSlot(Skill, TargetSlot);
}

bool UUnlockedSkillsPanelWidget::AssignSkillToMainHand(USkillBase* Skill)
{
	if (!SkillManagerComponent || !Skill)
	{
		return false;
	}

	return SkillManagerComponent->AssignMainHandSkill(Skill);
}

bool UUnlockedSkillsPanelWidget::AssignSkillToOffhand(USkillBase* Skill)
{
	if (!SkillManagerComponent || !Skill)
	{
		return false;
	}

	return SkillManagerComponent->AssignOffhandSkill(Skill);
}

void UUnlockedSkillsPanelWidget::OnSkillUnlockedInternal(USkillBase* Skill)
{
	RefreshUnlockedSkills();
}

USkillManagerComponent* UUnlockedSkillsPanelWidget::GetSkillManagerComponent() const
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AActionRPGPlayerCharacter* PlayerCharacter = Cast<AActionRPGPlayerCharacter>(PC->GetPawn()))
		{
			return PlayerCharacter->FindComponentByClass<USkillManagerComponent>();
		}
	}

	return nullptr;
}

int32 UUnlockedSkillsPanelWidget::FindFirstEmptySkillSlot() const
{
	if (!SkillManagerComponent)
	{
		return INDEX_NONE;
	}

	for (int32 SlotIndex = 0; SlotIndex < 8; SlotIndex++)
	{
		if (!SkillManagerComponent->GetSkillAtSlot(SlotIndex))
		{
			return SlotIndex;
		}
	}

	return INDEX_NONE;
}
