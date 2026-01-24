// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Progression/Core/ProgressionTypes.h"
#include "SecondaryAttributeComponent.generated.h"

class USecondaryAttributeDataAsset;
class UAttributeComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSecondaryAttributeChanged, ESecondaryAttribute, Attribute, float, NewValue);

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API USecondaryAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USecondaryAttributeComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void RecalculateSecondaryAttributes();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
	float GetSecondaryAttribute(ESecondaryAttribute Attribute) const;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnSecondaryAttributeChanged OnSecondaryAttributeChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TObjectPtr<USecondaryAttributeDataAsset> SecondaryAttributeDataAsset;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	TMap<ESecondaryAttribute, float> SecondaryAttributes;

private:
	UPROPERTY()
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UFUNCTION()
	void OnPrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue);
};
