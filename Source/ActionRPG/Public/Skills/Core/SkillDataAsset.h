// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillTypes.h"
#include "SkillDataAsset.generated.h"

UCLASS(BlueprintType)
class ACTIONRPG_API USkillDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USkillDataAsset();

	// Primary Data Asset Interface
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	// Skill Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FPrimaryAssetType SkillType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FName SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	FText SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (MultiLine = true))
	FText SkillDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TObjectPtr<UTexture2D> SkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	ESkillType Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	ESkillCategory Category;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	float CooldownDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	float ManaCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	float StaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	float CastTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	float Range;
};
