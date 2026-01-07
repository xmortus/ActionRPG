// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/ActionRPGPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/ActionRPGPlayerCharacter.h"

AActionRPGPlayerController::AActionRPGPlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
}

void AActionRPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController BeginPlay"));

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UE_LOG(LogTemp, Log, TEXT("Input Mapping Context added successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DefaultMappingContext is NULL - make sure it's assigned in Blueprint!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get EnhancedInputLocalPlayerSubsystem!"));
	}
}

void AActionRPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UE_LOG(LogTemp, Log, TEXT("SetupInputComponent called"));

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		UE_LOG(LogTemp, Log, TEXT("EnhancedInputComponent found"));
		
		// Bind movement actions
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AActionRPGPlayerController::OnMove);
			UE_LOG(LogTemp, Log, TEXT("MoveAction bound"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MoveAction is NULL - make sure it's assigned in Blueprint!"));
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AActionRPGPlayerController::OnLook);
			UE_LOG(LogTemp, Log, TEXT("LookAction bound"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LookAction is NULL"));
		}

		// Bind action inputs
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnInteract);
		}

		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnAttack);
		}

		if (DodgeAction)
		{
			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnDodge);
		}

		if (OpenInventoryAction)
		{
			EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnOpenInventory);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast InputComponent to UEnhancedInputComponent!"));
		UE_LOG(LogTemp, Error, TEXT("Make sure Enhanced Input is enabled in Build.cs"));
	}
}

void AActionRPGPlayerController::OnMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	// Debug logging
	UE_LOG(LogTemp, Log, TEXT("OnMove called: X=%.2f, Y=%.2f"), MovementVector.X, MovementVector.Y);

	if (AActionRPGPlayerCharacter* PlayerCharacter = Cast<AActionRPGPlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->Move(MovementVector);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnMove: Pawn is not AActionRPGPlayerCharacter or is NULL"));
	}
}

void AActionRPGPlayerController::OnLook(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();

	if (AActionRPGPlayerCharacter* PlayerCharacter = Cast<AActionRPGPlayerCharacter>(GetPawn()))
	{
		PlayerCharacter->Look(LookVector);
	}
}

void AActionRPGPlayerController::OnInteract()
{
	// TODO: Implement interaction in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Interact pressed"));
}

void AActionRPGPlayerController::OnAttack()
{
	// TODO: Implement attack in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Attack pressed"));
}

void AActionRPGPlayerController::OnDodge()
{
	// TODO: Implement dodge in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Dodge pressed"));
}

void AActionRPGPlayerController::OnOpenInventory()
{
	// TODO: Implement inventory UI in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Open Inventory pressed"));
}

