// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Progression/Core/ProgressionTypes.h"
#include "VitalBarsWidget.generated.h"

class UProgressBar;
class USecondaryAttributeComponent;
class UStatusEffectComponent;
class UTextBlock;
class UBorder;

/** Always-visible health, stamina, and mana bars for the local player. */
UCLASS()
class ACTIONRPG_API UVitalBarsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

	void BindAttributes(USecondaryAttributeComponent* InAttributes);
	void BindEffects(UStatusEffectComponent* InEffects);

private:
	UPROPERTY()
	TObjectPtr<USecondaryAttributeComponent> Attributes;

	UPROPERTY()
	TObjectPtr<UStatusEffectComponent> Effects;

	UPROPERTY()
	TObjectPtr<UBorder> EffectsPanel;

	UPROPERTY()
	TObjectPtr<UTextBlock> EffectsText;

	UPROPERTY()
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY()
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY()
	TObjectPtr<UProgressBar> ManaBar;

	UPROPERTY()
	TObjectPtr<UTextBlock> HealthValue;

	UPROPERTY()
	TObjectPtr<UTextBlock> StaminaValue;

	UPROPERTY()
	TObjectPtr<UTextBlock> ManaValue;

	void RefreshBars();
	void RefreshEffects();
	void RefreshBar(ESecondaryAttribute Attribute);

	UFUNCTION()
	void HandleMaxAttributeChanged(ESecondaryAttribute Attribute, float NewValue);

	UFUNCTION()
	void HandleCurrentAttributeChanged(ESecondaryAttribute Attribute, float NewValue, float OldValue);

	UFUNCTION()
	void HandleEffectsChanged();
};
