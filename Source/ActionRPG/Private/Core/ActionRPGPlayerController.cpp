// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/ActionRPGPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Items/Pickups/ItemPickupActor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UI/Inventory/InventoryWidget.h"

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
	UE_LOG(LogTemp, Log, TEXT("OnInteract called"));

	// Get player character
	AActionRPGPlayerCharacter* PlayerCharacter = Cast<AActionRPGPlayerCharacter>(GetPawn());
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnInteract: Pawn is not AActionRPGPlayerCharacter or is NULL"));
		return;
	}

	// Find all ItemPickupActors in the world
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItemPickupActor::StaticClass(), FoundActors);

	if (FoundActors.Num() == 0)
	{
		UE_LOG(LogTemp, Verbose, TEXT("OnInteract: No item pickups found in world"));
		return;
	}

	// Find the nearest pickup in range
	AItemPickupActor* NearestPickup = nullptr;
	float NearestDistance = MAX_FLT;
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	for (AActor* Actor : FoundActors)
	{
		if (AItemPickupActor* Pickup = Cast<AItemPickupActor>(Actor))
		{
			// Check if player is in range
			if (Pickup->IsPlayerInRange(PlayerCharacter))
			{
				float Distance = FVector::Dist(PlayerLocation, Pickup->GetActorLocation());
				if (Distance < NearestDistance)
				{
					NearestDistance = Distance;
					NearestPickup = Pickup;
				}
			}
		}
	}

	// Try to interact with the nearest pickup
	if (NearestPickup)
	{
		UE_LOG(LogTemp, Log, TEXT("OnInteract: Attempting to interact with pickup: %s (Distance: %.2f)"), 
		       *NearestPickup->GetName(), NearestDistance);
		
		if (NearestPickup->TryInteract(PlayerCharacter))
		{
			UE_LOG(LogTemp, Log, TEXT("OnInteract: Successfully interacted with pickup: %s"), 
			       *NearestPickup->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OnInteract: Failed to interact with pickup: %s"), 
			       *NearestPickup->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("OnInteract: No pickups in range"));
	}
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
	UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenInventory - Inventory key pressed"));

	// Create widget if it doesn't exist
	if (!InventoryWidget)
	{
		if (InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);
			if (InventoryWidget)
			{
				UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenInventory - Inventory widget created"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ActionRPGPlayerController::OnOpenInventory - Failed to create inventory widget! Check InventoryWidgetClass is set in Blueprint"));
				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerController::OnOpenInventory - InventoryWidgetClass is not set! Set it in Blueprint"));
			return;
		}
	}

	// Toggle widget visibility
	if (InventoryWidget)
	{
		if (InventoryWidget->IsInViewport())
		{
			// Hide inventory
			UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenInventory - Hiding inventory"));
			InventoryWidget->RemoveFromParent();
			// Keep mouse cursor visible for top-down gameplay (targeting/interaction)
			SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
			bShowMouseCursor = true;
			SetPause(false);
		}
		else
		{
			// Show inventory
			UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenInventory - Showing inventory"));
			
			// Ensure widget is visible
			InventoryWidget->SetVisibility(ESlateVisibility::Visible);
			
			// Add to viewport with Z-Order to ensure it's on top (higher number = on top)
			InventoryWidget->AddToViewport(100);
			
			// Log widget status for debugging
			if (InventoryWidget)
			{
				FVector2D WidgetSize = InventoryWidget->GetDesiredSize();
				UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenInventory - Widget added to viewport"));
				UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenInventory - Widget visibility: %d"), (int32)InventoryWidget->GetVisibility());
				UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenInventory - Widget is in viewport: %s"), InventoryWidget->IsInViewport() ? TEXT("YES") : TEXT("NO"));
				
				// Note: GetDesiredSize() may return 0.0 for Canvas Panel widgets until after layout pass
				// This is normal for Canvas Panel-based widgets and doesn't indicate a problem if the widget renders correctly
				if (WidgetSize.X > 0 && WidgetSize.Y > 0)
				{
					UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenInventory - Widget size: %.1f x %.1f"), WidgetSize.X, WidgetSize.Y);
				}
				else
				{
					UE_LOG(LogTemp, Verbose, TEXT("ActionRPGPlayerController::OnOpenInventory - Widget size not yet calculated (%.1f x %.1f) - this is normal for Canvas Panel widgets. If widget renders correctly, size is set properly."), WidgetSize.X, WidgetSize.Y);
				}
			}
			
			SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
			bShowMouseCursor = true;
			// Optional: Pause game when inventory is open
			// SetPause(true);
		}
	}
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