// Copyright Epic Games, Inc. All Rights Reserved.

#include "Skills/Implementations/SkillDash.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Components/Skills/SkillComponent.h"

USkillDash::USkillDash()
{
}

USkillBase* USkillDash::Activate(AActor* Target)
{
	// Call parent to handle cooldown and validation
	USkillBase* Result = Super::Activate(Target);
	if (!Result)
	{
		return nullptr;
	}

	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillDash::Activate - OwnerActor is null"));
		return this;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor);
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillDash::Activate - OwnerActor is not a Character"));
		return this;
	}

	const float DashSpeed = GetDashSpeed();
	const FVector DashDirection = OwnerCharacter->GetActorForwardVector();
	const FVector DashVelocity = DashDirection * DashSpeed;

	OwnerCharacter->LaunchCharacter(DashVelocity, true, true);
	UE_LOG(LogTemp, Log, TEXT("SkillDash::Activate - Dash launched with speed %.2f from %s"),
		DashSpeed, *OwnerActor->GetName());

	// Optional invincibility frames
	BeginInvincibility();

	// Grant base experience on use
	const float BaseExperience = 5.0f;
	if (USkillComponent* SkillComponent = OwnerActor->FindComponentByClass<USkillComponent>())
	{
		SkillComponent->AddSkillExperience(this, BaseExperience);
		UE_LOG(LogTemp, Log, TEXT("SkillDash::Activate - Granted %.2f base experience"), BaseExperience);
	}

	return this;
}

float USkillDash::GetDashSpeed() const
{
	const float BaseSpeed = SkillData && FMath::IsFinite(SkillData->AbilityMoveSpeed) && SkillData->AbilityMoveSpeed > 0.0f
		? SkillData->AbilityMoveSpeed : 1200.0f;
	if (!SkillData) return BaseSpeed;
	const float PerLevel = FMath::IsFinite(SkillData->DashSpeedPercentPerLevel)
		? FMath::Max(0.0f, SkillData->DashSpeedPercentPerLevel) : 0.0f;
	const float MaxMultiplier = FMath::IsFinite(SkillData->MaxDashSpeedMultiplier)
		? FMath::Max(1.0f, SkillData->MaxDashSpeedMultiplier) : 1.0f;
	const float LevelsGained = static_cast<float>(FMath::Max(0, SkillLevel - 1));
	return BaseSpeed * FMath::Clamp(1.0f + LevelsGained * PerLevel / 100.0f, 1.0f, MaxMultiplier);
}

float USkillDash::GetStaminaCost() const
{
	const float BaseCost = Super::GetStaminaCost();
	if (!SkillData || !FMath::IsFinite(BaseCost) || BaseCost <= 0.0f) return 0.0f;
	const float Reduction = FMath::IsFinite(SkillData->DashStaminaReductionPerLevel)
		? FMath::Max(0.0f, SkillData->DashStaminaReductionPerLevel) : 0.0f;
	const float Minimum = FMath::IsFinite(SkillData->MinDashStaminaCost)
		? FMath::Clamp(SkillData->MinDashStaminaCost, 0.0f, BaseCost) : BaseCost;
	const float LevelsGained = static_cast<float>(FMath::Max(0, SkillLevel - 1));
	return FMath::Max(Minimum, BaseCost - LevelsGained * Reduction);
}

float USkillDash::GetInvincibilityFrames() const
{
	if (SkillData && SkillData->InvincibilityFrames > 0.0f)
	{
		return SkillData->InvincibilityFrames;
	}

	return 0.0f;
}

void USkillDash::BeginInvincibility()
{
	const float InvincibilityDuration = GetInvincibilityFrames();
	if (InvincibilityDuration <= 0.0f || !OwnerActor)
	{
		return;
	}

	OwnerActor->SetActorEnableCollision(false);
	UE_LOG(LogTemp, Log, TEXT("SkillDash::BeginInvincibility - Collision disabled for %.2f seconds"), InvincibilityDuration);

	if (UWorld* World = OwnerActor->GetWorld())
	{
		World->GetTimerManager().ClearTimer(InvincibilityTimerHandle);
		World->GetTimerManager().SetTimer(
			InvincibilityTimerHandle,
			this,
			&USkillDash::EndInvincibility,
			InvincibilityDuration,
			false
		);
	}
}

void USkillDash::EndInvincibility()
{
	if (!OwnerActor)
	{
		return;
	}

	OwnerActor->SetActorEnableCollision(true);
	UE_LOG(LogTemp, Log, TEXT("SkillDash::EndInvincibility - Collision re-enabled"));
}
