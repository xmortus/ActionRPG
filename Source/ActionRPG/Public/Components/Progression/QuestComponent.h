// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestComponent.generated.h"

class UInventoryComponent;
class UItemBase;
class UQuestDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStateChanged, UQuestDataAsset*, Quest);

/** Tracks ordinary quests and their rewards independently of UTrialComponent. */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuestComponent();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Quests")
	bool StartQuest(UQuestDataAsset* Quest);

	UFUNCTION(BlueprintCallable, Category = "Quests")
	bool TryTurnInQuest();

	/** Credits only a real, unique enemy death while this quest is active. */
	bool ReportEnemyDefeated(FName EnemyID, AActor* Enemy);

	UFUNCTION(BlueprintPure, Category = "Quests")
	int32 GetRequiredItemCount() const;

	UFUNCTION(BlueprintPure, Category = "Quests")
	int32 GetOptionalDefeatCount() const { return OptionalDefeats; }

	UFUNCTION(BlueprintPure, Category = "Quests")
	bool IsQuestCompleted(const UQuestDataAsset* Quest) const;

	/** Stable-ID lookup for UI flows that outlive the active quest pointer. */
	UFUNCTION(BlueprintPure, Category = "Quests")
	bool IsQuestIDCompleted(FName QuestID) const;

	UFUNCTION(BlueprintPure, Category = "Quests")
	UQuestDataAsset* GetActiveQuest() const { return ActiveQuest; }

	UFUNCTION(BlueprintPure, Category = "Quests")
	FText GetLastNotice() const { return LastNotice; }

	UPROPERTY(BlueprintAssignable, Category = "Quests|Events")
	FOnQuestStateChanged OnQuestChanged;

	UPROPERTY(BlueprintAssignable, Category = "Quests|Events")
	FOnQuestStateChanged OnQuestCompleted;

private:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> Inventory;

	UPROPERTY(VisibleAnywhere, Category = "Quests")
	TObjectPtr<UQuestDataAsset> ActiveQuest;

	UPROPERTY(VisibleAnywhere, Category = "Quests")
	TArray<TObjectPtr<UQuestDataAsset>> CompletedQuests;

	UPROPERTY(VisibleAnywhere, Category = "Quests")
	int32 OptionalDefeats = 0;

	UPROPERTY(VisibleAnywhere, Category = "Quests")
	FText LastNotice;

	TSet<TWeakObjectPtr<AActor>> CreditedEnemies;

	UFUNCTION()
	void HandleInventoryChanged(int32 SlotIndex, UItemBase* Item);

	bool CanPayReward() const;
};
