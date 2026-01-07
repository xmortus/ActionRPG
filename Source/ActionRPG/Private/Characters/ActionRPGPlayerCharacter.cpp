// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/ActionRPGPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

AActionRPGPlayerCharacter::AActionRPGPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set default values for player character
	PrimaryActorTick.bCanEverTick = true;
	
	// Enable tick for movement updates
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AActionRPGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Verify character movement component exists
	if (!GetCharacterMovement())
	{
		UE_LOG(LogTemp, Error, TEXT("ActionRPGPlayerCharacter: CharacterMovementComponent is NULL!"));
	}
}

void AActionRPGPlayerCharacter::Move(const FVector2D& MovementVector)
{
	// Debug logging
	UE_LOG(LogTemp, VeryVerbose, TEXT("Move called with Vector: X=%.2f, Y=%.2f"), MovementVector.X, MovementVector.Y);
	
	if (MovementVector.IsZero())
	{
		return;
	}

	// For top-down movement, use world-space direction
	// MovementVector.X = forward/backward (W/S)
	// MovementVector.Y = left/right (A/D)
	// In Unreal: X = forward, Y = right
	FVector Direction = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	Direction.Normalize();

	// Apply movement
	if (GetCharacterMovement())
	{
		AddMovementInput(Direction, 1.0f);
		UE_LOG(LogTemp, VeryVerbose, TEXT("Movement applied: Direction=(%.2f, %.2f, %.2f)"), Direction.X, Direction.Y, Direction.Z);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Move: CharacterMovementComponent is NULL!"));
	}
}

void AActionRPGPlayerCharacter::Look(const FVector2D& LookVector)
{
	// For top-down camera, look input may not be needed
	// This is a placeholder for future camera control
	// TODO: Implement camera look/rotation if needed
}

