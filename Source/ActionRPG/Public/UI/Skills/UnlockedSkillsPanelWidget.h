// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnlockedSkillsPanelWidget.generated.h"

class UUniformGridPanel;
class UUnlockedSkillEntryWidget;
class USkillManagerComponent;
class USkillBase;

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

protected:
	// Widget references (bind in Blueprint)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> SkillGrid;

	// Entry widget class (set in Blueprint)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unlocked Skills")
	TSubclassOf<UUnlockedSkillEntryWidget> EntryWidgetClass;

	UFUNCTION()
	void OnSkillUnlockedInternal(USkillBase* Skill);

private:
	UPROPERTY()
	TObjectPtr<USkillManagerComponent> SkillManagerComponent;

	UPROPERTY()
	TArray<TObjectPtr<UUnlockedSkillEntryWidget>> EntryWidgets;

	int32 PendingAssignSlotIndex = -1;

	USkillManagerComponent* GetSkillManagerComponent() const;
	int32 FindFirstEmptySkillSlot() const;
};
