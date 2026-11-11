// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Progression/Core/ProgressionTypes.h"
#include "CharacterStatusPanelWidget.generated.h"

class UTextBlock;
class UButton;
class UWidgetSwitcher;
class UExperienceComponent;
class UClassComponent;
class UAttributeComponent;
class UAcquiredAttributeComponent;
class USecondaryAttributeComponent;
class USkillManagerComponent;
class UTrainingComponent;
class UStatusEffectComponent;
class UClassDataAsset;
class UProfessionDataAsset;

/**
 * Native character status panel with inline primary-attribute allocation.
 */
UCLASS()
class ACTIONRPG_API UCharacterStatusPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void EnsureContentBuilt();

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	FText GetPrimaryAttributesText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	int32 GetUnspentAttributePoints() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character Status UI")
	int32 GetUnspentSkillPoints() const;

	UFUNCTION(BlueprintCallable, Category = "Character Status UI")
	bool AllocateAttributePoint(EPrimaryAttribute Attribute);

protected:
	UPROPERTY()
	TObjectPtr<UTextBlock> SelectedClassesText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SelectedProfessionsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> ClassSlotsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> ProfessionSlotsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> PlayerLevelText;

	UPROPERTY()
	TObjectPtr<UTextBlock> UnallocatedExperienceText;

	UPROPERTY()
	TObjectPtr<UTextBlock> TotalExperienceText;

	UPROPERTY()
	TObjectPtr<UTextBlock> PrimaryAttributesText;

	UPROPERTY()
	TObjectPtr<UTextBlock> UnspentAttributePointsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> UnspentSkillPointsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> TrainingProgressText;

	UPROPERTY()
	TObjectPtr<UTextBlock> AcquiredOverviewText;

	UPROPERTY()
	TObjectPtr<UTextBlock> AcquiredSourcesText;

	UPROPERTY()
	TArray<TObjectPtr<UTextBlock>> AttributeSourceTexts;

	UPROPERTY()
	TObjectPtr<UWidgetSwitcher> StatusTabSwitcher;

	UPROPERTY()
	TObjectPtr<UButton> OverviewTabButton;

	UPROPERTY()
	TObjectPtr<UButton> TrainingTabButton;

	UPROPERTY()
	TObjectPtr<UButton> SourcesTabButton;

private:
	UPROPERTY()
	TObjectPtr<UExperienceComponent> ExperienceComponent;

	UPROPERTY()
	TObjectPtr<UClassComponent> ClassComponent;

	UPROPERTY()
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY()
	TObjectPtr<UAcquiredAttributeComponent> AcquiredAttributeComponent;

	UPROPERTY()
	TObjectPtr<USecondaryAttributeComponent> SecondaryAttributeComponent;

	UPROPERTY()
	TObjectPtr<USkillManagerComponent> SkillManagerComponent;

	UPROPERTY()
	TObjectPtr<UTrainingComponent> TrainingComponent;

	UPROPERTY()
	TObjectPtr<UStatusEffectComponent> StatusEffectComponent;

	UPROPERTY()
	TObjectPtr<UTextBlock> ActiveEffectsText;

	UPROPERTY()
	TObjectPtr<UButton> EffectsTabButton;

	void BindComponents();
	void AddAttributeRow(class UVerticalBox* Parent, const TCHAR* Name, const TCHAR* Label, EPrimaryAttribute Attribute);
	void SetStatusTab(int32 TabIndex);

	UFUNCTION()
	void HandleOverviewTabClicked();

	UFUNCTION()
	void HandleTrainingTabClicked();

	UFUNCTION()
	void HandleSourcesTabClicked();

	UFUNCTION()
	void HandleEffectsTabClicked();

	UFUNCTION()
	void HandleEffectsChanged();

	UFUNCTION()
	void HandleAllocateStrength();

	UFUNCTION()
	void HandleAllocateAgility();

	UFUNCTION()
	void HandleAllocateConstitution();

	UFUNCTION()
	void HandleAllocateDexterity();

	UFUNCTION()
	void HandleAllocateIntelligence();

	UFUNCTION()
	void HandleAllocateWisdom();

	UFUNCTION()
	void HandleCloseClicked();

	UFUNCTION()
	void HandleExperienceChanged(float NewUnallocatedExperience, float Delta);

	UFUNCTION()
	void HandleClassListChanged(UClassDataAsset* ChangedClass, bool bAdded);

	UFUNCTION()
	void HandleProfessionListChanged(UProfessionDataAsset* ChangedProfession, bool bAdded);

	UFUNCTION()
	void HandleClassLevelChanged(UClassDataAsset* ClassAsset, int32 NewLevel);

	UFUNCTION()
	void HandleProfessionLevelChanged(UProfessionDataAsset* ProfessionAsset, int32 NewLevel);

	UFUNCTION()
	void HandlePrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue);

	UFUNCTION()
	void HandleAcquiredAttributeChanged(EAcquiredAttribute Attribute, int32 NewValue, int32 Cap);

	UFUNCTION()
	void HandleSkillPointsChanged(int32 NewBalance, int32 Delta);

	UFUNCTION()
	void HandleAttributePractice(EPrimaryAttribute Attribute, FName Activity, int32 Progress,
		int32 Required, bool bGrantedGrowth);

	FText BuildClassListText() const;
	FText BuildProfessionListText() const;
	FText BuildPrimaryAttributeText() const;
	FText BuildTrainingProgressText() const;
	FText BuildActiveEffectsText() const;
	FText BuildAttributeBreakdownText(EPrimaryAttribute Attribute) const;
};
