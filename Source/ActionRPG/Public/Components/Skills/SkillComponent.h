// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Skills/Core/SkillBase.h"
#include "SkillComponent.generated.h"

/**
 * Structure representing a skill instance on a character.
 * Contains skill reference, cooldown, level, experience, and unlock status.
 */
USTRUCT(BlueprintType)
struct ACTIONRPG_API FSkillInstance
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Skill Instance")
	TObjectPtr<USkillBase> Skill;

	UPROPERTY(BlueprintReadWrite, Category = "Skill Instance")
	float CooldownRemaining;

	UPROPERTY(BlueprintReadWrite, Category = "Skill Instance")
	int32 Level;

	UPROPERTY(BlueprintReadWrite, Category = "Skill Instance")
	float Experience;

	UPROPERTY(BlueprintReadWrite, Category = "Skill Instance")
	bool bIsUnlocked;

	FSkillInstance()
		: Skill(nullptr)
		, CooldownRemaining(0.0f)
		, Level(1)
		, Experience(0.0f)
		, bIsUnlocked(false)
	{}
};

/**
 * Skill Component for managing character skills.
 * Handles skill instances, cooldowns, leveling, experience, and skill activation.
 * Supports skill granting, removal, and progression tracking.
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent(const FObjectInitializer& ObjectInitializer);

	// Component lifecycle
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Skill Management
	UFUNCTION(BlueprintCallable, Category = "Skill Component")
	bool GrantSkill(USkillBase* Skill);

	UFUNCTION(BlueprintCallable, Category = "Skill Component")
	bool RemoveSkill(USkillBase* Skill);

	UFUNCTION(BlueprintCallable, Category = "Skill Component")
	bool ActivateSkill(USkillBase* Skill, AActor* Target = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Skill Component")
	bool CanActivateSkill(USkillBase* Skill) const;

	// Skill Queries
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Component")
	int32 GetSkillLevel(USkillBase* Skill) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Component")
	float GetSkillExperience(USkillBase* Skill) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Component")
	bool IsSkillUnlocked(USkillBase* Skill) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Component")
	float GetSkillCooldownRemaining(USkillBase* Skill) const;

	// Get skill instance data (Blueprint-compatible - returns by value)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Component")
	FSkillInstance GetSkillInstanceData(USkillBase* Skill) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Component")
	const TArray<FSkillInstance>& GetActiveSkills() const { return ActiveSkills; }

	// Get all skills the player has
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Component")
	TArray<USkillBase*> GetSkills() const;

	// Get all unlocked skills
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Component")
	TArray<USkillBase*> GetUnlockedSkills() const;

	// Skill Experience
	UFUNCTION(BlueprintCallable, Category = "Skill Component")
	void AddSkillExperience(USkillBase* Skill, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Skill Component")
	float CalculateExperienceGain(USkillBase* Skill, float BaseAmount) const;

	// Component Configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Component")
	bool bShouldTickForCooldowns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Component", meta = (ClampMin = "0.0"))
	float ExperienceMultiplier;

	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillActivated, USkillBase*, Skill, AActor*, Target);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillCooldownChanged, USkillBase*, Skill);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillLevelUp, USkillBase*, Skill, int32, NewLevel);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSkillExperienceGained, USkillBase*, Skill, float, Amount, int32, NewLevel);

	UPROPERTY(BlueprintAssignable, Category = "Skill Component")
	FOnSkillActivated OnSkillActivated;

	UPROPERTY(BlueprintAssignable, Category = "Skill Component")
	FOnSkillCooldownChanged OnSkillCooldownChanged;

	UPROPERTY(BlueprintAssignable, Category = "Skill Component")
	FOnSkillLevelUp OnSkillLevelUp;

	UPROPERTY(BlueprintAssignable, Category = "Skill Component")
	FOnSkillExperienceGained OnSkillExperienceGained;

protected:
	// Skill Instances
	UPROPERTY(BlueprintReadOnly, Category = "Skill Component")
	TArray<FSkillInstance> ActiveSkills;

	// Update cooldowns for all skills
	void UpdateCooldowns(float DeltaTime);

	// Check and handle skill level ups
	void CheckSkillLevelUp(FSkillInstance& SkillInstance, float ExperienceAdded);

	// Calculate XP required for next level
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill Component")
	float GetExperienceRequiredForLevel(int32 CurrentLevel) const;

	// Validate and deduct resource costs
	bool ValidateResourceCosts(USkillBase* Skill) const;
	void DeductResourceCosts(USkillBase* Skill);

private:
	// Helper to find skill instance index
	int32 FindSkillInstanceIndex(USkillBase* Skill) const;

	// Helper to find skill instance
	FSkillInstance* FindSkillInstance(USkillBase* Skill);
	const FSkillInstance* FindSkillInstance(USkillBase* Skill) const;

	// Get skill instance (C++ only - returns pointer, for internal use)
	FSkillInstance* GetSkillInstance(USkillBase* Skill);
	const FSkillInstance* GetSkillInstance(USkillBase* Skill) const;
};
