// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ActionRPGCharacter.h"
#include "ActionRPGPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAttributeComponent;
class USecondaryAttributeComponent;
class UEquipmentComponent;
class UExperienceComponent;
class UClassComponent;

/**
 * Player-specific Character class for ActionRPG.
 * Handles player movement, camera setup, and input.
 * Configured for top-down action RPG gameplay with mouse-driven rotation.
 */
UCLASS(Blueprintable)
class ACTIONRPG_API AActionRPGPlayerCharacter : public AActionRPGCharacter
{
	GENERATED_BODY()

public:
	AActionRPGPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	// Movement
	void Move(const FVector2D& MovementVector);
	void Look(const FVector2D& LookVector);

	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<class UInventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<USecondaryAttributeComponent> SecondaryAttributeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<UExperienceComponent> ExperienceComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<UClassComponent> ClassComponent;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Rotate character to face mouse cursor
	void RotateToMouseCursor();

	// Item usage handler
	UFUNCTION()
	void OnItemUsed(class UItemBase* Item);
};

