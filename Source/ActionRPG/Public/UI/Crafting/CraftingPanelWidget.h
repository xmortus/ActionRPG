// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ComboBoxString.h"
#include "CraftingPanelWidget.generated.h"

class APawn;
class UButton;
class UClassComponent;
class UCraftingRecipeDataAsset;
class UInventoryComponent;
class UItemBase;
class UItemDataAsset;
class USkillBase;
class USkillManagerComponent;
class UTextBlock;
class UBorder;

/** Native crafting screen driven by the player's selected professions and unlocked skills. */
UCLASS()
class ACTIONRPG_API UCraftingPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindPlayer(APawn* InPawn);
	void EnsureContentBuilt();
	void TogglePanel();
	bool IsPanelOpen() const;
	void Refresh();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY()
	TObjectPtr<UClassComponent> Classes;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> Inventory;

	UPROPERTY()
	TObjectPtr<USkillManagerComponent> Skills;

	UPROPERTY()
	TArray<TObjectPtr<UCraftingRecipeDataAsset>> Recipes;

	UPROPERTY()
	TObjectPtr<UCraftingRecipeDataAsset> SelectedRecipe;

	UPROPERTY()
	TObjectPtr<UBorder> Panel;

	UPROPERTY()
	TObjectPtr<UComboBoxString> RecipePicker;

	UPROPERTY()
	TObjectPtr<UTextBlock> IngredientText;

	UPROPERTY()
	TObjectPtr<UTextBlock> OutputText;

	UPROPERTY()
	TObjectPtr<UTextBlock> StatusText;

	UPROPERTY()
	TObjectPtr<UButton> CraftButton;

	bool bUpdatingOptions = false;
	void UnbindPlayer();
	void RebuildRecipeOptions();
	void RefreshDetails();
	int32 GetOwnedQuantity(const UItemDataAsset* Item) const;

	UFUNCTION()
	void HandleRecipeSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void HandleCraftClicked();

	UFUNCTION()
	void HandleToggleClicked();

	UFUNCTION()
	void HandleInventoryChanged(int32 SlotIndex, UItemBase* Item);

	UFUNCTION()
	void HandleProfessionChanged(class UProfessionDataAsset* Profession, bool bAdded);

	UFUNCTION()
	void HandleSkillUnlocked(USkillBase* Skill);

	UFUNCTION()
	void HandleSkillRevoked(USkillBase* Skill);
};
