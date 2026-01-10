// Copyright Epic Games, Inc. All Rights Reserved.

#include "Items/Pickups/ItemPickupActor.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Data/ItemDatabase.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemTypes.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"

AItemPickupActor::AItemPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create collision component as root (better for overlap detection)
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetSphereRadius(50.0f);
	
	// Set collision settings for overlap detection
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	// Ensure collision is generated
	CollisionComponent->SetGenerateOverlapEvents(true);

	// Create mesh component for visual representation
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void AItemPickupActor::BeginPlay()
{
	Super::BeginPlay();

	// Ensure collision component is valid and registered
	if (CollisionComponent)
	{
		// Verify collision settings
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		CollisionComponent->SetGenerateOverlapEvents(true);
		
		// Bind overlap event
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemPickupActor::OnOverlapBegin);
		
		UE_LOG(LogTemp, Log, TEXT("ItemPickupActor: BeginPlay - CollisionComponent configured, Sphere Radius: %.2f"), 
		       CollisionComponent->GetScaledSphereRadius());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemPickupActor: BeginPlay - CollisionComponent is NULL!"));
	}

	// Setup visuals
	SetupVisuals();
	
	// Log pickup info for debugging
	if (ItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("ItemPickupActor: Spawned - %s (Quantity: %d)"), 
		       *ItemData->ItemName.ToString(), Quantity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor: Spawned without ItemData set!"));
	}
}

void AItemPickupActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                      bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("ItemPickupActor: OnOverlapBegin called - OtherActor: %s"), 
	       OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

	// Cast to player character
	if (AActionRPGPlayerCharacter* Player = Cast<AActionRPGPlayerCharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Log, TEXT("ItemPickupActor: Player overlapped - %s (Player: %s)"), 
		       *GetName(), *Player->GetName());

		// Only auto-pickup if enabled
		if (bAutoPickupOnOverlap)
		{
			// Check if pickup is valid
			if (CanPickup(Player))
			{
				PickupItem(Player);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor: Cannot pickup item - validation failed"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Verbose, TEXT("ItemPickupActor: Auto-pickup disabled - player must press interact key"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("ItemPickupActor: Overlap with non-player actor: %s"), 
		       OtherActor ? *OtherActor->GetName() : TEXT("NULL"));
	}
}

bool AItemPickupActor::CanPickup(AActionRPGPlayerCharacter* Player) const
{
	UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::CanPickup - Starting validation"));

	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor::CanPickup - Player is NULL"));
		return false;
	}

	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor::CanPickup - ItemData is NULL"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::CanPickup - ItemData: %s, Quantity: %d"), 
	       *ItemData->ItemName.ToString(), Quantity);

	// Get inventory component from player
	UInventoryComponent* InventoryComponent = Player->InventoryComponent;
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor::CanPickup - Player has no InventoryComponent"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::CanPickup - InventoryComponent found, Current Weight: %.2f/%.2f, Items: %d, Capacity: %d"),
	       InventoryComponent->GetCurrentWeight(), InventoryComponent->GetMaxWeight(),
	       InventoryComponent->GetTotalItemCount(), InventoryComponent->GetMaxCapacity());

		// Create temporary item to check if inventory has space
		if (UItemDatabase* ItemDB = UItemDatabase::Get())
		{
			UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::CanPickup - ItemDatabase found, creating temp item..."));
			
			// Create temp item with quantity 1 for template (HasSpaceFor now takes quantity parameter)
			if (UItemBase* TempItem = ItemDB->CreateItem(ItemData->ItemID, 1))
			{
				UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::CanPickup - Temp item created: %s (Template Quantity: 1, Pickup Quantity: %d)"),
				       TempItem->ItemData ? *TempItem->ItemData->ItemName.ToString() : TEXT("NULL ItemData"),
				       Quantity);
				
				// Pass the pickup quantity to HasSpaceFor
				bool bHasSpace = InventoryComponent->HasSpaceFor(TempItem, Quantity);
				
				UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::CanPickup - HasSpaceFor returned: %s"), 
				       bHasSpace ? TEXT("TRUE") : TEXT("FALSE"));
				
				// Clean up temporary item - let GC handle it since it's not referenced
				// In UE 5.7, objects without references will be garbage collected automatically
				
				if (!bHasSpace)
				{
					UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor::CanPickup - Inventory has no space for item: %s (Quantity: %d)"), 
					       *ItemData->ItemName.ToString(), Quantity);
					return false;
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor::CanPickup - Failed to create item from ItemData: %s"), 
				       *ItemData->ItemID.ToString());
				return false;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor::CanPickup - ItemDatabase is NULL"));
			return false;
		}

	UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::CanPickup - Validation passed!"));
	return true;
}

void AItemPickupActor::PickupItem(AActionRPGPlayerCharacter* Player)
{
	UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::PickupItem - Starting pickup process"));

	if (!Player || !ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemPickupActor::PickupItem - Invalid player or ItemData"));
		return;
	}

	UInventoryComponent* InventoryComponent = Player->InventoryComponent;
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemPickupActor::PickupItem - Player has no InventoryComponent"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::PickupItem - Creating item from ItemData: %s (Quantity: %d)"),
	       *ItemData->ItemName.ToString(), Quantity);

	// Create item instance from ItemDataAsset using ItemDatabase
	// The item is used as a template - AddItem will create new instances as needed
	if (UItemDatabase* ItemDB = UItemDatabase::Get())
	{
		UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::PickupItem - ItemDatabase found"));
		
		// Create item template with quantity 1 (AddItem uses the Quantity parameter)
		if (UItemBase* Item = ItemDB->CreateItem(ItemData->ItemID, 1))
		{
			UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::PickupItem - Item template created successfully: %s (Template Quantity: 1, Pickup Quantity: %d)"),
			       Item->ItemData ? *Item->ItemData->ItemName.ToString() : TEXT("NULL ItemData"),
			       Quantity);

			// Attempt to add item to inventory
			// AddItem uses the Item as a template and creates instances based on Quantity parameter
			UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::PickupItem - Calling AddItem with Quantity: %d..."), Quantity);
			
			bool bAddSuccess = InventoryComponent->AddItem(Item, Quantity);
			
			UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::PickupItem - AddItem returned: %s"),
			       bAddSuccess ? TEXT("TRUE") : TEXT("FALSE"));

			if (bAddSuccess)
			{
				UE_LOG(LogTemp, Log, TEXT("ItemPickupActor: Item picked up successfully - %s (Quantity: %d)"), 
				       *ItemData->ItemName.ToString(), Quantity);

				// Spawn pickup effect
				SpawnPickupEffect();

				// Template item will be garbage collected automatically since it's not stored
				// AddItem creates its own instances that are properly referenced by TObjectPtr

				// Destroy pickup
				UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::PickupItem - Destroying pickup actor..."));
				DestroyPickup();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ItemPickupActor: Failed to add item to inventory - %s (Quantity: %d)"), 
				       *ItemData->ItemName.ToString(), Quantity);
				
				// Item will be garbage collected automatically since it's not referenced
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ItemPickupActor::PickupItem - Failed to create item from ItemData: %s"), 
			       *ItemData->ItemID.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemPickupActor::PickupItem - ItemDatabase is NULL"));
	}
}

void AItemPickupActor::SpawnPickupEffect()
{
	// Log pickup for debugging
	UE_LOG(LogTemp, Log, TEXT("ItemPickupActor: Spawning pickup effect for %s"), 
	       ItemData ? *ItemData->ItemName.ToString() : TEXT("Unknown Item"));

	// TODO: Add particle effects, sound effects, etc. in Phase 2 polish
	// For now, just log the pickup
}

void AItemPickupActor::DestroyPickup()
{
	UE_LOG(LogTemp, Log, TEXT("ItemPickupActor: Destroying pickup - %s"), *GetName());
	Destroy();
}

void AItemPickupActor::SetupVisuals()
{
	if (!MeshComponent)
	{
		return;
	}

	// Update material based on rarity if ItemData is set
	if (ItemData)
	{
		UpdateMaterialBasedOnRarity();
	}
}

void AItemPickupActor::UpdateMaterialBasedOnRarity()
{
	if (!ItemData || !MeshComponent)
	{
		return;
	}

	// TODO: Implement material updates based on item rarity
	// This can be done in Blueprint or with Material Instances
	// For now, just log the rarity
	UE_LOG(LogTemp, Verbose, TEXT("ItemPickupActor: Item rarity - %d"), (int32)ItemData->Rarity);
}

bool AItemPickupActor::IsPlayerInRange(AActionRPGPlayerCharacter* Player) const
{
	if (!Player)
	{
		return false;
	}

	FVector PlayerLocation = Player->GetActorLocation();
	FVector PickupLocation = GetActorLocation();
	float Distance = FVector::Dist(PlayerLocation, PickupLocation);

	return Distance <= InteractionRange;
}

bool AItemPickupActor::TryInteract(AActionRPGPlayerCharacter* Player)
{
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor::TryInteract - Player is NULL"));
		return false;
	}

	// Check if player is in range
	if (!IsPlayerInRange(Player))
	{
		UE_LOG(LogTemp, Verbose, TEXT("ItemPickupActor::TryInteract - Player is not in range"));
		return false;
	}

	// Check if pickup is valid
	if (!CanPickup(Player))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor::TryInteract - Cannot pickup item - validation failed"));
		return false;
	}

	// Attempt pickup
	PickupItem(Player);
	return true;
}

void AItemPickupActor::DebugCollisionSettings() const
{
	if (!CollisionComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemPickupActor::DebugCollisionSettings - CollisionComponent is NULL"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("=== ItemPickupActor Collision Debug ==="));
	UE_LOG(LogTemp, Warning, TEXT("Actor Name: %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("CollisionComponent Name: %s"), *CollisionComponent->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Sphere Radius: %.2f"), CollisionComponent->GetScaledSphereRadius());
	UE_LOG(LogTemp, Warning, TEXT("Collision Enabled: %d"), (int32)CollisionComponent->GetCollisionEnabled());
	UE_LOG(LogTemp, Warning, TEXT("Object Type: %d"), (int32)CollisionComponent->GetCollisionObjectType());
	UE_LOG(LogTemp, Warning, TEXT("Generate Overlap Events: %s"), 
	       CollisionComponent->GetGenerateOverlapEvents() ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogTemp, Warning, TEXT("Response to Pawn: %d"), 
	       (int32)CollisionComponent->GetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn));
	UE_LOG(LogTemp, Warning, TEXT("Is Root Component: %s"), 
	       (RootComponent == CollisionComponent) ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogTemp, Warning, TEXT("Interaction Range: %.2f"), InteractionRange);
	UE_LOG(LogTemp, Warning, TEXT("Auto Pickup on Overlap: %s"), 
	       bAutoPickupOnOverlap ? TEXT("TRUE") : TEXT("FALSE"));
	UE_LOG(LogTemp, Warning, TEXT("========================================"));
}

void AItemPickupActor::SetItemData(UItemDataAsset* NewItemData)
{
	if (NewItemData)
	{
		ItemData = NewItemData;
		// Update visuals after setting item data
		SetupVisuals();
		
		UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::SetItemData - Item data set: %s"), 
			*ItemData->ItemName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor::SetItemData - NewItemData is null"));
	}
}

void AItemPickupActor::SetQuantity(int32 NewQuantity)
{
	if (NewQuantity > 0)
	{
		Quantity = NewQuantity;
		UE_LOG(LogTemp, Log, TEXT("ItemPickupActor::SetQuantity - Quantity set: %d"), Quantity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemPickupActor::SetQuantity - Invalid quantity: %d"), NewQuantity);
	}
}
