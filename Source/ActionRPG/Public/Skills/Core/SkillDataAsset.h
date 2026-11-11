// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillTypes.h"
#include "Progression/Core/ProgressionTypes.h"
#include "SkillDataAsset.generated.h"

USTRUCT(BlueprintType)
struct ACTIONRPG_API FSkillAttributeMilestone
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Progression", meta = (ClampMin = "1"))
	int32 SkillLevel = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Progression")
	EPrimaryAttribute Attribute = EPrimaryAttribute::Agility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Progression", meta = (ClampMin = "0.0"))
	float Amount = 1.0f;
};

/**
 * Data Asset for defining skill properties.
 * Used by the Skill System to store static skill data (name, description, cooldown, costs, etc.).
 * Can be created and edited in Blueprint.
 */
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "0.0"))
	float CooldownDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "0.0"))
	float ManaCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "0.0"))
	float StaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "0.0"))
	float CastTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "0.0"))
	float Range;

	// Requirements
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill", meta = (ClampMin = "1"))
	int32 RequiredLevel;

	/** Minimum persistent attributes needed to learn this skill. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Requirements")
	TMap<EPrimaryAttribute, float> RequiredAttributes;

	/** Optional selected class and its minimum class level. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Requirements")
	TObjectPtr<class UClassDataAsset> RequiredClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Requirements", meta = (ClampMin = "1"))
	int32 RequiredClassLevel = 1;

	/** Every listed skill ID must already be unlocked. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Requirements")
	TArray<FName> PrerequisiteSkillIDs;

	/** Cost when learned from the point catalog. Other skill grants remain free. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Requirements", meta = (ClampMin = "1"))
	int32 SkillPointCost = 1;

	/** Rare permanent attribute gains at exact use-based skill levels. Empty for ordinary skills. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Progression")
	TArray<FSkillAttributeMilestone> AttributeMilestones;

	// Damage/Effect Values (base values, can be modified by skill level)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Combat", meta = (ClampMin = "0.0"))
	float BaseDamage;

	/** Extra melee damage per effective Strength point. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Combat", meta = (ClampMin = "0.0"))
	float MeleeDamagePerStrength = 0.5f;

	/** Extra ranged projectile damage per effective Dexterity point. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Combat", meta = (ClampMin = "0.0"))
	float RangedDamagePerDexterity = 0.5f;

	/** Extra magic projectile damage per effective Intelligence point. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Combat", meta = (ClampMin = "0.0"))
	float MagicDamagePerIntelligence = 0.75f;

	/** Extra offensive damage per use-based skill level above one; zero disables level scaling. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Combat", meta = (ClampMin = "0.0"))
	float DamagePercentPerSkillLevel = 0.0f;

	/** Upper bound on damage gained from skill levels. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Combat", meta = (ClampMin = "1.0"))
	float MaxSkillDamageMultiplier = 2.0f;

	/** Melee attack speed gained per effective Agility point, as a percentage. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Combat", meta = (ClampMin = "0.0"))
	float MeleeSpeedPercentPerAgility = 1.0f;

	/** Upper bound on the Agility speed multiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Combat", meta = (ClampMin = "1.0"))
	float MaxMeleeSpeedMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Combat")
	EDamageType DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Combat", meta = (ClampMin = "0.0"))
	float AreaOfEffectRadius;

	// Projectile Properties (for ranged/magic skills)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Projectile", meta = (ClampMin = "0.0"))
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Projectile")
	TSubclassOf<class AActor> ProjectileClass;

	// Movement Properties (for movement/dash skills)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Movement", meta = (ClampMin = "0.0"))
	float AbilityMoveSpeed;

	/** Dash speed bonus per skill level above one. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Movement", meta = (ClampMin = "0.0"))
	float DashSpeedPercentPerLevel = 5.0f;

	/** Maximum multiplier for Dash speed gained from skill levels. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Movement", meta = (ClampMin = "1.0"))
	float MaxDashSpeedMultiplier = 2.0f;

	/** Stamina cost removed per Dash skill level above one. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Movement", meta = (ClampMin = "0.0"))
	float DashStaminaReductionPerLevel = 0.5f;

	/** Dash stamina cost cannot fall below this value or its base cost, whichever is lower. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Movement", meta = (ClampMin = "0.0"))
	float MinDashStaminaCost = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill|Movement", meta = (ClampMin = "0.0"))
	float InvincibilityFrames;
};
