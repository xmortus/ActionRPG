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
}

float USecondaryAttributeComponent::GetSecondaryAttribute(ESecondaryAttribute Attribute) const
{
	if (const float* Value = SecondaryAttributes.Find(Attribute))
	{
		return *Value;
	}

	return 0.0f;
}

void USecondaryAttributeComponent::OnPrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue)
{
	RecalculateSecondaryAttributes();
}
