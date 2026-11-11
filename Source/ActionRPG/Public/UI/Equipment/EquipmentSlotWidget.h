// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Core/ItemTypes.h"
#include "EquipmentSlotWidget.generated.h"

class UImage;
class UEquipmentComponent;
class UEquipmentItem;
class UBorder;

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
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Equipment UI")
	void OnSlotUpdated(UEquipmentItem* Item);
	virtual void OnSlotUpdated_Implementation(UEquipmentItem* Item);

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> SlotIcon;

private:
	UPROPERTY()
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY()
	TObjectPtr<UBorder> DropHighlightBorder;
	FLinearColor NormalBorderColor;

	void BindEquipmentComponent();

	UFUNCTION()
	void HandleEquipmentChanged(EEquipmentSlot ChangedSlot, UEquipmentItem* Item);
};
