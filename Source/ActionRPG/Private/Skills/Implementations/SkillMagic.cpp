// Copyright Epic Games, Inc. All Rights Reserved.

#include "Skills/Implementations/SkillMagic.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Actors/SkillProjectile.h"
#include "Components/Skills/SkillComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Engine/Engine.h"

USkillMagic::USkillMagic()
{
}

USkillBase* USkillMagic::Activate(AActor* Target)
{
	if (!CanActivate(Target))
	{
		return nullptr;
	}

	// Get owner actor (character performing attack)
	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillMagic::Activate - OwnerActor is null"));
		return nullptr;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerActor);
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillMagic::Activate - OwnerActor is not a Character"));
		return nullptr;
	}

	if (!SkillData)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillMagic::Activate - SkillData is null"));
		return nullptr;
	}

	// Check if ProjectileClass is set
	if (!SkillData->ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillMagic::Activate - ProjectileClass is not set in SkillDataAsset"));
		return nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("SkillMagic::Activate - Executing magic attack from %s"), *OwnerActor->GetName());

	// Get spawn location and direction
	const FVector SpawnLocation = GetProjectileSpawnLocation();
	const FVector Direction = GetProjectileDirection(Target);
	const FRotator SpawnRotation = Direction.Rotation();

	// Spawn projectile
	AActor* Projectile = SpawnProjectile(SpawnLocation, SpawnRotation);
	if (!Projectile)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillMagic::Activate - Failed to spawn projectile"));
		return nullptr;
	}
	if (!Super::Activate(Target))
	{
		Projectile->Destroy();
		return nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("SkillMagic::Activate - Spawned projectile %s at location %s"),
		*Projectile->GetName(), *SpawnLocation.ToString());

	// Grant base experience on cast (additional experience granted on hit by projectile)
	const float BaseExperience = 5.0f;
	if (USkillComponent* SkillComponent = OwnerActor->FindComponentByClass<USkillComponent>())
	{
		SkillComponent->AddSkillExperience(this, BaseExperience);
		UE_LOG(LogTemp, Log, TEXT("SkillMagic::Activate - Granted %.2f base experience"), BaseExperience);
	}

	return this;
}

AActor* USkillMagic::SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation)
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

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerActor;
	SpawnParams.Instigator = Cast<APawn>(OwnerActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Projectile = World->SpawnActor<AActor>(
		SkillData->ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (!Projectile)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillMagic::SpawnProjectile - Failed to spawn projectile actor"));
		return nullptr;
	}

	// Configure projectile properties if it is a SkillProjectile
	if (ASkillProjectile* SkillProjectile = Cast<ASkillProjectile>(Projectile))
	{
		SkillProjectile->SetDamage(GetAttackDamage());
		SkillProjectile->SetOwnerActor(OwnerActor);
		SkillProjectile->SetSkillReference(this);
		SkillProjectile->SetAreaOfEffectRadius(GetAreaOfEffectRadius());
	}

	// Set projectile velocity if it has a ProjectileMovementComponent
	if (UProjectileMovementComponent* ProjectileMovement = Projectile->FindComponentByClass<UProjectileMovementComponent>())
	{
		const float Speed = GetProjectileSpeed();
		ProjectileMovement->InitialSpeed = Speed;
		ProjectileMovement->MaxSpeed = Speed;
		ProjectileMovement->Velocity = SpawnRotation.Vector() * Speed;
		UE_LOG(LogTemp, Log, TEXT("SkillMagic::SpawnProjectile - Set projectile speed to %.2f"), Speed);
	}

	return Projectile;
}

FVector USkillMagic::GetProjectileSpawnLocation() const
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

	const FVector CharacterLocation = OwnerCharacter->GetActorLocation();
	const FVector ForwardOffset = OwnerCharacter->GetActorForwardVector() * 50.0f;
	return CharacterLocation + ForwardOffset;
}

FVector USkillMagic::GetProjectileDirection(AActor* Target) const
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

	if (Target)
	{
		const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
		const FVector TargetLocation = Target->GetActorLocation();
		return (TargetLocation - OwnerLocation).GetSafeNormal();
	}

	return OwnerCharacter->GetActorForwardVector();
}

float USkillMagic::GetProjectileSpeed() const
{
	if (SkillData && SkillData->ProjectileSpeed > 0.0f)
	{
		return SkillData->ProjectileSpeed;
	}

	return 2000.0f;
}

float USkillMagic::GetAttackDamage() const
{
	const float BaseDamage = SkillData && FMath::IsFinite(SkillData->BaseDamage)
		? FMath::Max(0.0f, SkillData->BaseDamage) : 0.0f;
	const UAttributeComponent* Attributes = OwnerActor ? OwnerActor->FindComponentByClass<UAttributeComponent>() : nullptr;
	const float Intelligence = Attributes ? FMath::Max(0.0f, Attributes->GetAttribute(EPrimaryAttribute::Intelligence)) : 0.0f;
	const float PerIntelligence = SkillData && FMath::IsFinite(SkillData->MagicDamagePerIntelligence)
		? FMath::Max(0.0f, SkillData->MagicDamagePerIntelligence) : 0.0f;
	return (BaseDamage + Intelligence * PerIntelligence) * GetDamageMultiplier();
}

float USkillMagic::GetAreaOfEffectRadius() const
{
	if (SkillData)
	{
		return SkillData->AreaOfEffectRadius;
	}

	return 0.0f;
}
