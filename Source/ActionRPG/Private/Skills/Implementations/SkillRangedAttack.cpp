// Copyright Epic Games, Inc. All Rights Reserved.

#include "Skills/Implementations/SkillRangedAttack.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Actors/SkillProjectile.h"
#include "Components/Skills/SkillComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Engine.h"

USkillRangedAttack::USkillRangedAttack()
{
}

USkillBase* USkillRangedAttack::Activate(AActor* Target)
{
	// Call parent to handle cooldown and validation
	USkillBase* Result = Super::Activate(Target);
	if (!Result)
	{
		return nullptr;
	}

	// Get owner actor (character performing attack)
	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillRangedAttack::Activate - OwnerActor is null"));
		return this;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor);
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillRangedAttack::Activate - OwnerActor is not a Character"));
		return this;
	}

	if (!SkillData)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillRangedAttack::Activate - SkillData is null"));
		return this;
	}

	// Check if ProjectileClass is set
	if (!SkillData->ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillRangedAttack::Activate - ProjectileClass is not set in SkillDataAsset"));
		return this;
	}

	UE_LOG(LogTemp, Log, TEXT("SkillRangedAttack::Activate - Executing ranged attack from %s"), *OwnerActor->GetName());

	// Get spawn location and direction
	FVector SpawnLocation = GetProjectileSpawnLocation();
	FVector Direction = GetProjectileDirection(Target);
	FRotator SpawnRotation = Direction.Rotation();

	// Spawn projectile
	AActor* Projectile = SpawnProjectile(SpawnLocation, SpawnRotation);

	if (!Projectile)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillRangedAttack::Activate - Failed to spawn projectile"));
		return this;
	}

	UE_LOG(LogTemp, Log, TEXT("SkillRangedAttack::Activate - Spawned projectile %s at location %s"), 
		*Projectile->GetName(), *SpawnLocation.ToString());

	// Grant base experience on cast (additional experience granted on hit by projectile)
	float BaseExperience = 5.0f;
	if (USkillComponent* SkillComponent = OwnerActor->FindComponentByClass<USkillComponent>())
	{
		SkillComponent->AddSkillExperience(this, BaseExperience);
		UE_LOG(LogTemp, Log, TEXT("SkillRangedAttack::Activate - Granted %.2f base experience"), BaseExperience);
	}

	return this;
}

AActor* USkillRangedAttack::SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (!OwnerActor || !SkillData || !SkillData->ProjectileClass)
	{
		return nullptr;
	}

	UWorld* World = OwnerActor->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	// Spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerActor;
	SpawnParams.Instigator = Cast<APawn>(OwnerActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawn projectile
	AActor* Projectile = World->SpawnActor<AActor>(
		SkillData->ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (!Projectile)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillRangedAttack::SpawnProjectile - Failed to spawn projectile actor"));
		return nullptr;
	}

	// Configure projectile properties
	// Try to set damage and speed on ASkillProjectile if it's that type
	if (ASkillProjectile* SkillProjectile = Cast<ASkillProjectile>(Projectile))
	{
		SkillProjectile->SetDamage(GetAttackDamage());
		SkillProjectile->SetOwnerActor(OwnerActor);
		SkillProjectile->SetSkillReference(this);
		SkillProjectile->SetAreaOfEffectRadius(SkillData ? SkillData->AreaOfEffectRadius : 0.0f);
	}

	// Set projectile velocity if it has a ProjectileMovementComponent
	if (UProjectileMovementComponent* ProjectileMovement = Projectile->FindComponentByClass<UProjectileMovementComponent>())
	{
		float Speed = GetProjectileSpeed();
		ProjectileMovement->InitialSpeed = Speed;
		ProjectileMovement->MaxSpeed = Speed;
		ProjectileMovement->Velocity = SpawnRotation.Vector() * Speed;
		UE_LOG(LogTemp, Log, TEXT("SkillRangedAttack::SpawnProjectile - Set projectile speed to %.2f"), Speed);
	}

	return Projectile;
}

FVector USkillRangedAttack::GetProjectileSpawnLocation() const
{
	if (!OwnerActor)
	{
		return FVector::ZeroVector;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor);
	if (!OwnerCharacter)
	{
		return OwnerActor->GetActorLocation();
	}

	// Spawn at character location + forward offset
	FVector CharacterLocation = OwnerCharacter->GetActorLocation();
	FVector ForwardOffset = OwnerCharacter->GetActorForwardVector() * 50.0f; // 50 units forward
	FVector SpawnLocation = CharacterLocation + ForwardOffset;

	// Optionally use socket location (e.g., "WeaponSocket" or "HandSocket")
	// if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
	// {
	//     FVector SocketLocation = Mesh->GetSocketLocation("WeaponSocket");
	//     if (!SocketLocation.IsNearlyZero())
	//     {
	//         SpawnLocation = SocketLocation;
	//     }
	// }

	return SpawnLocation;
}

FVector USkillRangedAttack::GetProjectileDirection(AActor* Target) const
{
	if (!OwnerActor)
	{
		return FVector::ForwardVector;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor);
	if (!OwnerCharacter)
	{
		return OwnerActor->GetActorForwardVector();
	}

	// If target is provided, aim at target
	if (Target)
	{
		FVector OwnerLocation = OwnerCharacter->GetActorLocation();
		FVector TargetLocation = Target->GetActorLocation();
		FVector Direction = (TargetLocation - OwnerLocation).GetSafeNormal();
		return Direction;
	}

	// Otherwise, use owner's forward direction
	return OwnerCharacter->GetActorForwardVector();
}

float USkillRangedAttack::GetProjectileSpeed() const
{
	if (!SkillData)
	{
		return 2000.0f; // Default speed
	}

	return SkillData->ProjectileSpeed;
}

float USkillRangedAttack::GetAttackDamage() const
{
	if (!SkillData)
	{
		return 20.0f; // Default damage
	}

	float BaseDamage = SkillData->BaseDamage;

	// Future: Scale with skill level
	// float LevelMultiplier = 1.0f + (SkillLevel - 1) * 0.15f;
	// return BaseDamage * LevelMultiplier;

	return BaseDamage;
}
