// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExperiencePickupActor.generated.h"

class AActionRPGPlayerCharacter;
class USphereComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;

/** A simple world reward that transfers XP to the player's unallocated pool. */
UCLASS(Blueprintable)
class ACTIONRPG_API AExperiencePickupActor : public AActor
{
	GENERATED_BODY()

public:
	AExperiencePickupActor();

	/** Returns true only when XP was awarded; a successful pickup destroys itself. */
	UFUNCTION(BlueprintCallable, Category = "Experience")
	bool TryCollect(AActionRPGPlayerCharacter* Player);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Experience", meta = (ClampMin = "0.0"))
	float ExperienceAmount = 25.0f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	bool bCollected = false;
};
