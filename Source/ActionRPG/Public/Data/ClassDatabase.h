// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ClassDatabase.generated.h"

class UClassDataAsset;
class UProfessionDataAsset;

/**
 * Singleton database for managing Class and Profession Data Assets.
 */
UCLASS()
class ACTIONRPG_API UClassDatabase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class Database")
	static UClassDatabase* Get();

	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "Class Database")
	UClassDataAsset* GetClassDataAsset(const FName& ClassID) const;

	UFUNCTION(BlueprintCallable, Category = "Class Database")
	UProfessionDataAsset* GetProfessionDataAsset(const FName& ProfessionID) const;

	UFUNCTION(BlueprintCallable, Category = "Class Database")
	TArray<UClassDataAsset*> GetAllClasses() const;

	UFUNCTION(BlueprintCallable, Category = "Class Database")
	TArray<UProfessionDataAsset*> GetAllProfessions() const;

protected:
	UPROPERTY()
	TMap<FName, TObjectPtr<UClassDataAsset>> ClassRegistry;

	UPROPERTY()
	TMap<FName, TObjectPtr<UProfessionDataAsset>> ProfessionRegistry;

private:
	static UClassDatabase* Instance;
};
