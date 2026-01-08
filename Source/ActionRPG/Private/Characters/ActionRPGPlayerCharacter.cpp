// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/ActionRPGPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

AActionRPGPlayerCharacter::AActionRPGPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set default values for player character
	PrimaryActorTick.bCanEverTick = true;
	
	// Don't rotate character to controller rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create Spring Arm Component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 500.0f; // Distance from character
	SpringArmComponent->bUsePawnControlRotation = false; // Don't rotate with controller
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;
	
	// Set Spring Arm rotation for top-down view
	SpringArmComponent->SetRelativeRotation(FRotator(-65.0f, 0.0f, 0.0f)); // 65 degrees down
	SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f)); // Position above character

	// Create Camera Component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false; // Camera doesn't rotate with controller
}

void AActionRPGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Verify character movement component exists
	if (!GetCharacterMovement())
	{
		UE_LOG(LogTemp, Error, TEXT("ActionRPGPlayerCharacter: CharacterMovementComponent is NULL!"));
	}

	// Set camera as view target
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetViewTarget(this);
	}
}

void AActionRPGPlayerCharacter::Move(const FVector2D& MovementVector)
{
	// Debug logging (disabled)
	// UE_LOG(LogTemp, VeryVerbose, TEXT("Move called with Vector: X=%.2f, Y=%.2f"), MovementVector.X, MovementVector.Y);
	
	if (MovementVector.IsZero())
	{
		return;
	}

	// Get character's forward and right vectors based on current rotation
	// MovementVector.X = forward/backward (W/S)
	// MovementVector.Y = left/right (A/D)
	FRotator CharacterRotation = GetActorRotation();
	FVector ForwardVector = FRotationMatrix(CharacterRotation).GetUnitAxis(EAxis::X); // Forward
	FVector RightVector = FRotationMatrix(CharacterRotation).GetUnitAxis(EAxis::Y);  // Right
	
	// Calculate movement direction relative to character's facing direction
	FVector Direction = (ForwardVector * MovementVector.X) + (RightVector * MovementVector.Y);
	Direction.Z = 0.0f; // Keep movement on horizontal plane
	Direction.Normalize();

	// Apply movement
	if (GetCharacterMovement())
	{
		AddMovementInput(Direction, 1.0f);
		// Debug logging (disabled)
		// UE_LOG(LogTemp, VeryVerbose, TEXT("Movement applied: Direction=(%.2f, %.2f, %.2f)"), Direction.X, Direction.Y, Direction.Z);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Move: CharacterMovementComponent is NULL!"));
	}
}

void AActionRPGPlayerCharacter::Look(const FVector2D& LookVector)
{
	// For top-down, we use mouse cursor position to rotate character
	// The LookVector from mouse movement can be used for camera rotation if needed
	// But character rotation is handled by RotateToMouseCursor() in Tick
}

void AActionRPGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Continuously update character rotation to face mouse cursor
	RotateToMouseCursor();
}

void AActionRPGPlayerCharacter::RotateToMouseCursor()
{
	// Get the player controller
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	// Get mouse position and convert to world position
	FVector WorldLocation;
	FVector WorldDirection;
	
	if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		// Calculate the point on the ground plane where the mouse is pointing
		FVector CharacterLocation = GetActorLocation();
		float PlaneZ = CharacterLocation.Z;
		
		// Calculate intersection with horizontal plane at character's Z level
		if (FMath::Abs(WorldDirection.Z) > 0.001f) // Avoid division by zero
		{
			float T = (PlaneZ - WorldLocation.Z) / WorldDirection.Z;
			FVector TargetLocation = WorldLocation + (WorldDirection * T);
			TargetLocation.Z = CharacterLocation.Z; // Keep at character's height
			
			// Calculate direction from character to target
			FVector Direction = TargetLocation - CharacterLocation;
			Direction.Z = 0.0f; // Keep rotation on horizontal plane
			
			if (!Direction.IsNearlyZero(1.0f)) // Only rotate if direction is significant
			{
				// Calculate rotation to face the target
				FRotator NewRotation = Direction.Rotation();
				
				// Rotate the character to face the mouse cursor
				SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
			}
		}
	}
}

