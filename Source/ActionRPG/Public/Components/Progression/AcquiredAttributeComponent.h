// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Progression/Core/ProgressionTypes.h"
#include "AcquiredAttributeComponent.generated.h"

class UAttributeComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAcquiredAttributeChanged,
	EAcquiredAttribute, Attribute, int32, NewValue, int32, Cap);

/** Book-style earned secondary attributes, separate from calculated MaxHealth, Armor, and similar stats. */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UAcquiredAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAcquiredAttributeComponent();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Attributes|Acquired")
	int32 GetAcquiredAttribute(EAcquiredAttribute Attribute) const;

	UFUNCTION(BlueprintPure, Category = "Attributes|Acquired")
	int32 GetAcquiredAttributeCap(EAcquiredAttribute Attribute) const;

	UFUNCTION(BlueprintPure, Category = "Attributes|Acquired")
	bool CanGrantAcquiredAttribute(EAcquiredAttribute Attribute, int32 Amount) const;

	/** Grants an earned value only when the full amount fits under its linked primary attribute. */
	UFUNCTION(BlueprintCallable, Category = "Attributes|Acquired")
	bool TryGrantAcquiredAttribute(EAcquiredAttribute Attribute, int32 Amount);

	UPROPERTY(BlueprintAssignable, Category = "Attributes|Acquired")
	FOnAcquiredAttributeChanged OnAcquiredAttributeChanged;

private:
	UPROPERTY()
	TObjectPtr<UAttributeComponent> PrimaryAttributes;

	UPROPERTY(VisibleAnywhere, Category = "Attributes|Acquired")
	TMap<EAcquiredAttribute, int32> EarnedAttributes;

	UFUNCTION()
	void HandlePrimaryAttributeChanged(EPrimaryAttribute Attribute, float NewValue, float OldValue);
};
