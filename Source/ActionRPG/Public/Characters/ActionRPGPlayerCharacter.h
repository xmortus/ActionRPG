// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ActionRPGCharacter.h"
#include "ActionRPGPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInventoryComponent> InventoryComponent;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Rotate character to face mouse cursor
	void RotateToMouseCursor();
};

