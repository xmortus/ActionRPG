// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Menu/PrototypeMenuWidget.h"
#include "UI/ActionRPGUITheme.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UPrototypeMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!WidgetTree || WidgetTree->RootWidget) return;

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("MenuCanvas"));
	WidgetTree->RootWidget = Canvas;
	UBorder* Backdrop = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("MenuBackdrop"));
	Backdrop->SetBrushColor(FLinearColor(0.01f, 0.015f, 0.025f, 0.86f));
	UCanvasPanelSlot* BackdropSlot = Canvas->AddChildToCanvas(Backdrop);
	BackdropSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	BackdropSlot->SetOffsets(FMargin(0.0f));

	USizeBox* CardSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("MenuCardSize"));
	CardSize->SetWidthOverride(480.0f);
	UCanvasPanelSlot* CardSlot = Canvas->AddChildToCanvas(CardSize);
	CardSlot->SetAnchors(FAnchors(0.5f, 0.5f));
	CardSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	CardSlot->SetAutoSize(true);

	UBorder* Card = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("MenuCard"));
	Card->SetPadding(FMargin(30.0f));
	Card->SetBrushColor(ActionRPGUITheme::Panel());
	CardSize->SetContent(Card);
	UVerticalBox* Rows = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MenuRows"));
	Card->SetContent(Rows);

	TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("MenuTitle"));
	TitleText->SetJustification(ETextJustify::Center);
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo TitleFont = TitleText->GetFont();
	TitleFont.Size = 34;
	TitleText->SetFont(TitleFont);
	Rows->AddChildToVerticalBox(TitleText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));

	DescriptionText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("MenuDescription"));
	DescriptionText->SetJustification(ETextJustify::Center);
	DescriptionText->SetAutoWrapText(true);
	DescriptionText->SetColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.9f, 0.95f)));
	Rows->AddChildToVerticalBox(DescriptionText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 22.0f));

	UButton* PlayButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("MenuPlayButton"));
	PlayButton->SetBackgroundColor(ActionRPGUITheme::Accent());
	PlayButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("MenuPlayLabel"));
	PlayButtonText->SetJustification(ETextJustify::Center);
	PlayButtonText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	PlayButton->SetContent(PlayButtonText);
	PlayButton->OnClicked.AddDynamic(this, &UPrototypeMenuWidget::HandlePlayClicked);
	Rows->AddChildToVerticalBox(PlayButton)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));

	UButton* ExitButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("MenuExitButton"));
	ExitButton->SetBackgroundColor(ActionRPGUITheme::Button());
	UTextBlock* ExitLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("MenuExitLabel"));
	ExitLabel->SetText(FText::FromString(TEXT("Exit Game")));
	ExitLabel->SetJustification(ETextJustify::Center);
	ExitLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	ExitButton->SetContent(ExitLabel);
	ExitButton->OnClicked.AddDynamic(this, &UPrototypeMenuWidget::HandleExitClicked);
	Rows->AddChildToVerticalBox(ExitButton);

	SetInitialMenu(true);
}

void UPrototypeMenuWidget::SetInitialMenu(bool bInitial)
{
	if (TitleText) TitleText->SetText(FText::FromString(bInitial ? TEXT("ACTION RPG") : TEXT("PAUSED")));
	if (DescriptionText) DescriptionText->SetText(FText::FromString(bInitial
		? TEXT("Choose a path, train your skills, and earn a growing sword.")
		: TEXT("Your journey waits. Press P or Escape, or choose Resume to continue.")));
	if (PlayButtonText) PlayButtonText->SetText(FText::FromString(bInitial ? TEXT("Start Game") : TEXT("Resume")));
}

void UPrototypeMenuWidget::HandlePlayClicked()
{
	OnPlayRequested.Broadcast();
}

void UPrototypeMenuWidget::HandleExitClicked()
{
	OnExitRequested.Broadcast();
}
