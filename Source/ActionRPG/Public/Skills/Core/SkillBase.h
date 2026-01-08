// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillDataAsset.h"
#include "SkillBase.generated.h"

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

	// Skill Activation
	UFUNCTION(BlueprintCallable, Category = "Skill", meta = (CallInEditor = "true"))
	virtual USkillBase* Activate(AActor* Target = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Skill")
	virtual bool CanActivate(AActor* Target = nullptr) const;

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

	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillActivated, USkillBase*, Skill, AActor*, Target);
	
	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnSkillActivated OnSkillActivated;
};
