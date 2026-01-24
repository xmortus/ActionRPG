// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Progression/AttributeComponent.h"
#include "Progression/Core/PrimaryAttributeDataAsset.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeFromDataAsset();
}

void UAttributeComponent::InitializeFromDataAsset()
{
	Attributes.Empty();

	if (AttributeDataAsset)
	{
		for (const TPair<EPrimaryAttribute, float>& Pair : AttributeDataAsset->BaseValues)
		{
			Attributes.Add(Pair.Key, ClampAttributeValue(Pair.Value));
		}
	}
}

float UAttributeComponent::GetAttribute(EPrimaryAttribute Attribute) const
{
	if (const float* Value = Attributes.Find(Attribute))
	{
		return *Value;
	}

	return 0.0f;
}

void UAttributeComponent::SetAttribute(EPrimaryAttribute Attribute, float NewValue)
{
	const float ClampedValue = ClampAttributeValue(NewValue);
	const float OldValue = GetAttribute(Attribute);

	if (!FMath::IsNearlyEqual(OldValue, ClampedValue))
	{
		Attributes.Add(Attribute, ClampedValue);
		OnPrimaryAttributeChanged.Broadcast(Attribute, ClampedValue, OldValue);
	}
}

void UAttributeComponent::ModifyAttribute(EPrimaryAttribute Attribute, float Delta)
{
	SetAttribute(Attribute, GetAttribute(Attribute) + Delta);
}

float UAttributeComponent::ClampAttributeValue(float Value) const
{
	const float MinValue = AttributeDataAsset ? AttributeDataAsset->MinValue : 0.0f;
	const float MaxValue = AttributeDataAsset ? AttributeDataAsset->MaxValue : 999.0f;
	return FMath::Clamp(Value, MinValue, MaxValue);
}
