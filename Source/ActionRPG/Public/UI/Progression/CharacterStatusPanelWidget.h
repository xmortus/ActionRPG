// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStatusPanelWidget.generated.h"

class UTextBlock;
class UExperienceComponent;
class UClassComponent;
class UClassDataAsset;
class UProfessionDataAsset;

/**
 * Character status panel (information only).
 * Displays class/profession slots, current class/profession, and XP totals.
 */
UCLASS()
class ACTIONRPG_API UCharacterStatusPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Character Status UI")
	void UpdateStatusDisplay();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	float GetUnallocatedExperience() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	float GetTotalExperience() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	int32 GetPlayerLevel() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	FText GetCurrentClassName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	FText GetCurrentProfessionName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	FText GetSelectedClassesText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	FText GetSelectedProfessionsText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	FText GetClassSlotsText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	FText GetProfessionSlotsText() const;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CurrentClassText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CurrentProfessionText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SelectedClassesText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SelectedProfessionsText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ClassSlotsText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ProfessionSlotsText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PlayerLevelText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> UnallocatedExperienceText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TotalExperienceText;

private:
	UPROPERTY()
	TObjectPtr<UExperienceComponent> ExperienceComponent;

	UPROPERTY()
	TObjectPtr<UClassComponent> ClassComponent;

	void BindComponents();

	UFUNCTION()
	void HandleExperienceChanged(float NewUnallocatedExperience, float Delta);

	UFUNCTION()
	void HandleClassListChanged(UClassDataAsset* ChangedClass, bool bAdded);

	UFUNCTION()
	void HandleProfessionListChanged(UProfessionDataAsset* ChangedProfession, bool bAdded);

	UFUNCTION()
	void HandleClassLevelChanged(UClassDataAsset* ClassAsset, int32 NewLevel);

	FText BuildClassListText() const;
	FText BuildProfessionListText() const;
};
