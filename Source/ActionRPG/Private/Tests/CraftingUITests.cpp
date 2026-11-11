// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Progression/ClassComponent.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Components/Skills/SkillComponent.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Crafting/CraftingRecipeDataAsset.h"
#include "Engine/World.h"
#include "Items/Core/ItemDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Skills/Implementations/SkillBlacksmithCrafting.h"
#include "UI/Crafting/CraftingPanelWidget.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCraftingPanelUITest, "ActionRPG.Crafting.PanelUI",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCraftingPanelUITest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world exists"), World)) return false;
	FActorSpawnParameters SpawnParams;
	SpawnParams.ObjectFlags |= RF_Transient;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(104000.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParams);
	if (!TestNotNull(TEXT("Player spawns"), Player)) return false;

	USkillComponent* SkillComponent = NewObject<USkillComponent>(Player);
	USkillManagerComponent* SkillManager = NewObject<USkillManagerComponent>(Player);
	Player->AddInstanceComponent(SkillComponent);
	Player->AddInstanceComponent(SkillManager);
	SkillComponent->RegisterComponent();
	SkillManager->RegisterComponent();

	UCraftingPanelWidget* Panel = NewObject<UCraftingPanelWidget>(GetTransientPackage());
	if (!TestNotNull(TEXT("Crafting panel can be created"), Panel))
	{
		Player->Destroy();
		return false;
	}
	Panel->WidgetTree = NewObject<UWidgetTree>(Panel);
	Panel->EnsureContentBuilt();
	Panel->BindPlayer(Player);
	UComboBoxString* Picker = Cast<UComboBoxString>(Panel->WidgetTree->FindWidget(TEXT("RecipePicker")));
	UButton* CraftButton = Cast<UButton>(Panel->WidgetTree->FindWidget(TEXT("CraftButton")));
	if (!TestNotNull(TEXT("Recipe picker exists"), Picker) || !TestNotNull(TEXT("Craft button exists"), CraftButton))
	{
		Player->Destroy();
		return false;
	}
	TestEqual(TEXT("No recipe before profession selection"), Picker->GetOptionCount(), 0);
	TestFalse(TEXT("Craft button starts disabled"), CraftButton->GetIsEnabled());
	TestFalse(TEXT("Panel starts closed"), Panel->IsPanelOpen());
	Panel->TogglePanel();
	TestTrue(TEXT("Panel opens"), Panel->IsPanelOpen());

	UItemDataAsset* Ore = NewObject<UItemDataAsset>(GetTransientPackage());
	Ore->ItemID = TEXT("IronOre");
	Ore->MaxStackSize = 99;
	Ore->Weight = 1.0f;
	UItemDataAsset* Sword = NewObject<UItemDataAsset>(GetTransientPackage());
	Sword->ItemID = TEXT("IronSword");
	Sword->MaxStackSize = 1;
	Sword->Weight = 3.0f;
	UCraftingRecipeDataAsset* Recipe = NewObject<UCraftingRecipeDataAsset>(GetTransientPackage());
	Recipe->RecipeID = TEXT("IronSword");
	Recipe->RecipeName = FText::FromString(TEXT("Iron Sword"));
	FCraftingIngredient Ingredient;
	Ingredient.Item = Ore;
	Ingredient.Quantity = 2;
	Recipe->Ingredients.Add(Ingredient);
	Recipe->OutputItem = Sword;
	Recipe->OutputQuantity = 1;
	UProfessionDataAsset* Blacksmith = NewObject<UProfessionDataAsset>(GetTransientPackage());
	Blacksmith->GrantedSkillClass = USkillBlacksmithCrafting::StaticClass();
	Blacksmith->GrantedSkillData = NewObject<USkillDataAsset>(Blacksmith);
	Blacksmith->Recipes.Add(Recipe);
	Player->ExperienceComponent->AddExperience(100.0f);
	TestTrue(TEXT("XP unlocks Blacksmith profession"), Player->ClassComponent->AllocateExperienceToProfession(Blacksmith, 100.0f));
	TestEqual(TEXT("Profession recipe appears in picker"), Picker->GetOptionCount(), 1);
	TestFalse(TEXT("Cannot craft without ore"), CraftButton->GetIsEnabled());

	UItemBase* OreItem = NewObject<UItemBase>(Player);
	OreItem->ItemData = Ore;
	TestTrue(TEXT("Two ore can be added"), Player->InventoryComponent->AddItem(OreItem, 2));
	TestTrue(TEXT("Craft button enables when requirements are met"), CraftButton->GetIsEnabled());
	CraftButton->OnClicked.Broadcast();
	TestEqual(TEXT("Craft button consumes ore"), Player->InventoryComponent->CountItemData(Ore), 0);
	TestEqual(TEXT("Craft button produces sword"), Player->InventoryComponent->CountItemData(Sword), 1);
	TestFalse(TEXT("Craft button disables after ingredients are spent"), CraftButton->GetIsEnabled());
	Panel->BindPlayer(nullptr);
	Player->Destroy();
	return true;
}

#endif
