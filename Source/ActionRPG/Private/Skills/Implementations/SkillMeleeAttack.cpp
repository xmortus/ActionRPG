// Copyright Epic Games, Inc. All Rights Reserved.

#include "Skills/Implementations/SkillMeleeAttack.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/DamageType.h"
#include "Engine/DamageEvents.h"
#include "Components/Skills/SkillComponent.h"
#include "DrawDebugHelpers.h"

USkillMeleeAttack::USkillMeleeAttack()
{
}

USkillBase* USkillMeleeAttack::Activate(AActor* Target)
{
	// Call parent to handle cooldown and validation
	USkillBase* Result = Super::Activate(Target);
	if (!Result)
	{
		return nullptr;
	}

	// Get owner actor (character performing attack)
	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillMeleeAttack::Activate - OwnerActor is null"));
		return this;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor);
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillMeleeAttack::Activate - OwnerActor is not a Character"));
		return this;
	}

	UE_LOG(LogTemp, Log, TEXT("SkillMeleeAttack::Activate - Executing melee attack from %s"), *OwnerActor->GetName());

	// Perform hit detection
	TArray<FHitResult> HitResults = PerformMeleeHitDetection();

	if (HitResults.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("SkillMeleeAttack::Activate - No targets hit"));
		return this;
	}

	// Get attack damage
	float Damage = GetAttackDamage();
	UE_LOG(LogTemp, Log, TEXT("SkillMeleeAttack::Activate - Applying %.2f damage to %d targets"), Damage, HitResults.Num());

	// Apply damage to all hit targets
	int32 SuccessfulHits = 0;
	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.GetActor() && Hit.GetActor() != OwnerActor)
		{
			if (ApplyDamageToTarget(Hit.GetActor(), Damage))
			{
				SuccessfulHits++;
				UE_LOG(LogTemp, Log, TEXT("SkillMeleeAttack::Activate - Applied %.2f damage to %s"), 
					Damage, *Hit.GetActor()->GetName());
			}
		}
	}

	// Grant experience for successful hits
	if (SuccessfulHits > 0)
	{
		// Base experience per hit (can be modified by skill level)
		float BaseExperiencePerHit = 10.0f;
		float TotalExperience = BaseExperiencePerHit * SuccessfulHits;

		// Try to grant experience via SkillComponent
		if (USkillComponent* SkillComponent = OwnerActor->FindComponentByClass<USkillComponent>())
		{
			SkillComponent->AddSkillExperience(this, TotalExperience);
			UE_LOG(LogTemp, Log, TEXT("SkillMeleeAttack::Activate - Granted %.2f experience (%d hits)"), 
				TotalExperience, SuccessfulHits);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SkillMeleeAttack::Activate - SkillComponent not found, cannot grant experience"));
		}
	}

	return this;
}

TArray<FHitResult> USkillMeleeAttack::PerformMeleeHitDetection() const
{
	TArray<FHitResult> HitResults;

	if (!OwnerActor || !SkillData)
	{
		return HitResults;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor);
	if (!OwnerCharacter)
	{
		return HitResults;
	}

	// Get attack range
	float AttackRange = GetAttackRange();

	// Calculate trace start location (character location + forward offset)
	FVector TraceStart = OwnerCharacter->GetActorLocation();
	FVector TraceEnd = TraceStart + (OwnerCharacter->GetActorForwardVector() * AttackRange);

	// Sphere trace radius (can be adjusted, using 50 units as default)
	float TraceRadius = 50.0f;

	// Object types to trace against (Pawns)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	// Actors to ignore (owner)
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerActor);

	// Perform sphere trace
	UKismetSystemLibrary::SphereTraceMultiForObjects(
		OwnerActor->GetWorld(),
		TraceStart,
		TraceEnd,
		TraceRadius,
		ObjectTypes,
		false, // bTraceComplex
		ActorsToIgnore,
		EDrawDebugTrace::None, // Draw debug trace (set to ForDuration or Persistent for debugging)
		HitResults,
		true // bIgnoreSelf
	);

	UE_LOG(LogTemp, Log, TEXT("SkillMeleeAttack::PerformMeleeHitDetection - Found %d potential targets"), HitResults.Num());

	// Filter out duplicate actors (in case multiple hit results hit the same actor)
	TSet<AActor*> UniqueActors;
	TArray<FHitResult> FilteredResults;
	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.GetActor() && !UniqueActors.Contains(Hit.GetActor()))
		{
			UniqueActors.Add(Hit.GetActor());
			FilteredResults.Add(Hit);
		}
	}

	return FilteredResults;
}

bool USkillMeleeAttack::ApplyDamageToTarget(AActor* Target, float Damage) const
{
	if (!Target)
	{
		return false;
	}

	// Try to apply damage using TakeDamage (standard Unreal damage system)
	FPointDamageEvent DamageEvent(Damage, FHitResult(), OwnerActor ? OwnerActor->GetActorForwardVector() : FVector::ZeroVector, nullptr);
	float ActualDamage = Target->TakeDamage(Damage, DamageEvent, OwnerActor ? OwnerActor->GetInstigatorController() : nullptr, OwnerActor);

	if (ActualDamage == 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillMeleeAttack::ApplyDamageToTarget - Target %s did not process damage"), 
			*Target->GetName());
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("SkillMeleeAttack::ApplyDamageToTarget - Applied %.2f damage to %s via TakeDamage"), 
			ActualDamage, *Target->GetName());
	}

	return ActualDamage > 0.0f;
}

float USkillMeleeAttack::GetAttackRange() const
{
	if (!SkillData)
	{
		return 150.0f; // Default range
	}

	float BaseRange = SkillData->Range;

	// Future: Scale with skill level
	// float LevelMultiplier = 1.0f + (SkillLevel - 1) * 0.1f;
	// return BaseRange * LevelMultiplier;

	return BaseRange;
}

float USkillMeleeAttack::GetAttackDamage() const
{
	if (!SkillData)
	{
		return 25.0f; // Default damage
	}

	float BaseDamage = SkillData->BaseDamage;

	// Future: Scale with skill level
	// float LevelMultiplier = 1.0f + (SkillLevel - 1) * 0.15f;
	// return BaseDamage * LevelMultiplier;

	return BaseDamage;
}
