// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClassEntryWidget.generated.h"

class UClassDataAsset;
class UProfessionDataAsset;
class UClassComponent;
class UTexture2D;

/**
 * Single class/profession row entry widget.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UClassEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Class UI")
	void SetClassEntry(UClassDataAsset* InClassAsset, UClassComponent* InClassComponent);

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	void SetProfessionEntry(UProfessionDataAsset* InProfessionAsset, UClassComponent* InClassComponent);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	bool IsProfessionEntry() const { return bIsProfessionEntry; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	FText GetEntryName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	FText GetEntryDescription() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	UTexture2D* GetEntryIcon() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	int32 GetEntryLevel() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	float GetEntryExperience() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	float GetEntryExperienceToNextLevel() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	bool IsSelected() const;

private:
	UPROPERTY()
	TObjectPtr<UClassDataAsset> ClassAsset;

	UPROPERTY()
	TObjectPtr<UProfessionDataAsset> ProfessionAsset;

	UPROPERTY()
	TObjectPtr<UClassComponent> ClassComponent;

	UPROPERTY()
	bool bIsProfessionEntry = false;
};
