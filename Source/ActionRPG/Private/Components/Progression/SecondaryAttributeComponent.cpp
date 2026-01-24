// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Progression/Core/SecondaryAttributeDataAsset.h"

USecondaryAttributeComponent::USecondaryAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USecondaryAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	AttributeComponent = GetOwner() ? GetOwner()->FindComponentByClass<UAttributeComponent>() : nullptr;
	if (AttributeComponent)
	{
		AttributeComponent->OnPrimaryAttributeChanged.AddDynamic(this, &USecondaryAttributeComponent::OnPrimaryAttributeChanged);
	}

	RecalculateSecondaryAttributes();
}

void USecondaryAttributeComponent::RecalculateSecondaryAttributes()
{
	SecondaryAttributes.Empty();

	if (!SecondaryAttributeDataAsset || !AttributeComponent)
	{
		return;
	}

	for (const TPair<ESecondaryAttribute, FSecondaryAttributeFormula>& Pair : SecondaryAttributeDataAsset->Formulas)
	{
		const FSecondaryAttributeFormula& Formula = Pair.Value;
		float Value = Formula.BaseValue;

		for (const TPair<EPrimaryAttribute, float>& Scaling : Formula.AttributeScaling)
		{
			const float PrimaryValue = AttributeComponent->GetAttribute(Scaling.Key);
			Value += PrimaryValue * Scaling.Value;
		}

		SecondaryAttributes.Add(Pair.Key, Value);
		OnSecondaryAttributeChanged.Broadcast(Pair.Key, Value);
	}

	InitializeCurrentFromMax();
}

float USecondaryAttributeComponent::GetSecondaryAttribute(ESecondaryAttribute Attribute) const
{
	if (const float* Value = SecondaryAttributes.Find(Attribute))
	{
		return *Value;
	}

	return 0.0f;
}

float USecondaryAttributeComponent::GetCurrentSecondaryAttribute(ESecondaryAttribute Attribute) const
{
	if (const float* Value = CurrentSecondaryAttributes.Find(Attribute))
	{
		return *Value;
	}

	return 0.0f;
}

void USecondaryAttributeComponent::SetCurrentSecondaryAttribute(ESecondaryAttribute Attribute, float NewValue)
{
	const float MaxValue = GetSecondaryAttribute(Attribute);
	const float ClampedValue = MaxValue > 0.0f ? FMath::Clamp(NewValue, 0.0f, MaxValue) : FMath::Max(0.0f, NewValue);
	const float OldValue = GetCurrentSecondaryAttribute(Attribute);

	if (!FMath::IsNearlyEqual(OldValue, ClampedValue))
	{
		CurrentSecondaryAttributes.Add(Attribute, ClampedValue);
		OnCurrentSecondaryAttributeChanged.Broadcast(Attribute, ClampedValue, OldValue);
	}
}

void USecondaryAttributeComponent::ModifyCurrentSecondaryAttribute(ESecondaryAttribute Attribute, float Delta)
{
	SetCurrentSecondaryAttribute(Attribute, GetCurrentSecondaryAttribute(Attribute) + Delta);
}

void USecondaryAttributeComponent::OnPrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue)
{
	RecalculateSecondaryAttributes();
}

void USecondaryAttributeComponent::InitializeCurrentFromMax()
{
	TMap<ESecondaryAttribute, float> PreviousValues = CurrentSecondaryAttributes;
	CurrentSecondaryAttributes.Empty();

	for (const TPair<ESecondaryAttribute, float>& Pair : SecondaryAttributes)
	{
		const float MaxValue = Pair.Value;
		const float OldValue = PreviousValues.Contains(Pair.Key) ? PreviousValues[Pair.Key] : MaxValue;
		const float ClampedValue = FMath::Clamp(OldValue, 0.0f, MaxValue);

		CurrentSecondaryAttributes.Add(Pair.Key, ClampedValue);

		if (!FMath::IsNearlyEqual(OldValue, ClampedValue) || !PreviousValues.Contains(Pair.Key))
		{
			OnCurrentSecondaryAttributeChanged.Broadcast(Pair.Key, ClampedValue, OldValue);
		}
	}
}
