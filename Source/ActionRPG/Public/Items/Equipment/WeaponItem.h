// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/Equipment/EquipmentItem.h"
#include "WeaponItem.generated.h"

class AActor;

UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UWeaponItem : public UEquipmentItem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	float GetWeaponDamage() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon|Growth")
	int32 GetGrowthRank() const { return GrowthRank; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon|Growth")
	int32 GetDefeatsTowardGrowth() const { return DefeatsTowardGrowth; }

	virtual FText GetItemTooltip() const override;

	/** A soulbound item can be claimed only once, by its initial inventory owner. */
	bool BindToOwner(AActor* NewOwner);
	bool IsBoundToOtherOwner(const AActor* NewOwner) const;
	bool RecordCreditedDefeat();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	EDamageType GetWeaponDamageType() const;

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon|Growth")
	TObjectPtr<AActor> BoundOwner;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Growth")
	int32 GrowthRank = 0;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Growth")
	int32 DefeatsTowardGrowth = 0;
};
