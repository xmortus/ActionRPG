// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Skills/UnlockedSkillEntryWidget.h"
#include "UI/ActionRPGUITheme.h"
#include "UI/Skills/SkillDragDropOperation.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Skills/SkillComponent.h"
#include "GameFramework/Actor.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Implementations/SkillDash.h"
#include "UI/Skills/UnlockedSkillsPanelWidget.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UUnlockedSkillEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ActionRPGUITheme::StyleBlueprintWidgetTree(WidgetTree, true);

	// Legacy assignment controls remain in older widget assets until they are resaved.
	for (const TCHAR* ButtonName : {TEXT("AssignButton"), TEXT("AssignMainHandButton"), TEXT("AssignOffHandButton")})
	{
		if (UWidget* Button = WidgetTree ? WidgetTree->FindWidget(FName(ButtonName)) : nullptr)
		{
			Button->RemoveFromParent();
		}
	}

	if (UCanvasPanel* Canvas = WidgetTree ? Cast<UCanvasPanel>(WidgetTree->RootWidget) : nullptr)
	{
		UBorder* Tile = Cast<UBorder>(WidgetTree->FindWidget(TEXT("SkillTileBackground")));
		if (!Tile)
		{
			Tile = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("SkillTileBackground"));
			Tile->SetBrushColor(ActionRPGUITheme::Slot());
			Tile->SetVisibility(ESlateVisibility::Visible);
			UCanvasPanelSlot* TileSlot = Canvas->AddChildToCanvas(Tile);
			TileSlot->SetPosition(FVector2D::ZeroVector);
			TileSlot->SetSize(FVector2D(64.0f, 118.0f));
			TileSlot->SetZOrder(-1);
		}
		else if (UCanvasPanelSlot* TileSlot = Cast<UCanvasPanelSlot>(Tile->Slot))
		{
			TileSlot->SetSize(FVector2D(64.0f, 118.0f));
		}
		ProgressText = Cast<UTextBlock>(WidgetTree->FindWidget(TEXT("SkillProgressText")));
		if (!ProgressText)
		{
			ProgressText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("SkillProgressText"));
			UCanvasPanelSlot* ProgressSlot = Canvas->AddChildToCanvas(ProgressText);
			ProgressSlot->SetPosition(FVector2D(2.0f, 91.0f));
			ProgressSlot->SetSize(FVector2D(60.0f, 26.0f));
		}
		ProgressText->SetJustification(ETextJustify::Center);
		ProgressText->SetAutoWrapText(true);
		ProgressText->SetVisibility(ESlateVisibility::HitTestInvisible);
		FSlateFontInfo ProgressFont = ProgressText->GetFont();
		ProgressFont.Size = 10;
		ProgressText->SetFont(ProgressFont);
	}
	if (UCanvasPanelSlot* IconSlot = SkillIcon ? Cast<UCanvasPanelSlot>(SkillIcon->Slot) : nullptr)
	{
		IconSlot->SetPosition(FVector2D(4.0f, 4.0f));
		IconSlot->SetSize(FVector2D(56.0f, 56.0f));
	}
	if (UCanvasPanelSlot* NameSlot = SkillNameText ? Cast<UCanvasPanelSlot>(SkillNameText->Slot) : nullptr)
	{
		NameSlot->SetPosition(FVector2D(2.0f, 62.0f));
		NameSlot->SetSize(FVector2D(60.0f, 28.0f));
		SkillNameText->SetAutoWrapText(true);
		SkillNameText->SetJustification(ETextJustify::Center);
		SkillNameText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	RefreshProgress();
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
			SkillIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
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
	RefreshProgress();
}

void UUnlockedSkillEntryWidget::RefreshProgress()
{
	if (!CurrentSkill || !CurrentSkill->SkillData)
	{
		if (ProgressText) ProgressText->SetText(FText::GetEmpty());
		SetToolTipText(FText::GetEmpty());
		return;
	}
	const USkillComponent* Skills = CurrentSkill->OwnerActor
		? CurrentSkill->OwnerActor->FindComponentByClass<USkillComponent>() : nullptr;
	const int32 Level = Skills ? Skills->GetSkillLevel(CurrentSkill) : CurrentSkill->SkillLevel;
	const float Experience = Skills ? Skills->GetSkillExperience(CurrentSkill) : CurrentSkill->Experience;
	const float Required = Skills ? Skills->GetSkillExperienceRequiredForNextLevel(CurrentSkill) : 0.0f;
	const FString Progress = Required > 0.0f
		? FString::Printf(TEXT("Lv %d\nXP %.0f/%.0f"), Level, Experience, Required)
		: FString::Printf(TEXT("Lv %d\nXP %.0f"), Level, Experience);
	if (ProgressText) ProgressText->SetText(FText::FromString(Progress));
	FString Tooltip = FString::Printf(
		TEXT("%s\nLevel %d - %s\nMana %.1f  Stamina %.1f  Cooldown %.1fs"),
		*CurrentSkill->SkillData->SkillName.ToString(), Level, *Progress.Replace(TEXT("\n"), TEXT("  ")),
		CurrentSkill->GetManaCost(), CurrentSkill->GetStaminaCost(), CurrentSkill->GetEffectiveCooldownDuration());
	if (const USkillDash* Dash = Cast<USkillDash>(CurrentSkill))
	{
		Tooltip += FString::Printf(TEXT("\nDash speed %.0f"), Dash->GetDashSpeed());
	}
	for (const FSkillAttributeMilestone& Milestone : CurrentSkill->SkillData->AttributeMilestones)
	{
		const UEnum* AttributeEnum = StaticEnum<EPrimaryAttribute>();
		const FString Name = AttributeEnum
			? AttributeEnum->GetDisplayNameTextByValue(static_cast<int64>(Milestone.Attribute)).ToString() : TEXT("Attribute");
		Tooltip += FString::Printf(TEXT("\nLevel %d: +%.0f %s%s"), Milestone.SkillLevel,
			Milestone.Amount, *Name, Level >= Milestone.SkillLevel ? TEXT(" (earned)") : TEXT(""));
	}
	SetToolTipText(FText::FromString(Tooltip));
}

FReply UUnlockedSkillEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!CurrentSkill || !ParentPanel)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		ParentPanel->ShowSkillContextMenu(CurrentSkill, InMouseEvent.GetScreenSpacePosition());
		return FReply::Handled();
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		ParentPanel->HideSkillContextMenu();
		if (TSharedPtr<SWidget> SlateWidget = GetCachedWidget())
		{
			return FReply::Handled().DetectDrag(SlateWidget.ToSharedRef(), EKeys::LeftMouseButton);
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UUnlockedSkillEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (!CurrentSkill)
	{
		return;
	}
	USkillDragDropOperation* DragOperation = NewObject<USkillDragDropOperation>(this);
	DragOperation->Skill = CurrentSkill;
	DragOperation->Pivot = EDragPivot::CenterCenter;
	if (UUnlockedSkillEntryWidget* Visual = CreateWidget<UUnlockedSkillEntryWidget>(GetOwningPlayer(), GetClass()))
	{
		Visual->SetSkillData(CurrentSkill, nullptr);
		DragOperation->DefaultDragVisual = Visual;
	}
	OutOperation = DragOperation;
}
