#include "Components/Combat/StatusEffectComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/TrainingComponent.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"

UStatusEffectComponent::UStatusEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatusEffectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(EffectTimer);
	for (const FActiveStatusEffect& Effect : ActiveEffects) ClearAttributeBonuses(Effect.AttributeSourceId);
	Super::EndPlay(EndPlayReason);
}

bool UStatusEffectComponent::ApplyEffect(UStatusEffectDataAsset* Definition, AActor* Source)
{
	if (!Definition || Definition->EffectID.IsNone() || !FMath::IsFinite(Definition->DurationSeconds)
		|| !FMath::IsFinite(Definition->TickIntervalSeconds) || Definition->DurationSeconds <= 0.0f
		|| Definition->TickIntervalSeconds <= 0.0f || !FMath::IsFinite(Definition->HealthPerTick)
		|| !FMath::IsFinite(Definition->HealthPerTickPerWisdom) || Definition->HealthPerTickPerWisdom < 0.0f
		|| !FMath::IsFinite(Definition->ManaPerTick) || !FMath::IsFinite(Definition->StaminaPerTick)
		|| !GetOwner() || !GetOwner()->FindComponentByClass<USecondaryAttributeComponent>()) return false;
	for (const TPair<EPrimaryAttribute, float>& Bonus : Definition->PrimaryAttributeBonuses)
	{
		if (!FMath::IsFinite(Bonus.Value)) return false;
	}
	if (!Definition->PrimaryAttributeBonuses.IsEmpty() && !GetOwner()->FindComponentByClass<UAttributeComponent>()) return false;
	for (FActiveStatusEffect& Active : ActiveEffects)
	{
		if (Active.Definition && Active.Definition->EffectID == Definition->EffectID)
		{
			if (Definition->StackPolicy == EStatusEffectStackPolicy::Ignore) return false;
			Active.Definition = Definition;
			Active.Source = Source;
			Active.RemainingSeconds = Definition->DurationSeconds;
			Active.TimeUntilTick = Definition->TickIntervalSeconds;
			Active.Stacks = Definition->StackPolicy == EStatusEffectStackPolicy::Stack
				? FMath::Min(Active.Stacks + 1, FMath::Max(1, Definition->MaxStacks)) : 1;
			UpdateAttributeBonuses(Active);
			OnStatusEffectsChanged.Broadcast();
			return true;
		}
	}
	FActiveStatusEffect& Active = ActiveEffects.AddDefaulted_GetRef();
	Active.Definition = Definition;
	Active.Source = Source;
	Active.RemainingSeconds = Definition->DurationSeconds;
	Active.TimeUntilTick = Definition->TickIntervalSeconds;
	Active.AttributeSourceId = FName(*FString::Printf(TEXT("Status.%s"), *Definition->EffectID.ToString()));
	UpdateAttributeBonuses(Active);
	if (UWorld* World = GetWorld())
	{
		if (!World->GetTimerManager().IsTimerActive(EffectTimer))
			World->GetTimerManager().SetTimer(EffectTimer, this, &UStatusEffectComponent::AdvanceEffects, UpdateInterval, true);
	}
	OnStatusEffectsChanged.Broadcast();
	return true;
}

bool UStatusEffectComponent::RemoveEffect(FName EffectID)
{
	for (const FActiveStatusEffect& Effect : ActiveEffects)
		if (Effect.Definition && Effect.Definition->EffectID == EffectID) ClearAttributeBonuses(Effect.AttributeSourceId);
	const int32 Count = ActiveEffects.RemoveAll([EffectID](const FActiveStatusEffect& Effect)
		{ return Effect.Definition && Effect.Definition->EffectID == EffectID; });
	if (Count > 0) OnStatusEffectsChanged.Broadcast();
	if (ActiveEffects.IsEmpty())
		if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(EffectTimer);
	return Count > 0;
}

void UStatusEffectComponent::RemoveEffectsOnDeath()
{
	for (const FActiveStatusEffect& Effect : ActiveEffects)
		if (!Effect.Definition || Effect.Definition->bRemoveOnDeath) ClearAttributeBonuses(Effect.AttributeSourceId);
	const int32 Count = ActiveEffects.RemoveAll([](const FActiveStatusEffect& Effect)
		{ return !Effect.Definition || Effect.Definition->bRemoveOnDeath; });
	if (Count > 0) OnStatusEffectsChanged.Broadcast();
	if (ActiveEffects.IsEmpty())
		if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(EffectTimer);
}

void UStatusEffectComponent::AdvanceEffects()
{
	bool bChanged = false;
	bool bDisplayedTimeChanged = false;
	for (int32 Index = ActiveEffects.Num() - 1; Index >= 0; --Index)
	{
		if (!ActiveEffects.IsValidIndex(Index)) continue;
		if (!ActiveEffects[Index].Definition)
		{
			ClearAttributeBonuses(ActiveEffects[Index].AttributeSourceId);
			ActiveEffects.RemoveAt(Index); bChanged = true; continue;
		}
		const int32 PreviousWholeSeconds = FMath::CeilToInt(ActiveEffects[Index].RemainingSeconds);
		ActiveEffects[Index].RemainingSeconds -= UpdateInterval;
		bDisplayedTimeChanged |= FMath::CeilToInt(ActiveEffects[Index].RemainingSeconds) != PreviousWholeSeconds;
		ActiveEffects[Index].TimeUntilTick -= UpdateInterval;
		const bool bShouldTick = ActiveEffects[Index].TimeUntilTick <= KINDA_SMALL_NUMBER;
		const FActiveStatusEffect Snapshot = ActiveEffects[Index];
		if (bShouldTick)
		{
			ActiveEffects[Index].TimeUntilTick += Snapshot.Definition->TickIntervalSeconds;
		}
		if (Snapshot.RemainingSeconds <= KINDA_SMALL_NUMBER)
		{
			ClearAttributeBonuses(Snapshot.AttributeSourceId);
			ActiveEffects.RemoveAt(Index);
			bChanged = true;
		}
		if (bShouldTick) ApplyPeriodicValues(Snapshot);
	}
	if (bChanged) OnStatusEffectsChanged.Broadcast();
	else if (bDisplayedTimeChanged) OnStatusEffectTimeChanged.Broadcast();
	if (ActiveEffects.IsEmpty())
		if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(EffectTimer);
}

void UStatusEffectComponent::UpdateAttributeBonuses(const FActiveStatusEffect& Effect)
{
	UAttributeComponent* Attributes = GetOwner() ? GetOwner()->FindComponentByClass<UAttributeComponent>() : nullptr;
	if (!Attributes || !Effect.Definition || Effect.AttributeSourceId.IsNone()) return;
	TMap<EPrimaryAttribute, float> Bonuses;
	for (const TPair<EPrimaryAttribute, float>& Bonus : Effect.Definition->PrimaryAttributeBonuses)
		Bonuses.Add(Bonus.Key, Bonus.Value * static_cast<float>(Effect.Stacks));
	Attributes->SetModifierSource(Effect.AttributeSourceId, Bonuses, false);
}

void UStatusEffectComponent::ClearAttributeBonuses(FName SourceId)
{
	if (SourceId.IsNone()) return;
	if (UAttributeComponent* Attributes = GetOwner() ? GetOwner()->FindComponentByClass<UAttributeComponent>() : nullptr)
		Attributes->RemoveModifierSource(SourceId);
}

void UStatusEffectComponent::ApplyPeriodicValues(const FActiveStatusEffect& Effect)
{
	AActor* Owner = GetOwner();
	USecondaryAttributeComponent* Attributes = Owner ? Owner->FindComponentByClass<USecondaryAttributeComponent>() : nullptr;
	if (!Attributes || !Effect.Definition) return;
	const float Multiplier = static_cast<float>(Effect.Stacks);
	const float Health = GetEffectiveHealthPerTick(Effect);
	if (Health > 0.0f)
	{
		const float Before = Attributes->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth);
		if (Attributes->RestoreResource(ESecondaryAttribute::MaxHealth, Health) &&
			Attributes->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth) - Before >= 2.0f)
		{
			if (AActionRPGPlayerCharacter* Healer = Cast<AActionRPGPlayerCharacter>(Effect.Source.Get()))
			{
				if (!Healer->IsDead() && Healer->TrainingComponent)
				{
					Healer->TrainingComponent->RecordPractice(EPrimaryAttribute::Wisdom,
						TEXT("EffectiveSkillHeal"), nullptr);
				}
			}
		}
	}
	else if (Health < 0.0f)
	{
		AController* Instigator = Effect.Source ? Effect.Source->GetInstigatorController() : nullptr;
		Owner->TakeDamage(-Health, FDamageEvent(), Instigator, Effect.Source.Get());
	}
	const float Mana = Effect.Definition->ManaPerTick * Multiplier;
	if (Mana > 0.0f) Attributes->RestoreResource(ESecondaryAttribute::MaxMana, Mana);
	else if (Mana < 0.0f) Attributes->TrySpendResource(ESecondaryAttribute::MaxMana,
		FMath::Min(-Mana, Attributes->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxMana)));
	const float Stamina = Effect.Definition->StaminaPerTick * Multiplier;
	if (Stamina > 0.0f) Attributes->RestoreResource(ESecondaryAttribute::MaxStamina, Stamina);
	else if (Stamina < 0.0f) Attributes->TrySpendResource(ESecondaryAttribute::MaxStamina,
		FMath::Min(-Stamina, Attributes->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina)));
}

float UStatusEffectComponent::GetEffectiveHealthPerTick(const FActiveStatusEffect& Effect) const
{
	if (!Effect.Definition) return 0.0f;
	float HealthPerTick = Effect.Definition->HealthPerTick;
	if (HealthPerTick > 0.0f && Effect.Definition->HealthPerTickPerWisdom > 0.0f)
	{
		const AActor* Source = Effect.Source.Get();
		const UAttributeComponent* SourceAttributes = Source ? Source->FindComponentByClass<UAttributeComponent>() : nullptr;
		if (SourceAttributes)
		{
			HealthPerTick += FMath::Max(0.0f, SourceAttributes->GetAttribute(EPrimaryAttribute::Wisdom))
				* Effect.Definition->HealthPerTickPerWisdom;
		}
	}
	return HealthPerTick * static_cast<float>(Effect.Stacks);
}
