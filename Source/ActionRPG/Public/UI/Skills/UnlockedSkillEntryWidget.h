// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnlockedSkillEntryWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
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

protected:
	// Widget references (bind in Blueprint)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SkillNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AssignButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AssignMainHandButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AssignOffhandButton;

	UFUNCTION()
	void OnAssignClicked();

	UFUNCTION()
	void OnAssignMainHandClicked();

	UFUNCTION()
	void OnAssignOffhandClicked();

private:
	UPROPERTY()
	TObjectPtr<USkillBase> CurrentSkill;

	UPROPERTY()
	TObjectPtr<UUnlockedSkillsPanelWidget> ParentPanel;
};
