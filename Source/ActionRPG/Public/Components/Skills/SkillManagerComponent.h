// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Skills/Core/SkillBase.h"
#include "SkillManagerComponent.generated.h"

class USkillComponent;

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

	// Skill Unlock Management
	UFUNCTION(BlueprintCallable, Category = "Skill Manager")
	bool UnlockSkill(USkillBase* Skill);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Manager")
	bool IsSkillUnlocked(USkillBase* Skill) const;

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
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillSlotChanged, int32, SlotIndex, USkillBase*, Skill);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillSlotCleared, int32, SlotIndex);

	UPROPERTY(BlueprintAssignable, Category = "Skill Manager")
	FOnSkillUnlocked OnSkillUnlocked;

	UPROPERTY(BlueprintAssignable, Category = "Skill Manager")
	FOnSkillSlotChanged OnSkillSlotChanged;

	UPROPERTY(BlueprintAssignable, Category = "Skill Manager")
	FOnSkillSlotCleared OnSkillSlotCleared;

protected:
	// Unlocked Skills (skills the player has unlocked)
	UPROPERTY(BlueprintReadOnly, Category = "Skill Manager")
	TArray<TObjectPtr<USkillBase>> UnlockedSkills;

	// Skill Bar Slots (0-7 map to hotkeys 1-8)
	UPROPERTY(BlueprintReadOnly, Category = "Skill Manager")
	TMap<int32, TObjectPtr<USkillBase>> SkillBarSlots;

	// Find SkillComponent on owner
	USkillComponent* FindSkillComponent() const;

	// Validate slot index (0-7 for slots 1-8)
	bool IsValidSlotIndex(int32 SlotIndex) const;

	// Check skill prerequisites
	bool CheckSkillPrerequisites(USkillBase* Skill) const;

	// Check attribute requirements (stub for Phase 4)
	bool CheckAttributeRequirements(USkillBase* Skill) const;

	// Check level requirements
	bool CheckLevelRequirements(USkillBase* Skill) const;

	// Check prerequisite skill requirements (stub for now)
	bool CheckPrerequisiteSkills(USkillBase* Skill) const;

	// Check class requirements (stub for Phase 4)
	bool CheckClassRequirements(USkillBase* Skill) const;
};
