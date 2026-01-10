// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StaticMesh.h"
#include "ItemTypes.h"
#include "ItemDataAsset.generated.h"

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

	// World representation for pickup actor (the static mesh to display when item is dropped in the world)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|World")
	TObjectPtr<UStaticMesh> ItemPickupMesh;

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
};

