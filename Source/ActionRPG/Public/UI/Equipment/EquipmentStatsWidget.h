// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Progression/Core/ProgressionTypes.h"
#include "EquipmentStatsWidget.generated.h"

class UAttributeComponent;
class USecondaryAttributeComponent;
class USkillManagerComponent;
class USkillComponent;
class USkillBase;
class UTextBlock;

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	FText GetPrimaryAttributesText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	FText GetSecondaryAttributesText() const;

	UFUNCTION(BlueprintCallable, Category = "Equipment UI")
	void RefreshStats();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment UI")
	void OnStatsUpdated();
	virtual void OnStatsUpdated_Implementation();

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PrimaryStatsText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SecondaryStatsText;

private:
	UPROPERTY()
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY()
	TObjectPtr<USecondaryAttributeComponent> SecondaryAttributeComponent;

	UPROPERTY()
	TObjectPtr<USkillManagerComponent> SkillManagerComponent;

	UPROPERTY()
	TObjectPtr<USkillComponent> SkillComponent;

	void BindComponents();

	UFUNCTION()
	void HandlePrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue);

	UFUNCTION()
	void HandleSecondaryAttributeChanged(ESecondaryAttribute Attribute, float NewValue);

	UFUNCTION()
	void HandleCurrentSecondaryAttributeChanged(ESecondaryAttribute Attribute, float NewValue, float OldValue);

	UFUNCTION()
	void HandleMainHandSkillChanged(USkillBase* Skill);

	UFUNCTION()
	void HandleSkillLevelUp(USkillBase* Skill, int32 NewLevel);

	FText BuildPrimaryAttributeText() const;
	FText BuildSecondaryAttributeText() const;
};
