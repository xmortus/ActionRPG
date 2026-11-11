// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Crafting/CraftingPanelWidget.h"
#include "UI/ActionRPGUITheme.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/ScrollBox.h"
#include "Components/ScaleBox.h"
#include "Components/SizeBox.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Progression/ClassComponent.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Crafting/CraftingRecipeDataAsset.h"
#include "GameFramework/Pawn.h"
#include "Items/Core/ItemDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Skills/Implementations/SkillBlacksmithCrafting.h"
#include "UI/Common/DraggablePanelHandleWidget.h"

void UCraftingPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	EnsureContentBuilt();
}

void UCraftingPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureContentBuilt();
	UDraggablePanelHandleWidget::AttachToOverlayTitle(this, TEXT("CraftingTitleArea"), TEXT("CraftingPanelSize"));
}

void UCraftingPanelWidget::EnsureContentBuilt()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CraftingCanvas"));
	Canvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	WidgetTree->RootWidget = Canvas;

	UButton* ToggleButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("OpenCraftingButton"));
	ToggleButton->SetBackgroundColor(ActionRPGUITheme::Button());
	UTextBlock* ToggleLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("OpenCraftingLabel"));
	ToggleLabel->SetText(FText::FromString(TEXT("Crafting [R]")));
	ToggleLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo ToggleFont = ToggleLabel->GetFont();
	ToggleFont.Size = 16;
	ToggleLabel->SetFont(ToggleFont);
	ToggleButton->SetContent(ToggleLabel);
	ToggleButton->OnClicked.AddDynamic(this, &UCraftingPanelWidget::HandleToggleClicked);
	UCanvasPanelSlot* ToggleSlot = Canvas->AddChildToCanvas(ToggleButton);
	ToggleSlot->SetAnchors(FAnchors(1.0f, 0.0f));
	ToggleSlot->SetAlignment(FVector2D(1.0f, 0.0f));
	ToggleSlot->SetPosition(FVector2D(-100.0f, 20.0f));
	ToggleSlot->SetSize(FVector2D(150.0f, 42.0f));

	UScaleBox* ResponsiveFrame = WidgetTree->ConstructWidget<UScaleBox>(UScaleBox::StaticClass(), TEXT("CraftingResponsiveFrame"));
	ResponsiveFrame->SetStretch(EStretch::ScaleToFit);
	ResponsiveFrame->SetStretchDirection(EStretchDirection::DownOnly);
	UCanvasPanelSlot* FrameSlot = Canvas->AddChildToCanvas(ResponsiveFrame);
	FrameSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	FrameSlot->SetOffsets(FMargin(16.0f, 16.0f, -16.0f, -16.0f));
	USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("CraftingPanelSize"));
	PanelSize->SetWidthOverride(760.0f);
	PanelSize->SetHeightOverride(320.0f);
	ResponsiveFrame->SetContent(PanelSize);
	Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CraftingPanel"));
	Panel->SetBrushColor(ActionRPGUITheme::Panel());
	Panel->SetPadding(FMargin(18.0f));
	PanelSize->SetContent(Panel);
	Panel->SetVisibility(ESlateVisibility::Collapsed);

	UVerticalBox* Main = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("CraftingMain"));
	Panel->SetContent(Main);
	UHorizontalBox* Header = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("CraftingHeader"));
	Main->AddChildToVerticalBox(Header)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CraftingTitle"));
	Title->SetText(FText::FromString(TEXT("CRAFTING")));
	Title->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo TitleFont = Title->GetFont();
	TitleFont.Size = 25;
	Title->SetFont(TitleFont);
	UOverlay* TitleArea = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("CraftingTitleArea"));
	Header->AddChildToHorizontalBox(TitleArea)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	TitleArea->AddChildToOverlay(Title);
	UButton* CloseButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("CloseCraftingButton"));
	CloseButton->SetBackgroundColor(ActionRPGUITheme::Button());
	UTextBlock* CloseLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CloseCraftingLabel"));
	CloseLabel->SetText(FText::FromString(TEXT("Close [R]")));
	CloseLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	FSlateFontInfo CloseFont = CloseLabel->GetFont();
	CloseFont.Size = 16;
	CloseLabel->SetFont(CloseFont);
	CloseButton->SetContent(CloseLabel);
	CloseButton->OnClicked.AddDynamic(this, &UCraftingPanelWidget::HandleToggleClicked);
	Header->AddChildToHorizontalBox(CloseButton);

	UScrollBox* Scroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("CraftingScroll"));
	Main->AddChildToVerticalBox(Scroll)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	UVerticalBox* Rows = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("CraftingRows"));
	Scroll->AddChild(Rows);
	auto AddText = [this, Rows](const TCHAR* Name, const TCHAR* InitialText, float BottomPadding) -> UTextBlock*
	{
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(Name));
		Text->SetText(FText::FromString(InitialText));
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Text->SetAutoWrapText(true);
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = 16;
		Text->SetFont(Font);
		Rows->AddChildToVerticalBox(Text)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, BottomPadding));
		return Text;
	};
	AddText(TEXT("RecipePrompt"), TEXT("RECIPE"), 8.0f);
	RecipePicker = WidgetTree->ConstructWidget<UComboBoxString>(UComboBoxString::StaticClass(), TEXT("RecipePicker"));
	RecipePicker->OnSelectionChanged.AddDynamic(this, &UCraftingPanelWidget::HandleRecipeSelected);
	Rows->AddChildToVerticalBox(RecipePicker)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 18.0f));
	UHorizontalBox* Details = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("CraftingDetails"));
	Rows->AddChildToVerticalBox(Details)->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
	UBorder* IngredientCard = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CraftingIngredientCard"));
	IngredientCard->SetBrushColor(ActionRPGUITheme::Surface());
	IngredientCard->SetPadding(FMargin(12.0f));
	UHorizontalBoxSlot* IngredientSlot = Details->AddChildToHorizontalBox(IngredientCard);
	IngredientSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	IngredientSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
	IngredientText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CraftingIngredients"));
	IngredientText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	IngredientText->SetAutoWrapText(true);
	FSlateFontInfo DetailFont = IngredientText->GetFont();
	DetailFont.Size = 16;
	IngredientText->SetFont(DetailFont);
	IngredientCard->SetContent(IngredientText);
	UBorder* OutputCard = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CraftingOutputCard"));
	OutputCard->SetBrushColor(ActionRPGUITheme::Surface());
	OutputCard->SetPadding(FMargin(12.0f));
	Details->AddChildToHorizontalBox(OutputCard)->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	OutputText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CraftingOutput"));
	OutputText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	OutputText->SetAutoWrapText(true);
	OutputText->SetFont(DetailFont);
	OutputCard->SetContent(OutputText);
	StatusText = AddText(TEXT("CraftingStatus"), TEXT("Select a profession to see recipes."), 18.0f);

	UHorizontalBox* Actions = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("CraftingActions"));
	Rows->AddChildToVerticalBox(Actions);
	CraftButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("CraftButton"));
	CraftButton->SetBackgroundColor(ActionRPGUITheme::Accent());
	UTextBlock* CraftLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CraftLabel"));
	CraftLabel->SetText(FText::FromString(TEXT("Craft")));
	CraftLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	CraftLabel->SetFont(DetailFont);
	CraftButton->SetContent(CraftLabel);
	CraftButton->OnClicked.AddDynamic(this, &UCraftingPanelWidget::HandleCraftClicked);
	Actions->AddChildToHorizontalBox(CraftButton)->SetPadding(FMargin(0.0f, 0.0f, 12.0f, 0.0f));
	Refresh();
}

void UCraftingPanelWidget::NativeDestruct()
{
	UnbindPlayer();
	Super::NativeDestruct();
}

void UCraftingPanelWidget::UnbindPlayer()
{
	if (Inventory) Inventory->OnInventoryChanged.RemoveAll(this);
	if (Classes) Classes->OnProfessionListChanged.RemoveAll(this);
	if (Skills)
	{
		Skills->OnSkillUnlocked.RemoveAll(this);
		Skills->OnSkillRevoked.RemoveAll(this);
	}
	Inventory = nullptr;
	Classes = nullptr;
	Skills = nullptr;
}

void UCraftingPanelWidget::BindPlayer(APawn* InPawn)
{
	UnbindPlayer();
	if (InPawn)
	{
		Inventory = InPawn->FindComponentByClass<UInventoryComponent>();
		Classes = InPawn->FindComponentByClass<UClassComponent>();
		Skills = InPawn->FindComponentByClass<USkillManagerComponent>();
	}
	if (Inventory) Inventory->OnInventoryChanged.AddUniqueDynamic(this, &UCraftingPanelWidget::HandleInventoryChanged);
	if (Classes) Classes->OnProfessionListChanged.AddUniqueDynamic(this, &UCraftingPanelWidget::HandleProfessionChanged);
	if (Skills)
	{
		Skills->OnSkillUnlocked.AddUniqueDynamic(this, &UCraftingPanelWidget::HandleSkillUnlocked);
		Skills->OnSkillRevoked.AddUniqueDynamic(this, &UCraftingPanelWidget::HandleSkillRevoked);
	}
	Refresh();
}

bool UCraftingPanelWidget::IsPanelOpen() const
{
	return Panel && Panel->GetVisibility() == ESlateVisibility::Visible;
}

void UCraftingPanelWidget::TogglePanel()
{
	if (!Panel) return;
	Panel->SetVisibility(IsPanelOpen() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	if (IsPanelOpen()) Refresh();
}

void UCraftingPanelWidget::Refresh()
{
	RebuildRecipeOptions();
	RefreshDetails();
}

void UCraftingPanelWidget::RebuildRecipeOptions()
{
	if (!RecipePicker) return;
	bUpdatingOptions = true;
	UCraftingRecipeDataAsset* Previous = SelectedRecipe;
	Recipes.Reset();
	RecipePicker->ClearOptions();
	if (Classes)
	{
		for (const UProfessionDataAsset* Profession : Classes->GetSelectedProfessions())
		{
			if (!Profession) continue;
			for (UCraftingRecipeDataAsset* Recipe : Profession->Recipes)
			{
				if (Recipe && !Recipes.Contains(Recipe)) Recipes.Add(Recipe);
			}
		}
	}
	SelectedRecipe = Recipes.Contains(Previous) ? Previous : (Recipes.IsEmpty() ? nullptr : Recipes[0].Get());
	for (int32 Index = 0; Index < Recipes.Num(); ++Index)
	{
		const UCraftingRecipeDataAsset* Recipe = Recipes[Index];
		const FString Name = Recipe->RecipeName.IsEmpty() ? Recipe->RecipeID.ToString() : Recipe->RecipeName.ToString();
		RecipePicker->AddOption(FString::Printf(TEXT("%s [%d]"), *Name, Index + 1));
		if (Recipe == SelectedRecipe) RecipePicker->SetSelectedIndex(Index);
	}
	bUpdatingOptions = false;
}

int32 UCraftingPanelWidget::GetOwnedQuantity(const UItemDataAsset* Item) const
{
	int32 Total = 0;
	if (Inventory && Item)
	{
		for (const FInventorySlot& InventorySlot : Inventory->GetInventorySlots())
		{
			if (!InventorySlot.bIsEmpty && InventorySlot.Item && InventorySlot.Item->ItemData == Item)
			{
				Total += InventorySlot.Quantity;
			}
		}
	}
	return Total;
}

void UCraftingPanelWidget::RefreshDetails()
{
	if (!IngredientText || !OutputText || !StatusText || !CraftButton) return;
	if (!SelectedRecipe)
	{
		IngredientText->SetText(FText::FromString(TEXT("Ingredients: none")));
		OutputText->SetText(FText::FromString(TEXT("Output: none")));
		StatusText->SetText(FText::FromString(TEXT("Select a profession to discover recipes.")));
		CraftButton->SetIsEnabled(false);
		return;
	}

	FString Ingredients = TEXT("Ingredients:\n");
	for (const FCraftingIngredient& Ingredient : SelectedRecipe->Ingredients)
	{
		if (!Ingredient.Item) continue;
		Ingredients += FString::Printf(TEXT("  %s  %d / %d\n"),
			*Ingredient.Item->ItemName.ToString(), GetOwnedQuantity(Ingredient.Item), Ingredient.Quantity);
	}
	IngredientText->SetText(FText::FromString(Ingredients));
	const FString OutputName = SelectedRecipe->OutputItem
		? SelectedRecipe->OutputItem->ItemName.ToString() : TEXT("Unknown item");
	OutputText->SetText(FText::FromString(FString::Printf(TEXT("Output: %d x %s"),
		SelectedRecipe->OutputQuantity, *OutputName)));

	USkillBlacksmithCrafting* CraftingSkill = nullptr;
	if (Skills)
	{
		for (USkillBase* Skill : Skills->GetUnlockedSkills())
		{
			CraftingSkill = Cast<USkillBlacksmithCrafting>(Skill);
			if (CraftingSkill) break;
		}
	}
	const bool bCanCraft = CraftingSkill && CraftingSkill->CanCraftRecipe(SelectedRecipe);
	CraftButton->SetIsEnabled(bCanCraft);
	StatusText->SetText(FText::FromString(!CraftingSkill
		? TEXT("Unlock the Blacksmith Crafting skill to craft this recipe.")
		: (bCanCraft ? TEXT("Ready to craft.") : TEXT("Need ingredients or free inventory capacity."))));
}

void UCraftingPanelWidget::HandleRecipeSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bUpdatingOptions || !RecipePicker) return;
	const int32 Index = RecipePicker->FindOptionIndex(SelectedItem);
	SelectedRecipe = Recipes.IsValidIndex(Index) ? Recipes[Index] : nullptr;
	RefreshDetails();
}

void UCraftingPanelWidget::HandleCraftClicked()
{
	if (!SelectedRecipe || !Skills) return;
	for (USkillBase* Skill : Skills->GetUnlockedSkills())
	{
		if (USkillBlacksmithCrafting* CraftingSkill = Cast<USkillBlacksmithCrafting>(Skill))
		{
			const bool bCrafted = CraftingSkill->CraftRecipe(SelectedRecipe);
			RefreshDetails();
			StatusText->SetText(FText::FromString(bCrafted
				? TEXT("Crafted successfully! Check your inventory.")
				: TEXT("Crafting failed. Check materials and inventory space.")));
			return;
		}
	}
}

void UCraftingPanelWidget::HandleToggleClicked() { TogglePanel(); }
void UCraftingPanelWidget::HandleInventoryChanged(int32 SlotIndex, UItemBase* Item) { RefreshDetails(); }
void UCraftingPanelWidget::HandleProfessionChanged(UProfessionDataAsset* Profession, bool bAdded) { Refresh(); }
void UCraftingPanelWidget::HandleSkillUnlocked(USkillBase* Skill) { RefreshDetails(); }
void UCraftingPanelWidget::HandleSkillRevoked(USkillBase* Skill) { RefreshDetails(); }
