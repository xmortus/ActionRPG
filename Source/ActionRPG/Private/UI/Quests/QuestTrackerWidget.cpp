// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Quests/QuestTrackerWidget.h"
#include "UI/ActionRPGUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Progression/QuestComponent.h"
#include "Items/Core/ItemDataAsset.h"
#include "Quests/QuestDataAsset.h"

void UQuestTrackerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!WidgetTree || WidgetTree->RootWidget) return;
	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("QuestCanvas"));
	WidgetTree->RootWidget = Canvas;
	PanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("QuestPanelSize"));
	PanelSize->SetWidthOverride(310.0f);
	UCanvasPanelSlot* CanvasSlot = Canvas->AddChildToCanvas(PanelSize);
	CanvasSlot->SetAnchors(FAnchors(1.0f, 0.0f));
	CanvasSlot->SetAlignment(FVector2D(1.0f, 0.0f));
	CanvasSlot->SetPosition(FVector2D(-20.0f, 60.0f));
	CanvasSlot->SetAutoSize(true);
	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("QuestBorder"));
	Border->SetPadding(FMargin(12.0f));
	Border->SetBrushColor(ActionRPGUITheme::Panel());
	PanelSize->SetContent(Border);
	UVerticalBox* Rows = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("QuestRows"));
	Border->SetContent(Rows);
	auto AddText = [this, Rows](const TCHAR* Name, int32 Size) -> UTextBlock*
	{
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(Name));
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Text->SetAutoWrapText(true);
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = Size;
		Text->SetFont(Font);
		Rows->AddChildToVerticalBox(Text)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 7.0f));
		return Text;
	};
	TitleText = AddText(TEXT("QuestTitle"), 18);
	ObjectiveText = AddText(TEXT("QuestObjectives"), 14);
	RewardText = AddText(TEXT("QuestRewards"), 13);
	Refresh();
}

void UQuestTrackerWidget::BindQuestComponent(UQuestComponent* InQuests)
{
	if (Quests)
	{
		Quests->OnQuestChanged.RemoveDynamic(this, &UQuestTrackerWidget::HandleQuestChanged);
		Quests->OnQuestCompleted.RemoveDynamic(this, &UQuestTrackerWidget::HandleQuestCompleted);
	}
	Quests = InQuests;
	LastCompletedQuest = nullptr;
	if (Quests)
	{
		Quests->OnQuestChanged.AddUniqueDynamic(this, &UQuestTrackerWidget::HandleQuestChanged);
		Quests->OnQuestCompleted.AddUniqueDynamic(this, &UQuestTrackerWidget::HandleQuestCompleted);
	}
	Refresh();
}

void UQuestTrackerWidget::NativeDestruct()
{
	BindQuestComponent(nullptr);
	Super::NativeDestruct();
}

void UQuestTrackerWidget::Refresh()
{
	if (!PanelSize || !TitleText || !ObjectiveText || !RewardText) return;
	const UQuestDataAsset* Quest = Quests ? Quests->GetActiveQuest() : nullptr;
	if (!Quest)
	{
		PanelSize->SetVisibility(LastCompletedQuest ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		if (LastCompletedQuest)
		{
			TitleText->SetText(LastCompletedQuest->QuestName);
			ObjectiveText->SetText(FText::FromString(TEXT("Quest complete")));
			RewardText->SetText(FText::GetEmpty());
		}
		return;
	}
	PanelSize->SetVisibility(ESlateVisibility::Visible);
	TitleText->SetText(Quest->QuestName);
	const int32 Collected = FMath::Min(Quests->GetRequiredItemCount(), Quest->RequiredItemCount);
	FString Objectives = FString::Printf(TEXT("Collect %s: %d / %d"),
		Quest->RequiredItem ? *Quest->RequiredItem->ItemName.ToString() : TEXT("item"),
		Collected, Quest->RequiredItemCount);
	if (Quest->OptionalEnemyDefeatCount > 0)
	{
		Objectives += FString::Printf(TEXT("\nOptional: defeat %s: %d / %d"),
			*Quest->OptionalEnemyID.ToString(), Quests->GetOptionalDefeatCount(), Quest->OptionalEnemyDefeatCount);
	}
	Objectives += TEXT("\nReturn to the quest marker to claim rewards.");
	if (!Quests->GetLastNotice().IsEmpty())
	{
		Objectives += FString(TEXT("\n")) + Quests->GetLastNotice().ToString();
	}
	ObjectiveText->SetText(FText::FromString(Objectives));
	FString Rewards = FString::Printf(TEXT("Reward: %.0f XP, %s"), Quest->ExperienceReward,
		Quest->RewardItem ? *Quest->RewardItem->ItemName.ToString() : TEXT("item"));
	if (Quest->OptionalSkillPointReward > 0)
	{
		Rewards += FString::Printf(TEXT("\nOptional bonus: %d free skill point%s"),
			Quest->OptionalSkillPointReward, Quest->OptionalSkillPointReward == 1 ? TEXT("") : TEXT("s"));
	}
	RewardText->SetText(FText::FromString(Rewards));
}

void UQuestTrackerWidget::HandleQuestChanged(UQuestDataAsset* Quest)
{
	if (Quest) LastCompletedQuest = nullptr;
	Refresh();
}

void UQuestTrackerWidget::HandleQuestCompleted(UQuestDataAsset* Quest)
{
	LastCompletedQuest = Quest;
	Refresh();
}
