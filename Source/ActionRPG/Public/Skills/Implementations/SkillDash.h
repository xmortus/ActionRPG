// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Skills/Core/SkillBase.h"
#include "TimerManager.h"
#include "SkillDash.generated.h"

class AActor;

/**
 * Dash skill implementation.
 * Applies a forward movement burst and optional invincibility frames.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API USkillDash : public USkillBase
{
	GENERATED_BODY()

public:
	USkillDash();

	// Override Activate to implement dash logic
	virtual USkillBase* Activate(AActor* Target = nullptr) override;

protected:
	/** Get dash speed from SkillDataAsset. */
	float GetDashSpeed() const;

	/** Get invincibility duration from SkillDataAsset. */
	float GetInvincibilityFrames() const;

	/** Begin temporary invincibility by disabling collision. */
	void BeginInvincibility();

	/** End temporary invincibility by re-enabling collision. */
	void EndInvincibility();

	FTimerHandle InvincibilityTimerHandle;
};
