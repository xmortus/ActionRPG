// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Core/ItemBase.h"
#include "Progression/Core/ProgressionTypes.h"
#include "Items/Core/ItemTypes.h"
#include "EquipmentItem.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UEquipmentItem : public UItemBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	EEquipmentSlot GetEquipmentSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	const TMap<ESecondaryAttribute, float>& GetEquipmentBonuses() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	const TMap<EPrimaryAttribute, float>& GetPrimaryAttributeBonuses() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	int32 GetRequiredLevel() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	const TMap<EPrimaryAttribute, float>& GetRequiredAttributes() const;
};
