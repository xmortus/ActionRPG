// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data/ClassDatabase.h"
#include "Progression/Core/ClassDataAsset.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

UClassDatabase* UClassDatabase::Instance = nullptr;

UClassDatabase* UClassDatabase::Get()
{
	if (!Instance)
	{
		Instance = NewObject<UClassDatabase>();
		Instance->AddToRoot();
		Instance->Initialize();
	}
	return Instance;
}

void UClassDatabase::Initialize()
{
	ClassRegistry.Empty();
	ProfessionRegistry.Empty();

	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		TArray<FPrimaryAssetId> ClassAssetIds;
		AssetManager->GetPrimaryAssetIdList(FPrimaryAssetType("Class"), ClassAssetIds);

		for (const FPrimaryAssetId& AssetId : ClassAssetIds)
		{
			TSharedPtr<FStreamableHandle> Handle = AssetManager->LoadPrimaryAsset(AssetId, TArray<FName>());
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete();
			}

			UObject* AssetObject = AssetManager->GetPrimaryAssetObject(AssetId);
			if (UClassDataAsset* ClassData = Cast<UClassDataAsset>(AssetObject))
			{
				if (ClassData->ClassID != NAME_None)
				{
					ClassRegistry.Add(ClassData->ClassID, ClassData);
				}
			}
		}

		TArray<FPrimaryAssetId> ProfessionAssetIds;
		AssetManager->GetPrimaryAssetIdList(FPrimaryAssetType("Profession"), ProfessionAssetIds);

		for (const FPrimaryAssetId& AssetId : ProfessionAssetIds)
		{
			TSharedPtr<FStreamableHandle> Handle = AssetManager->LoadPrimaryAsset(AssetId, TArray<FName>());
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete();
			}

			UObject* AssetObject = AssetManager->GetPrimaryAssetObject(AssetId);
			if (UProfessionDataAsset* ProfessionData = Cast<UProfessionDataAsset>(AssetObject))
			{
				if (ProfessionData->ProfessionID != NAME_None)
				{
					ProfessionRegistry.Add(ProfessionData->ProfessionID, ProfessionData);
				}
			}
		}
	}
}

UClassDataAsset* UClassDatabase::GetClassDataAsset(const FName& ClassID) const
{
	if (const TObjectPtr<UClassDataAsset>* Found = ClassRegistry.Find(ClassID))
	{
		return *Found;
	}

	return nullptr;
}

UProfessionDataAsset* UClassDatabase::GetProfessionDataAsset(const FName& ProfessionID) const
{
	if (const TObjectPtr<UProfessionDataAsset>* Found = ProfessionRegistry.Find(ProfessionID))
	{
		return *Found;
	}

	return nullptr;
}

TArray<UClassDataAsset*> UClassDatabase::GetAllClasses() const
{
	TArray<UClassDataAsset*> Result;
	Result.Reserve(ClassRegistry.Num());
	for (const auto& Pair : ClassRegistry)
	{
		if (Pair.Value)
		{
			Result.Add(Pair.Value);
		}
	}
	return Result;
}

TArray<UProfessionDataAsset*> UClassDatabase::GetAllProfessions() const
{
	TArray<UProfessionDataAsset*> Result;
	Result.Reserve(ProfessionRegistry.Num());
	for (const auto& Pair : ProfessionRegistry)
	{
		if (Pair.Value)
		{
			Result.Add(Pair.Value);
		}
	}
	return Result;
}
