// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClassSelectionPanelWidget.generated.h"

class UClassComponent;
class UExperienceComponent;
class UClassDataAsset;
class UProfessionDataAsset;

/**
 * Class/profession selection panel widget with bound component events.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UClassSelectionPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	void RefreshClassList();

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	bool AllocateExperienceToClass(UClassDataAsset* ClassAsset, float Amount);

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
	TArray<UProfessionDataAsset*> GetSelectedProfessions() const;

protected:
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

	void BindComponents();

	UFUNCTION()
	void HandleClassListChanged(UClassDataAsset* ChangedClass, bool bAdded);

	UFUNCTION()
	void HandleProfessionListChanged(UProfessionDataAsset* ChangedProfession, bool bAdded);

	UFUNCTION()
	void HandleClassExperienceChanged(UClassDataAsset* ClassAsset, float NewExperience);

	UFUNCTION()
	void HandleClassLevelChanged(UClassDataAsset* ClassAsset, int32 NewLevel);
};
