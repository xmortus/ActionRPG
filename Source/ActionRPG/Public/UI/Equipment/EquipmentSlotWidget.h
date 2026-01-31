// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Core/ItemTypes.h"
#include "EquipmentSlotWidget.generated.h"

class UEquipmentComponent;
class UEquipmentItem;

/**
 * Single equipment slot widget for paper-doll display.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment UI")
	EEquipmentSlot EquipmentSlot = EEquipmentSlot::None;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	UEquipmentItem* GetEquippedItem() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment UI")
	void OnSlotUpdated(UEquipmentItem* Item);

private:
	UPROPERTY()
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	void BindEquipmentComponent();

	UFUNCTION()
	void HandleEquipmentChanged(EEquipmentSlot ChangedSlot, UEquipmentItem* Item);
};
