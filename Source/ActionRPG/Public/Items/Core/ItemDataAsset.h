// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemTypes.h"
#include "ItemDataAsset.generated.h"

// Forward declaration
class AItemPickupActor;
class UItemBase;
class USkillBase;
class USkillDataAsset;

/**
 * Data Asset for defining item properties.
 * Used by the Item System to store static item data (name, description, stats, etc.).
 * Can be created and edited in Blueprint.
 */
UCLASS(BlueprintType)
class ACTIONRPG_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UItemDataAsset();

	// Primary Data Asset Interface
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	// Item Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FPrimaryAssetType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> ItemIcon;

	// World representation for pickup actor
	// Blueprint class to spawn when item is dropped in the world
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|World")
	TSubclassOf<class AItemPickupActor> ItemPickupActorClass;

	// Item class to instantiate for inventory (optional, defaults to ItemBase)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Class")
	TSubclassOf<UItemBase> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	EItemType Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	EItemRarity Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 Value;

	// Skill Item Data (used by SkillItem / SkillStone / BeastCore)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Skill")
	TObjectPtr<USkillDataAsset> SkillData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Skill")
	TSubclassOf<USkillBase> SkillClass;

	// Beast Core Requirements (optional, Phase 4 validation)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Skill|Requirements", meta = (ClampMin = "1"))
	int32 RequiredLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Skill|Requirements")
	TMap<FName, float> RequiredAttributes;
};

