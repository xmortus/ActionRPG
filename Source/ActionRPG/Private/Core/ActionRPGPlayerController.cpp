// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/ActionRPGPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Items/Pickups/ItemPickupActor.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "UI/Inventory/InventoryWidget.h"
#include "UI/Skills/UnlockedSkillsPanelWidget.h"
#include "UI/Progression/CharacterStatusPanelWidget.h"
#include "UI/Progression/ClassSelectionPanelWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/Skills/SkillManagerComponent.h"

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

		if (OffhandAction)
		{
			EnhancedInputComponent->BindAction(OffhandAction, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnOffhand);
		}

		if (DodgeAction)
		{
			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnDodge);
		}

		if (OpenInventoryAction)
		{
			EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnOpenInventory);
		}

		if (OpenSkillPanelAction)
		{
			EnhancedInputComponent->BindAction(OpenSkillPanelAction, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnOpenSkillPanel);
		}

		if (OpenCharacterStatusPanelAction)
		{
			EnhancedInputComponent->BindAction(OpenCharacterStatusPanelAction, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnOpenCharacterStatusPanel);
		}

		if (OpenClassPanelAction)
		{
			EnhancedInputComponent->BindAction(OpenClassPanelAction, ETriggerEvent::Started, this, &AActionRPGPlayerController::OnOpenClassPanel);
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

		// Quick-Use Bar bindings (for consumables - Phase 2)
		if (QuickUseSlot9Action)
		{
			EnhancedInputComponent->BindAction(QuickUseSlot9Action, ETriggerEvent::Triggered, this, &AActionRPGPlayerController::OnQuickUseSlot9);
		}

		if (QuickUseSlot10Action)
		{
			EnhancedInputComponent->BindAction(QuickUseSlot10Action, ETriggerEvent::Triggered, this, &AActionRPGPlayerController::OnQuickUseSlot10);
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

	// Get world context
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnInteract: World is null"));
		return;
	}

	// Get mouse cursor position and direction for line trace (cursor-based pickup)
	FVector TraceStart;
	FVector TraceDirection;
	FVector MouseWorldLocation;
	FVector MouseWorldDirection;
	
	if (DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
	{
		// Use camera position as trace start for top-down gameplay
		if (PlayerCharacter->CameraComponent)
		{
			TraceStart = PlayerCharacter->CameraComponent->GetComponentLocation();
		}
		else
		{
			// Fallback: use player location elevated for top-down view
			TraceStart = PlayerCharacter->GetActorLocation();
			TraceStart.Z += 500.0f;
		}
		TraceDirection = MouseWorldDirection;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnInteract: Failed to deproject mouse position to world"));
		return;
	}

	// Calculate trace end (trace from camera through mouse cursor direction)
	// For top-down view, trace a reasonable distance (1000 units should be enough)
	FVector TraceEnd = TraceStart + (TraceDirection * 1000.0f);

	// Use sphere sweep to create a cone-like detection area
	// This makes it easier to pick up small items
	// Cone parameters: start small at camera, expand as it goes down
	float ConeStartRadius = 10.0f;  // Small radius at start (camera)
	float ConeEndRadius = 100.0f;    // Larger radius at end (ground level)
	
	// Calculate distance for radius interpolation
	float TraceDistance = (TraceEnd - TraceStart).Size();
	
	// Use the larger radius for the sweep (sweep uses a single radius, so we use the max)
	// The expanding effect comes from doing multiple sweeps or using the end radius
	float SweepRadius = ConeEndRadius;
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(PlayerCharacter); // Ignore player character
	QueryParams.bTraceComplex = false; // Use simple collision for performance
	
	// Perform sphere sweep to find items in cone area
	TArray<FHitResult> HitResults;
	FCollisionShape SweepShape = FCollisionShape::MakeSphere(SweepRadius);
	
	// Sweep against world dynamic objects (item pickups use ECC_WorldDynamic)
	bool bHit = World->SweepMultiByChannel(
		HitResults,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_WorldDynamic,
		SweepShape,
		QueryParams
	);
	
	// Find the closest ItemPickupActor in the sweep results
	AItemPickupActor* ClosestPickup = nullptr;
	float ClosestDistance = MAX_FLT;
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	
	if (bHit && HitResults.Num() > 0)
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (AItemPickupActor* Pickup = Cast<AItemPickupActor>(Hit.GetActor()))
			{
				// Find the closest pickup to the player (or to the trace center)
				float Distance = FVector::Dist(PlayerLocation, Pickup->GetActorLocation());
				if (Distance < ClosestDistance)
				{
					ClosestDistance = Distance;
					ClosestPickup = Pickup;
				}
			}
		}
	}
	
	// If no dynamic objects found, try world static
	if (!ClosestPickup)
	{
		HitResults.Empty();
		bHit = World->SweepMultiByChannel(
			HitResults,
			TraceStart,
			TraceEnd,
			FQuat::Identity,
			ECC_WorldStatic,
			SweepShape,
			QueryParams
		);
		
		if (bHit && HitResults.Num() > 0)
		{
			for (const FHitResult& Hit : HitResults)
			{
				if (AItemPickupActor* Pickup = Cast<AItemPickupActor>(Hit.GetActor()))
				{
					float Distance = FVector::Dist(PlayerLocation, Pickup->GetActorLocation());
					if (Distance < ClosestDistance)
					{
						ClosestDistance = Distance;
						ClosestPickup = Pickup;
					}
				}
			}
		}
	}
	
	// Try to pickup the closest item found
	if (ClosestPickup)
	{
		UE_LOG(LogTemp, Log, TEXT("OnInteract: Found item pickup in cone area: %s (Distance: %.2f)"), 
		       *ClosestPickup->GetName(), ClosestDistance);
		
		if (ClosestPickup->CanPickup(PlayerCharacter))
		{
			UE_LOG(LogTemp, Log, TEXT("OnInteract: Attempting to pickup item: %s"), 
			       *ClosestPickup->GetName());
			
			ClosestPickup->PickupItem(PlayerCharacter);
			UE_LOG(LogTemp, Log, TEXT("OnInteract: Successfully picked up item: %s"), 
			       *ClosestPickup->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OnInteract: Cannot pickup item - validation failed"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("OnInteract: No item pickup found in cone area"));
	}
}

void AActionRPGPlayerController::OnAttack()
{
	AActor* OwnerPawn = GetPawn();
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnAttack - Pawn is null"));
		return;
	}

	USkillManagerComponent* SkillManager = OwnerPawn->FindComponentByClass<USkillManagerComponent>();
	if (!SkillManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnAttack - SkillManagerComponent not found"));
		return;
	}

	if (!SkillManager->ActivateMainHandSkill())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnAttack - Failed to activate main hand skill"));
	}
}

void AActionRPGPlayerController::OnDodge()
{
	// TODO: Implement dodge in Phase 2
	UE_LOG(LogTemp, Warning, TEXT("Dodge pressed"));
}

void AActionRPGPlayerController::OnOffhand()
{
	AActor* OwnerPawn = GetPawn();
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnOffhand - Pawn is null"));
		return;
	}

	USkillManagerComponent* SkillManager = OwnerPawn->FindComponentByClass<USkillManagerComponent>();
	if (!SkillManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnOffhand - SkillManagerComponent not found"));
		return;
	}

	if (!SkillManager->ActivateOffhandSkill())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnOffhand - Failed to activate offhand skill"));
	}
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
		// Check if widget is in viewport AND visible
		// A widget not in the viewport should always be treated as "hidden" (needs to be shown)
		// A widget in the viewport should check its visibility state
		bool bIsInViewport = InventoryWidget->IsInViewport();
		ESlateVisibility CurrentVisibility = InventoryWidget->GetVisibility();
		bool bIsVisible = bIsInViewport && (CurrentVisibility == ESlateVisibility::Visible || CurrentVisibility == ESlateVisibility::SelfHitTestInvisible || CurrentVisibility == ESlateVisibility::HitTestInvisible);
		
		if (bIsVisible)
		{
			// Hide inventory (widget is currently visible in viewport)
			UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenInventory - Hiding inventory (current visibility: %d, in viewport: %s)"), 
				(int32)CurrentVisibility, bIsInViewport ? TEXT("YES") : TEXT("NO"));
			
			// For widgets added via AddToViewport(), just hide them instead of removing
			// This avoids the "no UMG parent" warning and allows widget reuse
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
			
			// Keep mouse cursor visible for top-down gameplay (targeting/interaction)
			SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
			bShowMouseCursor = true;
			SetPause(false);
		}
		else
		{
			// Show inventory (widget is currently hidden or not in viewport)
			UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenInventory - Showing inventory (current visibility: %d, in viewport: %s)"), 
				(int32)CurrentVisibility, bIsInViewport ? TEXT("YES") : TEXT("NO"));
			
			// Add to viewport if not already added (only needed on first show)
			if (!bIsInViewport)
			{
				// Add to viewport with Z-Order 50 (lower than quick-use bar which should be at 200+)
				// This ensures the quick-use bar remains interactable when inventory is open
				// NOTE: The quick-use bar widget must be added with Z-Order 200 or higher in Blueprint/HUD
				InventoryWidget->AddToViewport(50);
				UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenInventory - Widget added to viewport with Z-Order 50 (quick-use bar should be at 200+)"));
			}
			
			// Ensure widget is visible but does not block clicks in empty areas
			InventoryWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			
			// Log widget status for debugging
			if (InventoryWidget)
			{
				FVector2D WidgetSize = InventoryWidget->GetDesiredSize();
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

void AActionRPGPlayerController::OnOpenSkillPanel()
{
	UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenSkillPanel - Skill panel key pressed"));

	// Create widget if it doesn't exist
	if (!SkillPanelWidget)
	{
		if (SkillPanelWidgetClass)
		{
			SkillPanelWidget = CreateWidget<UUnlockedSkillsPanelWidget>(this, SkillPanelWidgetClass);
			if (SkillPanelWidget)
			{
				UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenSkillPanel - Skill panel widget created"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ActionRPGPlayerController::OnOpenSkillPanel - Failed to create skill panel widget! Check SkillPanelWidgetClass is set in Blueprint"));
				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerController::OnOpenSkillPanel - SkillPanelWidgetClass is not set! Set it in Blueprint"));
			return;
		}
	}

	// Toggle widget visibility
	if (SkillPanelWidget)
	{
		bool bIsInViewport = SkillPanelWidget->IsInViewport();
		ESlateVisibility CurrentVisibility = SkillPanelWidget->GetVisibility();
		bool bIsVisible = bIsInViewport && (CurrentVisibility == ESlateVisibility::Visible || CurrentVisibility == ESlateVisibility::SelfHitTestInvisible || CurrentVisibility == ESlateVisibility::HitTestInvisible);

		if (bIsVisible)
		{
			UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenSkillPanel - Hiding skill panel (current visibility: %d, in viewport: %s)"),
				(int32)CurrentVisibility, bIsInViewport ? TEXT("YES") : TEXT("NO"));

			SkillPanelWidget->SetVisibility(ESlateVisibility::Collapsed);
			SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
			bShowMouseCursor = true;
			SetPause(false);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenSkillPanel - Showing skill panel (current visibility: %d, in viewport: %s)"),
				(int32)CurrentVisibility, bIsInViewport ? TEXT("YES") : TEXT("NO"));

			if (!bIsInViewport)
			{
				// Add to viewport with Z-Order 100 (above inventory, below quick-use bar)
				SkillPanelWidget->AddToViewport(100);
				UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenSkillPanel - Widget added to viewport with Z-Order 100 (quick-use bar should be at 200+)"));
			}

			// Ensure widget is visible but does not block clicks in empty areas
			SkillPanelWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
			bShowMouseCursor = true;
		}
	}
}

void AActionRPGPlayerController::OnOpenCharacterStatusPanel()
{
	UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenCharacterStatusPanel - Character status panel key pressed"));

	if (!CharacterStatusPanelWidget)
	{
		if (CharacterStatusPanelWidgetClass)
		{
			CharacterStatusPanelWidget = CreateWidget<UCharacterStatusPanelWidget>(this, CharacterStatusPanelWidgetClass);
			if (!CharacterStatusPanelWidget)
			{
				UE_LOG(LogTemp, Error, TEXT("ActionRPGPlayerController::OnOpenCharacterStatusPanel - Failed to create character status panel widget! Check CharacterStatusPanelWidgetClass is set in Blueprint"));
				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerController::OnOpenCharacterStatusPanel - CharacterStatusPanelWidgetClass is not set! Set it in Blueprint"));
			return;
		}
	}

	bool bIsInViewport = CharacterStatusPanelWidget->IsInViewport();
	ESlateVisibility CurrentVisibility = CharacterStatusPanelWidget->GetVisibility();
	bool bIsVisible = bIsInViewport && (CurrentVisibility == ESlateVisibility::Visible || CurrentVisibility == ESlateVisibility::SelfHitTestInvisible || CurrentVisibility == ESlateVisibility::HitTestInvisible);

	if (bIsVisible)
	{
		CharacterStatusPanelWidget->SetVisibility(ESlateVisibility::Collapsed);
		SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		bShowMouseCursor = true;
		SetPause(false);
	}
	else
	{
		if (!bIsInViewport)
		{
			CharacterStatusPanelWidget->AddToViewport(110);
		}

		CharacterStatusPanelWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		bShowMouseCursor = true;
	}
}

void AActionRPGPlayerController::OnOpenClassPanel()
{
	UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerController::OnOpenClassPanel - Class panel key pressed"));

	if (!ClassSelectionPanelWidget)
	{
		if (ClassSelectionPanelWidgetClass)
		{
			ClassSelectionPanelWidget = CreateWidget<UClassSelectionPanelWidget>(this, ClassSelectionPanelWidgetClass);
			if (!ClassSelectionPanelWidget)
			{
				UE_LOG(LogTemp, Error, TEXT("ActionRPGPlayerController::OnOpenClassPanel - Failed to create class panel widget! Check ClassSelectionPanelWidgetClass is set in Blueprint"));
				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerController::OnOpenClassPanel - ClassSelectionPanelWidgetClass is not set! Set it in Blueprint"));
			return;
		}
	}

	bool bIsInViewport = ClassSelectionPanelWidget->IsInViewport();
	ESlateVisibility CurrentVisibility = ClassSelectionPanelWidget->GetVisibility();
	bool bIsVisible = bIsInViewport && (CurrentVisibility == ESlateVisibility::Visible || CurrentVisibility == ESlateVisibility::SelfHitTestInvisible || CurrentVisibility == ESlateVisibility::HitTestInvisible);

	if (bIsVisible)
	{
		ClassSelectionPanelWidget->SetVisibility(ESlateVisibility::Collapsed);
		SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		bShowMouseCursor = true;
		SetPause(false);
	}
	else
	{
		if (!bIsInViewport)
		{
			ClassSelectionPanelWidget->AddToViewport(120);
		}

		ClassSelectionPanelWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		bShowMouseCursor = true;
	}
}

void AActionRPGPlayerController::OnSkillSlot1()
{
	ActivateSkillSlot(0);
}

void AActionRPGPlayerController::OnSkillSlot2()
{
	ActivateSkillSlot(1);
}

void AActionRPGPlayerController::OnSkillSlot3()
{
	ActivateSkillSlot(2);
}

void AActionRPGPlayerController::OnSkillSlot4()
{
	ActivateSkillSlot(3);
}

void AActionRPGPlayerController::OnSkillSlot5()
{
	ActivateSkillSlot(4);
}

void AActionRPGPlayerController::OnSkillSlot6()
{
	ActivateSkillSlot(5);
}

void AActionRPGPlayerController::OnSkillSlot7()
{
	ActivateSkillSlot(6);
}

void AActionRPGPlayerController::OnSkillSlot8()
{
	ActivateSkillSlot(7);
}

void AActionRPGPlayerController::ActivateSkillSlot(int32 SlotIndex)
{
	AActor* OwnerPawn = GetPawn();
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActivateSkillSlot - Pawn is null (slot %d)"), SlotIndex);
		return;
	}

	USkillManagerComponent* SkillManager = OwnerPawn->FindComponentByClass<USkillManagerComponent>();
	if (!SkillManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActivateSkillSlot - SkillManagerComponent not found (slot %d)"), SlotIndex);
		return;
	}

	if (!SkillManager->CanActivateSkillFromSlot(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("ActivateSkillSlot - Skill in slot %d cannot be activated"), SlotIndex);
		return;
	}

	bool bActivated = SkillManager->ActivateSkillFromSlot(SlotIndex);
	if (!bActivated)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActivateSkillSlot - Failed to activate skill in slot %d"), SlotIndex);
	}
}

void AActionRPGPlayerController::OnQuickUseSlot9(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		AActionRPGPlayerCharacter* PlayerCharacter = Cast<AActionRPGPlayerCharacter>(GetPawn());
		if (!PlayerCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnQuickUseSlot9: Pawn is not AActionRPGPlayerCharacter or is NULL"));
			return;
		}

		UInventoryComponent* InventoryComp = PlayerCharacter->InventoryComponent;
		if (InventoryComp)
		{
			bool bSuccess = InventoryComp->UseQuickUseSlot(8); // Slot 9 (index 8)
			if (bSuccess)
			{
				UE_LOG(LogTemp, Log, TEXT("OnQuickUseSlot9 - Used quick-use slot 9 successfully"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("OnQuickUseSlot9 - Failed to use quick-use slot 9 (slot may be empty)"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OnQuickUseSlot9: InventoryComponent not found"));
		}
	}
}

void AActionRPGPlayerController::OnQuickUseSlot10(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		AActionRPGPlayerCharacter* PlayerCharacter = Cast<AActionRPGPlayerCharacter>(GetPawn());
		if (!PlayerCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnQuickUseSlot10: Pawn is not AActionRPGPlayerCharacter or is NULL"));
			return;
		}

		UInventoryComponent* InventoryComp = PlayerCharacter->InventoryComponent;
		if (InventoryComp)
		{
			bool bSuccess = InventoryComp->UseQuickUseSlot(9); // Slot 10 (index 9)
			if (bSuccess)
			{
				UE_LOG(LogTemp, Log, TEXT("OnQuickUseSlot10 - Used quick-use slot 10 successfully"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("OnQuickUseSlot10 - Failed to use quick-use slot 10 (slot may be empty)"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OnQuickUseSlot10: InventoryComponent not found"));
		}
	}
}

UInputAction* AActionRPGPlayerController::GetInputActionForQuickUseSlot(int32 SlotIndex) const
{
	// Validate slot index
	if (SlotIndex < 0 || SlotIndex > 9)
	{
		return nullptr;
	}

	// Return the InputAction for this slot
	if (SlotIndex < 8)
	{
		// Slots 0-7 (displayed as 1-8) - Skill slots
		switch (SlotIndex)
		{
		case 0: return SkillSlot1Action;
		case 1: return SkillSlot2Action;
		case 2: return SkillSlot3Action;
		case 3: return SkillSlot4Action;
		case 4: return SkillSlot5Action;
		case 5: return SkillSlot6Action;
		case 6: return SkillSlot7Action;
		case 7: return SkillSlot8Action;
		}
	}
	else
	{
		// Slots 8-9 (displayed as 9-0) - Quick-use slots
		if (SlotIndex == 8)
		{
			return QuickUseSlot9Action;
		}
		else if (SlotIndex == 9)
		{
			return QuickUseSlot10Action;
		}
	}

	return nullptr;
}