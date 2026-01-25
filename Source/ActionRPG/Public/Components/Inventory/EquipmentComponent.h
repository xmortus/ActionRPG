// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Core/ItemTypes.h"
#include "EquipmentComponent.generated.h"

class UEquipmentItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentChanged, EEquipmentSlot, Slot, UEquipmentItem*, Item);

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentComponent();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipItem(UEquipmentItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	UEquipmentItem* UnequipSlot(EEquipmentSlot Slot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	bool IsSlotOccupied(EEquipmentSlot Slot) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	UEquipmentItem* GetEquippedItem(EEquipmentSlot Slot) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	TMap<EEquipmentSlot, UEquipmentItem*> GetEquippedItems() const;

	UPROPERTY(BlueprintAssignable, Category = "Equipment|Events")
	FOnEquipmentChanged OnEquipmentChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipmentSlot, TObjectPtr<UEquipmentItem>> EquippedItems;
};
