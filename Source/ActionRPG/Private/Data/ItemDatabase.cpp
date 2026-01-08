// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data/ItemDatabase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Core/ItemBase.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

UItemDatabase* UItemDatabase::Instance = nullptr;

UItemDatabase* UItemDatabase::Get()
{
	if (!Instance)
	{
		Instance = NewObject<UItemDatabase>();
		Instance->AddToRoot(); // Prevent garbage collection
		Instance->Initialize();
	}
	return Instance;
}

void UItemDatabase::Initialize()
{
	// Clear existing registry
	ItemRegistry.Empty();

	UE_LOG(LogTemp, Log, TEXT("ItemDatabase: Initializing..."));

	// Load all item data assets using Asset Manager
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		UE_LOG(LogTemp, Log, TEXT("ItemDatabase: AssetManager found, scanning for items..."));
		
		TArray<FPrimaryAssetId> ItemAssetIds;
		AssetManager->GetPrimaryAssetIdList(FPrimaryAssetType("Item"), ItemAssetIds);

		UE_LOG(LogTemp, Log, TEXT("ItemDatabase: Found %d item assets to process"), ItemAssetIds.Num());

		if (ItemAssetIds.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("ItemDatabase: No items found! Check Asset Manager configuration:"));
			UE_LOG(LogTemp, Warning, TEXT("  - Primary Asset Type 'Item' must be configured"));
			UE_LOG(LogTemp, Warning, TEXT("  - Directory '/Game/Data/Items/' must be in scan list"));
			UE_LOG(LogTemp, Warning, TEXT("  - Item Data Assets must have 'Item Type' set to 'Item'"));
		}

		for (const FPrimaryAssetId& AssetId : ItemAssetIds)
		{
			// Load the asset first to ensure it's available (synchronous load)
			TSharedPtr<FStreamableHandle> Handle = AssetManager->LoadPrimaryAsset(AssetId, TArray<FName>());
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete();
			}

			// Get the asset object
			UObject* AssetObject = AssetManager->GetPrimaryAssetObject(AssetId);
			
			if (!AssetObject)
			{
				UE_LOG(LogTemp, Warning, TEXT("ItemDatabase: Failed to get asset object: %s"), *AssetId.ToString());
				continue;
			}

			// Log the actual class name for debugging
			FString ClassName = AssetObject->GetClass()->GetName();
			FString ObjectName = AssetObject->GetName();
			UE_LOG(LogTemp, Log, TEXT("ItemDatabase: Found asset - ID: %s, Object: %s, Class: %s"), 
				*AssetId.ToString(), *ObjectName, *ClassName);

			// Check if it's a class instead of an instance (shouldn't happen, but just in case)
			if (UClass* AssetClass = Cast<UClass>(AssetObject))
			{
				UE_LOG(LogTemp, Error, TEXT("ItemDatabase: Asset is a class, not an instance! %s (Class: %s)"), 
					*AssetId.ToString(), *AssetClass->GetName());
				UE_LOG(LogTemp, Error, TEXT("  This usually means the asset was created as a Blueprint Class instead of a Data Asset."));
				UE_LOG(LogTemp, Error, TEXT("  Solution: Delete the asset and recreate it as 'Data Asset' -> 'Item Data Asset'"));
				continue;
			}

			// Try to cast to ItemDataAsset
			UItemDataAsset* ItemData = Cast<UItemDataAsset>(AssetObject);
			if (ItemData)
			{
				if (ItemData->ItemID != NAME_None)
				{
					ItemRegistry.Add(ItemData->ItemID, ItemData);
					UE_LOG(LogTemp, Log, TEXT("ItemDatabase: Registered item - ID: %s, Name: %s"), 
						*ItemData->ItemID.ToString(), *ItemData->ItemName.ToString());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("ItemDatabase: Item Data Asset has empty ItemID: %s"), *AssetId.ToString());
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ItemDatabase: Failed to cast asset to ItemDataAsset: %s"), *AssetId.ToString());
				UE_LOG(LogTemp, Error, TEXT("  Object Name: %s"), *ObjectName);
				UE_LOG(LogTemp, Error, TEXT("  Actual Class: %s"), *ClassName);
				UE_LOG(LogTemp, Error, TEXT("  Expected Class: ItemDataAsset"));
				UE_LOG(LogTemp, Error, TEXT("  This usually means:"));
				UE_LOG(LogTemp, Error, TEXT("    1. Asset was created as Blueprint Class (wrong!)"));
				UE_LOG(LogTemp, Error, TEXT("    2. Asset was created as regular DataAsset instead of ItemDataAsset"));
				UE_LOG(LogTemp, Error, TEXT("    3. Asset needs to be recreated: Right-click -> Miscellaneous -> Data Asset -> Item Data Asset"));
			}
		}

		UE_LOG(LogTemp, Log, TEXT("ItemDatabase: Initialization complete. Registered %d items."), ItemRegistry.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDatabase: AssetManager is not initialized!"));
		UE_LOG(LogTemp, Error, TEXT("ItemDatabase: Items will need to be registered manually or Asset Manager must be configured."));
	}
}

UItemDataAsset* UItemDatabase::GetItemDataAsset(const FName& ItemID) const
{
	if (const TObjectPtr<UItemDataAsset>* FoundItem = ItemRegistry.Find(ItemID))
	{
		return *FoundItem;
	}

	UE_LOG(LogTemp, Warning, TEXT("ItemDatabase: Item not found: %s"), *ItemID.ToString());
	return nullptr;
}

TArray<UItemDataAsset*> UItemDatabase::GetAllItemDataAssets() const
{
	TArray<UItemDataAsset*> Items;
	Items.Reserve(ItemRegistry.Num());
	for (const auto& Pair : ItemRegistry)
	{
		if (Pair.Value)
		{
			Items.Add(Pair.Value);
		}
	}
	return Items;
}

TArray<UItemDataAsset*> UItemDatabase::GetItemsByType(EItemType ItemType) const
{
	TArray<UItemDataAsset*> Items;
	for (const auto& Pair : ItemRegistry)
	{
		if (Pair.Value && Pair.Value->Type == ItemType)
		{
			Items.Add(Pair.Value);
		}
	}
	return Items;
}

TArray<UItemDataAsset*> UItemDatabase::GetItemsByRarity(EItemRarity Rarity) const
{
	TArray<UItemDataAsset*> Items;
	for (const auto& Pair : ItemRegistry)
	{
		if (Pair.Value && Pair.Value->Rarity == Rarity)
		{
			Items.Add(Pair.Value);
		}
	}
	return Items;
}

TArray<FName> UItemDatabase::GetAllItemIDs() const
{
	TArray<FName> ItemIDs;
	ItemIDs.Reserve(ItemRegistry.Num());
	for (const auto& Pair : ItemRegistry)
	{
		if (Pair.Value)
		{
			ItemIDs.Add(Pair.Value->ItemID);
		}
	}
	return ItemIDs;
}

void UItemDatabase::PrintAllItems() const
{
	UE_LOG(LogTemp, Log, TEXT("=== ItemDatabase: All Registered Items ==="));
	UE_LOG(LogTemp, Log, TEXT("Total Items: %d"), ItemRegistry.Num());
	
	if (ItemRegistry.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No items registered! Check Asset Manager configuration."));
		return;
	}

	for (const auto& Pair : ItemRegistry)
	{
		if (Pair.Value)
		{
			UE_LOG(LogTemp, Log, TEXT("  - ID: %s | Name: %s | Type: %d | Rarity: %d"),
				*Pair.Value->ItemID.ToString(),
				*Pair.Value->ItemName.ToString(),
				(int32)Pair.Value->Type,
				(int32)Pair.Value->Rarity);
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("=== End Item List ==="));
}

UItemBase* UItemDatabase::CreateItem(const FName& ItemID, int32 Quantity) const
{
	UItemDataAsset* ItemData = GetItemDataAsset(ItemID);
	if (!ItemData)
	{
		return nullptr;
	}

	UItemBase* NewItem = NewObject<UItemBase>();
	NewItem->ItemData = ItemData;
	NewItem->Quantity = FMath::Max(1, Quantity);

	return NewItem;
}

