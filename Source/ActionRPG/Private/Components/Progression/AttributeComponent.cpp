// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Progression/AttributeComponent.h"
#include "Progression/Core/PrimaryAttributeDataAsset.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Items/Equipment/EquipmentItem.h"

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
	if (!EquipmentComponent) EquipmentComponent = GetOwner() ? GetOwner()->FindComponentByClass<UEquipmentComponent>() : nullptr;
	if (EquipmentComponent)
		EquipmentComponent->OnEquipmentChanged.AddUniqueDynamic(this, &UAttributeComponent::OnEquipmentChanged);
	BaseAttributes.Empty();

	if (AttributeDataAsset)
	{
		for (const TPair<EPrimaryAttribute, float>& Pair : AttributeDataAsset->BaseValues)
		{
			if (FMath::IsFinite(Pair.Value)) BaseAttributes.Add(Pair.Key, Pair.Value);
		}
	}

	RefreshEquipmentSources();
	RecalculateAttributes();
}

float UAttributeComponent::GetAttribute(EPrimaryAttribute Attribute) const
{
	if (const float* Value = Attributes.Find(Attribute))
	{
		return *Value;
	}

	return 0.0f;
}

float UAttributeComponent::GetPersistentAttribute(EPrimaryAttribute Attribute) const
{
	return CalculateAttribute(Attribute, true);
}

bool UAttributeComponent::SetModifierSource(FName SourceId, const TMap<EPrimaryAttribute, float>& Bonuses, bool bPersistent)
{
	if (SourceId.IsNone()) return false;
	for (const TPair<EPrimaryAttribute, float>& Bonus : Bonuses)
	{
		if (!FMath::IsFinite(Bonus.Value)) return false;
	}
	if (Bonuses.IsEmpty()) return RemoveModifierSource(SourceId);
	FModifierSource& Source = ModifierSources.FindOrAdd(SourceId);
	Source.Bonuses = Bonuses;
	Source.bPersistent = bPersistent;
	RecalculateAttributes();
	return true;
}

bool UAttributeComponent::RemoveModifierSource(FName SourceId)
{
	if (ModifierSources.Remove(SourceId) == 0) return false;
	RecalculateAttributes();
	return true;
}

float UAttributeComponent::GetModifierSourceBonus(FName SourceId, EPrimaryAttribute Attribute) const
{
	const FModifierSource* Source = ModifierSources.Find(SourceId);
	return Source ? Source->Bonuses.FindRef(Attribute) : 0.0f;
}

FAttributeBreakdown UAttributeComponent::GetAttributeBreakdown(EPrimaryAttribute Attribute) const
{
	FAttributeBreakdown Breakdown;
	Breakdown.Base = BaseAttributes.FindRef(Attribute);
	Breakdown.Allocated = AllocatedAttributes.FindRef(Attribute);
	float RawTotal = Breakdown.Base + Breakdown.Allocated;
	TArray<FName> SourceIds;
	ModifierSources.GetKeys(SourceIds);
	SourceIds.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });
	for (const FName SourceId : SourceIds)
	{
		const FModifierSource& Source = ModifierSources.FindChecked(SourceId);
		const float Amount = Source.Bonuses.FindRef(Attribute);
		if (FMath::IsNearlyZero(Amount)) continue;
		FAttributeSourceContribution& Contribution = Breakdown.Sources.AddDefaulted_GetRef();
		Contribution.SourceId = SourceId;
		Contribution.Amount = Amount;
		Contribution.bPersistent = Source.bPersistent;
		RawTotal += Amount;
	}
	Breakdown.FinalTotal = ClampAttributeValue(RawTotal);
	Breakdown.LimitAdjustment = Breakdown.FinalTotal - RawTotal;
	return Breakdown;
}

void UAttributeComponent::SetAttribute(EPrimaryAttribute Attribute, float NewValue)
{
	if (!FMath::IsFinite(NewValue)) return;
	BaseAttributes.Add(Attribute, NewValue);
	RecalculateAttributes();
}

void UAttributeComponent::ModifyAttribute(EPrimaryAttribute Attribute, float Delta)
{
	if (!FMath::IsFinite(Delta)) return;
	const float NewValue = BaseAttributes.FindRef(Attribute) + Delta;
	if (!FMath::IsFinite(NewValue)) return;
	BaseAttributes.Add(Attribute, NewValue);
	RecalculateAttributes();
}

void UAttributeComponent::SetClassBonuses(const TMap<EPrimaryAttribute, float>& Bonuses)
{
	SetModifierSource(TEXT("Class.Legacy"), Bonuses, true);
}

void UAttributeComponent::SetClassProgressionBonuses(const TMap<EPrimaryAttribute, float>& SelectedBonuses,
	const TMap<EPrimaryAttribute, float>& EarnedBonuses)
{
	auto ReplaceSource = [this](FName SourceId, const TMap<EPrimaryAttribute, float>& Bonuses, bool bPersistent)
	{
		if (Bonuses.IsEmpty()) { ModifierSources.Remove(SourceId); return; }
		for (const TPair<EPrimaryAttribute, float>& Bonus : Bonuses)
		{
			if (!FMath::IsFinite(Bonus.Value)) return;
		}
		FModifierSource& Source = ModifierSources.FindOrAdd(SourceId);
		Source.Bonuses = Bonuses;
		Source.bPersistent = bPersistent;
	};
	ReplaceSource(TEXT("Class.Selected"), SelectedBonuses, false);
	ReplaceSource(TEXT("Class.Earned"), EarnedBonuses, true);
	RecalculateAttributes();
}

void UAttributeComponent::AddAttributePoints(int32 Points)
{
	if (Points <= 0)
	{
		return;
	}

	UnspentAttributePoints += Points;
}

bool UAttributeComponent::AllocateAttributePoints(EPrimaryAttribute Attribute, int32 Points)
{
	if (Points <= 0 || UnspentAttributePoints < Points)
	{
		return false;
	}

	UnspentAttributePoints -= Points;
	AllocatedAttributes.FindOrAdd(Attribute) += Points;
	RecalculateAttributes();
	return true;
}

float UAttributeComponent::ClampAttributeValue(float Value) const
{
	const float MinValue = AttributeDataAsset ? AttributeDataAsset->MinValue : 0.0f;
	const float MaxValue = AttributeDataAsset ? AttributeDataAsset->MaxValue : 999.0f;
	return FMath::Clamp(Value, MinValue, MaxValue);
}

void UAttributeComponent::OnEquipmentChanged(EEquipmentSlot Slot, UEquipmentItem* Item)
{
	RefreshEquipmentSources();
	RecalculateAttributes();
}

void UAttributeComponent::RefreshEquipmentSources()
{
	for (const FName SourceId : EquipmentSourceIds) ModifierSources.Remove(SourceId);
	EquipmentSourceIds.Empty();
	if (!EquipmentComponent) return;

	const TMap<EEquipmentSlot, UEquipmentItem*> EquippedItems = EquipmentComponent->GetEquippedItems();
	for (const TPair<EEquipmentSlot, UEquipmentItem*>& Pair : EquippedItems)
	{
		const UEquipmentItem* EquippedItem = Pair.Value;
		if (!EquippedItem)
		{
			continue;
		}

		const TMap<EPrimaryAttribute, float>& Bonuses = EquippedItem->GetPrimaryAttributeBonuses();
		bool bValid = true;
		for (const TPair<EPrimaryAttribute, float>& Bonus : Bonuses)
		{
			if (!FMath::IsFinite(Bonus.Value)) { bValid = false; break; }
		}
		if (!bValid || Bonuses.IsEmpty()) continue;
		const FName SourceId(*FString::Printf(TEXT("Equipment.%d"), static_cast<int32>(Pair.Key)));
		FModifierSource& Source = ModifierSources.FindOrAdd(SourceId);
		Source.Bonuses = Bonuses;
		Source.bPersistent = false;
		EquipmentSourceIds.Add(SourceId);
	}
}

float UAttributeComponent::CalculateAttribute(EPrimaryAttribute Attribute, bool bPersistentOnly) const
{
	float Value = BaseAttributes.FindRef(Attribute) + AllocatedAttributes.FindRef(Attribute);
	TArray<FName> SourceIds;
	ModifierSources.GetKeys(SourceIds);
	SourceIds.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });
	for (const FName SourceId : SourceIds)
	{
		const FModifierSource& Source = ModifierSources.FindChecked(SourceId);
		if (!bPersistentOnly || Source.bPersistent) Value += Source.Bonuses.FindRef(Attribute);
	}
	return ClampAttributeValue(Value);
}

void UAttributeComponent::RecalculateAttributes()
{
	const TMap<EPrimaryAttribute, float> PreviousAttributes = Attributes;
	TSet<EPrimaryAttribute> Keys;
	for (const TPair<EPrimaryAttribute, float>& Pair : BaseAttributes) Keys.Add(Pair.Key);
	for (const TPair<EPrimaryAttribute, float>& Pair : AllocatedAttributes) Keys.Add(Pair.Key);
	for (const TPair<FName, FModifierSource>& Source : ModifierSources)
		for (const TPair<EPrimaryAttribute, float>& Bonus : Source.Value.Bonuses) Keys.Add(Bonus.Key);
	for (const TPair<EPrimaryAttribute, float>& Pair : PreviousAttributes) Keys.Add(Pair.Key);
	Attributes.Empty();
	for (const EPrimaryAttribute Attribute : Keys)
	{
		const float NewValue = CalculateAttribute(Attribute, false);
		Attributes.Add(Attribute, NewValue);
		const float OldValue = PreviousAttributes.FindRef(Attribute);
		if (!FMath::IsNearlyEqual(OldValue, NewValue))
		{
			OnPrimaryAttributeChanged.Broadcast(Attribute, NewValue, OldValue);
		}
	}
}
