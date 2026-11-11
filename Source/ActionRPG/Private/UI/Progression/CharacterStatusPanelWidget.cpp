// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Progression/CharacterStatusPanelWidget.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Components/Progression/ClassComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/AcquiredAttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Progression/TrainingComponent.h"
#include "Progression/Core/SecondaryAttributeDataAsset.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Components/Combat/StatusEffectComponent.h"
#include "Combat/StatusEffectDataAsset.h"
#include "Progression/Core/ClassDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/ScaleBox.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "UI/Common/DraggablePanelHandleWidget.h"

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

void UCharacterStatusPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	EnsureContentBuilt();
}

void UCharacterStatusPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EnsureContentBuilt();
	UDraggablePanelHandleWidget::AttachToOverlayTitle(this, TEXT("CharacterStatusTitleArea"), TEXT("CharacterStatusPanelSize"));
	BindComponents();
	UpdateStatusDisplay();
}

void UCharacterStatusPanelWidget::EnsureContentBuilt()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CharacterStatusCanvas"));
	Canvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WidgetTree->RootWidget = Canvas;
	UScaleBox* ResponsiveFrame = WidgetTree->ConstructWidget<UScaleBox>(UScaleBox::StaticClass(), TEXT("CharacterStatusResponsiveFrame"));
	ResponsiveFrame->SetStretch(EStretch::ScaleToFit);
	ResponsiveFrame->SetStretchDirection(EStretchDirection::DownOnly);
	UCanvasPanelSlot* FrameSlot = Canvas->AddChildToCanvas(ResponsiveFrame);
	FrameSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	FrameSlot->SetOffsets(FMargin(16.0f, 16.0f, -16.0f, -16.0f));
	USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("CharacterStatusPanelSize"));
	PanelSize->SetWidthOverride(940.0f);
	PanelSize->SetHeightOverride(650.0f);
	ResponsiveFrame->SetContent(PanelSize);
	UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CharacterStatusPanel"));
	Panel->SetBrushColor(FLinearColor(0.035f, 0.055f, 0.085f, 0.97f));
	Panel->SetPadding(FMargin(18.0f));
	PanelSize->SetContent(Panel);
	UVerticalBox* Main = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("CharacterStatusRows"));
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
		UTextBlock* LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(FString(Name) + TEXT("Label")));
		LabelText->SetText(FText::FromString(Label));
		LabelText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo Font = LabelText->GetFont();
		Font.Size = 16;
		LabelText->SetFont(Font);
		Button->SetContent(LabelText);
		return Button;
	};

	UHorizontalBox* Header = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("CharacterStatusHeader"));
	Main->AddChildToVerticalBox(Header)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CharacterStatusTitle"));
	Title->SetText(FText::FromString(TEXT("CHARACTER STATUS")));
	Title->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo TitleFont = Title->GetFont();
	TitleFont.Size = 25;
	Title->SetFont(TitleFont);
	UOverlay* TitleArea = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("CharacterStatusTitleArea"));
	Header->AddChildToHorizontalBox(TitleArea)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	TitleArea->AddChildToOverlay(Title);
	UButton* CloseButton = MakeButton(TEXT("CloseCharacterStatusButton"), TEXT("Close [C]"), FLinearColor(0.20f, 0.26f, 0.33f, 1.0f));
	CloseButton->OnClicked.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleCloseClicked);
	Header->AddChildToHorizontalBox(CloseButton);

	UHorizontalBox* Summary = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("CharacterStatusSummary"));
	Main->AddChildToVerticalBox(Summary)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	auto AddSummary = [this, &AddText, Summary](const TCHAR* Name, const TCHAR* Heading, TObjectPtr<UTextBlock>& Result)
	{
		UBorder* Card = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(FString(Name) + TEXT("Card")));
		Card->SetBrushColor(FLinearColor(0.07f, 0.10f, 0.15f, 1.0f));
		Card->SetPadding(FMargin(10.0f));
		Summary->AddChildToHorizontalBox(Card)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		UVerticalBox* Body = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(FString(Name) + TEXT("Body")));
		Card->SetContent(Body);
		AddText(Body, *FString::Printf(TEXT("%sHeading"), Name), Heading, 13, 3.0f);
		Result = AddText(Body, Name, TEXT("—"), 18, 0.0f);
	};
	AddSummary(TEXT("PlayerLevelText"), TEXT("COMBINED LEVEL"), PlayerLevelText);
	if (UWidget* LevelCard = WidgetTree->FindWidget(TEXT("PlayerLevelTextCard")))
	{
		LevelCard->SetToolTipText(FText::FromString(
			TEXT("The sum of active class and profession levels. It is 0 when no path is active. Level 1 paths remain available as starting choices.")));
	}
	AddSummary(TEXT("UnallocatedExperienceText"), TEXT("AVAILABLE XP"), UnallocatedExperienceText);
	AddSummary(TEXT("UnspentAttributePointsText"), TEXT("FREE ATTRIBUTE POINTS"), UnspentAttributePointsText);
	AddSummary(TEXT("UnspentSkillPointsText"), TEXT("FREE SKILL POINTS"), UnspentSkillPointsText);
	AddSummary(TEXT("TotalExperienceText"), TEXT("TOTAL XP"), TotalExperienceText);

	UHorizontalBox* Tabs = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("CharacterStatusTabs"));
	Main->AddChildToVerticalBox(Tabs)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	OverviewTabButton = MakeButton(TEXT("OverviewTabButton"), TEXT("OVERVIEW"), FLinearColor(0.18f, 0.42f, 0.55f, 1.0f));
	TrainingTabButton = MakeButton(TEXT("TrainingTabButton"), TEXT("TRAINING"), FLinearColor(0.11f, 0.17f, 0.24f, 1.0f));
	SourcesTabButton = MakeButton(TEXT("SourcesTabButton"), TEXT("SOURCES"), FLinearColor(0.11f, 0.17f, 0.24f, 1.0f));
	EffectsTabButton = MakeButton(TEXT("EffectsTabButton"), TEXT("EFFECTS"), FLinearColor(0.11f, 0.17f, 0.24f, 1.0f));
	OverviewTabButton->OnClicked.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleOverviewTabClicked);
	TrainingTabButton->OnClicked.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleTrainingTabClicked);
	SourcesTabButton->OnClicked.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleSourcesTabClicked);
	EffectsTabButton->OnClicked.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleEffectsTabClicked);
	for (UButton* TabButton : {OverviewTabButton.Get(), TrainingTabButton.Get(), SourcesTabButton.Get(), EffectsTabButton.Get()})
	{
		Tabs->AddChildToHorizontalBox(TabButton)->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
	}

	StatusTabSwitcher = WidgetTree->ConstructWidget<UWidgetSwitcher>(UWidgetSwitcher::StaticClass(), TEXT("CharacterStatusTabSwitcher"));
	Main->AddChildToVerticalBox(StatusTabSwitcher)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

	UHorizontalBox* Columns = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("CharacterStatusColumns"));
	StatusTabSwitcher->AddChild(Columns);
	auto MakeColumn = [this, Columns, AddText](const TCHAR* Name, const TCHAR* Heading) -> UVerticalBox*
	{
		UBorder* Column = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(FString(Name) + TEXT("Border")));
		Column->SetBrushColor(FLinearColor(0.07f, 0.10f, 0.15f, 1.0f));
		Column->SetPadding(FMargin(12.0f));
		UHorizontalBoxSlot* Slot = Columns->AddChildToHorizontalBox(Column);
		Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		Slot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
		UVerticalBox* Body = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(FString(Name) + TEXT("Body")));
		Column->SetContent(Body);
		AddText(Body, Name, Heading, 19, 10.0f);
		return Body;
	};
	UVerticalBox* Profile = MakeColumn(TEXT("CharacterProfile"), TEXT("PATHS & PROGRESSION"));
	ClassSlotsText = AddText(Profile, TEXT("ClassSlotsText"), TEXT("Class Slots: 0 / 0"), 16, 8.0f);
	ProfessionSlotsText = AddText(Profile, TEXT("ProfessionSlotsText"), TEXT("Profession Slots: 0 / 0"), 16, 14.0f);
	AddText(Profile, TEXT("SelectedClassesHeading"), TEXT("ACTIVE CLASSES"), 14, 4.0f);
	SelectedClassesText = AddText(Profile, TEXT("SelectedClassesText"), TEXT("None"), 15, 12.0f);
	AddText(Profile, TEXT("SelectedProfessionsHeading"), TEXT("ACTIVE PROFESSIONS"), 14, 4.0f);
	SelectedProfessionsText = AddText(Profile, TEXT("SelectedProfessionsText"), TEXT("None"), 15, 0.0f);
	AddText(Profile, TEXT("AcquiredAttributesHeading"), TEXT("ACQUIRED ATTRIBUTES"), 14, 4.0f);
	AcquiredOverviewText = AddText(Profile, TEXT("AcquiredOverviewText"), TEXT("Endurance 0 / 1"), 15, 0.0f);

	UVerticalBox* Attributes = MakeColumn(TEXT("CharacterAttributes"), TEXT("PRIMARY ATTRIBUTES"));
	AddText(Attributes, TEXT("AttributesHelp"), TEXT("Spend free points with +1. Hover over a stat to see its sources."), 14, 10.0f);
	AddAttributeRow(Attributes, TEXT("Strength"), TEXT("Strength"), EPrimaryAttribute::Strength);
	AddAttributeRow(Attributes, TEXT("Agility"), TEXT("Agility"), EPrimaryAttribute::Agility);
	AddAttributeRow(Attributes, TEXT("Constitution"), TEXT("Constitution"), EPrimaryAttribute::Constitution);
	AddAttributeRow(Attributes, TEXT("Dexterity"), TEXT("Dexterity"), EPrimaryAttribute::Dexterity);
	AddAttributeRow(Attributes, TEXT("Intelligence"), TEXT("Intelligence"), EPrimaryAttribute::Intelligence);
	AddAttributeRow(Attributes, TEXT("Wisdom"), TEXT("Wisdom"), EPrimaryAttribute::Wisdom);

	UBorder* TrainingPage = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CharacterStatusTrainingPage"));
	TrainingPage->SetBrushColor(FLinearColor(0.07f, 0.10f, 0.15f, 1.0f));
	TrainingPage->SetPadding(FMargin(18.0f));
	StatusTabSwitcher->AddChild(TrainingPage);
	UVerticalBox* TrainingBody = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("CharacterStatusTrainingBody"));
	TrainingPage->SetContent(TrainingBody);
	AddText(TrainingBody, TEXT("TrainingHeading"), TEXT("ATTRIBUTE TRAINING"), 20, 10.0f);
	AddText(TrainingBody, TEXT("TrainingHelp"), TEXT("Successful actions add practice. The counter resets when an attribute grows; earned growth stays permanent for this session."), 15, 18.0f);
	TrainingProgressText = AddText(TrainingBody, TEXT("TrainingProgressText"), TEXT("No practice yet"), 19, 0.0f);

	UScrollBox* SourcesPage = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("CharacterStatusSourcesPage"));
	StatusTabSwitcher->AddChild(SourcesPage);
	UVerticalBox* SourcesList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("CharacterStatusSourcesList"));
	SourcesPage->AddChild(SourcesList);
	AddText(SourcesList, TEXT("SourcesHeading"), TEXT("ATTRIBUTE SOURCES"), 20, 8.0f);
	AddText(SourcesList, TEXT("SourcesHelp"), TEXT("Base and allocated points are shown separately from class, equipment, training, and temporary effects."), 15, 16.0f);
	AcquiredSourcesText = AddText(SourcesList, TEXT("AcquiredSourcesText"),
		TEXT("Endurance: 0 / 1 (capped by persistent Constitution)"), 16, 16.0f);
	const TCHAR* SourceNames[] = {TEXT("Strength"), TEXT("Agility"), TEXT("Constitution"), TEXT("Dexterity"), TEXT("Intelligence"), TEXT("Wisdom")};
	for (const TCHAR* SourceName : SourceNames)
	{
		UBorder* SourceCard = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(FString(SourceName) + TEXT("SourceCard")));
		SourceCard->SetBrushColor(FLinearColor(0.07f, 0.10f, 0.15f, 1.0f));
		SourceCard->SetPadding(FMargin(12.0f));
		SourcesList->AddChildToVerticalBox(SourceCard)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
		UVerticalBox* CardBody = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), FName(FString(SourceName) + TEXT("SourceBody")));
		SourceCard->SetContent(CardBody);
		UTextBlock* SourceText = AddText(CardBody, *FString::Printf(TEXT("%sSourceText"), SourceName), TEXT("No attribute data"), 16, 0.0f);
		AttributeSourceTexts.Add(SourceText);
	}
	UScrollBox* EffectsPage = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("CharacterStatusEffectsPage"));
	StatusTabSwitcher->AddChild(EffectsPage);
	UVerticalBox* EffectsBody = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("CharacterStatusEffectsBody"));
	EffectsPage->AddChild(EffectsBody);
	AddText(EffectsBody, TEXT("ActiveEffectsHeading"), TEXT("ACTIVE EFFECTS"), 20, 8.0f);
	AddText(EffectsBody, TEXT("ActiveEffectsHelp"), TEXT("Temporary effects end when their timer runs out. Stack counts and remaining time update as effects change."), 15, 16.0f);
	ActiveEffectsText = AddText(EffectsBody, TEXT("ActiveEffectsText"), TEXT("No active effects"), 18, 0.0f);
	SetStatusTab(0);
}

void UCharacterStatusPanelWidget::SetStatusTab(int32 TabIndex)
{
	if (!StatusTabSwitcher || TabIndex < 0 || TabIndex >= StatusTabSwitcher->GetChildrenCount()) return;
	StatusTabSwitcher->SetActiveWidgetIndex(TabIndex);
	const FLinearColor ActiveColor(0.18f, 0.42f, 0.55f, 1.0f);
	const FLinearColor InactiveColor(0.11f, 0.17f, 0.24f, 1.0f);
	if (OverviewTabButton) OverviewTabButton->SetBackgroundColor(TabIndex == 0 ? ActiveColor : InactiveColor);
	if (TrainingTabButton) TrainingTabButton->SetBackgroundColor(TabIndex == 1 ? ActiveColor : InactiveColor);
	if (SourcesTabButton) SourcesTabButton->SetBackgroundColor(TabIndex == 2 ? ActiveColor : InactiveColor);
	if (EffectsTabButton) EffectsTabButton->SetBackgroundColor(TabIndex == 3 ? ActiveColor : InactiveColor);
}

void UCharacterStatusPanelWidget::HandleOverviewTabClicked() { SetStatusTab(0); }
void UCharacterStatusPanelWidget::HandleTrainingTabClicked() { SetStatusTab(1); }
void UCharacterStatusPanelWidget::HandleSourcesTabClicked() { SetStatusTab(2); }
void UCharacterStatusPanelWidget::HandleEffectsTabClicked() { SetStatusTab(3); }

void UCharacterStatusPanelWidget::HandleEffectsChanged()
{
	if (ActiveEffectsText) ActiveEffectsText->SetText(BuildActiveEffectsText());
}

void UCharacterStatusPanelWidget::AddAttributeRow(UVerticalBox* Parent, const TCHAR* Name, const TCHAR* Label, EPrimaryAttribute Attribute)
{
	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), FName(FString(Name) + TEXT("AttributeRow")));
	Parent->AddChildToVerticalBox(Row)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 7.0f));
	UTextBlock* LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(FString(Name) + TEXT("AttributeLabel")));
	LabelText->SetText(FText::FromString(Label));
	LabelText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo Font = LabelText->GetFont(); Font.Size = 17; LabelText->SetFont(Font);
	Row->AddChildToHorizontalBox(LabelText)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	UTextBlock* ValueText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(FString(Name) + TEXT("AttributeValue")));
	ValueText->SetText(FText::FromString(TEXT("0")));
	ValueText->SetColorAndOpacity(FSlateColor(FLinearColor(0.68f, 0.84f, 1.0f, 1.0f)));
	Font = ValueText->GetFont(); Font.Size = 17; ValueText->SetFont(Font);
	Row->AddChildToHorizontalBox(ValueText)->SetPadding(FMargin(8.0f));
	UButton* AddButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(FString(Name) + TEXT("AddButton")));
	AddButton->SetBackgroundColor(FLinearColor(0.12f, 0.38f, 0.28f, 1.0f));
	UTextBlock* ButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(FString(Name) + TEXT("AddLabel")));
	ButtonText->SetText(FText::FromString(TEXT("+1"))); ButtonText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	AddButton->SetContent(ButtonText);
	switch (Attribute)
	{
	case EPrimaryAttribute::Strength: AddButton->OnClicked.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleAllocateStrength); break;
	case EPrimaryAttribute::Agility: AddButton->OnClicked.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleAllocateAgility); break;
	case EPrimaryAttribute::Constitution: AddButton->OnClicked.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleAllocateConstitution); break;
	case EPrimaryAttribute::Dexterity: AddButton->OnClicked.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleAllocateDexterity); break;
	case EPrimaryAttribute::Intelligence: AddButton->OnClicked.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleAllocateIntelligence); break;
	case EPrimaryAttribute::Wisdom: AddButton->OnClicked.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleAllocateWisdom); break;
	}
	Row->AddChildToHorizontalBox(AddButton);
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
		ClassComponent->OnProfessionLevelChanged.RemoveDynamic(this, &UCharacterStatusPanelWidget::HandleProfessionLevelChanged);
	}

	if (AttributeComponent)
	{
		AttributeComponent->OnPrimaryAttributeChanged.RemoveDynamic(this, &UCharacterStatusPanelWidget::HandlePrimaryAttributeChanged);
	}
	if (AcquiredAttributeComponent)
	{
		AcquiredAttributeComponent->OnAcquiredAttributeChanged.RemoveDynamic(
			this, &UCharacterStatusPanelWidget::HandleAcquiredAttributeChanged);
	}
	if (SkillManagerComponent)
	{
		SkillManagerComponent->OnSkillPointsChanged.RemoveDynamic(this, &UCharacterStatusPanelWidget::HandleSkillPointsChanged);
	}
	if (TrainingComponent)
	{
		TrainingComponent->OnAttributePractice.RemoveDynamic(this, &UCharacterStatusPanelWidget::HandleAttributePractice);
	}
	if (StatusEffectComponent)
	{
		StatusEffectComponent->OnStatusEffectsChanged.RemoveDynamic(this, &UCharacterStatusPanelWidget::HandleEffectsChanged);
		StatusEffectComponent->OnStatusEffectTimeChanged.RemoveDynamic(this, &UCharacterStatusPanelWidget::HandleEffectsChanged);
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
	AttributeComponent = OwningPawn->FindComponentByClass<UAttributeComponent>();
	AcquiredAttributeComponent = OwningPawn->FindComponentByClass<UAcquiredAttributeComponent>();
	SecondaryAttributeComponent = OwningPawn->FindComponentByClass<USecondaryAttributeComponent>();
	SkillManagerComponent = OwningPawn->FindComponentByClass<USkillManagerComponent>();
	TrainingComponent = OwningPawn->FindComponentByClass<UTrainingComponent>();
	StatusEffectComponent = OwningPawn->FindComponentByClass<UStatusEffectComponent>();

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
		ClassComponent->OnProfessionLevelChanged.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleProfessionLevelChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterStatusPanelWidget - ClassComponent not found on owning pawn."));
	}

	if (AttributeComponent)
	{
		AttributeComponent->OnPrimaryAttributeChanged.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandlePrimaryAttributeChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterStatusPanelWidget - AttributeComponent not found on owning pawn."));
	}
	if (AcquiredAttributeComponent)
	{
		AcquiredAttributeComponent->OnAcquiredAttributeChanged.AddUniqueDynamic(
			this, &UCharacterStatusPanelWidget::HandleAcquiredAttributeChanged);
	}
	if (SkillManagerComponent)
	{
		SkillManagerComponent->OnSkillPointsChanged.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleSkillPointsChanged);
	}
	if (TrainingComponent)
	{
		TrainingComponent->OnAttributePractice.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleAttributePractice);
	}
	if (StatusEffectComponent)
	{
		StatusEffectComponent->OnStatusEffectsChanged.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleEffectsChanged);
		StatusEffectComponent->OnStatusEffectTimeChanged.AddUniqueDynamic(this, &UCharacterStatusPanelWidget::HandleEffectsChanged);
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

void UCharacterStatusPanelWidget::HandleProfessionLevelChanged(UProfessionDataAsset* ProfessionAsset, int32 NewLevel)
{
	UpdateStatusDisplay();
}

void UCharacterStatusPanelWidget::HandlePrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue)
{
	UpdateStatusDisplay();
}

void UCharacterStatusPanelWidget::HandleAcquiredAttributeChanged(EAcquiredAttribute Attribute, int32 NewValue, int32 Cap)
{
	UpdateStatusDisplay();
}

void UCharacterStatusPanelWidget::HandleSkillPointsChanged(int32 NewBalance, int32 Delta)
{
	UpdateStatusDisplay();
}

void UCharacterStatusPanelWidget::HandleAttributePractice(EPrimaryAttribute Attribute, FName Activity,
	int32 Progress, int32 Required, bool bGrantedGrowth)
{
	UpdateStatusDisplay();
}

void UCharacterStatusPanelWidget::UpdateStatusDisplay()
{
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
		ClassSlotsText->SetText(FText::FromString(FString::Printf(TEXT("Class Slots: %s"), *GetClassSlotsText().ToString())));
	}

	if (ProfessionSlotsText)
	{
		ProfessionSlotsText->SetText(FText::FromString(FString::Printf(TEXT("Profession Slots: %s"), *GetProfessionSlotsText().ToString())));
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

	if (PrimaryAttributesText)
	{
		PrimaryAttributesText->SetText(FText::GetEmpty());
	}

	if (UnspentAttributePointsText)
	{
		UnspentAttributePointsText->SetText(FText::AsNumber(GetUnspentAttributePoints()));
	}
	if (UnspentSkillPointsText)
	{
		UnspentSkillPointsText->SetText(FText::AsNumber(GetUnspentSkillPoints()));
	}
	if (TrainingProgressText)
	{
		TrainingProgressText->SetText(BuildTrainingProgressText());
	}
	const int32 Endurance = AcquiredAttributeComponent
		? AcquiredAttributeComponent->GetAcquiredAttribute(EAcquiredAttribute::Endurance) : 0;
	const int32 EnduranceCap = AcquiredAttributeComponent
		? AcquiredAttributeComponent->GetAcquiredAttributeCap(EAcquiredAttribute::Endurance) : 0;
	const USecondaryAttributeDataAsset* ResourceRules = SecondaryAttributeComponent
		? SecondaryAttributeComponent->SecondaryAttributeDataAsset.Get() : nullptr;
	const float BaseStaminaRegeneration = ResourceRules ? ResourceRules->StaminaRegenerationPerSecond : 0.0f;
	const float StaminaRegeneration = SecondaryAttributeComponent
		? SecondaryAttributeComponent->GetRegenerationRate(ESecondaryAttribute::MaxStamina) : 0.0f;
	if (AcquiredOverviewText)
	{
		AcquiredOverviewText->SetText(FText::FromString(FString::Printf(
			TEXT("Endurance %d / %d\nStamina Regen %.1f / sec"), Endurance, EnduranceCap, StaminaRegeneration)));
	}
	if (AcquiredSourcesText)
	{
		AcquiredSourcesText->SetText(FText::FromString(FString::Printf(
			TEXT("Endurance: %d / %d (persistent Constitution cap)\nEarned through special rewards; affects resource maximums.\nStamina Regen: %.1f / sec (base %.1f + Endurance %.1f)"),
			Endurance, EnduranceCap, StaminaRegeneration, BaseStaminaRegeneration,
			StaminaRegeneration - BaseStaminaRegeneration)));
	}
	HandleEffectsChanged();

	const bool bCanAllocate = GetUnspentAttributePoints() > 0;
	const EPrimaryAttribute Attributes[] = { EPrimaryAttribute::Strength, EPrimaryAttribute::Agility, EPrimaryAttribute::Constitution, EPrimaryAttribute::Dexterity, EPrimaryAttribute::Intelligence, EPrimaryAttribute::Wisdom };
	const TCHAR* AttributeNames[] = { TEXT("Strength"), TEXT("Agility"), TEXT("Constitution"), TEXT("Dexterity"), TEXT("Intelligence"), TEXT("Wisdom") };
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(Attributes); ++Index)
	{
		const FText BreakdownText = BuildAttributeBreakdownText(Attributes[Index]);
		if (AttributeSourceTexts.IsValidIndex(Index) && AttributeSourceTexts[Index])
		{
			AttributeSourceTexts[Index]->SetText(BreakdownText);
		}
		if (UTextBlock* ValueText = Cast<UTextBlock>(WidgetTree->FindWidget(FName(FString(AttributeNames[Index]) + TEXT("AttributeValue")))))
		{
			ValueText->SetText(FText::AsNumber(FMath::RoundToInt(AttributeComponent ? AttributeComponent->GetAttribute(Attributes[Index]) : 0.0f)));
			ValueText->SetToolTipText(BreakdownText);
		}
		if (UTextBlock* LabelText = Cast<UTextBlock>(WidgetTree->FindWidget(FName(FString(AttributeNames[Index]) + TEXT("AttributeLabel")))))
		{
			LabelText->SetToolTipText(BreakdownText);
		}
		if (UButton* AddButton = Cast<UButton>(WidgetTree->FindWidget(FName(FString(AttributeNames[Index]) + TEXT("AddButton")))))
		{
			AddButton->SetIsEnabled(bCanAllocate);
		}
	}
}

FText UCharacterStatusPanelWidget::BuildAttributeBreakdownText(EPrimaryAttribute Attribute) const
{
	if (!AttributeComponent) return FText::FromString(TEXT("No attribute data"));
	const FAttributeBreakdown Breakdown = AttributeComponent->GetAttributeBreakdown(Attribute);
	const FString AttributeName = StaticEnum<EPrimaryAttribute>()->GetNameStringByValue(static_cast<int64>(Attribute));
	TArray<FString> Lines;
	Lines.Add(FString::Printf(TEXT("%s: %.1f"), *AttributeName, Breakdown.FinalTotal));
	Lines.Add(FString::Printf(TEXT("Base: %.1f"), Breakdown.Base));
	if (!FMath::IsNearlyZero(Breakdown.Allocated))
	{
		Lines.Add(FString::Printf(TEXT("Allocated free points: %+.1f"), Breakdown.Allocated));
	}
	for (const FAttributeSourceContribution& Source : Breakdown.Sources)
	{
		const FString Id = Source.SourceId.ToString();
		FString Label = Id;
		if (Id == TEXT("Class.Selected")) Label = TEXT("Selected class");
		else if (Id == TEXT("Class.Earned")) Label = TEXT("Earned class levels");
		else if (Id == TEXT("Class.Legacy")) Label = TEXT("Legacy class grants");
		else if (Id.StartsWith(TEXT("Training."))) Label = TEXT("Training");
		else if (Id.StartsWith(TEXT("Equipment."))) Label = TEXT("Equipment");
		else if (Id.StartsWith(TEXT("Status."))) Label = TEXT("Active effect");
		Lines.Add(FString::Printf(TEXT("%s: %+.1f%s"), *Label, Source.Amount,
			Source.bPersistent ? TEXT(" (permanent)") : TEXT("")));
	}
	if (!FMath::IsNearlyZero(Breakdown.LimitAdjustment))
	{
		Lines.Add(FString::Printf(TEXT("Attribute limit: %+.1f"), Breakdown.LimitAdjustment));
	}
	return FText::FromString(FString::Join(Lines, TEXT("\n")));
}

FText UCharacterStatusPanelWidget::BuildTrainingProgressText() const
{
	if (!TrainingComponent) return FText::FromString(TEXT("No training data"));
	TArray<FString> Lines;
	for (const FAttributeTrainingRule& Rule : TrainingComponent->Rules)
	{
		if (Rule.Activity.IsNone() || Rule.PracticePerGain <= 0) continue;
		const FString AttributeName = StaticEnum<EPrimaryAttribute>()->GetNameStringByValue(static_cast<int64>(Rule.Attribute));
		const FString ActivityName = Rule.Activity == TEXT("MeleeHit") ? TEXT("melee")
			: Rule.Activity == TEXT("CombatDodge") ? TEXT("dodge")
			: Rule.Activity == TEXT("SurviveEnemyHit") ? TEXT("survive hit")
			: Rule.Activity == TEXT("RangedHit") ? TEXT("ranged hit")
			: Rule.Activity == TEXT("MagicHit") ? TEXT("magic hit")
			: Rule.Activity == TEXT("EffectiveSkillHeal") ? TEXT("skill heal") : Rule.Activity.ToString();
		Lines.Add(FString::Printf(TEXT("%s %s: %d/%d (+%d)"), *AttributeName, *ActivityName,
			TrainingComponent->GetPracticeProgress(Rule.Attribute, Rule.Activity), Rule.PracticePerGain,
			TrainingComponent->GetEarnedTrainingGain(Rule.Attribute)));
	}
	return FText::FromString(Lines.IsEmpty() ? TEXT("No training paths") : FString::Join(Lines, TEXT("\n")));
}

FText UCharacterStatusPanelWidget::BuildActiveEffectsText() const
{
	if (!StatusEffectComponent || StatusEffectComponent->GetActiveEffects().IsEmpty())
	{
		return FText::FromString(TEXT("No active effects"));
	}
	TArray<FString> Lines;
	for (const FActiveStatusEffect& Effect : StatusEffectComponent->GetActiveEffects())
	{
		if (!Effect.Definition) continue;
		const UStatusEffectDataAsset* Definition = Effect.Definition;
		const FString Name = Definition->DisplayName.IsEmpty()
			? Definition->EffectID.ToString() : Definition->DisplayName.ToString();
		Lines.Add(FString::Printf(TEXT("%s%s — %d seconds remaining"), *Name,
			Effect.Stacks > 1 ? *FString::Printf(TEXT(" x%d"), Effect.Stacks) : TEXT(""),
			FMath::Max(0, FMath::CeilToInt(Effect.RemainingSeconds))));
		TArray<FString> Changes;
		const float EffectiveHealthPerTick = StatusEffectComponent->GetEffectiveHealthPerTick(Effect);
		if (!FMath::IsNearlyZero(EffectiveHealthPerTick))
			Changes.Add(FString::Printf(TEXT("%+.1f health"), EffectiveHealthPerTick));
		if (!FMath::IsNearlyZero(Definition->ManaPerTick))
			Changes.Add(FString::Printf(TEXT("%+.0f mana"), Definition->ManaPerTick * Effect.Stacks));
		if (!FMath::IsNearlyZero(Definition->StaminaPerTick))
			Changes.Add(FString::Printf(TEXT("%+.0f stamina"), Definition->StaminaPerTick * Effect.Stacks));
		if (!Changes.IsEmpty())
		{
			Lines.Add(FString::Printf(TEXT("  %s every %.1f seconds"), *FString::Join(Changes, TEXT(", ")),
				Definition->TickIntervalSeconds));
		}
	}
	return FText::FromString(Lines.IsEmpty() ? TEXT("No active effects") : FString::Join(Lines, TEXT("\n\n")));
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

FText UCharacterStatusPanelWidget::GetPrimaryAttributesText() const
{
	return BuildPrimaryAttributeText();
}

int32 UCharacterStatusPanelWidget::GetUnspentAttributePoints() const
{
	return AttributeComponent ? AttributeComponent->GetUnspentAttributePoints() : 0;
}

int32 UCharacterStatusPanelWidget::GetUnspentSkillPoints() const
{
	return SkillManagerComponent ? SkillManagerComponent->GetUnspentSkillPoints() : 0;
}

bool UCharacterStatusPanelWidget::AllocateAttributePoint(EPrimaryAttribute Attribute)
{
	if (!AttributeComponent)
	{
		return false;
	}

	const bool bAllocated = AttributeComponent->AllocateAttributePoints(Attribute, 1);
	if (bAllocated)
	{
		UpdateStatusDisplay();
	}
	return bAllocated;
}

void UCharacterStatusPanelWidget::HandleAllocateStrength() { AllocateAttributePoint(EPrimaryAttribute::Strength); }
void UCharacterStatusPanelWidget::HandleAllocateAgility() { AllocateAttributePoint(EPrimaryAttribute::Agility); }
void UCharacterStatusPanelWidget::HandleAllocateConstitution() { AllocateAttributePoint(EPrimaryAttribute::Constitution); }
void UCharacterStatusPanelWidget::HandleAllocateDexterity() { AllocateAttributePoint(EPrimaryAttribute::Dexterity); }
void UCharacterStatusPanelWidget::HandleAllocateIntelligence() { AllocateAttributePoint(EPrimaryAttribute::Intelligence); }
void UCharacterStatusPanelWidget::HandleAllocateWisdom() { AllocateAttributePoint(EPrimaryAttribute::Wisdom); }

void UCharacterStatusPanelWidget::HandleCloseClicked()
{
	SetVisibility(ESlateVisibility::Collapsed);
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetPause(false);
	}
}

FText UCharacterStatusPanelWidget::BuildClassListText() const
{
	if (!ClassComponent)
	{
		return FText::FromString(TEXT("None"));
	}

	return ClassComponent->GetSelectedClassesText();
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
			Names.Add(FString::Printf(TEXT("%s: %d"), *ProfessionAsset->ProfessionName.ToString(), ClassComponent->GetProfessionLevel(ProfessionAsset)));
		}
	}

	return JoinNames(Names);
}

FText UCharacterStatusPanelWidget::BuildPrimaryAttributeText() const
{
	if (!AttributeComponent)
	{
		return FText::FromString(TEXT("None"));
	}

	const UEnum* Enum = StaticEnum<EPrimaryAttribute>();
	TArray<FString> Lines;
	const int32 EnumCount = Enum ? Enum->NumEnums() : 0;
	for (int32 Index = 0; Index < EnumCount; ++Index)
	{
		if (Enum)
		{
			const FString EnumName = Enum->GetNameStringByIndex(Index);
			if (EnumName.EndsWith(TEXT("MAX")))
			{
				continue;
			}
		}

		const EPrimaryAttribute Attribute = Enum ? static_cast<EPrimaryAttribute>(Enum->GetValueByIndex(Index)) : EPrimaryAttribute::Strength;
		const float Value = AttributeComponent->GetAttribute(Attribute);
		const FString DisplayName = Enum ? Enum->GetDisplayNameTextByIndex(Index).ToString() : TEXT("Attribute");
		Lines.Add(FString::Printf(TEXT("%s: %.0f"), *DisplayName, Value));
	}

	if (Lines.Num() == 0)
	{
		return FText::FromString(TEXT("None"));
	}

	const int32 UnspentPoints = GetUnspentAttributePoints();
	Lines.Add(FString::Printf(TEXT("Free Attribute Points: %d"), UnspentPoints));

	return FText::FromString(FString::Join(Lines, TEXT("\n")));
}
