// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Progression/ClassSelectionPanelWidget.h"
#include "Components/Progression/ClassComponent.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Components/Skills/SkillComponent.h"
#include "Progression/Core/ClassDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Data/ClassDatabase.h"
#include "Data/SkillDatabase.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/ScrollBox.h"
#include "Components/ScaleBox.h"
#include "Components/SizeBox.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "GameFramework/PlayerController.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Skills/Core/SkillBase.h"
#include "UI/Progression/ClassEntryWidget.h"
#include "UI/Common/DraggablePanelHandleWidget.h"

void UClassSelectionPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	EnsureContentBuilt();
}

void UClassSelectionPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindComponents();
	EnsureContentBuilt();
	UDraggablePanelHandleWidget::AttachToOverlayTitle(this, TEXT("ProgressionTitleArea"), TEXT("ProgressionPanelSize"));
	RefreshAllClassesList();
	RefreshClassList();
	RefreshPanel();
}

void UClassSelectionPanelWidget::EnsureContentBuilt()
{
	if (!WidgetTree || WidgetTree->RootWidget) return;

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("ProgressionCanvas"));
	Canvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WidgetTree->RootWidget = Canvas;
	UScaleBox* ResponsiveFrame = WidgetTree->ConstructWidget<UScaleBox>(UScaleBox::StaticClass(), TEXT("ProgressionResponsiveFrame"));
	ResponsiveFrame->SetStretch(EStretch::ScaleToFit);
	ResponsiveFrame->SetStretchDirection(EStretchDirection::DownOnly);
	UCanvasPanelSlot* FrameSlot = Canvas->AddChildToCanvas(ResponsiveFrame);
	FrameSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	FrameSlot->SetOffsets(FMargin(16.0f, 16.0f, -16.0f, -16.0f));
	USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("ProgressionPanelSize"));
	PanelSize->SetWidthOverride(940.0f);
	PanelSize->SetHeightOverride(650.0f);
	ResponsiveFrame->SetContent(PanelSize);
	UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ProgressionPanel"));
	Panel->SetBrushColor(FLinearColor(0.035f, 0.055f, 0.085f, 0.97f));
	Panel->SetPadding(FMargin(18.0f));
	PanelSize->SetContent(Panel);

	UVerticalBox* Main = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ProgressionRows"));
	Panel->SetContent(Main);
	auto AddText = [this](UVerticalBox* Parent, const TCHAR* Name, const TCHAR* Initial, int32 FontSize, float Bottom) -> UTextBlock*
	{
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(Name));
		Text->SetText(FText::FromString(Initial));
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Text->SetAutoWrapText(true);
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = FontSize;
		Text->SetFont(Font);
		Parent->AddChildToVerticalBox(Text)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, Bottom));
		return Text;
	};
	auto MakeButton = [this](const TCHAR* Name, const TCHAR* Label, const FLinearColor& Color) -> UButton*
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(Name));
		Button->SetBackgroundColor(Color);
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(FString(Name) + TEXT("Label")));
		Text->SetText(FText::FromString(Label));
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = 16;
		Text->SetFont(Font);
		Button->SetContent(Text);
		return Button;
	};

	UHorizontalBox* Header = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ProgressionHeader"));
	Main->AddChildToVerticalBox(Header)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ProgressionTitle"));
	Title->SetText(FText::FromString(TEXT("CLASSES & PROFESSIONS")));
	Title->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo TitleFont = Title->GetFont();
	TitleFont.Size = 25;
	Title->SetFont(TitleFont);
	UOverlay* TitleArea = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("ProgressionTitleArea"));
	Header->AddChildToHorizontalBox(TitleArea)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	TitleArea->AddChildToOverlay(Title);
	UButton* CloseButton = MakeButton(TEXT("CloseProgressionButton"), TEXT("Close [B]"), FLinearColor(0.20f, 0.26f, 0.33f, 1.0f));
	CloseButton->OnClicked.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleCloseClicked);
	Header->AddChildToHorizontalBox(CloseButton);

	AvailableXpText = AddText(Main, TEXT("AvailableXpText"), TEXT("AVAILABLE XP  0"), 20, 4.0f);
	OverviewText = AddText(Main, TEXT("ProgressionOverview"), TEXT("Level 1  |  Classes 0/1  |  Professions 0/1"), 15, 5.0f);
	AddText(Main, TEXT("ProgressionHelp"), TEXT("Choose a path, assign earned XP to unlock or level it. Classes grant attributes; professions grant skills."), 14, 13.0f);

	UHorizontalBox* Columns = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("PathColumns"));
	Main->AddChildToVerticalBox(Columns)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	auto MakeColumn = [this, Columns, AddText](const TCHAR* Name, const TCHAR* Heading) -> UVerticalBox*
	{
		UBorder* Column = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(FString(Name) + TEXT("Border")));
		Column->SetBrushColor(FLinearColor(0.07f, 0.10f, 0.15f, 1.0f));
		Column->SetPadding(FMargin(10.0f));
		UHorizontalBoxSlot* Slot = Columns->AddChildToHorizontalBox(Column);
		Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		Slot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
		UVerticalBox* Body = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(FString(Name) + TEXT("Body")));
		Column->SetContent(Body);
		AddText(Body, Name, Heading, 19, 8.0f);
		USizeBox* ListSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(FString(Name) + TEXT("Size")));
		ListSize->SetHeightOverride(226.0f);
		Body->AddChildToVerticalBox(ListSize);
		UScrollBox* Scroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), FName(FString(Name) + TEXT("Scroll")));
		ListSize->SetContent(Scroll);
		UVerticalBox* Rows = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(FString(Name) + TEXT("Rows")));
		Scroll->AddChild(Rows);
		return Rows;
	};
	ClassRows = MakeColumn(TEXT("ClassList"), TEXT("CLASSES"));
	ProfessionRows = MakeColumn(TEXT("ProfessionList"), TEXT("PROFESSIONS"));

	UBorder* Details = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PathDetailsBorder"));
	Details->SetBrushColor(FLinearColor(0.07f, 0.10f, 0.15f, 1.0f));
	Details->SetPadding(FMargin(10.0f));
	Main->AddChildToVerticalBox(Details)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	USizeBox* DetailSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("PathDetailsSize"));
	DetailSize->SetHeightOverride(155.0f);
	Details->SetContent(DetailSize);
	UScrollBox* DetailScroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("PathDetailsScroll"));
	DetailSize->SetContent(DetailScroll);
	UVerticalBox* DetailRows = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PathDetailsRows"));
	DetailScroll->AddChild(DetailRows);
	SelectedNameText = AddText(DetailRows, TEXT("SelectedPathName"), TEXT("SELECT A CLASS OR PROFESSION"), 19, 4.0f);
	SelectedDescriptionText = AddText(DetailRows, TEXT("SelectedPathDescription"), TEXT("Click a row above to see its unlock requirements and rewards."), 14, 4.0f);
	SelectedStatusText = AddText(DetailRows, TEXT("SelectedPathStatus"), TEXT(""), 15, 4.0f);
	SelectedRequirementsText = AddText(DetailRows, TEXT("SelectedPathRequirements"), TEXT(""), 14, 4.0f);
	SelectedRewardsText = AddText(DetailRows, TEXT("SelectedPathRewards"), TEXT(""), 14, 0.0f);

	UHorizontalBox* Actions = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ProgressionActions"));
	Main->AddChildToVerticalBox(Actions)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 6.0f));
	AmountInput = WidgetTree->ConstructWidget<USpinBox>(USpinBox::StaticClass(), TEXT("Input_XPAmount"));
	AmountInput->SetMinValue(0.0f);
	AmountInput->SetMaxValue(100000.0f);
	AmountInput->SetValue(100.0f);
	AmountInput->OnValueChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleAmountChanged);
	USizeBox* AmountSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("AmountSize"));
	AmountSize->SetWidthOverride(120.0f);
	AmountSize->SetContent(AmountInput);
	Actions->AddChildToHorizontalBox(AmountSize)->SetPadding(FMargin(0.0f, 0.0f, 10.0f, 0.0f));
	UButton* HundredButton = MakeButton(TEXT("SetHundredButton"), TEXT("100"), FLinearColor(0.16f, 0.23f, 0.32f, 1.0f));
	HundredButton->OnClicked.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleSetHundredClicked);
	Actions->AddChildToHorizontalBox(HundredButton)->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
	UButton* AvailableButton = MakeButton(TEXT("SetAvailableButton"), TEXT("All XP"), FLinearColor(0.16f, 0.23f, 0.32f, 1.0f));
	AvailableButton->OnClicked.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleSetAvailableClicked);
	Actions->AddChildToHorizontalBox(AvailableButton)->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
	AssignButton = MakeButton(TEXT("Button_AllocateXP"), TEXT("Assign XP"), FLinearColor(0.12f, 0.38f, 0.28f, 1.0f));
	AssignButton->OnClicked.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleAssignClicked);
	Actions->AddChildToHorizontalBox(AssignButton)->SetPadding(FMargin(0.0f, 0.0f, 18.0f, 0.0f));
	ActivateButton = MakeButton(TEXT("ActivatePathButton"), TEXT("Add to slot"), FLinearColor(0.16f, 0.28f, 0.45f, 1.0f));
	ActivateButton->OnClicked.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleActivateClicked);
	Actions->AddChildToHorizontalBox(ActivateButton)->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
	RemoveButton = MakeButton(TEXT("RemovePathButton"), TEXT("Remove"), FLinearColor(0.40f, 0.16f, 0.17f, 1.0f));
	RemoveButton->OnClicked.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleRemoveClicked);
	Actions->AddChildToHorizontalBox(RemoveButton);
	MessageText = AddText(Main, TEXT("ProgressionMessage"), TEXT("Earn XP from trials, then assign it to a selected path."), 14, 0.0f);
}

void UClassSelectionPanelWidget::RefreshPanel()
{
	if (!AvailableXpText || !OverviewText) return;
	const float Available = GetUnallocatedExperience();
	AvailableXpText->SetText(FText::FromString(FString::Printf(TEXT("AVAILABLE XP   %.0f"), Available)));
	const int32 ClassCount = ClassComponent ? ClassComponent->GetSelectedClasses().Num() : 0;
	const int32 ProfessionCount = ClassComponent ? ClassComponent->GetSelectedProfessions().Num() : 0;
	OverviewText->SetText(FText::FromString(FString::Printf(TEXT("Combined level %d   |   Class slots %d / %d   |   Profession slots %d / %d"),
		GetPlayerLevel(), ClassCount, ClassComponent ? ClassComponent->MaxClasses : 0,
		ProfessionCount, ClassComponent ? ClassComponent->MaxProfessions : 0)));
	RebuildEntries();
	RefreshSelectedDetails();
}

void UClassSelectionPanelWidget::RebuildEntries()
{
	if (!ClassRows || !ProfessionRows || !GetOwningPlayer()) return;
	ClassRows->ClearChildren();
	ProfessionRows->ClearChildren();
	for (UClassDataAsset* ClassAsset : GetAllClasses())
	{
		if (!ClassAsset) continue;
		UClassEntryWidget* Entry = CreateWidget<UClassEntryWidget>(GetOwningPlayer(), UClassEntryWidget::StaticClass());
		if (!Entry) continue;
		Entry->ParentPanel = this;
		Entry->SetClassEntry(ClassAsset, ClassComponent);
		Entry->SetSelectedForXp(SelectedClassForXp == ClassAsset);
		ClassRows->AddChildToVerticalBox(Entry)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 5.0f));
	}
	if (UClassDatabase* Database = UClassDatabase::Get())
	{
		for (UProfessionDataAsset* ProfessionAsset : Database->GetAllProfessions())
		{
			if (!ProfessionAsset) continue;
			UClassEntryWidget* Entry = CreateWidget<UClassEntryWidget>(GetOwningPlayer(), UClassEntryWidget::StaticClass());
			if (!Entry) continue;
			Entry->ParentPanel = this;
			Entry->SetProfessionEntry(ProfessionAsset, ClassComponent);
			Entry->SetSelectedForXp(SelectedProfessionForXp == ProfessionAsset);
			ProfessionRows->AddChildToVerticalBox(Entry)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 5.0f));
		}
	}
}

void UClassSelectionPanelWidget::RefreshSelectedDetails()
{
	if (!SelectedNameText || !SelectedStatusText || !SelectedDescriptionText || !SelectedRequirementsText || !SelectedRewardsText) return;
	const bool bClass = SelectedClassForXp != nullptr;
	const bool bProfession = SelectedProfessionForXp != nullptr;
	if (!bClass && !bProfession)
	{
		SelectedNameText->SetText(FText::FromString(TEXT("SELECT A CLASS OR PROFESSION")));
		SelectedDescriptionText->SetText(FText::FromString(TEXT("Click a row above to inspect its unlock requirements, bonuses or skill, and XP progress.")));
		SelectedStatusText->SetText(FText::GetEmpty());
		SelectedRequirementsText->SetText(FText::GetEmpty());
		SelectedRewardsText->SetText(FText::GetEmpty());
		if (AssignButton) AssignButton->SetIsEnabled(false);
		if (ActivateButton) ActivateButton->SetIsEnabled(false);
		if (RemoveButton) RemoveButton->SetIsEnabled(false);
		return;
	}

	const int32 Level = ClassComponent ? (bClass ? ClassComponent->GetClassLevel(SelectedClassForXp)
		: ClassComponent->GetProfessionLevel(SelectedProfessionForXp)) : 0;
	const float Progress = ClassComponent ? (bClass ? ClassComponent->GetClassExperience(SelectedClassForXp)
		: ClassComponent->GetProfessionExperience(SelectedProfessionForXp)) : 0.0f;
	const float Next = ClassComponent ? (bClass ? ClassComponent->GetClassExperienceToNextLevel(SelectedClassForXp)
		: ClassComponent->GetProfessionExperienceToNextLevel(SelectedProfessionForXp)) : 0.0f;
	const bool bActive = ClassComponent && (bClass
		? ClassComponent->GetSelectedClasses().Contains(SelectedClassForXp)
		: ClassComponent->GetSelectedProfessions().Contains(SelectedProfessionForXp));
	const FString Name = bClass ? SelectedClassForXp->ClassName.ToString() : SelectedProfessionForXp->ProfessionName.ToString();
	const FString TierName = bClass ? StaticEnum<ECharacterClassTier>()->GetNameStringByValue(
		static_cast<int64>(SelectedClassForXp->ClassTier)) : FString();
	const FString DisplayName = bClass ? FString::Printf(TEXT("%s (%s)"), *Name, *TierName) : Name;
	SelectedNameText->SetText(FText::FromString(FString::Printf(TEXT("%s   |   %s"), *DisplayName,
		bActive ? TEXT("ACTIVE") : Level > 0 ? TEXT("UNLOCKED") : TEXT("LOCKED"))));
	SelectedDescriptionText->SetText(bClass ? SelectedClassForXp->ClassDescription : SelectedProfessionForXp->ProfessionDescription);
	SelectedStatusText->SetText(FText::FromString(FString::Printf(TEXT("Level %d   |   %.0f / %.0f XP   |   %.0f XP to %s"),
		Level, Progress, Next, FMath::Max(0.0f, Next - Progress), Level == 0 ? TEXT("unlock") : TEXT("next level"))));

	const int32 RequiredLevel = bClass ? SelectedClassForXp->RequiredLevel : SelectedProfessionForXp->RequiredLevel;
	FString Requirements = FString::Printf(TEXT("Requires player level %d"), RequiredLevel);
	if (bClass && SelectedClassForXp->RequiredClassLevelToUnlock > 1)
	{
		Requirements += FString::Printf(TEXT("; class level %d to equip"), SelectedClassForXp->RequiredClassLevelToUnlock);
	}
	if (bClass && ClassComponent)
	{
		const USkillComponent* Skills = GetOwningPlayerPawn()
			? GetOwningPlayerPawn()->FindComponentByClass<USkillComponent>() : nullptr;
		for (const FClassSkillLevelRequirement& Requirement : SelectedClassForXp->SkillLevelRequirements)
		{
			if (Requirement.TargetClassLevel <= Level || Requirement.SkillID.IsNone() || Requirement.RequiredSkillLevel < 1)
			{
				continue;
			}
			FString SkillName = Requirement.SkillID.ToString();
			if (const USkillDataAsset* SkillData = USkillDatabase::Get()->GetSkillDataAsset(Requirement.SkillID))
			{
				SkillName = SkillData->SkillName.ToString();
			}
			int32 SkillLevel = 0;
			if (Skills)
			{
				for (USkillBase* Skill : Skills->GetUnlockedSkills())
				{
					if (Skill && Skill->GetSkillID() == Requirement.SkillID)
					{
						SkillLevel = Skills->GetSkillLevel(Skill);
						break;
					}
				}
			}
			const TCHAR* GateState = SkillLevel >= Requirement.RequiredSkillLevel ? TEXT("met")
				: Requirement.TargetClassLevel == Level + 1 ? TEXT("needed") : TEXT("upcoming");
			Requirements += FString::Printf(TEXT("; Level %d gate: %s skill level %d (%s)"),
				Requirement.TargetClassLevel, *SkillName, Requirement.RequiredSkillLevel, GateState);
		}
		FName RequiredSkillID;
		int32 RequiredSkillLevel = 0;
		if (ClassComponent->GetUnmetClassSkillRequirement(SelectedClassForXp, RequiredSkillID, RequiredSkillLevel))
		{
			FString RequiredSkillName = RequiredSkillID.ToString();
			if (const USkillDataAsset* SkillData = USkillDatabase::Get()->GetSkillDataAsset(RequiredSkillID))
			{
				RequiredSkillName = SkillData->SkillName.ToString();
			}
			if (Progress >= Next && Next > 0.0f)
			{
				SelectedStatusText->SetText(FText::FromString(FString::Printf(
					TEXT("Level %d   |   %.0f / %.0f XP   |   XP ready; train %s to level %d"),
					Level, Progress, Next, *RequiredSkillName, RequiredSkillLevel)));
			}
		}
	}
	const TMap<EPrimaryAttribute, float>& AttributeRequirements = bClass
		? SelectedClassForXp->RequiredAttributes : SelectedProfessionForXp->RequiredAttributes;
	for (const TPair<EPrimaryAttribute, float>& Requirement : AttributeRequirements)
	{
		Requirements += FString::Printf(TEXT("; %s %.0f"),
			*StaticEnum<EPrimaryAttribute>()->GetNameStringByValue(static_cast<int64>(Requirement.Key)), Requirement.Value);
	}
	SelectedRequirementsText->SetText(FText::FromString(Requirements));

	FString Rewards;
	if (bClass)
	{
		Rewards = FString::Printf(TEXT("Class: %d free Attribute Points per level; fixed attribute gains per level"),
			SelectedClassForXp->GetFreeAttributePointsPerLevel());
		for (const TPair<EPrimaryAttribute, float>& Gain : SelectedClassForXp->AttributeGainsPerLevel)
		{
			Rewards += FString::Printf(TEXT("; %s +%.0f/level"),
				*StaticEnum<EPrimaryAttribute>()->GetNameStringByValue(static_cast<int64>(Gain.Key)), Gain.Value);
		}
		for (const TPair<EPrimaryAttribute, float>& Bonus : SelectedClassForXp->PrimaryAttributeBonuses)
		{
			Rewards += FString::Printf(TEXT("; %s +%.0f"),
				*StaticEnum<EPrimaryAttribute>()->GetNameStringByValue(static_cast<int64>(Bonus.Key)), Bonus.Value);
		}
		for (const TPair<ESecondaryAttribute, float>& Bonus : SelectedClassForXp->SecondaryAttributeBonuses)
		{
			Rewards += FString::Printf(TEXT("; %s +%.0f"),
				*StaticEnum<ESecondaryAttribute>()->GetNameStringByValue(static_cast<int64>(Bonus.Key)), Bonus.Value);
		}
		for (const FClassGrantedSkill& Grant : SelectedClassForXp->GrantedSkills)
		{
			if (Grant.SkillData && Grant.TargetClassLevel > 0)
			{
				Rewards += FString::Printf(TEXT("; class level %d grants %s while selected"),
					Grant.TargetClassLevel, *Grant.SkillData->SkillName.ToString());
			}
		}
	}
	else
	{
		Rewards = TEXT("Profession: fixed attribute gains per level");
		for (const TPair<EPrimaryAttribute, float>& Gain : SelectedProfessionForXp->AttributeGainsPerLevel)
		{
			Rewards += FString::Printf(TEXT("; %s +%.0f/level"),
				*StaticEnum<EPrimaryAttribute>()->GetNameStringByValue(static_cast<int64>(Gain.Key)), Gain.Value);
		}
		if (SelectedProfessionForXp->GrantedSkillData)
		{
			Rewards += FString::Printf(TEXT("; unlocks %s"), *SelectedProfessionForXp->GrantedSkillData->SkillName.ToString());
		}
		Rewards += FString::Printf(TEXT("; %d recipe(s)"), SelectedProfessionForXp->Recipes.Num());
	}
	SelectedRewardsText->SetText(FText::FromString(Rewards));
	if (AssignButton) AssignButton->SetIsEnabled(ExperienceComponent && AmountInput
		&& ExperienceComponent->CanSpendExperience(AmountInput->GetValue()));
	const int32 UnlockLevel = bClass ? SelectedClassForXp->RequiredClassLevelToUnlock : 1;
	if (ActivateButton) ActivateButton->SetIsEnabled(Level >= UnlockLevel && !bActive);
	if (RemoveButton) RemoveButton->SetIsEnabled(bActive);
}

void UClassSelectionPanelWidget::HandleAssignClicked()
{
	if (!AmountInput) return;
	const float Amount = AmountInput->GetValue();
	const bool bSucceeded = AllocateExperienceToSelectedClass(Amount);
	if (MessageText)
	{
		MessageText->SetText(FText::FromString(bSucceeded
			? FString::Printf(TEXT("Assigned %.0f XP. Progress and slots have been updated."), Amount)
			: TEXT("Could not assign XP. Check your available XP and selected path.")));
	}
	RefreshPanel();
}

void UClassSelectionPanelWidget::HandleSetHundredClicked()
{
	if (AmountInput) AmountInput->SetValue(100.0f);
	RefreshSelectedDetails();
}

void UClassSelectionPanelWidget::HandleSetAvailableClicked()
{
	if (AmountInput) AmountInput->SetValue(GetUnallocatedExperience());
	RefreshSelectedDetails();
}

void UClassSelectionPanelWidget::HandleAmountChanged(float NewValue)
{
	RefreshSelectedDetails();
}

void UClassSelectionPanelWidget::HandleActivateClicked()
{
	const bool bSucceeded = SelectedClassForXp ? AddClass(SelectedClassForXp)
		: SelectedProfessionForXp ? AddProfession(SelectedProfessionForXp) : false;
	if (MessageText) MessageText->SetText(FText::FromString(bSucceeded
		? TEXT("Added to an active slot.") : TEXT("Cannot add: check unlock level, requirements, and free slots.")));
	RefreshPanel();
}

void UClassSelectionPanelWidget::HandleRemoveClicked()
{
	const bool bSucceeded = SelectedClassForXp ? RemoveClass(SelectedClassForXp)
		: SelectedProfessionForXp ? RemoveProfession(SelectedProfessionForXp) : false;
	if (MessageText) MessageText->SetText(FText::FromString(bSucceeded
		? TEXT("Removed from active slots. Earned progress is retained.") : TEXT("The selected path is not active.")));
	RefreshPanel();
}

void UClassSelectionPanelWidget::HandleCloseClicked()
{
	SetVisibility(ESlateVisibility::Collapsed);
	if (APlayerController* Controller = GetOwningPlayer())
	{
		Controller->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		Controller->bShowMouseCursor = true;
	}
}

void UClassSelectionPanelWidget::HandlePoolChanged(float NewUnallocatedExperience, float Delta)
{
	RefreshPanel();
}

void UClassSelectionPanelWidget::NativeDestruct()
{
	if (ClassComponent)
	{
		ClassComponent->OnClassListChanged.RemoveDynamic(this, &UClassSelectionPanelWidget::HandleClassListChanged);
		ClassComponent->OnProfessionListChanged.RemoveDynamic(this, &UClassSelectionPanelWidget::HandleProfessionListChanged);
		ClassComponent->OnClassExperienceChanged.RemoveDynamic(this, &UClassSelectionPanelWidget::HandleClassExperienceChanged);
		ClassComponent->OnClassLevelChanged.RemoveDynamic(this, &UClassSelectionPanelWidget::HandleClassLevelChanged);
		ClassComponent->OnProfessionExperienceChanged.RemoveDynamic(this, &UClassSelectionPanelWidget::HandleProfessionExperienceChanged);
		ClassComponent->OnProfessionLevelChanged.RemoveDynamic(this, &UClassSelectionPanelWidget::HandleProfessionLevelChanged);
	}
	if (ExperienceComponent)
	{
		ExperienceComponent->OnExperienceChanged.RemoveDynamic(this, &UClassSelectionPanelWidget::HandlePoolChanged);
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
	if (ExperienceComponent)
	{
		ExperienceComponent->OnExperienceChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandlePoolChanged);
	}

	if (!ClassComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClassSelectionPanelWidget - ClassComponent not found on owning pawn."));
		return;
	}

	ClassComponent->OnClassListChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleClassListChanged);
	ClassComponent->OnProfessionListChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleProfessionListChanged);
	ClassComponent->OnClassExperienceChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleClassExperienceChanged);
	ClassComponent->OnClassLevelChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleClassLevelChanged);
	ClassComponent->OnProfessionExperienceChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleProfessionExperienceChanged);
	ClassComponent->OnProfessionLevelChanged.AddUniqueDynamic(this, &UClassSelectionPanelWidget::HandleProfessionLevelChanged);
}

void UClassSelectionPanelWidget::RefreshClassList()
{
	if (!ClassComponent)
	{
		return;
	}

	OnClassListUpdated(ClassComponent->GetSelectedClasses(), ClassComponent->GetSelectedProfessions());
	RefreshPanel();
}

void UClassSelectionPanelWidget::RefreshAllClassesList()
{
	OnAllClassesListUpdated(GetAllClasses());
}

void UClassSelectionPanelWidget::SetSelectedClassForXp(UClassDataAsset* ClassAsset)
{
	if (!ClassAsset)
	{
		return;
	}

	SelectedClassForXp = ClassAsset;
	SelectedProfessionForXp = nullptr;
	RefreshSelectedDetails();
	OnSelectedClassForXpChanged(SelectedClassForXp);
	RefreshPanel();
}

void UClassSelectionPanelWidget::SetSelectedProfessionForXp(UProfessionDataAsset* ProfessionAsset)
{
	if (!ProfessionAsset || !ClassComponent)
	{
		return;
	}
	SelectedClassForXp = nullptr;
	SelectedProfessionForXp = ProfessionAsset;
	RefreshSelectedDetails();
	OnSelectedClassForXpChanged(nullptr);
	RefreshPanel();
}

bool UClassSelectionPanelWidget::AllocateExperienceToSelectedClass(float Amount)
{
	if (SelectedProfessionForXp)
	{
		return ClassComponent && ClassComponent->AllocateExperienceToProfession(SelectedProfessionForXp, Amount);
	}
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
	if (!bAdded && SelectedProfessionForXp == ChangedProfession)
	{
		SelectedProfessionForXp = nullptr;
		RefreshSelectedDetails();
	}
	RefreshClassList();
	if (ClassComponent)
	{
		OnPlayerLevelUpdated(ClassComponent->GetPlayerLevel());
	}
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
	RefreshPanel();
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
	RefreshPanel();
}

void UClassSelectionPanelWidget::HandleProfessionLevelChanged(UProfessionDataAsset* ProfessionAsset, int32 NewLevel)
{
	RefreshClassList();
	if (ClassComponent)
	{
		OnPlayerLevelUpdated(ClassComponent->GetPlayerLevel());
	}
}

void UClassSelectionPanelWidget::HandleProfessionExperienceChanged(UProfessionDataAsset* ProfessionAsset, float NewExperience)
{
	RefreshClassList();
}
