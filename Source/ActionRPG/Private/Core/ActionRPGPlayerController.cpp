// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/ActionRPGPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/ActionRPGPlayerCharacter.h"

AActionRPGPlayerController::AActionRPGPlayerController()
{
	// For top-down action RPG, show mouse cursor for aiming
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	
	// Set default mouse cursor
	DefaultMouseCursor = EMouseCursor::Crosshairs;
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

		// Bind skill slot actions
		if (SkillSlot1Action)
		{
			EnhancedInputComponent->BindAction(SkillSlot1Action, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnSkillSlot1);
		}

		if (SkillSlot2Action)
		{
			EnhancedInputComponent->BindAction(SkillSlot2Action, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnSkillSlot2);
		}

		if (SkillSlot3Action)
		{
			EnhancedInputComponent->BindAction(SkillSlot3Action, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnSkillSlot3);
		}

		if (SkillSlot4Action)
		{
			EnhancedInputComponent->BindAction(SkillSlot4Action, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnSkillSlot4);
		}

		if (SkillSlot5Action)
		{
			EnhancedInputComponent->BindAction(SkillSlot5Action, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnSkillSlot5);
		}

		if (SkillSlot6Action)
		{
			EnhancedInputComponent->BindAction(SkillSlot6Action, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnSkillSlot6);
		}

		if (SkillSlot7Action)
		{
			EnhancedInputComponent->BindAction(SkillSlot7Action, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnSkillSlot7);
		}

		if (SkillSlot8Action)
		{
			EnhancedInputComponent->BindAction(SkillSlot8Action, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnSkillSlot8);
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
	
	// Debug logging (disabled)
	// UE_LOG(LogTemp, Log, TEXT("OnMove called: X=%.2f, Y=%.2f"), MovementVector.X, MovementVector.Y);

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

void AActionRPGPlayerController::OnSkillSlot1()
{
	// TODO: Implement skill activation in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Skill Slot 1 pressed"));
}

void AActionRPGPlayerController::OnSkillSlot2()
{
	// TODO: Implement skill activation in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Skill Slot 2 pressed"));
}

void AActionRPGPlayerController::OnSkillSlot3()
{
	// TODO: Implement skill activation in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Skill Slot 3 pressed"));
}

void AActionRPGPlayerController::OnSkillSlot4()
{
	// TODO: Implement skill activation in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Skill Slot 4 pressed"));
}

void AActionRPGPlayerController::OnSkillSlot5()
{
	// TODO: Implement skill activation in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Skill Slot 5 pressed"));
}

void AActionRPGPlayerController::OnSkillSlot6()
{
	// TODO: Implement skill activation in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Skill Slot 6 pressed"));
}

void AActionRPGPlayerController::OnSkillSlot7()
{
	// TODO: Implement skill activation in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Skill Slot 7 pressed"));
}

void AActionRPGPlayerController::OnSkillSlot8()
{
	// TODO: Implement skill activation in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Skill Slot 8 pressed"));
}
