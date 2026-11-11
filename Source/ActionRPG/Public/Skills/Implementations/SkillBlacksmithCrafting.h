// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Skills/Core/SkillBase.h"
#include "SkillBlacksmithCrafting.generated.h"

class UCraftingRecipeDataAsset;

UCLASS(BlueprintType)
class ACTIONRPG_API USkillBlacksmithCrafting : public USkillBase
{
	GENERATED_BODY()

public:
	virtual bool CanActivate(AActor* Target = nullptr) const override;
	virtual USkillBase* Activate(AActor* Target = nullptr) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Crafting")
	bool CanCraftRecipe(const UCraftingRecipeDataAsset* Recipe) const;

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CraftRecipe(const UCraftingRecipeDataAsset* Recipe);

private:
	const UCraftingRecipeDataAsset* FindCraftableRecipe() const;
};
