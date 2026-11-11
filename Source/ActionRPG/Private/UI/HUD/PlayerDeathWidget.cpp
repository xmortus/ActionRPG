#include "UI/HUD/PlayerDeathWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UPlayerDeathWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!WidgetTree || WidgetTree->RootWidget) return;
	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	WidgetTree->RootWidget = Canvas;
	UBorder* Backdrop = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	Backdrop->SetBrushColor(FLinearColor(0.015f, 0.01f, 0.015f, 0.88f));
	UCanvasPanelSlot* BackdropSlot = Canvas->AddChildToCanvas(Backdrop);
	BackdropSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	BackdropSlot->SetOffsets(FMargin(0.0f));
	UVerticalBox* Content = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UCanvasPanelSlot* ContentSlot = Canvas->AddChildToCanvas(Content);
	ContentSlot->SetAnchors(FAnchors(0.5f, 0.5f));
	ContentSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	ContentSlot->SetSize(FVector2D(420.0f, 220.0f));
	UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Title->SetText(FText::FromString(TEXT("You have fallen")));
	Title->SetJustification(ETextJustify::Center);
	Title->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.75f, 0.55f)));
	FSlateFontInfo Font = Title->GetFont();
	Font.Size = 30;
	Title->SetFont(Font);
	Content->AddChildToVerticalBox(Title)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 16.0f));
	InstructionText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	InstructionText->SetJustification(ETextJustify::Center);
	InstructionText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	Content->AddChildToVerticalBox(InstructionText)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 14.0f));
	RespawnButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	UTextBlock* ButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ButtonText->SetText(FText::FromString(TEXT("RESPAWN")));
	ButtonText->SetJustification(ETextJustify::Center);
	RespawnButton->AddChild(ButtonText);
	Content->AddChildToVerticalBox(RespawnButton);
	RespawnButton->OnClicked.AddDynamic(this, &UPlayerDeathWidget::HandleRespawnClicked);
	UButton* ExitButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	UTextBlock* ExitText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ExitText->SetText(FText::FromString(TEXT("EXIT GAME")));
	ExitText->SetJustification(ETextJustify::Center);
	ExitButton->AddChild(ExitText);
	Content->AddChildToVerticalBox(ExitButton)->SetPadding(FMargin(0.0f, 10.0f, 0.0f, 0.0f));
	ExitButton->OnClicked.AddDynamic(this, &UPlayerDeathWidget::HandleExitClicked);
	SetRespawnReady(false);
}

void UPlayerDeathWidget::SetRespawnReady(bool bReady)
{
	if (RespawnButton) RespawnButton->SetIsEnabled(bReady);
	if (InstructionText) InstructionText->SetText(FText::FromString(bReady
		? TEXT("Choose when to return to the world.") : TEXT("Gathering strength to return...")));
}

void UPlayerDeathWidget::HandleRespawnClicked()
{
	OnRespawnRequested.Broadcast();
}

void UPlayerDeathWidget::HandleExitClicked()
{
	OnExitRequested.Broadcast();
}
