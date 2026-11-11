// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/AcquiredAttributeComponent.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Progression/Core/SecondaryAttributeDataAsset.h"
#include "Engine/World.h"
#include "Characters/ActionRPGPlayerCharacter.h"

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
	AcquiredAttributeComponent = GetOwner() ? GetOwner()->FindComponentByClass<UAcquiredAttributeComponent>() : nullptr;
	if (AcquiredAttributeComponent)
	{
		AcquiredAttributeComponent->OnAcquiredAttributeChanged.AddUniqueDynamic(
			this, &USecondaryAttributeComponent::OnAcquiredAttributeChanged);
	}

	EquipmentComponent = GetOwner() ? GetOwner()->FindComponentByClass<UEquipmentComponent>() : nullptr;
	if (EquipmentComponent)
	{
		EquipmentComponent->OnEquipmentChanged.AddDynamic(this, &USecondaryAttributeComponent::OnEquipmentChanged);
	}

	RecalculateSecondaryAttributes();
}

void USecondaryAttributeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& Timers = World->GetTimerManager();
		Timers.ClearTimer(HealthRegenerationTimer);
		Timers.ClearTimer(StaminaRegenerationTimer);
		Timers.ClearTimer(ManaRegenerationTimer);
	}
	Super::EndPlay(EndPlayReason);
}

void USecondaryAttributeComponent::RecalculateSecondaryAttributes()
{
	SecondaryAttributes.Empty();
	DamageResistances.Empty();
	if (!AttributeComponent && GetOwner())
	{
		AttributeComponent = GetOwner()->FindComponentByClass<UAttributeComponent>();
	}
	if (!EquipmentComponent && GetOwner())
	{
		EquipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>();
	}
	if (!AcquiredAttributeComponent && GetOwner())
	{
		AcquiredAttributeComponent = GetOwner()->FindComponentByClass<UAcquiredAttributeComponent>();
	}

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
		if (AcquiredAttributeComponent)
		{
			for (const TPair<EAcquiredAttribute, float>& Scaling : Formula.AcquiredAttributeScaling)
			{
				if (FMath::IsFinite(Scaling.Value))
				{
					Value += AcquiredAttributeComponent->GetAcquiredAttribute(Scaling.Key) * Scaling.Value;
				}
			}
		}

		SecondaryAttributes.Add(Pair.Key, Value);
	}
	for (const TPair<EDamageType, float>& Pair : SecondaryAttributeDataAsset->DamageResistancePercent)
	{
		if (FMath::IsFinite(Pair.Value)) DamageResistances.Add(Pair.Key, Pair.Value);
	}

	if (EquipmentComponent)
	{
		const TMap<EEquipmentSlot, UEquipmentItem*> EquippedItems = EquipmentComponent->GetEquippedItems();
		for (const TPair<EEquipmentSlot, UEquipmentItem*>& Pair : EquippedItems)
		{
			const UEquipmentItem* Item = Pair.Value;
			if (!Item)
			{
				continue;
			}

			for (const TPair<ESecondaryAttribute, float>& Bonus : Item->GetEquipmentBonuses())
			{
				float& AttributeValue = SecondaryAttributes.FindOrAdd(Bonus.Key);
				AttributeValue += Bonus.Value;
			}
			for (const TPair<EDamageType, float>& Bonus : Item->GetDamageResistanceBonuses())
			{
				if (FMath::IsFinite(Bonus.Value)) DamageResistances.FindOrAdd(Bonus.Key) += Bonus.Value;
			}
		}
	}

	for (const TPair<ESecondaryAttribute, float>& Bonus : ClassBonuses)
	{
		float& AttributeValue = SecondaryAttributes.FindOrAdd(Bonus.Key);
		AttributeValue += Bonus.Value;
	}

	for (const TPair<ESecondaryAttribute, float>& Pair : SecondaryAttributes)
	{
		OnSecondaryAttributeChanged.Broadcast(Pair.Key, Pair.Value);
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

float USecondaryAttributeComponent::GetDamageResistancePercent(EDamageType DamageType) const
{
	const float* Value = DamageResistances.Find(DamageType);
	return Value && FMath::IsFinite(*Value) ? FMath::Clamp(*Value, 0.0f, 100.0f) : 0.0f;
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
	if (!FMath::IsFinite(NewValue))
	{
		return;
	}

	const float MaxValue = GetSecondaryAttribute(Attribute);
	const float ClampedValue = MaxValue > 0.0f ? FMath::Clamp(NewValue, 0.0f, MaxValue) : FMath::Max(0.0f, NewValue);
	const float OldValue = GetCurrentSecondaryAttribute(Attribute);

	if (!FMath::IsNearlyEqual(OldValue, ClampedValue))
	{
		CurrentSecondaryAttributes.Add(Attribute, ClampedValue);
		OnCurrentSecondaryAttributeChanged.Broadcast(Attribute, ClampedValue, OldValue);
		UpdateRegenerationTimer(Attribute, ClampedValue < OldValue);
	}
}

void USecondaryAttributeComponent::ModifyCurrentSecondaryAttribute(ESecondaryAttribute Attribute, float Delta)
{
	SetCurrentSecondaryAttribute(Attribute, GetCurrentSecondaryAttribute(Attribute) + Delta);
}

bool USecondaryAttributeComponent::CanSpendResource(ESecondaryAttribute Attribute, float Amount) const
{
	if (Attribute != ESecondaryAttribute::MaxMana && Attribute != ESecondaryAttribute::MaxStamina)
	{
		return false;
	}
	return FMath::IsFinite(Amount) && Amount >= 0.0f
		&& GetCurrentSecondaryAttribute(Attribute) >= Amount;
}

bool USecondaryAttributeComponent::TrySpendResource(ESecondaryAttribute Attribute, float Amount)
{
	if (!CanSpendResource(Attribute, Amount)) return false;
	if (Amount > 0.0f) ModifyCurrentSecondaryAttribute(Attribute, -Amount);
	return true;
}

bool USecondaryAttributeComponent::CanSpendResources(float ManaCost, float StaminaCost) const
{
	return CanSpendResource(ESecondaryAttribute::MaxMana, ManaCost)
		&& CanSpendResource(ESecondaryAttribute::MaxStamina, StaminaCost);
}

bool USecondaryAttributeComponent::TrySpendResources(float ManaCost, float StaminaCost)
{
	if (!CanSpendResources(ManaCost, StaminaCost)) return false;
	TrySpendResource(ESecondaryAttribute::MaxMana, ManaCost);
	TrySpendResource(ESecondaryAttribute::MaxStamina, StaminaCost);
	return true;
}

bool USecondaryAttributeComponent::RestoreResource(ESecondaryAttribute Attribute, float Amount)
{
	if (Attribute != ESecondaryAttribute::MaxHealth && Attribute != ESecondaryAttribute::MaxMana
		&& Attribute != ESecondaryAttribute::MaxStamina) return false;
	if (!FMath::IsFinite(Amount) || Amount <= 0.0f) return false;
	const float Maximum = GetSecondaryAttribute(Attribute);
	const float Current = GetCurrentSecondaryAttribute(Attribute);
	if (!FMath::IsFinite(Maximum) || Maximum <= 0.0f || Current >= Maximum) return false;
	if (Attribute == ESecondaryAttribute::MaxHealth)
	{
		if (const AActionRPGPlayerCharacter* Player = Cast<AActionRPGPlayerCharacter>(GetOwner()))
		{
			if (Player->IsDead()) return false;
		}
		if (Current <= 0.0f) return false;
	}
	SetCurrentSecondaryAttribute(Attribute, Current + Amount);
	return true;
}

void USecondaryAttributeComponent::SetClassBonuses(const TMap<ESecondaryAttribute, float>& Bonuses)
{
	ClassBonuses = Bonuses;
	RecalculateSecondaryAttributes();
}

void USecondaryAttributeComponent::SetHealthRegenerationSuppressed(bool bSuppressed)
{
	if (bHealthRegenerationSuppressed != bSuppressed)
	{
		bHealthRegenerationSuppressed = bSuppressed;
		UpdateRegenerationTimer(ESecondaryAttribute::MaxHealth, false);
	}
}

void USecondaryAttributeComponent::OnPrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue)
{
	RecalculateSecondaryAttributes();
}

void USecondaryAttributeComponent::OnAcquiredAttributeChanged(EAcquiredAttribute Attribute, int32 NewValue, int32 Cap)
{
	RecalculateSecondaryAttributes();
}

void USecondaryAttributeComponent::OnEquipmentChanged(EEquipmentSlot Slot, UEquipmentItem* Item)
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

	UpdateRegenerationTimer(ESecondaryAttribute::MaxHealth, false);
	UpdateRegenerationTimer(ESecondaryAttribute::MaxStamina, false);
	UpdateRegenerationTimer(ESecondaryAttribute::MaxMana, false);
}

FTimerHandle* USecondaryAttributeComponent::GetRegenerationTimer(ESecondaryAttribute Attribute)
{
	switch (Attribute)
	{
	case ESecondaryAttribute::MaxHealth: return &HealthRegenerationTimer;
	case ESecondaryAttribute::MaxStamina: return &StaminaRegenerationTimer;
	case ESecondaryAttribute::MaxMana: return &ManaRegenerationTimer;
	default: return nullptr;
	}
}

float USecondaryAttributeComponent::GetRegenerationRate(ESecondaryAttribute Attribute) const
{
	if (!SecondaryAttributeDataAsset) return 0.0f;
	switch (Attribute)
	{
	case ESecondaryAttribute::MaxHealth: return SecondaryAttributeDataAsset->HealthRegenerationPerSecond;
	case ESecondaryAttribute::MaxStamina:
	{
		const UAcquiredAttributeComponent* Acquired = AcquiredAttributeComponent
			? AcquiredAttributeComponent.Get()
			: GetOwner() ? GetOwner()->FindComponentByClass<UAcquiredAttributeComponent>() : nullptr;
		const int32 Endurance = Acquired ? Acquired->GetAcquiredAttribute(EAcquiredAttribute::Endurance) : 0;
		const float Rate = SecondaryAttributeDataAsset->StaminaRegenerationPerSecond
			+ Endurance * SecondaryAttributeDataAsset->StaminaRegenerationPerEndurance;
		return FMath::IsFinite(Rate) ? FMath::Max(0.0f, Rate) : 0.0f;
	}
	case ESecondaryAttribute::MaxMana: return SecondaryAttributeDataAsset->ManaRegenerationPerSecond;
	default: return 0.0f;
	}
}

float USecondaryAttributeComponent::GetRegenerationDelay(ESecondaryAttribute Attribute) const
{
	if (!SecondaryAttributeDataAsset) return 0.0f;
	switch (Attribute)
	{
	case ESecondaryAttribute::MaxHealth: return SecondaryAttributeDataAsset->HealthRegenerationDelay;
	case ESecondaryAttribute::MaxStamina: return SecondaryAttributeDataAsset->StaminaRegenerationDelay;
	case ESecondaryAttribute::MaxMana: return SecondaryAttributeDataAsset->ManaRegenerationDelay;
	default: return 0.0f;
	}
}

void USecondaryAttributeComponent::UpdateRegenerationTimer(ESecondaryAttribute Attribute, bool bResetDelay)
{
	UWorld* World = GetWorld();
	FTimerHandle* Handle = GetRegenerationTimer(Attribute);
	if (!World || !Handle) return;

	FTimerManager& Timers = World->GetTimerManager();
	const float Rate = GetRegenerationRate(Attribute);
	const float Delay = GetRegenerationDelay(Attribute);
	const float Maximum = GetSecondaryAttribute(Attribute);
	const float Current = GetCurrentSecondaryAttribute(Attribute);
	if (!FMath::IsFinite(Rate) || Rate <= 0.0f || !FMath::IsFinite(Delay)
		|| Maximum <= 0.0f || Current >= Maximum - KINDA_SMALL_NUMBER
		|| (Attribute == ESecondaryAttribute::MaxHealth && bHealthRegenerationSuppressed)
		|| (Attribute == ESecondaryAttribute::MaxHealth && Current <= 0.0f))
	{
		Timers.ClearTimer(*Handle);
		return;
	}

	if (bResetDelay || !Timers.IsTimerActive(*Handle))
	{
		FTimerDelegate Callback = FTimerDelegate::CreateUObject(this, &USecondaryAttributeComponent::RegenerateResource, Attribute);
		Timers.SetTimer(*Handle, Callback, RegenerationInterval, true, FMath::Max(Delay, RegenerationInterval));
	}
}

void USecondaryAttributeComponent::RegenerateResource(ESecondaryAttribute Attribute)
{
	const float Rate = GetRegenerationRate(Attribute);
	if (!FMath::IsFinite(Rate) || Rate <= 0.0f)
	{
		UpdateRegenerationTimer(Attribute, false);
		return;
	}
	ModifyCurrentSecondaryAttribute(Attribute, Rate * RegenerationInterval);
	UpdateRegenerationTimer(Attribute, false);
}
