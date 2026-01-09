// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Skills/Core/SkillTypes.h"
#include "SkillDatabase.generated.h"

class USkillDataAsset;

/**
 * Singleton database for managing Skill Data Assets.
 * Provides lookup and retrieval of skill data assets by ID, type, or category.
 * Automatically loads all skill data assets via Asset Manager on initialization.
 */
UCLASS()
class ACTIONRPG_API USkillDatabase : public UObject
{
	GENERATED_BODY()

public:
	// Singleton accessor
	UFUNCTION(BlueprintCallable, Category = "Skill Database")
	static USkillDatabase* Get();

	// Initialize database - loads all skill data assets
	void Initialize();

	// Lookup methods
	UFUNCTION(BlueprintCallable, Category = "Skill Database")
	USkillDataAsset* GetSkillDataAsset(const FName& SkillID) const;

	UFUNCTION(BlueprintCallable, Category = "Skill Database")
	TArray<USkillDataAsset*> GetAllSkillDataAssets() const;

	UFUNCTION(BlueprintCallable, Category = "Skill Database")
	TArray<USkillDataAsset*> GetSkillsByType(ESkillType SkillType) const;

	UFUNCTION(BlueprintCallable, Category = "Skill Database")
	TArray<USkillDataAsset*> GetSkillsByCategory(ESkillCategory SkillCategory) const;

	// Debug: Get all registered skill IDs
	UFUNCTION(BlueprintCallable, Category = "Skill Database|Debug")
	TArray<FName> GetAllSkillIDs() const;

	// Debug: Print all registered skills to log
	UFUNCTION(BlueprintCallable, Category = "Skill Database|Debug")
	void PrintAllSkills() const;

	// Create skill instance from data asset
	UFUNCTION(BlueprintCallable, Category = "Skill Database")
	class USkillBase* CreateSkill(const FName& SkillID) const;

protected:
	// Registry of all skill data assets
	UPROPERTY()
	TMap<FName, TObjectPtr<USkillDataAsset>> SkillRegistry;

private:
	static USkillDatabase* Instance;
};
