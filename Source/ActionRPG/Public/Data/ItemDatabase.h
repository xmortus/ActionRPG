// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Items/Core/ItemTypes.h"
#include "ItemDatabase.generated.h"

class UItemDataAsset;

/**
 * Singleton database for managing Item Data Assets.
 * Provides lookup and retrieval of item data assets by ID, type, or rarity.
 * Automatically loads all item data assets via Asset Manager on initialization.
 */
UCLASS()
class ACTIONRPG_API UItemDatabase : public UObject
{
	GENERATED_BODY()

public:
	// Singleton accessor
	UFUNCTION(BlueprintCallable, Category = "Item Database")
	static UItemDatabase* Get();

	// Initialize database - loads all item data assets
	void Initialize();

	// Lookup methods
	UFUNCTION(BlueprintCallable, Category = "Item Database")
	UItemDataAsset* GetItemDataAsset(const FName& ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Item Database")
	TArray<UItemDataAsset*> GetAllItemDataAssets() const;

	UFUNCTION(BlueprintCallable, Category = "Item Database")
	TArray<UItemDataAsset*> GetItemsByType(EItemType ItemType) const;

	UFUNCTION(BlueprintCallable, Category = "Item Database")
	TArray<UItemDataAsset*> GetItemsByRarity(EItemRarity Rarity) const;

	// Debug: Get all registered item IDs
	UFUNCTION(BlueprintCallable, Category = "Item Database|Debug")
	TArray<FName> GetAllItemIDs() const;

	// Debug: Print all registered items to log
	UFUNCTION(BlueprintCallable, Category = "Item Database|Debug")
	void PrintAllItems() const;

	// Create item instance from data asset
	UFUNCTION(BlueprintCallable, Category = "Item Database")
	class UItemBase* CreateItem(const FName& ItemID, int32 Quantity = 1) const;

protected:
	// Registry of all item data assets
	UPROPERTY()
	TMap<FName, TObjectPtr<UItemDataAsset>> ItemRegistry;

private:
	static UItemDatabase* Instance;
};

