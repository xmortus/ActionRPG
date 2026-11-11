// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CraftingRecipeDataAsset.generated.h"

class UItemDataAsset;

USTRUCT(BlueprintType)
struct ACTIONRPG_API FCraftingIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
	TObjectPtr<UItemDataAsset> Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting", meta = (ClampMin = "1"))
	int32 Quantity = 1;
};

UCLASS(BlueprintType)
class ACTIONRPG_API UCraftingRecipeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
	FName RecipeID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
	FText RecipeName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
	TArray<FCraftingIngredient> Ingredients;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting")
	TObjectPtr<UItemDataAsset> OutputItem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crafting", meta = (ClampMin = "1"))
	int32 OutputQuantity = 1;
};
