// Copyright Epic Games, Inc. All Rights Reserved.

#include "Skills/Implementations/SkillMeleeAttack.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Characters/ActionRPGEnemyCharacter.h"
#include "Components/Progression/TrainingComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "GameFramework/DamageType.h"
#include "Combat/ActionRPGDamageTypes.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Items/Core/ItemDataAsset.h"
#include "Engine/DamageEvents.h"
#include "Engine/OverlapResult.h"
#include "Components/Skills/SkillComponent.h"
#include "Items/Equipment/WeaponItem.h"
#include "CollisionQueryParams.h"

USkillMeleeAttack::USkillMeleeAttack()
{
}

const UItemDataAsset* USkillMeleeAttack::GetMainHandWeaponData() const
{
	if (OwnerActor)
	{
		if (const UEquipmentComponent* Equipment = OwnerActor->FindComponentByClass<UEquipmentComponent>())
		{
			const UEquipmentItem* MainHand = Equipment->GetEquippedItem(EEquipmentSlot::MainHand);
			const UItemDataAsset* WeaponData = MainHand ? MainHand->ItemData : nullptr;
			if (WeaponData && FMath::IsFinite(WeaponData->WeaponDamage) && WeaponData->WeaponDamage > 0.0f)
				return WeaponData;
		}
	}
	return nullptr;
}

TSubclassOf<UDamageType> USkillMeleeAttack::GetMeleeDamageTypeClass() const
{
	if (const UItemDataAsset* WeaponData = GetMainHandWeaponData())
		return FActionRPGDamageTypes::GetClass(WeaponData->WeaponDamageType);
	return FActionRPGDamageTypes::GetClass(SkillData ? SkillData->DamageType : EDamageType::Physical);
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
	if (AActionRPGPlayerCharacter* PlayerCharacter = Cast<AActionRPGPlayerCharacter>(OwnerCharacter))
	{
		PlayerCharacter->PlayBasicMeleeAttackAnimation(GetEffectiveCooldownDuration());
	}

	// Perform hit detection
	TArray<AActor*> HitTargets = PerformMeleeHitDetection();

	if (HitTargets.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("SkillMeleeAttack::Activate - No targets hit"));
		return this;
	}

	// Get attack damage
	float Damage = GetAttackDamage();
	UE_LOG(LogTemp, Log, TEXT("SkillMeleeAttack::Activate - Applying %.2f damage to %d targets"), Damage, HitTargets.Num());

	// Apply damage to all hit targets
	int32 SuccessfulHits = 0;
	for (AActor* HitActor : HitTargets)
	{
		if (HitActor && HitActor != OwnerActor)
		{
			if (ApplyDamageToTarget(HitActor, Damage))
			{
				SuccessfulHits++;
				if (Cast<AActionRPGEnemyCharacter>(HitActor))
				{
					if (AActionRPGPlayerCharacter* Player = Cast<AActionRPGPlayerCharacter>(OwnerActor))
					{
						if (!Player->IsDead() && Player->TrainingComponent)
						{
							Player->TrainingComponent->RecordPractice(EPrimaryAttribute::Strength, TEXT("MeleeHit"), HitActor);
						}
					}
				}
				UE_LOG(LogTemp, Log, TEXT("SkillMeleeAttack::Activate - Applied %.2f damage to %s"), 
					Damage, *HitActor->GetName());
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

TArray<AActor*> USkillMeleeAttack::PerformMeleeHitDetection() const
{
	TArray<AActor*> HitTargets;

	if (!OwnerActor || !SkillData)
	{
		return HitTargets;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor);
	if (!OwnerCharacter)
	{
		return HitTargets;
	}

	const float AttackRange = GetAttackRange();
	if (AttackRange <= 0.0f || !OwnerCharacter->GetWorld())
	{
		return HitTargets;
	}

	// A single thin sweep can slip past a nearby capsule in a top-down game.
	// Query the whole melee reach, then keep targets in front (or immediately adjacent).
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MeleeAttack), false, OwnerActor);
	TArray<FOverlapResult> Overlaps;
	OwnerCharacter->GetWorld()->OverlapMultiByObjectType(
		Overlaps, OwnerCharacter->GetActorLocation(), FQuat::Identity,
		ObjectParams, FCollisionShape::MakeSphere(AttackRange), QueryParams);

	const FVector Forward = OwnerCharacter->GetActorForwardVector().GetSafeNormal2D();
	TSet<AActor*> UniqueActors;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Candidate = Overlap.GetActor();
		if (!Candidate || Candidate == OwnerActor || UniqueActors.Contains(Candidate))
		{
			continue;
		}
		const FVector ToTarget = (Candidate->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal2D();
		const float Distance2D = FVector::Dist2D(Candidate->GetActorLocation(), OwnerCharacter->GetActorLocation());
		if (Distance2D <= 100.0f || FVector::DotProduct(Forward, ToTarget) >= 0.25f)
		{
			UniqueActors.Add(Candidate);
			HitTargets.Add(Candidate);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("SkillMeleeAttack::PerformMeleeHitDetection - Found %d potential targets (range %.0f)"),
		HitTargets.Num(), AttackRange);
	return HitTargets;
}

bool USkillMeleeAttack::ApplyDamageToTarget(AActor* Target, float Damage) const
{
	if (!Target)
	{
		return false;
	}

	// Try to apply damage using TakeDamage (standard Unreal damage system)
	FPointDamageEvent DamageEvent(Damage, FHitResult(), OwnerActor ? OwnerActor->GetActorForwardVector() : FVector::ZeroVector,
		GetMeleeDamageTypeClass());
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
	const float BaseDamage = SkillData && FMath::IsFinite(SkillData->BaseDamage)
		? FMath::Max(0.0f, SkillData->BaseDamage) : 25.0f;
	const UItemDataAsset* WeaponData = GetMainHandWeaponData();
	const UEquipmentComponent* Equipment = OwnerActor ? OwnerActor->FindComponentByClass<UEquipmentComponent>() : nullptr;
	const UWeaponItem* Weapon = Equipment ? Cast<UWeaponItem>(Equipment->GetEquippedItem(EEquipmentSlot::MainHand)) : nullptr;
	const float WeaponDamage = Weapon ? Weapon->GetWeaponDamage() : WeaponData ? WeaponData->WeaponDamage : 0.0f;
	const UAttributeComponent* Attributes = OwnerActor ? OwnerActor->FindComponentByClass<UAttributeComponent>() : nullptr;
	const float Strength = Attributes ? FMath::Max(0.0f, Attributes->GetAttribute(EPrimaryAttribute::Strength)) : 0.0f;
	const float PerStrength = SkillData && FMath::IsFinite(SkillData->MeleeDamagePerStrength)
		? FMath::Max(0.0f, SkillData->MeleeDamagePerStrength) : 0.0f;

	// Future: Scale with skill level
	// float LevelMultiplier = 1.0f + (SkillLevel - 1) * 0.15f;
	// return BaseDamage * LevelMultiplier;

	return (BaseDamage + WeaponDamage + Strength * PerStrength) * GetDamageMultiplier();
}

float USkillMeleeAttack::GetEffectiveCooldownDuration() const
{
	const float BaseCooldown = Super::GetEffectiveCooldownDuration();
	if (BaseCooldown <= 0.0f || !SkillData) return BaseCooldown;
	const UAttributeComponent* Attributes = OwnerActor ? OwnerActor->FindComponentByClass<UAttributeComponent>() : nullptr;
	const float Agility = Attributes ? FMath::Max(0.0f, Attributes->GetAttribute(EPrimaryAttribute::Agility)) : 0.0f;
	const float PercentPerPoint = FMath::IsFinite(SkillData->MeleeSpeedPercentPerAgility)
		? FMath::Max(0.0f, SkillData->MeleeSpeedPercentPerAgility) : 0.0f;
	const float MaxMultiplier = FMath::IsFinite(SkillData->MaxMeleeSpeedMultiplier)
		? FMath::Max(1.0f, SkillData->MaxMeleeSpeedMultiplier) : 1.0f;
	const float Multiplier = FMath::Clamp(1.0f + Agility * PercentPerPoint / 100.0f, 1.0f, MaxMultiplier);
	return BaseCooldown / Multiplier;
}
