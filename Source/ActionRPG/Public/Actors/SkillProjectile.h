// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class USkillBase;

/**
 * Basic projectile actor for skill system.
 * Handles movement, collision, and damage application on hit.
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API ASkillProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASkillProjectile();

protected:
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// Projectile Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (ClampMin = "0.0"))
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (ClampMin = "0.0"))
	float AreaOfEffectRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (ClampMin = "0.0"))
	float Lifetime;

	// References
	UPROPERTY(BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<AActor> OwnerActor;

	UPROPERTY(BlueprintReadOnly, Category = "Projectile")
	TObjectPtr<USkillBase> SkillReference;

	// Collision Handlers
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Lifetime Timer
	FTimerHandle LifetimeTimerHandle;
	void DestroyProjectile();

public:
	// Setters for configuration
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetDamage(float NewDamage) { Damage = NewDamage; }

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetOwnerActor(AActor* NewOwner) { OwnerActor = NewOwner; }

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetSkillReference(USkillBase* Skill) { SkillReference = Skill; }

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetAreaOfEffectRadius(float NewRadius) { AreaOfEffectRadius = NewRadius; }
};
