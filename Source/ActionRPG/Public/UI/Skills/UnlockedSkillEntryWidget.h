// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnlockedSkillEntryWidget.generated.h"

class UImage;
class UTextBlock;
class USkillBase;
class UUnlockedSkillsPanelWidget;

/**
 * Entry widget for an unlocked skill in the Unlocked Skills panel.
 */
UCLASS()
class ACTIONRPG_API UUnlockedSkillEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Unlocked Skills")
	void SetSkillData(USkillBase* InSkill, UUnlockedSkillsPanelWidget* InParentPanel);

	void RefreshProgress();
	USkillBase* GetSkill() const { return CurrentSkill; }

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	// Widget references (bind in Blueprint)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillNameText;

	UPROPERTY()
	TObjectPtr<UTextBlock> ProgressText;

private:
	UPROPERTY()
	TObjectPtr<USkillBase> CurrentSkill;

	UPROPERTY()
	TObjectPtr<UUnlockedSkillsPanelWidget> ParentPanel;
};
