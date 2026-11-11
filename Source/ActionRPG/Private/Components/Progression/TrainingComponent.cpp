#include "Components/Progression/TrainingComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Engine/World.h"

UTrainingComponent::UTrainingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	FAttributeTrainingRule MeleeRule;
	MeleeRule.Attribute = EPrimaryAttribute::Strength;
	MeleeRule.Activity = TEXT("MeleeHit");
	Rules.Add(MeleeRule);

	FAttributeTrainingRule DodgeRule;
	DodgeRule.Attribute = EPrimaryAttribute::Agility;
	DodgeRule.Activity = TEXT("CombatDodge");
	DodgeRule.PracticePerGain = 12;
	DodgeRule.ActivityCooldownSeconds = 3.0f;
	DodgeRule.TargetCooldownSeconds = 8.0f;
	DodgeRule.MaxPracticePerWindow = 6;
	Rules.Add(DodgeRule);

	FAttributeTrainingRule SurvivalRule;
	SurvivalRule.Attribute = EPrimaryAttribute::Constitution;
	SurvivalRule.Activity = TEXT("SurviveEnemyHit");
	SurvivalRule.PracticePerGain = 12;
	SurvivalRule.ActivityCooldownSeconds = 3.0f;
	SurvivalRule.TargetCooldownSeconds = 8.0f;
	SurvivalRule.MaxPracticePerWindow = 6;
	Rules.Add(SurvivalRule);

	FAttributeTrainingRule RangedRule;
	RangedRule.Attribute = EPrimaryAttribute::Dexterity;
	RangedRule.Activity = TEXT("RangedHit");
	RangedRule.PracticePerGain = 12;
	RangedRule.ActivityCooldownSeconds = 3.0f;
	RangedRule.TargetCooldownSeconds = 8.0f;
	RangedRule.MaxPracticePerWindow = 6;
	Rules.Add(RangedRule);

	FAttributeTrainingRule MagicRule;
	MagicRule.Attribute = EPrimaryAttribute::Intelligence;
	MagicRule.Activity = TEXT("MagicHit");
	MagicRule.PracticePerGain = 12;
	MagicRule.ActivityCooldownSeconds = 3.0f;
	MagicRule.TargetCooldownSeconds = 8.0f;
	MagicRule.MaxPracticePerWindow = 6;
	Rules.Add(MagicRule);

	FAttributeTrainingRule HealingRule;
	HealingRule.Attribute = EPrimaryAttribute::Wisdom;
	HealingRule.Activity = TEXT("EffectiveSkillHeal");
	HealingRule.PracticePerGain = 12;
	HealingRule.ActivityCooldownSeconds = 3.0f;
	HealingRule.bRequiresTarget = false;
	HealingRule.MaxPracticePerWindow = 6;
	Rules.Add(HealingRule);
}

FName UTrainingComponent::MakeRuleKey(EPrimaryAttribute Attribute, FName Activity)
{
	return FName(*FString::Printf(TEXT("%d.%s"), static_cast<int32>(Attribute), *Activity.ToString()));
}

FName UTrainingComponent::MakeSourceId(EPrimaryAttribute Attribute)
{
	const UEnum* Enum = StaticEnum<EPrimaryAttribute>();
	return FName(*FString::Printf(TEXT("Training.%s"), *Enum->GetNameStringByValue(static_cast<int64>(Attribute))));
}

bool UTrainingComponent::RecordPractice(EPrimaryAttribute Attribute, FName Activity, AActor* Target)
{
	if (Activity.IsNone() || !GetWorld()) return false;
	const FAttributeTrainingRule* Rule = Rules.FindByPredicate([Attribute, Activity](const FAttributeTrainingRule& Candidate)
	{
		return Candidate.Attribute == Attribute && Candidate.Activity == Activity;
	});
	if (!Rule || Rule->PracticePerGain <= 0 || Rule->AttributeGain <= 0 ||
		!FMath::IsFinite(Rule->ActivityCooldownSeconds) || Rule->ActivityCooldownSeconds < 0.0f ||
		!FMath::IsFinite(Rule->TargetCooldownSeconds) || Rule->TargetCooldownSeconds < 0.0f ||
		!FMath::IsFinite(Rule->WindowSeconds) || Rule->WindowSeconds < 0.0f ||
		Rule->MaxPracticePerWindow < 0 || (Rule->bRequiresTarget && !IsValid(Target)) ||
		Target == GetOwner()) return false;

	UAttributeComponent* Attributes = GetOwner() ? GetOwner()->FindComponentByClass<UAttributeComponent>() : nullptr;
	if (!Attributes) return false;

	const double Now = GetWorld()->GetTimeSeconds();
	const FName RuleKey = MakeRuleKey(Attribute, Activity);
	FPracticeState& State = PracticeStates.FindOrAdd(RuleKey);
	if (Rule->WindowSeconds > 0.0f &&
		(State.AcceptedInWindow == 0 || Now - State.WindowStart >= Rule->WindowSeconds))
	{
		State.WindowStart = Now;
		State.AcceptedInWindow = 0;
		for (auto It = State.LastTargetTimes.CreateIterator(); It; ++It)
		{
			if (Now - It.Value() >= Rule->TargetCooldownSeconds) It.RemoveCurrent();
		}
	}
	if (Rule->MaxPracticePerWindow > 0 && State.AcceptedInWindow >= Rule->MaxPracticePerWindow) return false;
	if (State.bHasActivityTime && Now - State.LastActivityTime < Rule->ActivityCooldownSeconds) return false;
	const uint32 TargetId = IsValid(Target) ? Target->GetUniqueID() : 0;
	if (TargetId != 0)
	{
		if (const double* LastTime = State.LastTargetTimes.Find(TargetId))
		{
			if (Now - *LastTime < Rule->TargetCooldownSeconds) return false;
		}
	}

	const bool bGrowth = State.Progress + 1 >= Rule->PracticePerGain;
	if (bGrowth)
	{
		const int32 CurrentGain = EarnedGains.FindRef(Attribute);
		if (CurrentGain > MAX_int32 - Rule->AttributeGain) return false;
		const int32 NewGain = CurrentGain + Rule->AttributeGain;
		TMap<EPrimaryAttribute, float> Bonus;
		Bonus.Add(Attribute, static_cast<float>(NewGain));
		if (!Attributes->SetModifierSource(MakeSourceId(Attribute), Bonus, true)) return false;
		EarnedGains.Add(Attribute, NewGain);
		UE_LOG(LogTemp, Log, TEXT("Training: %s grew by %d after %s practice"),
			*MakeSourceId(Attribute).ToString(), Rule->AttributeGain, *Activity.ToString());
	}
	State.Progress = bGrowth ? 0 : State.Progress + 1;
	State.LastActivityTime = Now;
	State.bHasActivityTime = true;
	if (TargetId != 0) State.LastTargetTimes.Add(TargetId, Now);
	++State.AcceptedInWindow;
	OnAttributePractice.Broadcast(Attribute, Activity, State.Progress, Rule->PracticePerGain, bGrowth);
	return true;
}

int32 UTrainingComponent::GetPracticeProgress(EPrimaryAttribute Attribute, FName Activity) const
{
	const FPracticeState* State = PracticeStates.Find(MakeRuleKey(Attribute, Activity));
	return State ? State->Progress : 0;
}

int32 UTrainingComponent::GetEarnedTrainingGain(EPrimaryAttribute Attribute) const
{
	return EarnedGains.FindRef(Attribute);
}
