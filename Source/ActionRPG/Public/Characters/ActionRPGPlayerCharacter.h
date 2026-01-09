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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<class UInventoryComponent> InventoryComponent;

	// Health Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (ClampMin = "0.0"))
	float CurrentHealth = 50.0f;

	// Health Functions
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Stats")
	bool IsHealthAtMax() const { return CurrentHealth >= MaxHealth; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Stats")
	float GetHealthPercent() const { return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Rotate character to face mouse cursor
	void RotateToMouseCursor();

	// Item usage handler
	UFUNCTION()
	void OnItemUsed(class UItemBase* Item);
};

