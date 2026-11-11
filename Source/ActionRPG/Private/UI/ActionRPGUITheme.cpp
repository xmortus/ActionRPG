// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/ActionRPGUITheme.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void ActionRPGUITheme::StyleBlueprintWidgetTree(UWidgetTree* WidgetTree, bool bCompact)
{
	if (!WidgetTree)
	{
		return;
	}

	TArray<UWidget*> Widgets;
	WidgetTree->GetAllWidgets(Widgets);
	for (UWidget* Widget : Widgets)
	{
		if (!Widget)
		{
			continue;
		}

		const FString Name = Widget->GetName();
		if (UBorder* Border = Cast<UBorder>(Widget))
		{
			const bool bSlot = bCompact || Name.Contains(TEXT("Slot"));
			FSlateBrush Brush = Border->Background;
			Brush.TintColor = FSlateColor(FLinearColor::White);
			Border->SetBrush(Brush);
			Border->SetRenderOpacity(1.0f);
			Border->SetBrushColor(bSlot ? Slot() : Name.Contains(TEXT("Section")) ? Surface() : Panel());
		}
		else if (UImage* Image = Cast<UImage>(Widget))
		{
			// Solid Blueprint images are panel fills. Keep textures and item icons intact.
			if (!Image->GetBrush().GetResourceObject() && !Name.Contains(TEXT("Icon")))
			{
				Image->SetBrushTintColor(FSlateColor(FLinearColor::White));
				Image->SetRenderOpacity(1.0f);
				Image->SetColorAndOpacity(bCompact ? Slot() : Panel());
			}
		}
		else if (UButton* Action = Cast<UButton>(Widget))
		{
			const bool bPrimary = Name.Contains(TEXT("Assign")) || Name.Contains(TEXT("Equip"))
				|| Name.Contains(TEXT("Use")) || Name.Contains(TEXT("Craft"));
			Action->SetBackgroundColor(bPrimary ? Accent() : Button());
			if (Name.Contains(TEXT("Close")))
			{
				UTextBlock* CloseLabel = Cast<UTextBlock>(Action->GetContent());
				if (!CloseLabel && !Action->GetContent())
				{
					CloseLabel = WidgetTree->ConstructWidget<UTextBlock>();
					Action->SetContent(CloseLabel);
				}
				if (CloseLabel && CloseLabel->GetText().IsEmpty())
				{
					CloseLabel->SetText(FText::FromString(TEXT("X")));
					CloseLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
					CloseLabel->SetJustification(ETextJustify::Center);
					FSlateFontInfo CloseFont = CloseLabel->GetFont();
					CloseFont.Size = 18;
					CloseLabel->SetFont(CloseFont);
				}
			}
		}
		else if (UTextBlock* Text = Cast<UTextBlock>(Widget))
		{
			Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
			FSlateFontInfo Font = Text->GetFont();
			const FString Label = Text->GetText().ToString();
			const bool bTitle = Name.Contains(TEXT("Title"))
				|| Label.Equals(TEXT("Inventory"), ESearchCase::IgnoreCase)
				|| Label.Equals(TEXT("Equipment"), ESearchCase::IgnoreCase)
				|| Label.Equals(TEXT("Skills"), ESearchCase::IgnoreCase);
			Font.Size = bTitle ? (bCompact ? 18 : 25) : FMath::Max(Font.Size, bCompact ? 12 : 14);
			Text->SetFont(Font);
		}
	}
}
