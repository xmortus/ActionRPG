// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/Core/ItemDataAsset.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "ItemPickupActor.generated.h"

class AActionRPGPlayerCharacter;

/**
 * Actor that represents an item pickup in the world.
 * Supports both automatic pickup on overlap (if enabled) and manual interaction via IA_Interact.
 * Supports visual representation, collision detection, and inventory integration.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API AItemPickupActor : public AActor
{
	GENERATED_BODY()

public:
	AItemPickupActor();

	// Interaction Functions
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool IsPlayerInRange(AActionRPGPlayerCharacter* Player) const;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool TryInteract(AActionRPGPlayerCharacter* Player);

protected:
	virtual void BeginPlay() override;

	// Item Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	TObjectPtr<UItemDataAsset> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	int32 Quantity = 1;

	// Interaction Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = "0.0", ClampMax = "1000.0"))
	float InteractionRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	bool bAutoPickupOnOverlap = false;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	// Pickup Logic
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                    bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void PickupItem(AActionRPGPlayerCharacter* Player);
	
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	bool CanPickup(AActionRPGPlayerCharacter* Player) const;
	
	void SpawnPickupEffect();
	void DestroyPickup();

	// Visual Effects
	void SetupVisuals();
	void UpdateMaterialBasedOnRarity();

	// Debug
	UFUNCTION(BlueprintCallable, Category = "Pickup|Debug")
	void DebugCollisionSettings() const;
};
