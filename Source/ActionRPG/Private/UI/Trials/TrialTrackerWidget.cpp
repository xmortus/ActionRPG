// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Trials/TrialTrackerWidget.h"
#include "UI/ActionRPGUITheme.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Progression/TrialComponent.h"
#include "Components/Progression/QuestComponent.h"
#include "Components/Progression/ClassComponent.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Items/Equipment/WeaponItem.h"
#include "Items/Core/ItemDataAsset.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Trials/TrialDataAsset.h"
#include "Quests/QuestDataAsset.h"

void UTrialTrackerWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("TrialCanvas"));
	WidgetTree->RootWidget = Canvas;
	USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("TrialPanelSize"));
	PanelSize->SetWidthOverride(300.0f);
	UCanvasPanelSlot* PanelSlot = Canvas->AddChildToCanvas(PanelSize);
	PanelSlot->SetPosition(FVector2D(20.0f, 20.0f));
	PanelSlot->SetAutoSize(true);
	UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("TrialPanel"));
	Panel->SetPadding(FMargin(12.0f));
	Panel->SetBrushColor(ActionRPGUITheme::Panel());
	PanelSize->SetContent(Panel);

	UVerticalBox* Rows = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("TrialRows"));
	Panel->SetContent(Rows);
	auto AddText = [this, Rows](const TCHAR* Name) -> UTextBlock*
	{
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(Name));
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Text->SetAutoWrapText(true);
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = FCString::Strcmp(Name, TEXT("TrialTitle")) == 0 ? 19 : 14;
		Text->SetFont(Font);
		Rows->AddChildToVerticalBox(Text)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
		return Text;
	};
	TitleText = AddText(TEXT("TrialTitle"));
	JourneyText = AddText(TEXT("JourneyNextStep"));
	JourneyText->SetColorAndOpacity(FSlateColor(ActionRPGUITheme::Accent()));
	ObjectiveText = AddText(TEXT("TrialObjectives"));
	ProgressBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("TrialProgress"));
	ProgressBar->SetFillColorAndOpacity(ActionRPGUITheme::Accent());
	Rows->AddChildToVerticalBox(ProgressBar)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	RewardText = AddText(TEXT("TrialRewards"));
	auto MakeChoiceButton = [this, Rows](const TCHAR* ButtonName, const TCHAR* LabelName, UTextBlock*& OutLabel) -> UButton*
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(ButtonName));
		Button->SetBackgroundColor(ActionRPGUITheme::Accent());
		OutLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(LabelName));
		OutLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		OutLabel->SetAutoWrapText(true);
		Button->SetContent(OutLabel);
		Rows->AddChildToVerticalBox(Button)->SetPadding(FMargin(2.0f, 4.0f));
		return Button;
	};
	UTextBlock* FirstLabel = nullptr;
	UTextBlock* SecondLabel = nullptr;
	ChoiceOneButton = MakeChoiceButton(TEXT("TrialChoiceOneButton"), TEXT("TrialChoiceOneLabel"), FirstLabel);
	ChoiceTwoButton = MakeChoiceButton(TEXT("TrialChoiceTwoButton"), TEXT("TrialChoiceTwoLabel"), SecondLabel);
	ChoiceOneLabel = FirstLabel;
	ChoiceTwoLabel = SecondLabel;
	ChoiceOneButton->OnClicked.AddDynamic(this, &UTrialTrackerWidget::HandleChoiceOneClicked);
	ChoiceTwoButton->OnClicked.AddDynamic(this, &UTrialTrackerWidget::HandleChoiceTwoClicked);
	KnownTrialsText = AddText(TEXT("KnownTrials"));
	NoticeText = AddText(TEXT("TrialNotice"));
	FailButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("FailTrialButton"));
	FailButton->SetBackgroundColor(ActionRPGUITheme::Button());
	UTextBlock* FailLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FailTrialLabel"));
	FailLabel->SetText(FText::FromString(TEXT("Abandon Trial")));
	FailLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FailButton->SetContent(FailLabel);
	FailButton->OnClicked.AddDynamic(this, &UTrialTrackerWidget::HandleFailClicked);
	Rows->AddChildToVerticalBox(FailButton)->SetPadding(FMargin(2.0f, 6.0f));
	RetryRewardButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("RetryTrialRewardButton"));
	RetryRewardButton->SetBackgroundColor(ActionRPGUITheme::Accent());
	UTextBlock* RetryLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RetryTrialRewardLabel"));
	RetryLabel->SetText(FText::FromString(TEXT("Claim Trial Reward")));
	RetryLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	RetryRewardButton->SetContent(RetryLabel);
	RetryRewardButton->OnClicked.AddDynamic(this, &UTrialTrackerWidget::HandleRetryRewardClicked);
	Rows->AddChildToVerticalBox(RetryRewardButton)->SetPadding(FMargin(2.0f, 6.0f));
	Refresh();
}

void UTrialTrackerWidget::BindTrialComponent(UTrialComponent* InTrials)
{
	if (Trials)
	{
		Trials->OnTrialStarted.RemoveAll(this);
		Trials->OnTrialCompleted.RemoveAll(this);
		Trials->OnTrialFailed.RemoveAll(this);
		Trials->OnTrialProgressChanged.RemoveAll(this);
	}
	Trials = InTrials;
	if (Trials)
	{
		Trials->OnTrialStarted.AddDynamic(this, &UTrialTrackerWidget::HandleTrialStarted);
		Trials->OnTrialCompleted.AddDynamic(this, &UTrialTrackerWidget::HandleTrialCompleted);
		Trials->OnTrialFailed.AddDynamic(this, &UTrialTrackerWidget::HandleTrialFailed);
		Trials->OnTrialProgressChanged.AddDynamic(this, &UTrialTrackerWidget::HandleTrialProgress);
	}
	Refresh();
}

void UTrialTrackerWidget::BindJourneySources(UQuestComponent* InQuests, UClassComponent* InClasses,
	UEquipmentComponent* InEquipment, UInventoryComponent* InInventory)
{
	if (Quests)
	{
		Quests->OnQuestChanged.RemoveDynamic(this, &UTrialTrackerWidget::HandleJourneyQuestChanged);
		Quests->OnQuestCompleted.RemoveDynamic(this, &UTrialTrackerWidget::HandleJourneyQuestChanged);
	}
	if (Classes)
	{
		Classes->OnClassListChanged.RemoveDynamic(this, &UTrialTrackerWidget::HandleJourneyClassListChanged);
		Classes->OnClassLevelChanged.RemoveDynamic(this, &UTrialTrackerWidget::HandleJourneyClassLevelChanged);
	}
	if (Equipment)
	{
		Equipment->OnEquipmentChanged.RemoveDynamic(this, &UTrialTrackerWidget::HandleJourneyEquipmentChanged);
	}
	if (Inventory)
	{
		Inventory->OnInventoryChanged.RemoveDynamic(this, &UTrialTrackerWidget::HandleJourneyInventoryChanged);
	}
	Quests = InQuests;
	Classes = InClasses;
	Equipment = InEquipment;
	Inventory = InInventory;
	if (Quests)
	{
		Quests->OnQuestChanged.AddUniqueDynamic(this, &UTrialTrackerWidget::HandleJourneyQuestChanged);
		Quests->OnQuestCompleted.AddUniqueDynamic(this, &UTrialTrackerWidget::HandleJourneyQuestChanged);
	}
	if (Classes)
	{
		Classes->OnClassListChanged.AddUniqueDynamic(this, &UTrialTrackerWidget::HandleJourneyClassListChanged);
		Classes->OnClassLevelChanged.AddUniqueDynamic(this, &UTrialTrackerWidget::HandleJourneyClassLevelChanged);
	}
	if (Equipment)
	{
		Equipment->OnEquipmentChanged.AddUniqueDynamic(this, &UTrialTrackerWidget::HandleJourneyEquipmentChanged);
	}
	if (Inventory)
	{
		Inventory->OnInventoryChanged.AddUniqueDynamic(this, &UTrialTrackerWidget::HandleJourneyInventoryChanged);
	}
	Refresh();
}

void UTrialTrackerWidget::NativeDestruct()
{
	BindTrialComponent(nullptr);
	BindJourneySources(nullptr, nullptr, nullptr, nullptr);
	Super::NativeDestruct();
}

FString UTrialTrackerWidget::BuildRewardPreview(const UTrialDataAsset* Trial) const
{
	if (!Trial)
	{
		return FString();
	}
	if (!Trial->RewardChoices.IsEmpty())
	{
		FString Preview = TEXT("Choose one reward:");
		for (const FTrialRewardChoice& Choice : Trial->RewardChoices)
		{
			Preview += FString::Printf(TEXT("\n%s — %s"), *Choice.ChoiceName.ToString(), *BuildRewardPreview(Choice));
		}
		return Preview;
	}
	FTrialRewardChoice Reward;
	Reward.ExperienceReward = Trial->ExperienceReward;
	Reward.ItemReward = Trial->ItemReward;
	Reward.SkillReward = Trial->SkillReward;
	Reward.AttributePointReward = Trial->AttributePointReward;
	Reward.SkillPointReward = Trial->SkillPointReward;
	Reward.EnduranceReward = Trial->EnduranceReward;
	return BuildRewardPreview(Reward);
}

FString UTrialTrackerWidget::BuildRewardPreview(const FTrialRewardChoice& Reward) const
{
	TArray<FString> Parts;
	if (Reward.ExperienceReward > 0.0f)
	{
		Parts.Add(FString::Printf(TEXT("%.0f XP"), Reward.ExperienceReward));
	}
	if (Reward.ItemReward.Item)
	{
		Parts.Add(FString::Printf(TEXT("%d x %s"), Reward.ItemReward.Quantity,
			*Reward.ItemReward.Item->ItemName.ToString()));
	}
	if (Reward.SkillReward.SkillData)
	{
		Parts.Add(FString::Printf(TEXT("Skill: %s"), *Reward.SkillReward.SkillData->SkillName.ToString()));
	}
	if (Reward.AttributePointReward > 0)
	{
		Parts.Add(FString::Printf(TEXT("%d attribute point(s)"), Reward.AttributePointReward));
	}
	if (Reward.SkillPointReward > 0)
	{
		Parts.Add(FString::Printf(TEXT("%d skill point(s)"), Reward.SkillPointReward));
	}
	if (Reward.EnduranceReward > 0)
	{
		Parts.Add(FString::Printf(TEXT("+%d Endurance (Constitution capped)"), Reward.EnduranceReward));
	}
	return Parts.IsEmpty() ? TEXT("Reward: none") : TEXT("Reward: ") + FString::Join(Parts, TEXT(" | "));
}

void UTrialTrackerWidget::Refresh()
{
	if (!TitleText || !JourneyText || !ObjectiveText || !RewardText || !KnownTrialsText || !NoticeText || !ProgressBar || !FailButton || !RetryRewardButton
		|| !ChoiceOneButton || !ChoiceTwoButton || !ChoiceOneLabel || !ChoiceTwoLabel)
	{
		return;
	}
	const UTrialDataAsset* Active = Trials ? Trials->GetActiveTrial() : nullptr;
	TitleText->SetText(FText::FromString(Active ? Active->TrialName.ToString() : TEXT("Trials")));
	bool bFirstStepsDone = false;
	bool bCombatDone = false;
	const TArray<UClassDataAsset*> SelectedClasses = Classes ? Classes->GetSelectedClasses() : TArray<UClassDataAsset*>();
	auto IsGrownSword = [](const UItemBase* Item)
	{
		const UWeaponItem* Weapon = Cast<UWeaponItem>(Item);
		return Weapon && Weapon->ItemData && Weapon->ItemData->bSoulboundGrowth && Weapon->GetGrowthRank() >= 1;
	};
	const UWeaponItem* EquippedSword = Equipment ? Cast<UWeaponItem>(Equipment->GetEquippedItem(EEquipmentSlot::MainHand)) : nullptr;
	bool bSwordGrown = IsGrownSword(EquippedSword);
	if (Inventory && !bSwordGrown)
	{
		for (const FInventorySlot& InventorySlot : Inventory->GetInventorySlots())
		{
			if (IsGrownSword(InventorySlot.Item.Get()))
			{
				bSwordGrown = true;
				break;
			}
		}
	}
	if (Trials)
	{
		for (const FTrialRecord& Record : Trials->GetTrialRecords())
		{
			if (!Record.Trial || Record.CompletionCount <= 0) continue;
			bFirstStepsDone |= Record.Trial->TrialId == TEXT("FirstSteps");
			bCombatDone |= Record.Trial->TrialId == TEXT("TrainingCombat");
		}
	}
	FString Journey;
	if (!bFirstStepsDone)
	{
		if (Active && Active->TrialId == TEXT("FirstSteps"))
		{
			Journey = Trials->GetTrialProgress(Active) >= 1.0f
				? TEXT("JOURNEY 1/5: Choose Growth or Learning below.")
				: TEXT("JOURNEY 1/5: Reach the First Steps goal marker north of the start.");
		}
		else Journey = TEXT("JOURNEY 1/5: Enter the First Steps marker southwest of your start.");
	}
	else if (SelectedClasses.IsEmpty() || Classes->GetClassLevel(SelectedClasses[0]) < 1)
	{
		Journey = TEXT("JOURNEY 2/5: Press B to unlock Warrior or Mage with 100 XP. Need XP? Try Dash Practice.");
	}
	else if (!bCombatDone)
	{
		Journey = Active && Active->TrialId == TEXT("TrainingCombat")
			? TEXT("JOURNEY 3/5: Press K to assign an attack, then defeat the Training Enemy to the northeast.")
			: TEXT("JOURNEY 3/5: Enter the Training Combat marker southeast of your start.");
	}
	else if (Quests && Quests->IsQuestIDCompleted(TEXT("QUEST_FORGEKEEPERS_REQUEST")))
	{
		if (bSwordGrown)
		{
			Journey = TEXT("JOURNEY COMPLETE: Your Trialbound Sword reached rank 1. Explore Dash Practice or crafting next.");
		}
		else if (!EquippedSword || !EquippedSword->ItemData || !EquippedSword->ItemData->bSoulboundGrowth)
		{
			Journey = TEXT("JOURNEY 5/5: Press U and equip the Trialbound Sword. Unequip other main-hand gear first.");
		}
		else Journey = FString::Printf(TEXT("JOURNEY 5/5: Trialbound Sword growth %d/%d. Defeat the Training Enemy while the sword is equipped."),
			EquippedSword->GetDefeatsTowardGrowth(), FMath::Max(1, EquippedSword->ItemData->DefeatsPerGrowthRank));
	}
	else if (Quests && Quests->GetActiveQuest())
	{
		Journey = Quests->GetRequiredItemCount() < Quests->GetActiveQuest()->RequiredItemCount
			? TEXT("JOURNEY 4/5: Collect two Iron Ore north of your start (F to interact). Defeating the enemy is optional.")
			: TEXT("JOURNEY 4/5: Return to the Forgekeeper marker west of your start for your reward.");
	}
	else Journey = TEXT("JOURNEY 4/5: Visit the Forgekeeper marker northwest of your start.");
	JourneyText->SetText(FText::FromString(Journey));
	FString ObjectiveSummary = TEXT("Enter a trial marker to begin.");
	if (Active)
	{
		ObjectiveSummary.Empty();
		const FTrialRecord* Record = Trials->GetTrialRecords().FindByPredicate(
			[Active](const FTrialRecord& Candidate) { return Candidate.Trial == Active; });
		for (int32 Index = 0; Index < Active->Objectives.Num(); ++Index)
		{
			const FTrialObjectiveDefinition& Objective = Active->Objectives[Index];
			const int32 Count = Record && Record->ObjectiveCounts.IsValidIndex(Index)
				? Record->ObjectiveCounts[Index] : 0;
			ObjectiveSummary += FString::Printf(TEXT("%s (%d/%d)\n"),
				*Objective.Description.ToString(), Count, Objective.TargetCount);
		}
	}
	ObjectiveText->SetText(FText::FromString(ObjectiveSummary));
	ProgressBar->SetPercent(Active ? Trials->GetTrialProgress(Active) : 0.0f);
	RewardText->SetText(FText::FromString(BuildRewardPreview(Active)));
	const bool bChooseReward = Active && Active->RewardChoices.Num() == 2 && Trials->GetTrialProgress(Active) >= 1.0f;
	ChoiceOneButton->SetVisibility(bChooseReward ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	ChoiceTwoButton->SetVisibility(bChooseReward ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (bChooseReward)
	{
		ChoiceOneLabel->SetText(FText::FromString(FString::Printf(TEXT("Choose %s"), *Active->RewardChoices[0].ChoiceName.ToString())));
		ChoiceTwoLabel->SetText(FText::FromString(FString::Printf(TEXT("Choose %s"), *Active->RewardChoices[1].ChoiceName.ToString())));
	}
	FailButton->SetVisibility(Active ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	RetryRewardButton->SetVisibility(Active && Trials->GetTrialProgress(Active) >= 1.0f
		&& (Active->RewardChoices.IsEmpty() || Trials->GetActiveRewardChoiceIndex() != INDEX_NONE)
		? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	FString Known = TEXT("Known trials:\n");
	if (Trials)
	{
		for (const FTrialRecord& Record : Trials->GetTrialRecords())
		{
			if (!Record.Trial)
			{
				continue;
			}
			Known += FString::Printf(TEXT("%s: %s (done %d, failed %d)\n"),
				*Record.Trial->TrialName.ToString(), Record.bActive ? TEXT("active") : TEXT("inactive"),
				Record.CompletionCount, Record.FailureCount);
		}
	}
	KnownTrialsText->SetText(FText::FromString(Known));
	NoticeText->SetText(LastNotice);
}

void UTrialTrackerWidget::HandleTrialStarted(UTrialDataAsset* Trial)
{
	LastNotice = FText::FromString(FString::Printf(TEXT("Started: %s"), *Trial->TrialName.ToString()));
	Refresh();
}

void UTrialTrackerWidget::HandleTrialCompleted(UTrialDataAsset* Trial)
{
	FString RewardPreview = BuildRewardPreview(Trial);
	if (Trial && !Trial->RewardChoices.IsEmpty() && Trials)
	{
		const FTrialRecord* Record = Trials->GetTrialRecords().FindByPredicate(
			[Trial](const FTrialRecord& Candidate) { return Candidate.Trial == Trial; });
		if (Record && Trial->RewardChoices.IsValidIndex(Record->LastRewardChoiceIndex))
		{
			RewardPreview = Trial->RewardChoices[Record->LastRewardChoiceIndex].ChoiceName.ToString()
				+ TEXT(": ") + BuildRewardPreview(Trial->RewardChoices[Record->LastRewardChoiceIndex]);
		}
	}
	LastNotice = FText::FromString(FString::Printf(TEXT("Completed: %s. %s"),
		*Trial->TrialName.ToString(), *RewardPreview));
	Refresh();
}

void UTrialTrackerWidget::HandleTrialFailed(UTrialDataAsset* Trial)
{
	LastNotice = FText::FromString(FString::Printf(TEXT("Failed: %s. Return to its start marker to retry."),
		*Trial->TrialName.ToString()));
	Refresh();
}

void UTrialTrackerWidget::HandleTrialProgress(UTrialDataAsset* Trial, float Progress)
{
	if (Trial && Progress >= 1.0f && !Trial->RewardChoices.IsEmpty())
	{
		LastNotice = FText::FromString(TEXT("Objective complete. Choose one reward."));
	}
	Refresh();
}

void UTrialTrackerWidget::HandleJourneyQuestChanged(UQuestDataAsset* Quest)
{
	Refresh();
}

void UTrialTrackerWidget::HandleJourneyClassListChanged(UClassDataAsset* ClassAsset, bool bAdded)
{
	Refresh();
}

void UTrialTrackerWidget::HandleJourneyClassLevelChanged(UClassDataAsset* ClassAsset, int32 NewLevel)
{
	Refresh();
}

void UTrialTrackerWidget::HandleJourneyEquipmentChanged(EEquipmentSlot InSlot, UEquipmentItem* Item)
{
	Refresh();
}

void UTrialTrackerWidget::HandleJourneyInventoryChanged(int32 SlotIndex, UItemBase* Item)
{
	Refresh();
}

void UTrialTrackerWidget::HandleFailClicked()
{
	if (Trials)
	{
		Trials->FailActiveTrial();
	}
}

void UTrialTrackerWidget::HandleRetryRewardClicked()
{
	if (Trials && !Trials->TryCompleteActiveTrial())
	{
		LastNotice = FText::FromString(TEXT("Reward is still blocked. Free inventory space or meet the reward requirements, then retry."));
		Refresh();
	}
}

void UTrialTrackerWidget::HandleChoiceOneClicked()
{
	if (Trials && !Trials->ChooseActiveTrialReward(0))
	{
		LastNotice = FText::FromString(TEXT("Reward blocked. Check inventory space or skill requirements; choose again or retry."));
		Refresh();
	}
}

void UTrialTrackerWidget::HandleChoiceTwoClicked()
{
	if (Trials && !Trials->ChooseActiveTrialReward(1))
	{
		LastNotice = FText::FromString(TEXT("Reward blocked. Check inventory space or skill requirements; choose again or retry."));
		Refresh();
	}
}
