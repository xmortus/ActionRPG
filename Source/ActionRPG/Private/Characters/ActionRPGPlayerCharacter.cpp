// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/ActionRPGPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Core/ItemTypes.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

AActionRPGPlayerCharacter::AActionRPGPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set default values for player character
	PrimaryActorTick.bCanEverTick = true;
	
	// Don't rotate character to controller rotation (top-down camera)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure Character Movement for top-down gameplay
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		// Don't orient rotation to movement - we handle rotation manually via mouse cursor
		MovementComp->bOrientRotationToMovement = false;
		
		// Don't use controller desired rotation - we set rotation directly
		MovementComp->bUseControllerDesiredRotation = false;
		
		// Set rotation rate for smooth rotation (if we add interpolation later)
		MovementComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // 540 degrees per second on Yaw
	}

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

	// Create Inventory Component
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void AActionRPGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Verify character movement component exists
	if (!GetCharacterMovement())
	{
		UE_LOG(LogTemp, Error, TEXT("ActionRPGPlayerCharacter: CharacterMovementComponent is NULL!"));
	}
	else
	{
		// Ensure movement settings are correct (in case Blueprint overrides them)
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}

	// Set camera as view target for player controller
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetViewTarget(this);
	}

	// Initialize health
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerCharacter: Health initialized - %.1f/%.1f"), CurrentHealth, MaxHealth);

	// Bind to inventory item used event
	if (InventoryComponent)
	{
		InventoryComponent->OnItemUsed.AddDynamic(this, &AActionRPGPlayerCharacter::OnItemUsed);
		UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerCharacter: Bound to InventoryComponent OnItemUsed event"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter: InventoryComponent is NULL, cannot bind to OnItemUsed"));
	}
}

void AActionRPGPlayerCharacter::Move(const FVector2D& MovementVector)
{
	// Early return if no movement input
	if (MovementVector.IsZero())
	{
		return;
	}

	// Get character's forward and right vectors based on current rotation
	// MovementVector.X = forward/backward (W/S keys)
	// MovementVector.Y = left/right (A/D keys)
	FRotator CharacterRotation = GetActorRotation();
	FVector ForwardVector = FRotationMatrix(CharacterRotation).GetUnitAxis(EAxis::X); // Forward
	FVector RightVector = FRotationMatrix(CharacterRotation).GetUnitAxis(EAxis::Y);  // Right
	
	// Calculate movement direction relative to character's facing direction
	// This allows movement relative to where the character is facing (top-down style)
	FVector Direction = (ForwardVector * MovementVector.X) + (RightVector * MovementVector.Y);
	Direction.Z = 0.0f; // Keep movement on horizontal plane
	Direction.Normalize();

	// Apply movement to Character Movement Component
	if (GetCharacterMovement())
	{
		AddMovementInput(Direction, 1.0f);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Move: CharacterMovementComponent is NULL!"));
	}
}

void AActionRPGPlayerCharacter::Look(const FVector2D& LookVector)
{
	// For top-down gameplay, character rotation is handled by RotateToMouseCursor() in Tick
	// This function is called by the input system but doesn't need to do anything
	// The LookVector parameter could be used for camera rotation if needed in the future
}

void AActionRPGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Continuously update character rotation to face mouse cursor
	RotateToMouseCursor();
}

void AActionRPGPlayerCharacter::RotateToMouseCursor()
{
	// Get the player controller (required for mouse projection)
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	// Project mouse cursor position to world space
	FVector WorldLocation;
	FVector WorldDirection;
	
	if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		// Calculate intersection point on the ground plane at character's height
		FVector CharacterLocation = GetActorLocation();
		float PlaneZ = CharacterLocation.Z;
		
		// Calculate intersection with horizontal plane using ray-plane intersection
		if (FMath::Abs(WorldDirection.Z) > 0.001f) // Avoid division by zero
		{
			// Ray-plane intersection formula: T = (PlaneZ - RayOrigin.Z) / RayDirection.Z
			float T = (PlaneZ - WorldLocation.Z) / WorldDirection.Z;
			FVector TargetLocation = WorldLocation + (WorldDirection * T);
			TargetLocation.Z = CharacterLocation.Z; // Ensure target is at character's height
			
			// Calculate direction from character to target location
			FVector Direction = TargetLocation - CharacterLocation;
			Direction.Z = 0.0f; // Keep rotation on horizontal plane only
			
			// Only rotate if direction is significant (avoid jitter from tiny movements)
			if (!Direction.IsNearlyZero(1.0f))
			{
				// Calculate rotation to face the target
				FRotator NewRotation = Direction.Rotation();
				
				// Set character rotation to face mouse cursor (Yaw only, no pitch/roll)
				SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
			}
		}
	}
}

void AActionRPGPlayerCharacter::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter::Heal - Invalid heal amount: %.2f"), HealAmount);
		return;
	}

	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	float ActualHeal = CurrentHealth - OldHealth;

	UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerCharacter::Heal - Healed %.1f health (%.1f -> %.1f/%.1f)"), 
		ActualHeal, OldHealth, CurrentHealth, MaxHealth);
}

void AActionRPGPlayerCharacter::OnItemUsed(UItemBase* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter::OnItemUsed - EVENT FIRED! Item: %s"), 
		Item ? TEXT("Valid") : TEXT("NULL"));

	if (!Item || !Item->ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter::OnItemUsed - Item or ItemData is NULL"));
		return;
	}

	FName ItemID = Item->ItemData->ItemID;
	EItemType ItemType = Item->ItemData->Type;
	FString ItemIDString = ItemID.ToString();
	FString ItemNameString = Item->ItemData->ItemName.ToString();

	UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter::OnItemUsed - Item used: %s (ID: %s, Type: %d, Current Health: %.1f/%.1f)"), 
		*ItemNameString, *ItemIDString, (int32)ItemType, CurrentHealth, MaxHealth);

	// Handle health potion - check ItemID (case-insensitive comparison)
	FString ItemIDLower = ItemIDString.ToLower();
	if (ItemIDLower == TEXT("healthpotion"))
	{
		UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter::OnItemUsed - Health potion detected!"));
		
		// Default healing amount (can be customized per item later)
		float HealAmount = 25.0f; // Heal 25 health per potion
		
		// Check if health is already at max
		if (IsHealthAtMax())
		{
			UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter::OnItemUsed - Health already at max (%.1f/%.1f), cannot use health potion"), 
				CurrentHealth, MaxHealth);
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter::OnItemUsed - Healing player for %.1f health (Current: %.1f/%.1f)"), 
			HealAmount, CurrentHealth, MaxHealth);
		
		Heal(HealAmount);
		
		UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter::OnItemUsed - Health potion used successfully! New health: %.1f/%.1f"), 
			CurrentHealth, MaxHealth);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerCharacter::OnItemUsed - Item is not a health potion (ID: %s)"), *ItemIDString);
	}
	// Add other consumable types here as needed
}
