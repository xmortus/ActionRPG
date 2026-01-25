// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Equipment/EquipmentItem.h"
#include "WeaponItem.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UWeaponItem : public UEquipmentItem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	float GetWeaponDamage() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	EDamageType GetWeaponDamageType() const;
};
