// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actors/SkillProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Engine/DamageEvents.h"
#include "Engine/Engine.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Skills/SkillComponent.h"
#include "Skills/Core/SkillBase.h"

ASkillProjectile::ASkillProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create collision sphere
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;
	CollisionSphere->SetSphereRadius(10.0f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	// Create mesh component
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->InitialSpeed = 2000.0f;
	ProjectileMovement->MaxSpeed = 2000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Default values
	Damage = 20.0f;
	Lifetime = 5.0f;
	OwnerActor = nullptr;
	SkillReference = nullptr;
}

void ASkillProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Bind collision events
	if (CollisionSphere)
	{
		CollisionSphere->OnComponentHit.AddDynamic(this, &ASkillProjectile::OnHit);
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ASkillProjectile::OnBeginOverlap);
	}

	// Set lifetime timer
	if (Lifetime > 0.0f)
	{
		GetWorldTimerManager().SetTimer(LifetimeTimerHandle, this, &ASkillProjectile::DestroyProjectile, Lifetime, false);
	}
}

void ASkillProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Ignore owner
	if (OtherActor == OwnerActor)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("SkillProjectile::OnHit - Hit %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

	// Apply damage if other actor is a character
	if (OtherActor)
	{
		// Try to apply damage using TakeDamage
		FPointDamageEvent DamageEvent(Damage, Hit, GetActorForwardVector(), nullptr);
		float ActualDamage = OtherActor->TakeDamage(Damage, DamageEvent, OwnerActor ? OwnerActor->GetInstigatorController() : nullptr, OwnerActor);

		// If TakeDamage returns 0, try direct health modification
		if (ActualDamage == 0.0f)
		{
			AActionRPGPlayerCharacter* TargetCharacter = Cast<AActionRPGPlayerCharacter>(OtherActor);
			if (TargetCharacter)
			{
				float NewHealth = FMath::Max(0.0f, TargetCharacter->CurrentHealth - Damage);
				TargetCharacter->CurrentHealth = NewHealth;
				ActualDamage = Damage;
				UE_LOG(LogTemp, Log, TEXT("SkillProjectile::OnHit - Applied %.2f damage directly to %s (Health: %.2f)"), 
					Damage, *OtherActor->GetName(), NewHealth);
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("SkillProjectile::OnHit - Applied %.2f damage to %s via TakeDamage"), 
				ActualDamage, *OtherActor->GetName());
		}

		// Grant experience if damage was applied and skill reference exists
		if (ActualDamage > 0.0f && SkillReference && OwnerActor)
		{
			float HitExperience = 10.0f; // Base experience per hit
			if (USkillComponent* SkillComponent = OwnerActor->FindComponentByClass<USkillComponent>())
			{
				SkillComponent->AddSkillExperience(SkillReference, HitExperience);
				UE_LOG(LogTemp, Log, TEXT("SkillProjectile::OnHit - Granted %.2f experience for hit"), HitExperience);
			}
		}
	}

	// Destroy projectile on hit
	Destroy();
}

void ASkillProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Ignore owner
	if (OtherActor == OwnerActor)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("SkillProjectile::OnBeginOverlap - Overlapped %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

	// Apply damage (same logic as OnHit)
	if (OtherActor)
	{
		FPointDamageEvent DamageEvent(Damage, SweepResult, GetActorForwardVector(), nullptr);
		float ActualDamage = OtherActor->TakeDamage(Damage, DamageEvent, OwnerActor ? OwnerActor->GetInstigatorController() : nullptr, OwnerActor);

		if (ActualDamage == 0.0f)
		{
			AActionRPGPlayerCharacter* TargetCharacter = Cast<AActionRPGPlayerCharacter>(OtherActor);
			if (TargetCharacter)
			{
				float NewHealth = FMath::Max(0.0f, TargetCharacter->CurrentHealth - Damage);
				TargetCharacter->CurrentHealth = NewHealth;
				ActualDamage = Damage;
			}
		}

		// Grant experience
		if (ActualDamage > 0.0f && SkillReference && OwnerActor)
		{
			float HitExperience = 10.0f;
			if (USkillComponent* SkillComponent = OwnerActor->FindComponentByClass<USkillComponent>())
			{
				SkillComponent->AddSkillExperience(SkillReference, HitExperience);
			}
		}
	}

	// Destroy projectile on overlap
	Destroy();
}

void ASkillProjectile::DestroyProjectile()
{
	UE_LOG(LogTemp, Log, TEXT("SkillProjectile::DestroyProjectile - Lifetime expired, destroying projectile"));
	Destroy();
}
