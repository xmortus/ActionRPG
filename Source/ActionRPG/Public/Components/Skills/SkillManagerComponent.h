// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Skills/Core/SkillBase.h"
#include "SkillManagerComponent.generated.h"

class USkillComponent;
class USkillDataAsset;

/** One selectable skill in the player's point-spend catalog. */
USTRUCT(BlueprintType)
struct ACTIONRPG_API FSkillCatalogEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Catalog")
	TSubclassOf<USkillBase> SkillClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Catalog")
	TObjectPtr<USkillDataAsset> SkillData;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillPointsChanged, int32, NewBalance, int32, Delta);

/**
 * Skill Manager Component for managing skill unlocks and skill bar assignments.
 * Handles skill unlocking, skill bar slot management (8 slots), and skill activation from hotkeys.
 * Integrates with SkillComponent for actual skill execution and cooldown management.
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API USkillManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillManagerComponent(const FObjectInitializer& ObjectInitializer);

	// Component lifecycle
	virtual void BeginPlay() override;

	/** Free skill points are a separate special-reward balance, not unlock XP. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager|Points")
	int32 GetUnspentSkillPoints() const { return UnspentSkillPoints; }

	UFUNCTION(BlueprintCallable, Category = "Skill Manager|Points")
	bool AddSkillPoints(int32 Points);

	UFUNCTION(BlueprintCallable, Category = "Skill Manager|Points")
	bool SpendSkillPoints(int32 Points);

	/** Skills available for purchase with free skill points. Set on player component defaults. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Manager|Catalog")
	TArray<FSkillCatalogEntry> SkillCatalog;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager|Catalog")
	bool IsCatalogSkillUnlocked(int32 CatalogIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager|Catalog")
	bool CanLearnCatalogSkill(int32 CatalogIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Skill Manager|Catalog")
	bool LearnCatalogSkill(int32 CatalogIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager|Catalog")
	FText GetCatalogSkillStatus(int32 CatalogIndex) const;

	UPROPERTY(BlueprintAssignable, Category = "Skill Manager|Points")
	FOnSkillPointsChanged OnSkillPointsChanged;


	// Skill Unlock Management
	UFUNCTION(BlueprintCallable, Category = "Skill Manager")
	bool UnlockSkill(USkillBase* Skill);

	/** Skill stones grant their named skill without catalog entry requirements. */
	bool UnlockSkillFromStone(USkillBase* Skill);

	UFUNCTION(BlueprintCallable, Category = "Skill Manager")
	bool RevokeSkill(USkillBase* Skill);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager")
	bool IsSkillUnlocked(USkillBase* Skill) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager")
	bool IsSkillIDUnlocked(FName SkillID) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager")
	TArray<USkillBase*> GetUnlockedSkills() const;

	// Skill Bar Management (8 slots: 0-7 map to hotkeys 1-8)
	UFUNCTION(BlueprintCallable, Category = "Skill Manager")
	bool AssignSkillToSlot(int32 SlotIndex, USkillBase* Skill);

	UFUNCTION(BlueprintCallable, Category = "Skill Manager")
	bool RemoveSkillFromSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Skill Manager")
	void ClearSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager")
	USkillBase* GetSkillAtSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager")
	bool IsSlotOccupied(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager")
	TMap<int32, USkillBase*> GetAllSlotAssignments() const;

	// Main/Offhand Action Slots
	UFUNCTION(BlueprintCallable, Category = "Skill Manager|Actions")
	bool AssignMainHandSkill(USkillBase* Skill);

	UFUNCTION(BlueprintCallable, Category = "Skill Manager|Actions")
	bool AssignOffhandSkill(USkillBase* Skill);

	UFUNCTION(BlueprintCallable, Category = "Skill Manager|Actions")
	void ClearMainHandSkill();

	UFUNCTION(BlueprintCallable, Category = "Skill Manager|Actions")
	void ClearOffhandSkill();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager|Actions")
	USkillBase* GetMainHandSkill() const { return MainHandSkill; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager|Actions")
	USkillBase* GetOffhandSkill() const { return OffhandSkill; }

	UFUNCTION(BlueprintCallable, Category = "Skill Manager|Actions")
	bool ActivateMainHandSkill(AActor* Target = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Skill Manager|Actions")
	bool ActivateOffhandSkill(AActor* Target = nullptr);

	// Skill Activation from Slots
	UFUNCTION(BlueprintCallable, Category = "Skill Manager")
	bool ActivateSkillFromSlot(int32 SlotIndex, AActor* Target = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager")
	bool CanActivateSkillFromSlot(int32 SlotIndex) const;

	// Skill Prerequisites Validation
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager")
	bool CanUnlockSkill(USkillBase* Skill) const;

	// Component Reference
	UPROPERTY(BlueprintReadOnly, Category = "Skill Manager")
	TObjectPtr<USkillComponent> SkillComponent;

	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillUnlocked, USkillBase*, Skill);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillRevoked, USkillBase*, Skill);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillSlotChanged, int32, SlotIndex, USkillBase*, Skill);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillSlotCleared, int32, SlotIndex);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMainHandSkillChanged, USkillBase*, Skill);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOffhandSkillChanged, USkillBase*, Skill);

	UPROPERTY(BlueprintAssignable, Category = "Skill Manager")
	FOnSkillUnlocked OnSkillUnlocked;

	UPROPERTY(BlueprintAssignable, Category = "Skill Manager")
	FOnSkillRevoked OnSkillRevoked;

	UPROPERTY(BlueprintAssignable, Category = "Skill Manager")
	FOnSkillSlotChanged OnSkillSlotChanged;

	UPROPERTY(BlueprintAssignable, Category = "Skill Manager")
	FOnSkillSlotCleared OnSkillSlotCleared;

	UPROPERTY(BlueprintAssignable, Category = "Skill Manager|Actions")
	FOnMainHandSkillChanged OnMainHandSkillChanged;

	UPROPERTY(BlueprintAssignable, Category = "Skill Manager|Actions")
	FOnOffhandSkillChanged OnOffhandSkillChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill Manager|Points")
	int32 UnspentSkillPoints = 0;

	// Unlocked Skills (skills the player has unlocked)
	UPROPERTY(BlueprintReadOnly, Category = "Skill Manager")
	TArray<TObjectPtr<USkillBase>> UnlockedSkills;

	// Skill Bar Slots (0-7 map to hotkeys 1-8)
	UPROPERTY(BlueprintReadOnly, Category = "Skill Manager")
	TMap<int32, TObjectPtr<USkillBase>> SkillBarSlots;

	// Main/Offhand action slots (triggered by IA_Attack/IA_Offhand)
	UPROPERTY(BlueprintReadOnly, Category = "Skill Manager|Actions")
	TObjectPtr<USkillBase> MainHandSkill;

	UPROPERTY(BlueprintReadOnly, Category = "Skill Manager|Actions")
	TObjectPtr<USkillBase> OffhandSkill;

	// Find SkillComponent on owner
	USkillComponent* FindSkillComponent() const;

	// Validate slot index (0-7 for slots 1-8)
	bool IsValidSlotIndex(int32 SlotIndex) const;

	// Check skill prerequisites
	bool CheckSkillPrerequisites(USkillBase* Skill) const;
	FString GetRequirementFailure(const USkillDataAsset* SkillData) const;
	bool UnlockSkillInternal(USkillBase* Skill, bool bCheckPrerequisites);

};
