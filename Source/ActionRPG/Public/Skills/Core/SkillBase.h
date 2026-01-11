// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Engine.h"
#include "SkillDataAsset.h"
#include "SkillBase.generated.h"

class AActor;

UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API USkillBase : public UObject
{
	GENERATED_BODY()

public:
	USkillBase();

	// Skill Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TObjectPtr<USkillDataAsset> SkillData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float CooldownRemaining;

	// Skill Level and Experience
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (ClampMin = "1"))
	int32 SkillLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill", meta = (ClampMin = "0.0"))
	float Experience;

	// Owner Reference
	UPROPERTY(BlueprintReadOnly, Category = "Skill")
	TObjectPtr<AActor> OwnerActor;

	// Skill Activation
	UFUNCTION(BlueprintCallable, Category = "Skill", meta = (CallInEditor = "true"))
	virtual USkillBase* Activate(AActor* Target = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Skill")
	virtual bool CanActivate(AActor* Target = nullptr) const;

	// Cooldown Management
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void UpdateCooldown(float DeltaTime);

	// Skill Information
	UFUNCTION(BlueprintCallable, Category = "Skill")
	FName GetSkillID() const;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	FText GetSkillName() const;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	ESkillType GetSkillType() const;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	ESkillCategory GetSkillCategory() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	float GetCooldownRemaining() const;

	// Resource Costs (may vary by level)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	float GetManaCost() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	float GetStaminaCost() const;

	// Skill Level and Experience
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	int32 GetSkillLevel() const { return SkillLevel; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skill")
	float GetExperience() const { return Experience; }

	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillActivated, USkillBase*, Skill, AActor*, Target);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCooldownComplete, USkillBase*, Skill);
	
	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnSkillActivated OnSkillActivated;

	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnCooldownComplete OnCooldownComplete;
};
