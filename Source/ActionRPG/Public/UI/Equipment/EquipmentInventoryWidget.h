// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentInventoryWidget.generated.h"

class UInventoryComponent;
class UItemBase;

/**
 * Inventory grid panel used in the equipment UI.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UEquipmentInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment UI")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment UI")
	void OnInventoryUpdated();

private:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	void BindInventoryComponent();

	UFUNCTION()
	void HandleInventoryChanged(int32 SlotIndex, UItemBase* Item);
};
