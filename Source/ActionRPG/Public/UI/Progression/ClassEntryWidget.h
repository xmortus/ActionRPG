// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClassEntryWidget.generated.h"

class UClassDataAsset;
class UProfessionDataAsset;
class UClassComponent;
class UClassSelectionPanelWidget;
class UTexture2D;
class UButton;

/**
 * Single class/profession row entry widget.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UClassEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

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

	UFUNCTION(BlueprintCallable, Category = "Class UI")
	void SetSelectedForXp(bool bSelected);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	bool IsSelectedForXp() const { return bIsSelectedForXp; }

	void RefreshRow();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	UClassDataAsset* GetClassAsset() const { return ClassAsset; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class UI")
	UProfessionDataAsset* GetProfessionAsset() const { return ProfessionAsset; }

	UPROPERTY(BlueprintReadWrite, Category = "Class UI", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UClassSelectionPanelWidget> ParentPanel;

private:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Button_SelectClass;

	UPROPERTY()
	TObjectPtr<class UTextBlock> RowText;

	UFUNCTION()
	void HandleSelectionClicked();

	UPROPERTY()
	TObjectPtr<UClassDataAsset> ClassAsset;

	UPROPERTY()
	TObjectPtr<UProfessionDataAsset> ProfessionAsset;

	UPROPERTY()
	TObjectPtr<UClassComponent> ClassComponent;

	UPROPERTY()
	bool bIsProfessionEntry = false;

	UPROPERTY()
	bool bIsSelectedForXp = false;
};
