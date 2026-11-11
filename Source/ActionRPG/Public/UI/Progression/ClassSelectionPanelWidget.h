// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClassSelectionPanelWidget.generated.h"

class UClassComponent;
class UExperienceComponent;
class UClassDataAsset;
class UProfessionDataAsset;
class UTextBlock;
class UButton;
class USpinBox;
class UVerticalBox;

/**
 * Class/profession selection panel widget with bound component events.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UClassSelectionPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void EnsureContentBuilt();
	void RefreshPanel();

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	void RefreshClassList();

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	void RefreshAllClassesList();

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	void SetSelectedClassForXp(UClassDataAsset* ClassAsset);

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	void SetSelectedProfessionForXp(UProfessionDataAsset* ProfessionAsset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	UClassDataAsset* GetSelectedClassForXp() const { return SelectedClassForXp; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	UProfessionDataAsset* GetSelectedProfessionForXp() const { return SelectedProfessionForXp; }

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	bool AllocateExperienceToSelectedClass(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	bool AllocateExperienceToClass(UClassDataAsset* ClassAsset, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	bool AllocateExperienceToClassAllowUnselected(UClassDataAsset* ClassAsset, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	bool AddClass(UClassDataAsset* ClassAsset);

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	bool AddProfession(UProfessionDataAsset* ProfessionAsset);

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	bool RemoveClass(UClassDataAsset* ClassAsset);

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	bool RemoveProfession(UProfessionDataAsset* ProfessionAsset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	int32 GetPlayerLevel() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	float GetUnallocatedExperience() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	TArray<UClassDataAsset*> GetSelectedClasses() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	FText GetSelectedClassesText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	TArray<UClassDataAsset*> GetAllClasses() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	TArray<UProfessionDataAsset*> GetSelectedProfessions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	FText GetSelectedProfessionsText() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Class UI")
	void OnAllClassesListUpdated(const TArray<UClassDataAsset*>& Classes);

	UFUNCTION(BlueprintImplementableEvent, Category = "Class UI")
	void OnSelectedClassForXpChanged(UClassDataAsset* ClassAsset);

	UFUNCTION(BlueprintImplementableEvent, Category = "Class UI")
	void OnClassListUpdated(const TArray<UClassDataAsset*>& Classes, const TArray<UProfessionDataAsset*>& Professions);

	UFUNCTION(BlueprintImplementableEvent, Category = "Class UI")
	void OnClassProgressUpdated(UClassDataAsset* ClassAsset, int32 NewLevel, float NewExperience, float ExperienceToNextLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Class UI")
	void OnPlayerLevelUpdated(int32 NewPlayerLevel);

private:
	UPROPERTY()
	TObjectPtr<UClassComponent> ClassComponent;

	UPROPERTY()
	TObjectPtr<UExperienceComponent> ExperienceComponent;

	UPROPERTY()
	TObjectPtr<UClassDataAsset> SelectedClassForXp;

	UPROPERTY()
	TObjectPtr<UProfessionDataAsset> SelectedProfessionForXp;

	UPROPERTY()
	TObjectPtr<UTextBlock> AvailableXpText;

	UPROPERTY()
	TObjectPtr<UTextBlock> OverviewText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SelectedNameText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SelectedDescriptionText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SelectedStatusText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SelectedRequirementsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> SelectedRewardsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> MessageText;

	UPROPERTY()
	TObjectPtr<UVerticalBox> ClassRows;

	UPROPERTY()
	TObjectPtr<UVerticalBox> ProfessionRows;

	UPROPERTY()
	TObjectPtr<USpinBox> AmountInput;

	UPROPERTY()
	TObjectPtr<UButton> AssignButton;

	UPROPERTY()
	TObjectPtr<UButton> ActivateButton;

	UPROPERTY()
	TObjectPtr<UButton> RemoveButton;

	void BindComponents();
	void RebuildEntries();
	void RefreshSelectedDetails();

	UFUNCTION()
	void HandleAssignClicked();

	UFUNCTION()
	void HandleSetHundredClicked();

	UFUNCTION()
	void HandleSetAvailableClicked();

	UFUNCTION()
	void HandleAmountChanged(float NewValue);

	UFUNCTION()
	void HandleActivateClicked();

	UFUNCTION()
	void HandleRemoveClicked();

	UFUNCTION()
	void HandleCloseClicked();

	UFUNCTION()
	void HandlePoolChanged(float NewUnallocatedExperience, float Delta);

	UFUNCTION()
	void HandleClassListChanged(UClassDataAsset* ChangedClass, bool bAdded);

	UFUNCTION()
	void HandleProfessionListChanged(UProfessionDataAsset* ChangedProfession, bool bAdded);

	UFUNCTION()
	void HandleClassExperienceChanged(UClassDataAsset* ClassAsset, float NewExperience);

	UFUNCTION()
	void HandleClassLevelChanged(UClassDataAsset* ClassAsset, int32 NewLevel);

	UFUNCTION()
	void HandleProfessionExperienceChanged(UProfessionDataAsset* ProfessionAsset, float NewExperience);

	UFUNCTION()
	void HandleProfessionLevelChanged(UProfessionDataAsset* ProfessionAsset, int32 NewLevel);
};
