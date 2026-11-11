// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Core/ItemTypes.h"
#include "TrialTrackerWidget.generated.h"

class UButton;
class UProgressBar;
class UTextBlock;
class UTrialComponent;
class UTrialDataAsset;
class UQuestComponent;
class UQuestDataAsset;
class UClassComponent;
class UClassDataAsset;
class UEquipmentComponent;
class UEquipmentItem;
class UInventoryComponent;
class UItemBase;
struct FTrialRewardChoice;

/** Native, data-bound trial list, objective details, reward preview, and retry/fail controls. */
UCLASS()
class ACTIONRPG_API UTrialTrackerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindTrialComponent(UTrialComponent* InTrials);
	void BindJourneySources(UQuestComponent* InQuests, UClassComponent* InClasses,
		UEquipmentComponent* InEquipment, UInventoryComponent* InInventory);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY()
	TObjectPtr<UTrialComponent> Trials;

	UPROPERTY()
	TObjectPtr<UQuestComponent> Quests;

	UPROPERTY()
	TObjectPtr<UClassComponent> Classes;

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> Equipment;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> Inventory;

	UPROPERTY()
	TObjectPtr<UTextBlock> JourneyText;

	UPROPERTY()
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY()
	TObjectPtr<UTextBlock> ObjectiveText;

	UPROPERTY()
	TObjectPtr<UTextBlock> RewardText;

	UPROPERTY()
	TObjectPtr<UTextBlock> KnownTrialsText;

	UPROPERTY()
	TObjectPtr<UTextBlock> NoticeText;

	UPROPERTY()
	TObjectPtr<UProgressBar> ProgressBar;

	UPROPERTY()
	TObjectPtr<UButton> FailButton;

	UPROPERTY()
	TObjectPtr<UButton> RetryRewardButton;

	UPROPERTY()
	TObjectPtr<UButton> ChoiceOneButton;

	UPROPERTY()
	TObjectPtr<UButton> ChoiceTwoButton;

	UPROPERTY()
	TObjectPtr<UTextBlock> ChoiceOneLabel;

	UPROPERTY()
	TObjectPtr<UTextBlock> ChoiceTwoLabel;

	FText LastNotice;

	void Refresh();
	FString BuildRewardPreview(const UTrialDataAsset* Trial) const;
	FString BuildRewardPreview(const FTrialRewardChoice& Reward) const;

	UFUNCTION()
	void HandleTrialStarted(UTrialDataAsset* Trial);

	UFUNCTION()
	void HandleTrialCompleted(UTrialDataAsset* Trial);

	UFUNCTION()
	void HandleTrialFailed(UTrialDataAsset* Trial);

	UFUNCTION()
	void HandleTrialProgress(UTrialDataAsset* Trial, float Progress);

	UFUNCTION()
	void HandleJourneyQuestChanged(UQuestDataAsset* Quest);

	UFUNCTION()
	void HandleJourneyClassListChanged(UClassDataAsset* ClassAsset, bool bAdded);

	UFUNCTION()
	void HandleJourneyClassLevelChanged(UClassDataAsset* ClassAsset, int32 NewLevel);

	UFUNCTION()
	void HandleJourneyEquipmentChanged(EEquipmentSlot InSlot, UEquipmentItem* Item);

	UFUNCTION()
	void HandleJourneyInventoryChanged(int32 SlotIndex, UItemBase* Item);

	UFUNCTION()
	void HandleFailClicked();

	UFUNCTION()
	void HandleRetryRewardClicked();

	UFUNCTION()
	void HandleChoiceOneClicked();

	UFUNCTION()
	void HandleChoiceTwoClicked();
};
