#include "UI/Skills/SkillCatalogEntryWidget.h"
#include "UI/ActionRPGUITheme.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Progression/Core/ClassDataAsset.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void USkillCatalogEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	UBorder* Background = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CatalogEntryBackground"));
	Background->SetBrushColor(ActionRPGUITheme::Slot());
	Background->SetPadding(FMargin(6.0f));
	WidgetTree->RootWidget = Background;
	UVerticalBox* Layout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("CatalogEntryLayout"));
	Background->SetContent(Layout);
	const auto AddText = [this, Layout](const TCHAR* Name, int32 Size) -> UTextBlock*
	{
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(Name));
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Text->SetAutoWrapText(true);
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = Size;
		Text->SetFont(Font);
		Layout->AddChildToVerticalBox(Text);
		return Text;
	};
	NameText = AddText(TEXT("CatalogSkillName"), 15);
	StatusText = AddText(TEXT("CatalogSkillStatus"), 12);
	LearnButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("LearnSkillButton"));
	LearnButton->SetBackgroundColor(ActionRPGUITheme::Accent());
	LearnText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LearnSkillButtonText"));
	LearnText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	LearnText->SetJustification(ETextJustify::Center);
	LearnText->SetVisibility(ESlateVisibility::HitTestInvisible);
	FSlateFontInfo LearnFont = LearnText->GetFont();
	LearnFont.Size = 14;
	LearnText->SetFont(LearnFont);
	LearnButton->SetContent(LearnText);
	UVerticalBoxSlot* ButtonSlot = Layout->AddChildToVerticalBox(LearnButton);
	ButtonSlot->SetPadding(FMargin(0.0f, 4.0f, 0.0f, 0.0f));
	LearnButton->OnPressed.AddDynamic(this, &USkillCatalogEntryWidget::OnLearnClicked);
	RefreshDisplay();
}

void USkillCatalogEntryWidget::SetCatalogEntry(USkillManagerComponent* InManager, int32 InCatalogIndex)
{
	SkillManager = InManager;
	CatalogIndex = InCatalogIndex;
	RefreshDisplay();
}

void USkillCatalogEntryWidget::RefreshDisplay()
{
	if (!NameText || !StatusText || !LearnText || !LearnButton) return;
	const FSkillCatalogEntry* Entry = SkillManager && SkillManager->SkillCatalog.IsValidIndex(CatalogIndex)
		? &SkillManager->SkillCatalog[CatalogIndex] : nullptr;
	const USkillDataAsset* Data = Entry ? Entry->SkillData.Get() : nullptr;
	if (!Data)
	{
		NameText->SetText(FText::FromString(TEXT("Unavailable skill")));
		StatusText->SetText(FText::FromString(TEXT("Skill definition missing")));
		LearnButton->SetIsEnabled(false);
		return;
	}
	NameText->SetText(Data->SkillName);
	StatusText->SetText(SkillManager->GetCatalogSkillStatus(CatalogIndex));
	const int32 Cost = Data->SkillPointCost;
	LearnText->SetText(FText::FromString(FString::Printf(TEXT("Learn (%d point%s)"), Cost, Cost == 1 ? TEXT("") : TEXT("s"))));
	LearnButton->SetIsEnabled(SkillManager->CanLearnCatalogSkill(CatalogIndex));

	FString Requirements = Data->SkillDescription.ToString();
	Requirements += FString::Printf(TEXT("\nRequires player level %d"), Data->RequiredLevel);
	for (const TPair<EPrimaryAttribute, float>& Requirement : Data->RequiredAttributes)
	{
		const UEnum* AttributeEnum = StaticEnum<EPrimaryAttribute>();
		const FString Name = AttributeEnum ? AttributeEnum->GetDisplayNameTextByValue(static_cast<int64>(Requirement.Key)).ToString() : TEXT("Attribute");
		Requirements += FString::Printf(TEXT("\nRequires %s %.0f"), *Name, Requirement.Value);
	}
	if (Data->RequiredClass)
	{
		Requirements += FString::Printf(TEXT("\nRequires %s level %d"), *Data->RequiredClass->ClassName.ToString(), Data->RequiredClassLevel);
	}
	for (const FName SkillID : Data->PrerequisiteSkillIDs)
	{
		Requirements += FString::Printf(TEXT("\nRequires skill %s"), *SkillID.ToString());
	}
	for (const FSkillAttributeMilestone& Milestone : Data->AttributeMilestones)
	{
		const UEnum* AttributeEnum = StaticEnum<EPrimaryAttribute>();
		const FString Name = AttributeEnum
			? AttributeEnum->GetDisplayNameTextByValue(static_cast<int64>(Milestone.Attribute)).ToString() : TEXT("Attribute");
		Requirements += FString::Printf(TEXT("\nAt skill level %d: +%.0f %s"), Milestone.SkillLevel, Milestone.Amount, *Name);
	}
	Requirements += FString::Printf(TEXT("\nCost: %d free skill point%s"), Cost, Cost == 1 ? TEXT("") : TEXT("s"));
	SetToolTipText(FText::FromString(Requirements));
}

void USkillCatalogEntryWidget::OnLearnClicked()
{
	if (SkillManager) SkillManager->LearnCatalogSkill(CatalogIndex);
}
