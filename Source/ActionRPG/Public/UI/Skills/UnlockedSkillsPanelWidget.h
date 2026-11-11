// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnlockedSkillsPanelWidget.generated.h"

class UUniformGridPanel;
class UUnlockedSkillEntryWidget;
class USkillManagerComponent;
class USkillComponent;
class USkillBase;
class UBorder;
class UButton;
class UTextBlock;
class UScrollBox;
class UVerticalBox;
class USkillCatalogEntryWidget;

/**
 * Panel widget that lists unlocked skills and allows assignment to Quick-Use slots.
 */
UCLASS()
class ACTIONRPG_API UUnlockedSkillsPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Refresh the list of unlocked skills.
	UFUNCTION(BlueprintCallable, Category = "Unlocked Skills")
	void RefreshUnlockedSkills();
	void RefreshCatalog();

	// Set the slot index to assign skills into (0-7). If invalid, first empty slot is used.
	UFUNCTION(BlueprintCallable, Category = "Unlocked Skills")
	void SetPendingAssignSlotIndex(int32 SlotIndex);

	// Assign a skill to a slot.
	UFUNCTION(BlueprintCallable, Category = "Unlocked Skills")
	bool AssignSkillToSlot(USkillBase* Skill, int32 SlotIndex);

	// Assign a skill using the pending slot index (or first empty slot).
	UFUNCTION(BlueprintCallable, Category = "Unlocked Skills")
	bool AssignSkillToPendingSlot(USkillBase* Skill);

	UFUNCTION(BlueprintCallable, Category = "Unlocked Skills")
	bool AssignSkillToMainHand(USkillBase* Skill);

	UFUNCTION(BlueprintCallable, Category = "Unlocked Skills")
	bool AssignSkillToOffhand(USkillBase* Skill);

	void ShowSkillContextMenu(USkillBase* Skill, const FVector2D& ScreenPosition);
	void HideSkillContextMenu();

protected:
	// Widget references (bind in Blueprint)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> SkillGrid;

	// Entry widget class (set in Blueprint)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unlocked Skills")
	TSubclassOf<UUnlockedSkillEntryWidget> EntryWidgetClass;

	UFUNCTION()
	void OnSkillUnlockedInternal(USkillBase* Skill);
	UFUNCTION() void OnAssignNextClicked();
	UFUNCTION() void OnAssignMainHandClicked();
	UFUNCTION() void OnAssignOffhandClicked();
	UFUNCTION() void OnSkillPointsChangedInternal(int32 NewBalance, int32 Delta);
	UFUNCTION() void OnSkillExperienceGainedInternal(USkillBase* Skill, float Amount, int32 NewLevel);
	UFUNCTION() void OnKnownTabClicked();
	UFUNCTION() void OnLearnTabClicked();
	UFUNCTION() void OnCloseClicked();

private:
	UPROPERTY()
	TObjectPtr<USkillManagerComponent> SkillManagerComponent;
	UPROPERTY()
	TObjectPtr<USkillComponent> SkillComponent;

	UPROPERTY()
	TArray<TObjectPtr<UUnlockedSkillEntryWidget>> EntryWidgets;
	UPROPERTY() TObjectPtr<USkillBase> ContextSkill;
	UPROPERTY() TObjectPtr<UBorder> ContextMenu;
	UPROPERTY() TObjectPtr<UButton> AssignNextButton;
	UPROPERTY() TObjectPtr<UButton> AssignMainHandButton;
	UPROPERTY() TObjectPtr<UButton> AssignOffhandButton;
	UPROPERTY() TObjectPtr<UTextBlock> SkillPointsText;
	UPROPERTY() TObjectPtr<UButton> KnownTabButton;
	UPROPERTY() TObjectPtr<UButton> LearnTabButton;
	UPROPERTY() TObjectPtr<UButton> PanelCloseButton;
	UPROPERTY() TObjectPtr<UScrollBox> CatalogScroll;
	UPROPERTY() TObjectPtr<UVerticalBox> CatalogList;
	UPROPERTY() TArray<TObjectPtr<USkillCatalogEntryWidget>> CatalogEntries;
	bool bShowingCatalog = false;

	int32 PendingAssignSlotIndex = -1;

	USkillManagerComponent* GetSkillManagerComponent() const;
	int32 FindFirstEmptySkillSlot() const;
	void BuildSkillContextMenu();
	void BuildCatalogControls();
	void ShowCatalog(bool bShow);
	void RefreshSkillPointsDisplay();
};
