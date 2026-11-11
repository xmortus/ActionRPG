// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/ScaleBox.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "UI/Progression/ClassSelectionPanelWidget.h"
#include "UI/Progression/CharacterStatusPanelWidget.h"
#include "Progression/Core/ClassDataAsset.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProgressionPanelUITest, "ActionRPG.Progression.PanelUI",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FProgressionPanelUITest::RunTest(const FString& Parameters)
{
	UClassSelectionPanelWidget* Panel = NewObject<UClassSelectionPanelWidget>(GetTransientPackage());
	if (!TestNotNull(TEXT("Progression panel can be created"), Panel)) return false;
	Panel->WidgetTree = NewObject<UWidgetTree>(Panel);
	Panel->EnsureContentBuilt();
	TestNotNull(TEXT("Responsive panel frame exists"), Cast<UScaleBox>(Panel->WidgetTree->FindWidget(TEXT("ProgressionResponsiveFrame"))));
	TestNotNull(TEXT("Available XP is visible"), Cast<UTextBlock>(Panel->WidgetTree->FindWidget(TEXT("AvailableXpText"))));
	TestNotNull(TEXT("Class list exists"), Cast<UVerticalBox>(Panel->WidgetTree->FindWidget(TEXT("ClassListRows"))));
	TestNotNull(TEXT("Profession list exists"), Cast<UVerticalBox>(Panel->WidgetTree->FindWidget(TEXT("ProfessionListRows"))));
	TestNotNull(TEXT("Requirements are shown"), Cast<UTextBlock>(Panel->WidgetTree->FindWidget(TEXT("SelectedPathRequirements"))));
	TestNotNull(TEXT("Rewards are shown"), Cast<UTextBlock>(Panel->WidgetTree->FindWidget(TEXT("SelectedPathRewards"))));
	TestNotNull(TEXT("XP amount input exists"), Cast<USpinBox>(Panel->WidgetTree->FindWidget(TEXT("Input_XPAmount"))));
	TestNotNull(TEXT("Assign action exists"), Cast<UButton>(Panel->WidgetTree->FindWidget(TEXT("Button_AllocateXP"))));
	TestNotNull(TEXT("Activate action exists"), Cast<UButton>(Panel->WidgetTree->FindWidget(TEXT("ActivatePathButton"))));
	TestNotNull(TEXT("Remove action exists"), Cast<UButton>(Panel->WidgetTree->FindWidget(TEXT("RemovePathButton"))));
	UClassDataAsset* AdvancedClass = NewObject<UClassDataAsset>(Panel);
	AdvancedClass->ClassName = FText::FromString(TEXT("Test Advanced Class"));
	AdvancedClass->ClassTier = ECharacterClassTier::Advanced;
	Panel->SetSelectedClassForXp(AdvancedClass);
	const UTextBlock* ClassName = Cast<UTextBlock>(Panel->WidgetTree->FindWidget(TEXT("SelectedPathName")));
	const UTextBlock* Rewards = Cast<UTextBlock>(Panel->WidgetTree->FindWidget(TEXT("SelectedPathRewards")));
	TestTrue(TEXT("Selected class shows its tier"), ClassName && ClassName->GetText().ToString().Contains(TEXT("Advanced")));
	TestTrue(TEXT("Selected class shows its free point rate"), Rewards && Rewards->GetText().ToString().Contains(TEXT("5 free Attribute Points per level")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterStatusPanelUITest, "ActionRPG.Progression.CharacterStatusPanelUI",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCharacterStatusPanelUITest::RunTest(const FString& Parameters)
{
	UCharacterStatusPanelWidget* Panel = NewObject<UCharacterStatusPanelWidget>(GetTransientPackage());
	if (!TestNotNull(TEXT("Character status panel can be created"), Panel)) return false;
	Panel->WidgetTree = NewObject<UWidgetTree>(Panel);
	Panel->EnsureContentBuilt();
	TestNotNull(TEXT("Status responsive frame exists"), Cast<UScaleBox>(Panel->WidgetTree->FindWidget(TEXT("CharacterStatusResponsiveFrame"))));
	TestNotNull(TEXT("Free points summary exists"), Cast<UTextBlock>(Panel->WidgetTree->FindWidget(TEXT("UnspentAttributePointsText"))));
	TestNotNull(TEXT("Strength value exists"), Cast<UTextBlock>(Panel->WidgetTree->FindWidget(TEXT("StrengthAttributeValue"))));
	TestNotNull(TEXT("Training progress exists"), Cast<UTextBlock>(Panel->WidgetTree->FindWidget(TEXT("TrainingProgressText"))));
	UWidgetSwitcher* TabSwitcher = Cast<UWidgetSwitcher>(Panel->WidgetTree->FindWidget(TEXT("CharacterStatusTabSwitcher")));
	UButton* OverviewTab = Cast<UButton>(Panel->WidgetTree->FindWidget(TEXT("OverviewTabButton")));
	UButton* TrainingTab = Cast<UButton>(Panel->WidgetTree->FindWidget(TEXT("TrainingTabButton")));
	UButton* SourcesTab = Cast<UButton>(Panel->WidgetTree->FindWidget(TEXT("SourcesTabButton")));
	UButton* EffectsTab = Cast<UButton>(Panel->WidgetTree->FindWidget(TEXT("EffectsTabButton")));
	if (TestNotNull(TEXT("Character status tab switcher exists"), TabSwitcher) &&
		TestNotNull(TEXT("Overview tab exists"), OverviewTab) &&
		TestNotNull(TEXT("Training tab exists"), TrainingTab) &&
		TestNotNull(TEXT("Sources tab exists"), SourcesTab) &&
		TestNotNull(TEXT("Effects tab exists"), EffectsTab))
	{
		TestEqual(TEXT("Four status pages exist"), TabSwitcher->GetChildrenCount(), 4);
		TestEqual(TEXT("Overview is open by default"), TabSwitcher->GetActiveWidgetIndex(), 0);
		TrainingTab->OnClicked.Broadcast();
		TestEqual(TEXT("Training tab opens practice page"), TabSwitcher->GetActiveWidgetIndex(), 1);
		SourcesTab->OnClicked.Broadcast();
		TestEqual(TEXT("Sources tab opens breakdown page"), TabSwitcher->GetActiveWidgetIndex(), 2);
		EffectsTab->OnClicked.Broadcast();
		TestEqual(TEXT("Effects tab opens active effects page"), TabSwitcher->GetActiveWidgetIndex(), 3);
		OverviewTab->OnClicked.Broadcast();
		TestEqual(TEXT("Overview tab restores main page"), TabSwitcher->GetActiveWidgetIndex(), 0);
	}
	TestNotNull(TEXT("Strength source breakdown exists"), Cast<UTextBlock>(Panel->WidgetTree->FindWidget(TEXT("StrengthSourceText"))));
	TestNotNull(TEXT("Wisdom source breakdown exists"), Cast<UTextBlock>(Panel->WidgetTree->FindWidget(TEXT("WisdomSourceText"))));
	TestNotNull(TEXT("Effect details exist"), Cast<UTextBlock>(Panel->WidgetTree->FindWidget(TEXT("ActiveEffectsText"))));
	TestNotNull(TEXT("Strength allocation action exists"), Cast<UButton>(Panel->WidgetTree->FindWidget(TEXT("StrengthAddButton"))));
	TestNotNull(TEXT("Wisdom allocation action exists"), Cast<UButton>(Panel->WidgetTree->FindWidget(TEXT("WisdomAddButton"))));
	return true;
}

#endif
