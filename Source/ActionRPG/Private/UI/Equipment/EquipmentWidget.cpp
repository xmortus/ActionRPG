// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Equipment/EquipmentWidget.h"
#include "UI/ActionRPGUITheme.h"
#include "UI/Common/DraggablePanelHandleWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Items/Equipment/EquipmentItem.h"
#include "UI/Equipment/EquipmentInventoryWidget.h"
#include "UI/Equipment/EquipmentStatsWidget.h"
#include "GameFramework/Pawn.h"

void UEquipmentWidget::StyleEquipmentLayout()
{
	UCanvasPanel* Canvas = WidgetTree ? Cast<UCanvasPanel>(WidgetTree->RootWidget) : nullptr;
	if (!Canvas)
	{
		return;
	}

	auto Place = [](UWidget* Widget, const FVector2D Position, const FVector2D Size, const int32 ZOrder = 0)
	{
		if (UCanvasPanelSlot* Slot = Widget ? Cast<UCanvasPanelSlot>(Widget->Slot) : nullptr)
		{
			Slot->SetAnchors(FAnchors(0.0f, 0.0f));
			Slot->SetAlignment(FVector2D::ZeroVector);
			Slot->SetAutoSize(false);
			Slot->SetPosition(Position);
			Slot->SetSize(Size);
			Slot->SetZOrder(ZOrder);
		}
	};

	UBorder* Background = Cast<UBorder>(WidgetTree->FindWidget(TEXT("EquipmentPanel")));
	UBorder* StatsCard = Cast<UBorder>(WidgetTree->FindWidget(TEXT("EquipmentStatsPanel")));
	UBorder* InventoryCard = Cast<UBorder>(WidgetTree->FindWidget(TEXT("EquipmentInventoryPanel")));
	if (Background)
	{
		Background->SetBrushColor(ActionRPGUITheme::Panel());
		Place(Background, FVector2D::ZeroVector, FVector2D(940.0f, 910.0f), -2);
	}
	if (StatsCard)
	{
		StatsCard->SetBrushColor(ActionRPGUITheme::Surface());
		StatsCard->SetPadding(FMargin(14.0f, 44.0f, 14.0f, 12.0f));
		Place(StatsCard, FVector2D(414.0f, 72.0f), FVector2D(514.0f, 420.0f), -1);
	}
	if (InventoryCard)
	{
		InventoryCard->SetBrushColor(ActionRPGUITheme::Surface());
		InventoryCard->SetPadding(FMargin(12.0f, 42.0f, 12.0f, 10.0f));
		InventoryCard->SetHorizontalAlignment(HAlign_Center);
		Place(InventoryCard, FVector2D(12.0f, 504.0f), FVector2D(916.0f, 390.0f), -1);
	}

	UBorder* EquippedCard = Cast<UBorder>(WidgetTree->FindWidget(TEXT("EquippedItemsCard")));
	if (!EquippedCard)
	{
		EquippedCard = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("EquippedItemsCard"));
		EquippedCard->SetBrushColor(ActionRPGUITheme::Surface());
		EquippedCard->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Canvas->AddChildToCanvas(EquippedCard);
	}
	Place(EquippedCard, FVector2D(12.0f, 72.0f), FVector2D(390.0f, 420.0f), -1);

	auto AddHeading = [this, Canvas, &Place](const TCHAR* Name, const TCHAR* Text, const FVector2D Position, const FVector2D Size, const int32 FontSize)
	{
		UTextBlock* Heading = Cast<UTextBlock>(WidgetTree->FindWidget(Name));
		if (!Heading)
		{
			Heading = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(Name));
			Canvas->AddChildToCanvas(Heading);
		}
		Heading->SetText(FText::FromString(Text));
		Heading->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo Font = Heading->GetFont();
		Font.Size = FontSize;
		Heading->SetFont(Font);
		Place(Heading, Position, Size, 1);
	};
	AddHeading(TEXT("EquipmentTitle"), TEXT("EQUIPMENT"), FVector2D(20.0f, 14.0f), FVector2D(700.0f, 42.0f), 25);
	AddHeading(TEXT("EquippedItemsHeading"), TEXT("EQUIPPED"), FVector2D(24.0f, 84.0f), FVector2D(300.0f, 28.0f), 16);
	AddHeading(TEXT("EquipmentAttributesHeading"), TEXT("STATS"), FVector2D(428.0f, 84.0f), FVector2D(400.0f, 28.0f), 16);
	AddHeading(TEXT("CarriedItemsHeading"), TEXT("CARRIED ITEMS"), FVector2D(24.0f, 516.0f), FVector2D(400.0f, 28.0f), 16);
	UTextBlock* InventorySummary = Cast<UTextBlock>(WidgetTree->FindWidget(TEXT("EquipmentInventorySummary")));
	if (!InventorySummary)
	{
		InventorySummary = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EquipmentInventorySummary"));
		Canvas->AddChildToCanvas(InventorySummary);
	}
	InventorySummary->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	InventorySummary->SetJustification(ETextJustify::Right);
	Place(InventorySummary, FVector2D(490.0f, 516.0f), FVector2D(420.0f, 28.0f), 1);

	struct FEquipmentCell { const TCHAR* SlotName; const TCHAR* LabelName; const TCHAR* Text; int32 Column; int32 Row; };
	const FEquipmentCell Cells[] = {
		{TEXT("WBP_EquipmentSlot_Head"), TEXT("EquipmentHeadLabel"), TEXT("Head"), 1, 0},
		{TEXT("WBP_EquipmentSlot_Amulate"), TEXT("EquipmentAmuletLabel"), TEXT("Amulet"), 2, 0},
		{TEXT("WBP_EquipmentSlot_Hands"), TEXT("EquipmentHandsLabel"), TEXT("Hands"), 0, 1},
		{TEXT("WBP_EquipmentSlot_Chest"), TEXT("EquipmentChestLabel"), TEXT("Chest"), 1, 1},
		{TEXT("WBP_EquipmentSlot_Ring"), TEXT("EquipmentRingLabel"), TEXT("Ring"), 2, 1},
		{TEXT("WBP_EquipmentSlot_MainHand"), TEXT("EquipmentMainHandLabel"), TEXT("Main Hand"), 0, 2},
		{TEXT("WBP_EquipmentSlot_Legs"), TEXT("EquipmentLegsLabel"), TEXT("Legs"), 1, 2},
		{TEXT("WBP_EquipmentSlot_Offhand"), TEXT("EquipmentOffhandLabel"), TEXT("Offhand"), 2, 2},
		{TEXT("WBP_EquipmentSlot_Feet"), TEXT("EquipmentFeetLabel"), TEXT("Feet"), 1, 3}
	};
	for (const FEquipmentCell& Cell : Cells)
	{
		UWidget* EquipmentSlot = WidgetTree->FindWidget(FName(Cell.SlotName));
		if (!EquipmentSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("EquipmentWidget - Missing slot widget %s"), Cell.SlotName);
			continue;
		}
		const FVector2D SlotPosition(72.0f + 103.0f * Cell.Column, 142.0f + 92.0f * Cell.Row);
		Place(EquipmentSlot, SlotPosition, FVector2D(64.0f, 64.0f));

		UTextBlock* Label = Cast<UTextBlock>(WidgetTree->FindWidget(FName(Cell.LabelName)));
		if (!Label)
		{
			Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(Cell.LabelName));
			Canvas->AddChildToCanvas(Label);
		}
		Label->SetText(FText::FromString(Cell.Text));
		Label->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Label->SetJustification(ETextJustify::Center);
		FSlateFontInfo Font = Label->GetFont();
		Font.Size = 14;
		Label->SetFont(Font);
		Place(Label, SlotPosition + FVector2D(-16.0f, -22.0f), FVector2D(96.0f, 18.0f), 1);
	}

	UButton* CloseButton = Cast<UButton>(WidgetTree->FindWidget(TEXT("EquipmentCloseButton")));
	if (!CloseButton)
	{
		CloseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("EquipmentCloseButton"));
		CloseButton->SetBackgroundColor(ActionRPGUITheme::Button());
		UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("EquipmentCloseLabel"));
		Label->SetText(FText::FromString(TEXT("Close [U]")));
		Label->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		FSlateFontInfo Font = Label->GetFont();
		Font.Size = 16;
		Label->SetFont(Font);
		CloseButton->SetContent(Label);
		Canvas->AddChildToCanvas(CloseButton);
	}
	CloseButton->OnClicked.RemoveDynamic(this, &UEquipmentWidget::CloseEquipmentPanel);
	CloseButton->OnClicked.AddDynamic(this, &UEquipmentWidget::CloseEquipmentPanel);
	Place(CloseButton, FVector2D(820.0f, 16.0f), FVector2D(100.0f, 36.0f), 1);
}

void UEquipmentWidget::CloseEquipmentPanel()
{
	HideInventoryContextMenu();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UEquipmentWidget::HideInventoryContextMenu()
{
	if (EquipmentInventoryWidget)
	{
		EquipmentInventoryWidget->HideContextMenu();
	}
}

void UEquipmentWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ActionRPGUITheme::StyleBlueprintWidgetTree(WidgetTree);
	StyleEquipmentLayout();
	UDraggablePanelHandleWidget::AttachToCanvasTitle(this, TEXT("EquipmentTitle"));

	BindComponents();
	RefreshEquipment();
	RefreshInventory();
	RefreshStats();
}

void UEquipmentWidget::NativeDestruct()
{
	if (EquipmentComponent)
	{
		EquipmentComponent->OnEquipmentChanged.RemoveDynamic(this, &UEquipmentWidget::HandleEquipmentChanged);
	}

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UEquipmentWidget::HandleInventoryChanged);
	}

	if (AttributeComponent)
	{
		AttributeComponent->OnPrimaryAttributeChanged.RemoveDynamic(this, &UEquipmentWidget::HandlePrimaryAttributeChanged);
	}

	if (SecondaryAttributeComponent)
	{
		SecondaryAttributeComponent->OnSecondaryAttributeChanged.RemoveDynamic(this, &UEquipmentWidget::HandleSecondaryAttributeChanged);
	}

	Super::NativeDestruct();
}

void UEquipmentWidget::BindComponents()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentWidget - No owning pawn found."));
		return;
	}

	EquipmentComponent = OwningPawn->FindComponentByClass<UEquipmentComponent>();
	InventoryComponent = OwningPawn->FindComponentByClass<UInventoryComponent>();
	AttributeComponent = OwningPawn->FindComponentByClass<UAttributeComponent>();
	SecondaryAttributeComponent = OwningPawn->FindComponentByClass<USecondaryAttributeComponent>();

	if (EquipmentComponent)
	{
		EquipmentComponent->OnEquipmentChanged.AddUniqueDynamic(this, &UEquipmentWidget::HandleEquipmentChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentWidget - EquipmentComponent not found."));
	}

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddUniqueDynamic(this, &UEquipmentWidget::HandleInventoryChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentWidget - InventoryComponent not found."));
	}

	if (AttributeComponent)
	{
		AttributeComponent->OnPrimaryAttributeChanged.AddUniqueDynamic(this, &UEquipmentWidget::HandlePrimaryAttributeChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentWidget - AttributeComponent not found."));
	}

	if (SecondaryAttributeComponent)
	{
		SecondaryAttributeComponent->OnSecondaryAttributeChanged.AddUniqueDynamic(this, &UEquipmentWidget::HandleSecondaryAttributeChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentWidget - SecondaryAttributeComponent not found."));
	}
}

void UEquipmentWidget::RefreshEquipment()
{
	OnEquipmentUpdated();
}

void UEquipmentWidget::RefreshInventory()
{
	OnInventoryUpdated();
}

void UEquipmentWidget::RefreshStats()
{
	OnStatsUpdated();
}

void UEquipmentWidget::OnEquipmentUpdated_Implementation()
{
	// Slot widgets listen directly to EquipmentComponent, so no default work here.
}

void UEquipmentWidget::OnInventoryUpdated_Implementation()
{
	if (EquipmentInventoryWidget)
	{
		EquipmentInventoryWidget->RefreshInventory();
	}
	if (InventoryComponent && WidgetTree)
	{
		if (UTextBlock* Summary = Cast<UTextBlock>(WidgetTree->FindWidget(TEXT("EquipmentInventorySummary"))))
		{
			const TArray<FInventorySlot>& Slots = InventoryComponent->GetInventorySlots();
			int32 UsedSlots = 0;
			for (const FInventorySlot& InventorySlot : Slots)
			{
				UsedSlots += !InventorySlot.bIsEmpty ? 1 : 0;
			}
			Summary->SetText(FText::FromString(FString::Printf(TEXT("Weight: %.0f / %.0f   Slots: %d / %d"),
				InventoryComponent->GetCurrentWeight(), InventoryComponent->GetMaxWeight(), UsedSlots, Slots.Num())));
		}
	}
}

void UEquipmentWidget::OnStatsUpdated_Implementation()
{
	if (EquipmentStatsWidget)
	{
		EquipmentStatsWidget->RefreshStats();
	}
}

void UEquipmentWidget::HandleEquipmentChanged(EEquipmentSlot ChangedSlot, UEquipmentItem* Item)
{
	RefreshEquipment();
	RefreshStats();
}

void UEquipmentWidget::HandleInventoryChanged(int32 SlotIndex, UItemBase* Item)
{
	RefreshInventory();
}

void UEquipmentWidget::HandlePrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue)
{
	RefreshStats();
}

void UEquipmentWidget::HandleSecondaryAttributeChanged(ESecondaryAttribute Attribute, float NewValue)
{
	RefreshStats();
}
