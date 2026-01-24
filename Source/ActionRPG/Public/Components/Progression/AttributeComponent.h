// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Progression/Core/ProgressionTypes.h"
#include "AttributeComponent.generated.h"

class UPrimaryAttributeDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPrimaryAttributeChanged, EPrimaryAttribute, Attribute, float, NewValue, float, OldValue);

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void InitializeFromDataAsset();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
	float GetAttribute(EPrimaryAttribute Attribute) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetAttribute(EPrimaryAttribute Attribute, float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ModifyAttribute(EPrimaryAttribute Attribute, float Delta);

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnPrimaryAttributeChanged OnPrimaryAttributeChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	TObjectPtr<UPrimaryAttributeDataAsset> AttributeDataAsset;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	TMap<EPrimaryAttribute, float> Attributes;

	float ClampAttributeValue(float Value) const;
};
