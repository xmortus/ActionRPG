// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Skills/UnlockedSkillsPanelWidget.h"
#include "UI/ActionRPGUITheme.h"
#include "UI/Common/DraggablePanelHandleWidget.h"
#include "Components/UniformGridPanel.h"
#include "UI/Skills/UnlockedSkillEntryWidget.h"
#include "UI/Skills/SkillCatalogEntryWidget.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Components/Skills/SkillComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/ScrollBox.h"

void UUnlockedSkillsPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ActionRPGUITheme::StyleBlueprintWidgetTree(WidgetTree);
	UDraggablePanelHandleWidget::AttachToCanvasTitle(this, TEXT("TextBlock_99"));
	BuildSkillContextMenu();
	BuildCatalogControls();
	PanelCloseButton = WidgetTree ? Cast<UButton>(WidgetTree->FindWidget(TEXT("CloseButton"))) : nullptr;
	if (PanelCloseButton)
	{
		if (UCanvasPanelSlot* CloseSlot = Cast<UCanvasPanelSlot>(PanelCloseButton->Slot)) CloseSlot->SetZOrder(50);
		if (UWidget* Label = PanelCloseButton->GetContent()) Label->SetVisibility(ESlateVisibility::HitTestInvisible);
		PanelCloseButton->OnPressed.AddUniqueDynamic(this, &UUnlockedSkillsPanelWidget::OnCloseClicked);
	}
	if (KnownTabButton) KnownTabButton->OnPressed.AddUniqueDynamic(this, &UUnlockedSkillsPanelWidget::OnKnownTabClicked);
	if (LearnTabButton) LearnTabButton->OnPressed.AddUniqueDynamic(this, &UUnlockedSkillsPanelWidget::OnLearnTabClicked);

	SkillManagerComponent = GetSkillManagerComponent();
	SkillComponent = SkillManagerComponent && SkillManagerComponent->GetOwner()
		? SkillManagerComponent->GetOwner()->FindComponentByClass<USkillComponent>() : nullptr;
	if (!SkillManagerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("UnlockedSkillsPanelWidget::NativeConstruct - SkillManagerComponent not found"));
	}
	else
	{
		SkillManagerComponent->OnSkillUnlocked.AddDynamic(this, &UUnlockedSkillsPanelWidget::OnSkillUnlockedInternal);
		SkillManagerComponent->OnSkillPointsChanged.AddDynamic(this, &UUnlockedSkillsPanelWidget::OnSkillPointsChangedInternal);
	}
	if (SkillComponent)
	{
		SkillComponent->OnSkillExperienceGained.AddUniqueDynamic(this, &UUnlockedSkillsPanelWidget::OnSkillExperienceGainedInternal);
	}

	RefreshUnlockedSkills();
	RefreshCatalog();
	RefreshSkillPointsDisplay();
	ShowCatalog(bShowingCatalog);
}

void UUnlockedSkillsPanelWidget::NativeDestruct()
{
	HideSkillContextMenu();
	if (SkillManagerComponent)
	{
		SkillManagerComponent->OnSkillUnlocked.RemoveAll(this);
		SkillManagerComponent->OnSkillPointsChanged.RemoveAll(this);
	}
	if (SkillComponent) SkillComponent->OnSkillExperienceGained.RemoveAll(this);
	if (PanelCloseButton) PanelCloseButton->OnPressed.RemoveAll(this);
	if (KnownTabButton) KnownTabButton->OnPressed.RemoveAll(this);
	if (LearnTabButton) LearnTabButton->OnPressed.RemoveAll(this);

	EntryWidgets.Empty();
	CatalogEntries.Empty();

	Super::NativeDestruct();
}

void UUnlockedSkillsPanelWidget::BuildSkillContextMenu()
{
	UCanvasPanel* Canvas = WidgetTree ? Cast<UCanvasPanel>(WidgetTree->RootWidget) : nullptr;
	if (!Canvas || ContextMenu)
	{
		return;
	}
	SkillPointsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("FreeSkillPointsText"));
	SkillPointsText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	SkillPointsText->SetVisibility(ESlateVisibility::HitTestInvisible);
	FSlateFontInfo PointsFont = SkillPointsText->GetFont();
	PointsFont.Size = 14;
	SkillPointsText->SetFont(PointsFont);
	UCanvasPanelSlot* PointsSlot = Canvas->AddChildToCanvas(SkillPointsText);
	PointsSlot->SetPosition(FVector2D(16.0f, 49.0f));
	PointsSlot->SetSize(FVector2D(250.0f, 22.0f));
	PointsSlot->SetZOrder(5);
	ContextMenu = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("SkillContextMenu"));
	ContextMenu->SetBrushColor(ActionRPGUITheme::Panel());
	ContextMenu->SetPadding(FMargin(4.0f));
	UCanvasPanelSlot* MenuSlot = Canvas->AddChildToCanvas(ContextMenu);
	MenuSlot->SetSize(FVector2D(180.0f, 88.0f));
	MenuSlot->SetZOrder(100);
	UVerticalBox* Actions = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("SkillContextActions"));
	ContextMenu->SetContent(Actions);
	const auto AddAction = [this, Actions](const TCHAR* Name, const TCHAR* Label) -> UButton*
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(Name));
		Button->SetBackgroundColor(ActionRPGUITheme::Accent());
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Text->SetText(FText::FromString(Label));
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Text->SetJustification(ETextJustify::Center);
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = 14;
		Text->SetFont(Font);
		Button->SetContent(Text);
		Actions->AddChildToVerticalBox(Button);
		return Button;
	};
	AssignNextButton = AddAction(TEXT("SkillAssignNext"), TEXT("Assign next slot"));
	AssignMainHandButton = AddAction(TEXT("SkillAssignMainHand"), TEXT("Assign main hand"));
	AssignOffhandButton = AddAction(TEXT("SkillAssignOffhand"), TEXT("Assign offhand"));
	AssignNextButton->OnClicked.AddDynamic(this, &UUnlockedSkillsPanelWidget::OnAssignNextClicked);
	AssignMainHandButton->OnClicked.AddDynamic(this, &UUnlockedSkillsPanelWidget::OnAssignMainHandClicked);
	AssignOffhandButton->OnClicked.AddDynamic(this, &UUnlockedSkillsPanelWidget::OnAssignOffhandClicked);
	ContextMenu->SetVisibility(ESlateVisibility::Collapsed);
}

void UUnlockedSkillsPanelWidget::BuildCatalogControls()
{
	UCanvasPanel* Canvas = WidgetTree ? Cast<UCanvasPanel>(WidgetTree->RootWidget) : nullptr;
	if (!Canvas || CatalogScroll) return;
	const auto AddTab = [this, Canvas](const TCHAR* Name, const TCHAR* Label, float X) -> UButton*
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(Name));
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Text->SetText(FText::FromString(Label));
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Text->SetJustification(ETextJustify::Center);
		Text->SetVisibility(ESlateVisibility::HitTestInvisible);
		FSlateFontInfo TabFont = Text->GetFont();
		TabFont.Size = 14;
		Text->SetFont(TabFont);
		Button->SetContent(Text);
		UCanvasPanelSlot* Slot = Canvas->AddChildToCanvas(Button);
		Slot->SetPosition(FVector2D(X, 73.0f));
		Slot->SetSize(FVector2D(125.0f, 30.0f));
		Slot->SetZOrder(5);
		return Button;
	};
	KnownTabButton = AddTab(TEXT("KnownSkillTab"), TEXT("Known"), 16.0f);
	LearnTabButton = AddTab(TEXT("LearnSkillTab"), TEXT("Learn"), 148.0f);
	if (UCanvasPanelSlot* GridSlot = SkillGrid ? Cast<UCanvasPanelSlot>(SkillGrid->Slot) : nullptr)
	{
		GridSlot->SetPosition(FVector2D(16.0f, 111.0f));
		GridSlot->SetSize(FVector2D(267.0f, 367.0f));
	}
	CatalogScroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("SkillCatalogScroll"));
	UCanvasPanelSlot* ScrollSlot = Canvas->AddChildToCanvas(CatalogScroll);
	ScrollSlot->SetPosition(FVector2D(16.0f, 111.0f));
	ScrollSlot->SetSize(FVector2D(267.0f, 367.0f));
	ScrollSlot->SetZOrder(5);
	CatalogList = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("SkillCatalogList"));
	CatalogScroll->AddChild(CatalogList);
}

void UUnlockedSkillsPanelWidget::ShowCatalog(bool bShow)
{
	bShowingCatalog = bShow;
	HideSkillContextMenu();
	if (SkillGrid) SkillGrid->SetVisibility(bShow ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (CatalogScroll) CatalogScroll->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (KnownTabButton) KnownTabButton->SetBackgroundColor(bShow ? ActionRPGUITheme::Slot() : ActionRPGUITheme::Accent());
	if (LearnTabButton) LearnTabButton->SetBackgroundColor(bShow ? ActionRPGUITheme::Accent() : ActionRPGUITheme::Slot());
}

void UUnlockedSkillsPanelWidget::RefreshCatalog()
{
	if (!CatalogList) return;
	CatalogList->ClearChildren();
	CatalogEntries.Empty();
	if (!SkillManagerComponent || SkillManagerComponent->SkillCatalog.IsEmpty())
	{
		UTextBlock* Empty = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Empty->SetText(FText::FromString(TEXT("No skills available to learn")));
		Empty->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		CatalogList->AddChildToVerticalBox(Empty);
		return;
	}
	for (int32 Index = 0; Index < SkillManagerComponent->SkillCatalog.Num(); ++Index)
	{
		USkillCatalogEntryWidget* Entry = CreateWidget<USkillCatalogEntryWidget>(this, USkillCatalogEntryWidget::StaticClass());
		if (!Entry) continue;
		Entry->SetCatalogEntry(SkillManagerComponent, Index);
		UVerticalBoxSlot* CatalogSlot = CatalogList->AddChildToVerticalBox(Entry);
		CatalogSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 5.0f));
		CatalogEntries.Add(Entry);
	}
}

void UUnlockedSkillsPanelWidget::ShowSkillContextMenu(USkillBase* Skill, const FVector2D& ScreenPosition)
{
	if (!Skill || !ContextMenu)
	{
		return;
	}
	ContextSkill = Skill;
	if (UCanvasPanelSlot* MenuSlot = Cast<UCanvasPanelSlot>(ContextMenu->Slot))
	{
		const FVector2D Local = GetCachedGeometry().AbsoluteToLocal(ScreenPosition);
		const FVector2D Bounds = GetCachedGeometry().GetLocalSize();
		MenuSlot->SetPosition(FVector2D(FMath::Clamp(Local.X, 0.0f, FMath::Max(0.0f, Bounds.X - 180.0f)),
			FMath::Clamp(Local.Y, 0.0f, FMath::Max(0.0f, Bounds.Y - 88.0f))));
	}
	ContextMenu->SetVisibility(ESlateVisibility::Visible);
}

void UUnlockedSkillsPanelWidget::HideSkillContextMenu()
{
	if (ContextMenu)
	{
		ContextMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	ContextSkill = nullptr;
}

void UUnlockedSkillsPanelWidget::OnAssignNextClicked()
{
	AssignSkillToPendingSlot(ContextSkill);
	HideSkillContextMenu();
}

void UUnlockedSkillsPanelWidget::OnAssignMainHandClicked()
{
	AssignSkillToMainHand(ContextSkill);
	HideSkillContextMenu();
}

void UUnlockedSkillsPanelWidget::OnAssignOffhandClicked()
{
	AssignSkillToOffhand(ContextSkill);
	HideSkillContextMenu();
}

void UUnlockedSkillsPanelWidget::OnSkillPointsChangedInternal(int32 NewBalance, int32 Delta)
{
	RefreshSkillPointsDisplay();
	RefreshCatalog();
}

void UUnlockedSkillsPanelWidget::OnKnownTabClicked()
{
	ShowCatalog(false);
}

void UUnlockedSkillsPanelWidget::OnLearnTabClicked()
{
	RefreshCatalog();
	ShowCatalog(true);
}

void UUnlockedSkillsPanelWidget::OnCloseClicked()
{
	HideSkillContextMenu();
	SetVisibility(ESlateVisibility::Collapsed);
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		PC->bShowMouseCursor = true;
		PC->SetPause(false);
	}
}

void UUnlockedSkillsPanelWidget::RefreshSkillPointsDisplay()
{
	if (SkillPointsText)
	{
		SkillPointsText->SetText(FText::FromString(FString::Printf(TEXT("Free skill points: %d"),
			SkillManagerComponent ? SkillManagerComponent->GetUnspentSkillPoints() : 0)));
	}
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
	RefreshCatalog();
}

void UUnlockedSkillsPanelWidget::OnSkillExperienceGainedInternal(USkillBase* Skill, float Amount, int32 NewLevel)
{
	for (UUnlockedSkillEntryWidget* Entry : EntryWidgets)
	{
		if (Entry && Entry->GetSkill() == Skill)
		{
			Entry->RefreshProgress();
			break;
		}
	}
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
