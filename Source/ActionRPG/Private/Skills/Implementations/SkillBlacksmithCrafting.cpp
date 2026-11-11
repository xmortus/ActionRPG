// Copyright Epic Games, Inc. All Rights Reserved.

#include "Skills/Implementations/SkillBlacksmithCrafting.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Progression/ClassComponent.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Crafting/CraftingRecipeDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Skills/Core/SkillDataAsset.h"
#include "GameFramework/Actor.h"

const UCraftingRecipeDataAsset* USkillBlacksmithCrafting::FindCraftableRecipe() const
{
	if (!OwnerActor)
	{
		return nullptr;
	}
	const UClassComponent* Classes = OwnerActor->FindComponentByClass<UClassComponent>();
	if (!Classes)
	{
		return nullptr;
	}
	for (const UProfessionDataAsset* Profession : Classes->GetSelectedProfessions())
	{
		if (Profession)
		{
			for (const TObjectPtr<UCraftingRecipeDataAsset>& Recipe : Profession->Recipes)
			{
				if (CanCraftRecipe(Recipe))
				{
					return Recipe;
				}
			}
		}
	}
	return nullptr;
}

bool USkillBlacksmithCrafting::CanActivate(AActor* Target) const
{
	return Super::CanActivate(Target) && FindCraftableRecipe() != nullptr;
}

USkillBase* USkillBlacksmithCrafting::Activate(AActor* Target)
{
	if (!Super::CanActivate(Target))
	{
		return nullptr;
	}
	const UCraftingRecipeDataAsset* Recipe = FindCraftableRecipe();
	if (!Recipe || !CraftRecipe(Recipe))
	{
		return nullptr;
	}
	CooldownRemaining = SkillData ? SkillData->CooldownDuration : 0.0f;
	OnSkillActivated.Broadcast(this, Target);
	return this;
}

bool USkillBlacksmithCrafting::CanCraftRecipe(const UCraftingRecipeDataAsset* Recipe) const
{
	if (!OwnerActor || !Recipe)
	{
		return false;
	}
	const UClassComponent* Classes = OwnerActor->FindComponentByClass<UClassComponent>();
	const UInventoryComponent* Inventory = OwnerActor->FindComponentByClass<UInventoryComponent>();
	const USkillManagerComponent* Skills = OwnerActor->FindComponentByClass<USkillManagerComponent>();
	if (!Classes || !Inventory || !Skills || !Skills->IsSkillUnlocked(const_cast<USkillBlacksmithCrafting*>(this)))
	{
		return false;
	}
	for (const UProfessionDataAsset* Profession : Classes->GetSelectedProfessions())
	{
		if (Profession && Profession->Recipes.Contains(Recipe))
		{
			return Inventory->CanCraftRecipe(Recipe);
		}
	}
	return false;
}

bool USkillBlacksmithCrafting::CraftRecipe(const UCraftingRecipeDataAsset* Recipe)
{
	if (!CanCraftRecipe(Recipe))
	{
		return false;
	}
	UInventoryComponent* Inventory = OwnerActor->FindComponentByClass<UInventoryComponent>();
	return Inventory && Inventory->CraftRecipe(Recipe);
}
