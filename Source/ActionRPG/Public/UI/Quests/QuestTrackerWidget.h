// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestTrackerWidget.generated.h"

class UQuestComponent;
class UQuestDataAsset;
class USizeBox;
class UTextBlock;

/** Small event-driven HUD card for a quest, separate from the Trial tracker. */
UCLASS()
class ACTIONRPG_API UQuestTrackerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindQuestComponent(UQuestComponent* InQuests);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY()
	TObjectPtr<UQuestComponent> Quests;

	UPROPERTY()
	TObjectPtr<UQuestDataAsset> LastCompletedQuest;

	UPROPERTY()
	TObjectPtr<USizeBox> PanelSize;

	UPROPERTY()
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY()
	TObjectPtr<UTextBlock> ObjectiveText;

	UPROPERTY()
	TObjectPtr<UTextBlock> RewardText;

	void Refresh();

	UFUNCTION()
	void HandleQuestChanged(UQuestDataAsset* Quest);

	UFUNCTION()
	void HandleQuestCompleted(UQuestDataAsset* Quest);
};
