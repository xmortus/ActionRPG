// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Progression/ExperienceComponent.h"

UExperienceComponent::UExperienceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UExperienceComponent::AddExperience(float Amount)
{
	if (!FMath::IsFinite(Amount) || Amount <= 0.0f ||
		!FMath::IsFinite(UnallocatedExperience + Amount) ||
		!FMath::IsFinite(TotalExperience + Amount))
	{
		return;
	}

	UnallocatedExperience += Amount;
	TotalExperience += Amount;
	OnExperienceChanged.Broadcast(UnallocatedExperience, Amount);
}

bool UExperienceComponent::CanSpendExperience(float Amount) const
{
	return FMath::IsFinite(Amount) && Amount > 0.0f && UnallocatedExperience >= Amount;
}

bool UExperienceComponent::SpendExperience(float Amount)
{
	if (!CanSpendExperience(Amount))
	{
		return false;
	}

	UnallocatedExperience -= Amount;
	OnExperienceChanged.Broadcast(UnallocatedExperience, -Amount);
	return true;
}
