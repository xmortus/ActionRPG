// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Progression/Core/ProgressionTypes.h"
#include "ProfessionDataAsset.generated.h"

class USkillBase;
class USkillDataAsset;
class UCraftingRecipeDataAsset;

	/** Data Asset for profession skills and fixed attribute growth. */
UCLASS(BlueprintType)
class ACTIONRPG_API UProfessionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UProfessionDataAsset();

	// Primary Data Asset Interface
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession")
	FPrimaryAssetType ProfessionType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession")
	FName ProfessionID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession")
	FText ProfessionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession", meta = (MultiLine = true))
	FText ProfessionDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession")
	TObjectPtr<UTexture2D> ProfessionIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession|Requirements", meta = (ClampMin = "1"))
	int32 RequiredLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession|Requirements")
	TMap<EPrimaryAttribute, float> RequiredAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession|Skills")
	TSubclassOf<USkillBase> GrantedSkillClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession|Skills")
	TObjectPtr<USkillDataAsset> GrantedSkillData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession|Crafting")
	TArray<TObjectPtr<UCraftingRecipeDataAsset>> Recipes;

	/** Permanent attribute gain for each earned profession level, including level one. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession|Progression")
	TMap<EPrimaryAttribute, float> AttributeGainsPerLevel;

	// Legacy serialized fields. ClassComponent does not apply profession attribute bonuses.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession|Bonuses")
	TMap<EPrimaryAttribute, float> PrimaryAttributeBonuses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Profession|Bonuses")
	TMap<ESecondaryAttribute, float> SecondaryAttributeBonuses;
};
