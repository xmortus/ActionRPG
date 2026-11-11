// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/HUD/VitalBarsWidget.h"

#include "UI/ActionRPGUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ProgressBar.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Combat/StatusEffectComponent.h"
#include "Combat/StatusEffectDataAsset.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UVitalBarsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("VitalsCanvas"));
	WidgetTree->RootWidget = Canvas;
	Canvas->SetVisibility(ESlateVisibility::HitTestInvisible);
	UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("VitalsPanel"));
	Panel->SetBrushColor(ActionRPGUITheme::Panel());
	Panel->SetPadding(FMargin(12.0f, 10.0f));
	Panel->SetVisibility(ESlateVisibility::HitTestInvisible);
	UCanvasPanelSlot* PanelSlot = Canvas->AddChildToCanvas(Panel);
	PanelSlot->SetAnchors(FAnchors(0.0f, 1.0f));
	PanelSlot->SetAlignment(FVector2D(0.0f, 1.0f));
	PanelSlot->SetPosition(FVector2D(20.0f, -16.0f));
	PanelSlot->SetSize(FVector2D(320.0f, 154.0f));

	UVerticalBox* Rows = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("VitalsRows"));
	Panel->SetContent(Rows);

	auto AddBar = [this, Rows](const TCHAR* Name, const TCHAR* Label, const FLinearColor Fill,
		TObjectPtr<UProgressBar>& OutBar, TObjectPtr<UTextBlock>& OutValue)
	{
		UHorizontalBox* Heading = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(),
			FName(FString(Name) + TEXT("Heading")));
		Rows->AddChildToVerticalBox(Heading)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 3.0f));

		UTextBlock* LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
			FName(FString(Name) + TEXT("Label")));
		LabelText->SetText(FText::FromString(Label));
		LabelText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo LabelFont = LabelText->GetFont();
		LabelFont.Size = 13;
		LabelText->SetFont(LabelFont);
		UHorizontalBoxSlot* LabelSlot = Heading->AddChildToHorizontalBox(LabelText);
		LabelSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

		UTextBlock* ValueText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
			FName(FString(Name) + TEXT("Value")));
		ValueText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo ValueFont = ValueText->GetFont();
		ValueFont.Size = 13;
		ValueText->SetFont(ValueFont);
		Heading->AddChildToHorizontalBox(ValueText);
		OutValue = ValueText;

		USizeBox* BarHeight = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(),
			FName(FString(Name) + TEXT("Height")));
		BarHeight->SetHeightOverride(13.0f);
		Rows->AddChildToVerticalBox(BarHeight)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
		UProgressBar* Bar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), FName(Name));
		Bar->SetFillColorAndOpacity(Fill);
		BarHeight->SetContent(Bar);
		OutBar = Bar;
	};

	AddBar(TEXT("HealthBar"), TEXT("HEALTH"), FLinearColor(0.78f, 0.17f, 0.19f), HealthBar, HealthValue);
	AddBar(TEXT("StaminaBar"), TEXT("STAMINA"), FLinearColor(0.72f, 0.55f, 0.16f), StaminaBar, StaminaValue);
	AddBar(TEXT("ManaBar"), TEXT("MANA"), FLinearColor(0.18f, 0.42f, 0.78f), ManaBar, ManaValue);
	EffectsPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ActiveEffectsPanel"));
	EffectsPanel->SetBrushColor(ActionRPGUITheme::Panel());
	EffectsPanel->SetPadding(FMargin(10.0f, 7.0f));
	EffectsPanel->SetVisibility(ESlateVisibility::Collapsed);
	UCanvasPanelSlot* EffectsSlot = Canvas->AddChildToCanvas(EffectsPanel);
	EffectsSlot->SetAnchors(FAnchors(0.0f, 1.0f));
	EffectsSlot->SetAlignment(FVector2D(0.0f, 1.0f));
	EffectsSlot->SetPosition(FVector2D(20.0f, -178.0f));
	EffectsSlot->SetAutoSize(true);
	EffectsText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ActiveEffectsText"));
	EffectsText->SetColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.92f, 1.0f, 1.0f)));
	FSlateFontInfo EffectFont = EffectsText->GetFont();
	EffectFont.Size = 13;
	EffectsText->SetFont(EffectFont);
	EffectsPanel->SetContent(EffectsText);
	RefreshBars();
}

void UVitalBarsWidget::NativeDestruct()
{
	BindAttributes(nullptr);
	BindEffects(nullptr);
	Super::NativeDestruct();
}

void UVitalBarsWidget::BindEffects(UStatusEffectComponent* InEffects)
{
	if (Effects)
	{
		Effects->OnStatusEffectsChanged.RemoveDynamic(this, &UVitalBarsWidget::HandleEffectsChanged);
		Effects->OnStatusEffectTimeChanged.RemoveDynamic(this, &UVitalBarsWidget::HandleEffectsChanged);
	}
	Effects = InEffects;
	if (Effects)
	{
		Effects->OnStatusEffectsChanged.AddUniqueDynamic(this, &UVitalBarsWidget::HandleEffectsChanged);
		Effects->OnStatusEffectTimeChanged.AddUniqueDynamic(this, &UVitalBarsWidget::HandleEffectsChanged);
	}
	RefreshEffects();
}

void UVitalBarsWidget::RefreshEffects()
{
	if (!EffectsPanel || !EffectsText)
	{
		return;
	}
	const TArray<FActiveStatusEffect>* Active = Effects ? &Effects->GetActiveEffects() : nullptr;
	if (!Active || Active->IsEmpty())
	{
		EffectsPanel->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	TArray<FString> Lines;
	const int32 VisibleCount = FMath::Min(3, Active->Num());
	for (int32 Index = 0; Index < VisibleCount; ++Index)
	{
		const FActiveStatusEffect& Effect = (*Active)[Index];
		if (!Effect.Definition) continue;
		const FString Name = Effect.Definition->DisplayName.IsEmpty()
			? Effect.Definition->EffectID.ToString() : Effect.Definition->DisplayName.ToString();
		Lines.Add(FString::Printf(TEXT("%s%s  %ds"), *Name,
			Effect.Stacks > 1 ? *FString::Printf(TEXT(" x%d"), Effect.Stacks) : TEXT(""),
			FMath::Max(0, FMath::CeilToInt(Effect.RemainingSeconds))));
	}
	if (Active->Num() > VisibleCount)
	{
		Lines.Add(FString::Printf(TEXT("+%d more effects"), Active->Num() - VisibleCount));
	}
	EffectsText->SetText(FText::FromString(FString::Join(Lines, TEXT("\n"))));
	EffectsPanel->SetVisibility(Lines.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
}

void UVitalBarsWidget::HandleEffectsChanged()
{
	RefreshEffects();
}

void UVitalBarsWidget::BindAttributes(USecondaryAttributeComponent* InAttributes)
{
	if (Attributes)
	{
		Attributes->OnSecondaryAttributeChanged.RemoveDynamic(this, &UVitalBarsWidget::HandleMaxAttributeChanged);
		Attributes->OnCurrentSecondaryAttributeChanged.RemoveDynamic(this, &UVitalBarsWidget::HandleCurrentAttributeChanged);
	}
	Attributes = InAttributes;
	if (Attributes)
	{
		Attributes->OnSecondaryAttributeChanged.AddUniqueDynamic(this, &UVitalBarsWidget::HandleMaxAttributeChanged);
		Attributes->OnCurrentSecondaryAttributeChanged.AddUniqueDynamic(this, &UVitalBarsWidget::HandleCurrentAttributeChanged);
	}
	RefreshBars();
}

void UVitalBarsWidget::RefreshBars()
{
	RefreshBar(ESecondaryAttribute::MaxHealth);
	RefreshBar(ESecondaryAttribute::MaxStamina);
	RefreshBar(ESecondaryAttribute::MaxMana);
}

void UVitalBarsWidget::RefreshBar(ESecondaryAttribute Attribute)
{
	UProgressBar* Bar = nullptr;
	UTextBlock* ValueText = nullptr;
	switch (Attribute)
	{
	case ESecondaryAttribute::MaxHealth:
		Bar = HealthBar;
		ValueText = HealthValue;
		break;
	case ESecondaryAttribute::MaxStamina:
		Bar = StaminaBar;
		ValueText = StaminaValue;
		break;
	case ESecondaryAttribute::MaxMana:
		Bar = ManaBar;
		ValueText = ManaValue;
		break;
	default:
		return;
	}
	if (!Bar || !ValueText)
	{
		return;
	}
	const float Maximum = Attributes ? Attributes->GetSecondaryAttribute(Attribute) : 0.0f;
	const float Current = Attributes ? Attributes->GetCurrentSecondaryAttribute(Attribute) : 0.0f;
	Bar->SetPercent(Maximum > 0.0f ? FMath::Clamp(Current / Maximum, 0.0f, 1.0f) : 0.0f);
	ValueText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Current, Maximum)));
}

void UVitalBarsWidget::HandleMaxAttributeChanged(ESecondaryAttribute Attribute, float NewValue)
{
	RefreshBar(Attribute);
}

void UVitalBarsWidget::HandleCurrentAttributeChanged(ESecondaryAttribute Attribute, float NewValue, float OldValue)
{
	RefreshBar(Attribute);
}
