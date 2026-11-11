// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Progression/AcquiredAttributeComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "GameFramework/Actor.h"

UAcquiredAttributeComponent::UAcquiredAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UAcquiredAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	PrimaryAttributes = GetOwner() ? GetOwner()->FindComponentByClass<UAttributeComponent>() : nullptr;
	if (PrimaryAttributes)
	{
		PrimaryAttributes->OnPrimaryAttributeChanged.AddUniqueDynamic(
			this, &UAcquiredAttributeComponent::HandlePrimaryAttributeChanged);
	}
}

int32 UAcquiredAttributeComponent::GetAcquiredAttribute(EAcquiredAttribute Attribute) const
{
	const int32 Earned = EarnedAttributes.FindRef(Attribute);
	return FMath::Min(FMath::Max(0, Earned), GetAcquiredAttributeCap(Attribute));
}

int32 UAcquiredAttributeComponent::GetAcquiredAttributeCap(EAcquiredAttribute Attribute) const
{
	const UAttributeComponent* Primary = PrimaryAttributes
		? PrimaryAttributes.Get()
		: GetOwner() ? GetOwner()->FindComponentByClass<UAttributeComponent>() : nullptr;
	if (!Primary) return 0;
	if (Attribute == EAcquiredAttribute::Endurance)
	{
		const float Constitution = Primary->GetPersistentAttribute(EPrimaryAttribute::Constitution);
		if (!FMath::IsFinite(Constitution)) return 0;
		if (Constitution >= static_cast<float>(MAX_int32)) return MAX_int32;
		return FMath::FloorToInt(FMath::Max(0.0f, Constitution));
	}
	return 0;
}

bool UAcquiredAttributeComponent::CanGrantAcquiredAttribute(EAcquiredAttribute Attribute, int32 Amount) const
{
	if (Amount <= 0) return false;
	const int32 Earned = FMath::Max(0, EarnedAttributes.FindRef(Attribute));
	const int32 Cap = GetAcquiredAttributeCap(Attribute);
	return Earned <= Cap && Amount <= Cap - Earned;
}

bool UAcquiredAttributeComponent::TryGrantAcquiredAttribute(EAcquiredAttribute Attribute, int32 Amount)
{
	if (!CanGrantAcquiredAttribute(Attribute, Amount)) return false;
	int32& Earned = EarnedAttributes.FindOrAdd(Attribute);
	Earned += Amount;
	OnAcquiredAttributeChanged.Broadcast(Attribute, GetAcquiredAttribute(Attribute), GetAcquiredAttributeCap(Attribute));
	return true;
}

void UAcquiredAttributeComponent::HandlePrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue)
{
	if (Attribute == EPrimaryAttribute::Constitution)
	{
		OnAcquiredAttributeChanged.Broadcast(EAcquiredAttribute::Endurance,
			GetAcquiredAttribute(EAcquiredAttribute::Endurance), GetAcquiredAttributeCap(EAcquiredAttribute::Endurance));
	}
}
