// Copyright Epic Games, Inc. All Rights Reserved.

#include "Data/SkillDatabase.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Skills/Core/SkillBase.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

USkillDatabase* USkillDatabase::Instance = nullptr;

USkillDatabase* USkillDatabase::Get()
{
	if (!Instance)
	{
		Instance = NewObject<USkillDatabase>();
		Instance->AddToRoot(); // Prevent garbage collection
		Instance->Initialize();
	}
	return Instance;
}

void USkillDatabase::Initialize()
{
	// Clear existing registry
	SkillRegistry.Empty();

	UE_LOG(LogTemp, Log, TEXT("SkillDatabase: Initializing..."));

	// Load all skill data assets using Asset Manager
	if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
	{
		UE_LOG(LogTemp, Log, TEXT("SkillDatabase: AssetManager found, scanning for skills..."));
		
		TArray<FPrimaryAssetId> SkillAssetIds;
		AssetManager->GetPrimaryAssetIdList(FPrimaryAssetType("Skill"), SkillAssetIds);

		UE_LOG(LogTemp, Log, TEXT("SkillDatabase: Found %d skill assets to process"), SkillAssetIds.Num());

		if (SkillAssetIds.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("SkillDatabase: No skills found! Check Asset Manager configuration:"));
			UE_LOG(LogTemp, Warning, TEXT("  - Primary Asset Type 'Skill' must be configured"));
			UE_LOG(LogTemp, Warning, TEXT("  - Directory '/Game/Data/Skills/' must be in scan list"));
			UE_LOG(LogTemp, Warning, TEXT("  - Skill Data Assets must have 'Skill Type' set to 'Skill'"));
		}

		for (const FPrimaryAssetId& AssetId : SkillAssetIds)
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
				UE_LOG(LogTemp, Warning, TEXT("SkillDatabase: Failed to get asset object: %s"), *AssetId.ToString());
				continue;
			}

			// Log the actual class name for debugging
			FString ClassName = AssetObject->GetClass()->GetName();
			FString ObjectName = AssetObject->GetName();
			UE_LOG(LogTemp, Log, TEXT("SkillDatabase: Found asset - ID: %s, Object: %s, Class: %s"), 
				*AssetId.ToString(), *ObjectName, *ClassName);

			// Check if it's a class instead of an instance (shouldn't happen, but just in case)
			if (UClass* AssetClass = Cast<UClass>(AssetObject))
			{
				UE_LOG(LogTemp, Error, TEXT("SkillDatabase: Asset is a class, not an instance! %s (Class: %s)"), 
					*AssetId.ToString(), *AssetClass->GetName());
				UE_LOG(LogTemp, Error, TEXT("  This usually means the asset was created as a Blueprint Class instead of a Data Asset."));
				UE_LOG(LogTemp, Error, TEXT("  Solution: Delete the asset and recreate it as 'Data Asset' -> 'Skill Data Asset'"));
				continue;
			}

			// Try to cast to SkillDataAsset
			USkillDataAsset* SkillData = Cast<USkillDataAsset>(AssetObject);
			if (SkillData)
			{
				if (SkillData->SkillID != NAME_None)
				{
					SkillRegistry.Add(SkillData->SkillID, SkillData);
					UE_LOG(LogTemp, Log, TEXT("SkillDatabase: Registered SkillDataAsset (template) - ID: %s, Name: %s"), 
						*SkillData->SkillID.ToString(), *SkillData->SkillName.ToString());
					UE_LOG(LogTemp, Verbose, TEXT("  Note: SkillDatabase stores SkillDataAssets (templates), not actual skill instances."));
					UE_LOG(LogTemp, Verbose, TEXT("  Actual skill instances are stored in each player's SkillComponent."));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("SkillDatabase: Skill Data Asset has empty SkillID: %s"), *AssetId.ToString());
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("SkillDatabase: Failed to cast asset to SkillDataAsset: %s"), *AssetId.ToString());
				UE_LOG(LogTemp, Error, TEXT("  Object Name: %s"), *ObjectName);
				UE_LOG(LogTemp, Error, TEXT("  Actual Class: %s"), *ClassName);
				UE_LOG(LogTemp, Error, TEXT("  Expected Class: SkillDataAsset"));
				UE_LOG(LogTemp, Error, TEXT("  This usually means:"));
				UE_LOG(LogTemp, Error, TEXT("    1. Asset was created as Blueprint Class (wrong!)"));
				UE_LOG(LogTemp, Error, TEXT("    2. Asset was created as regular DataAsset instead of SkillDataAsset"));
				UE_LOG(LogTemp, Error, TEXT("    3. Asset needs to be recreated: Right-click -> Miscellaneous -> Data Asset -> Skill Data Asset"));
			}
		}

		UE_LOG(LogTemp, Log, TEXT("SkillDatabase: Initialization complete. Registered %d SkillDataAssets (templates)."), SkillRegistry.Num());
		UE_LOG(LogTemp, Log, TEXT("  SkillDatabase is a SINGLETON - stores SkillDataAssets (templates) shared by all players."));
		UE_LOG(LogTemp, Log, TEXT("  Actual skill instances are stored in each player's SkillComponent (unique per player)."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SkillDatabase: AssetManager is not initialized!"));
		UE_LOG(LogTemp, Error, TEXT("SkillDatabase: Skills will need to be registered manually or Asset Manager must be configured."));
	}
}

USkillDataAsset* USkillDatabase::GetSkillDataAsset(const FName& SkillID) const
{
	if (const TObjectPtr<USkillDataAsset>* FoundSkill = SkillRegistry.Find(SkillID))
	{
		return *FoundSkill;
	}

	UE_LOG(LogTemp, Warning, TEXT("SkillDatabase: Skill not found: %s"), *SkillID.ToString());
	return nullptr;
}

TArray<USkillDataAsset*> USkillDatabase::GetAllSkillDataAssets() const
{
	TArray<USkillDataAsset*> Skills;
	Skills.Reserve(SkillRegistry.Num());
	for (const auto& Pair : SkillRegistry)
	{
		if (Pair.Value)
		{
			Skills.Add(Pair.Value);
		}
	}
	return Skills;
}

TArray<USkillDataAsset*> USkillDatabase::GetSkillsByType(ESkillType SkillType) const
{
	TArray<USkillDataAsset*> Skills;
	Skills.Reserve(SkillRegistry.Num() / 5); // Reserve approximate size (assuming roughly 5 skill types)
	for (const auto& Pair : SkillRegistry)
	{
		if (Pair.Value && Pair.Value->Type == SkillType)
		{
			Skills.Add(Pair.Value);
		}
	}
	return Skills;
}

TArray<USkillDataAsset*> USkillDatabase::GetSkillsByCategory(ESkillCategory SkillCategory) const
{
	TArray<USkillDataAsset*> Skills;
	Skills.Reserve(SkillRegistry.Num() / 4); // Reserve approximate size (assuming roughly 4 categories)
	for (const auto& Pair : SkillRegistry)
	{
		if (Pair.Value && Pair.Value->Category == SkillCategory)
		{
			Skills.Add(Pair.Value);
		}
	}
	return Skills;
}

TArray<FName> USkillDatabase::GetAllSkillIDs() const
{
	TArray<FName> SkillIDs;
	SkillIDs.Reserve(SkillRegistry.Num());
	for (const auto& Pair : SkillRegistry)
	{
		if (Pair.Value)
		{
			SkillIDs.Add(Pair.Value->SkillID);
		}
	}
	return SkillIDs;
}

void USkillDatabase::PrintAllSkills() const
{
	UE_LOG(LogTemp, Log, TEXT("=== SkillDatabase: All Registered SkillDataAssets (Templates) ==="));
	UE_LOG(LogTemp, Log, TEXT("Note: SkillDatabase stores SkillDataAssets (templates), NOT actual skill instances."));
	UE_LOG(LogTemp, Log, TEXT("Actual skill instances are stored in each player's SkillComponent."));
	UE_LOG(LogTemp, Log, TEXT("Total SkillDataAssets: %d"), SkillRegistry.Num());
	
	if (SkillRegistry.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No skills registered! Check Asset Manager configuration."));
		return;
	}

	for (const auto& Pair : SkillRegistry)
	{
		if (Pair.Value)
		{
			UE_LOG(LogTemp, Log, TEXT("  - ID: %s | Name: %s | Type: %d | Category: %d | Cooldown: %.2f | Mana: %.2f | Stamina: %.2f"),
				*Pair.Value->SkillID.ToString(),
				*Pair.Value->SkillName.ToString(),
				(int32)Pair.Value->Type,
				(int32)Pair.Value->Category,
				Pair.Value->CooldownDuration,
				Pair.Value->ManaCost,
				Pair.Value->StaminaCost);
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("=== End Skill List ==="));
}

USkillBase* USkillDatabase::CreateSkill(const FName& SkillID) const
{
	UE_LOG(LogTemp, Verbose, TEXT("SkillDatabase::CreateSkill - Creating skill instance (NOT storing in database): %s"), 
		*SkillID.ToString());

	USkillDataAsset* SkillData = GetSkillDataAsset(SkillID);
	if (!SkillData)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillDatabase::CreateSkill - SkillDataAsset not found: %s"), *SkillID.ToString());
		return nullptr;
	}

	// Create a new skill instance - this is NOT stored in the database
	// The database only stores SkillDataAssets (templates), not actual skill instances
	// Actual skill instances are stored in the player's SkillComponent
	// Use GetTransientPackage() as outer for temporary skills that will be moved to SkillComponent
	USkillBase* NewSkill = NewObject<USkillBase>(GetTransientPackage(), USkillBase::StaticClass());
	if (NewSkill)
	{
		NewSkill->SkillData = SkillData;
		NewSkill->CooldownRemaining = 0.0f;
		NewSkill->SkillLevel = 1;
		NewSkill->Experience = 0.0f;

		UE_LOG(LogTemp, Verbose, TEXT("SkillDatabase::CreateSkill - Skill instance created (will be stored in SkillComponent): %s"),
			*SkillData->SkillName.ToString());
	}

	return NewSkill;
}
