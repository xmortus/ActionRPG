// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Core/ItemBase.h"
#include "SkillItem.generated.h"

class USkillBase;
class USkillDataAsset;
class USkillManagerComponent;

/**
 * Skill-granting item that unlocks a skill via SkillManagerComponent.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API USkillItem : public UItemBase
{
	GENERATED_BODY()

public:
	// Item usage
	virtual void Use() override;
	virtual bool CanUse() const override;

protected:
	bool ResolveSkillDefinition(USkillDataAsset*& OutSkillData, TSubclassOf<USkillBase>& OutSkillClass) const;
	USkillManagerComponent* FindSkillManager(AActor*& OutOwnerActor) const;
	bool IsSkillAlreadyUnlocked(USkillManagerComponent* SkillManager, const USkillDataAsset* SkillData, TSubclassOf<USkillBase> SkillClass) const;
	USkillBase* CreateSkillInstance(UObject* Outer, TSubclassOf<USkillBase> SkillClass, USkillDataAsset* SkillData) const;
};
