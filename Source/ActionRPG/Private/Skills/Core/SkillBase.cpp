// Copyright Epic Games, Inc. All Rights Reserved.

#include "Skills/Core/SkillBase.h"

USkillBase::USkillBase()
{
	SkillData = nullptr;
	CooldownRemaining = 0.0f;
}

USkillBase* USkillBase::Activate(AActor* Target)
{
	if (!CanActivate(Target))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillBase::Activate - Skill cannot be activated"));
		return nullptr;
	}

	// Set cooldown
	if (SkillData)
	{
		CooldownRemaining = SkillData->CooldownDuration;
		UE_LOG(LogTemp, Log, TEXT("SkillBase::Activate - Cooldown set: %.2f (from SkillData->CooldownDuration: %.2f)"), 
			CooldownRemaining, SkillData->CooldownDuration);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillBase::Activate - SkillData is NULL, cannot set cooldown!"));
	}

	// Broadcast event
	OnSkillActivated.Broadcast(this, Target);

	UE_LOG(LogTemp, Log, TEXT("SkillBase::Activate - Skill activated: %s (Target: %s), CooldownRemaining: %.2f"), 
		SkillData ? *SkillData->SkillName.ToString() : TEXT("NULL"),
		Target ? *Target->GetName() : TEXT("None"),
		CooldownRemaining);

	// Return self to allow chaining in Blueprint
	return this;
}

bool USkillBase::CanActivate(AActor* Target) const
{
	if (!SkillData)
	{
		return false;
	}

	// Check cooldown
	if (CooldownRemaining > 0.0f)
	{
		return false;
	}

	// Base validation - can be overridden in derived classes
	// Additional checks (mana, range, etc.) can be added here or in derived classes
	return true;
}

FName USkillBase::GetSkillID() const
{
	return SkillData ? SkillData->SkillID : NAME_None;
}

FText USkillBase::GetSkillName() const
{
	return SkillData ? SkillData->SkillName : FText::GetEmpty();
}

ESkillType USkillBase::GetSkillType() const
{
	return SkillData ? SkillData->Type : ESkillType::Utility;
}

ESkillCategory USkillBase::GetSkillCategory() const
{
	return SkillData ? SkillData->Category : ESkillCategory::Combat;
}

float USkillBase::GetCooldownRemaining() const
{
	return CooldownRemaining;
}
