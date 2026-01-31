// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Progression/Core/ProgressionTypes.h"
#include "EquipmentStatsWidget.generated.h"

class UAttributeComponent;
class USecondaryAttributeComponent;

/**
 * Stats panel widget for equipment UI (primary + secondary stats).
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UEquipmentStatsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	UAttributeComponent* GetAttributeComponent() const { return AttributeComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	USecondaryAttributeComponent* GetSecondaryAttributeComponent() const { return SecondaryAttributeComponent; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment UI")
	void OnStatsUpdated();

private:
	UPROPERTY()
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY()
	TObjectPtr<USecondaryAttributeComponent> SecondaryAttributeComponent;

	void BindComponents();

	UFUNCTION()
	void HandlePrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue);

	UFUNCTION()
	void HandleSecondaryAttributeChanged(ESecondaryAttribute Attribute, float NewValue);
};
