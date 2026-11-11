// Copyright Epic Games, Inc. All Rights Reserved.

#include "Actors/SkillProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Combat/ActionRPGDamageTypes.h"
#include "Engine/DamageEvents.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Skills/SkillComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Characters/ActionRPGEnemyCharacter.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Progression/TrainingComponent.h"

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
	AreaOfEffectRadius = 0.0f;
	Lifetime = 5.0f;
	OwnerActor = nullptr;
	SkillReference = nullptr;
}

TSubclassOf<UDamageType> ASkillProjectile::GetDamageTypeClass() const
{
	if (!SkillReference || !SkillReference->SkillData) return UDamageType::StaticClass();
	return FActionRPGDamageTypes::GetClass(SkillReference->SkillData->DamageType);
}

void ASkillProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (!OwnerActor) OwnerActor = GetOwner();

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
	UE_LOG(LogTemp, Log, TEXT("SkillProjectile::OnHit - Hit %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));
	ProcessImpact(OtherActor, Hit);
}

void ASkillProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("SkillProjectile::OnBeginOverlap - Overlapped %s"), OtherActor ? *OtherActor->GetName() : TEXT("NULL"));
	ProcessImpact(OtherActor, SweepResult);
}


void ASkillProjectile::ProcessImpact(AActor* OtherActor, const FHitResult& Hit)
{
	if (IsActorBeingDestroyed() || OtherActor == OwnerActor || OtherActor == GetOwner()) return;
	if (!OtherActor) { Destroy(); return; }

	FPointDamageEvent DamageEvent(Damage, Hit, GetActorForwardVector(), GetDamageTypeClass());
	const float ActualDamage = OtherActor->TakeDamage(Damage, DamageEvent,
		OwnerActor ? OwnerActor->GetInstigatorController() : nullptr, OwnerActor);
	RecordProjectileHitPractice(OtherActor, ActualDamage);
	int32 DamagingHits = ActualDamage > 0.0f && Cast<AActionRPGEnemyCharacter>(OtherActor) ? 1 : 0;
	DamagingHits += ApplyAreaDamageAndRecordHits(OtherActor);
	if (DamagingHits > 0 && SkillReference && OwnerActor)
	{
		if (USkillComponent* SkillComponent = OwnerActor->FindComponentByClass<USkillComponent>())
		{
			SkillComponent->AddSkillExperience(SkillReference, 10.0f * DamagingHits);
		}
	}
	Destroy();
}

int32 ASkillProjectile::ApplyAreaDamageAndRecordHits(AActor* DirectTarget)
{
	if (AreaOfEffectRadius <= 0.0f || !GetWorld()) return 0;

	TMap<AActionRPGEnemyCharacter*, float> EnemyHealthBefore;
	for (TActorIterator<AActionRPGEnemyCharacter> It(GetWorld()); It; ++It)
	{
		AActionRPGEnemyCharacter* Enemy = *It;
		if (IsValid(Enemy) && Enemy != DirectTarget && !Enemy->IsDead())
		{
			EnemyHealthBefore.FindOrAdd(Enemy) = Enemy->GetCurrentHealth();
		}
	}

	TArray<AActor*> IgnoreActors;
	if (OwnerActor) IgnoreActors.Add(OwnerActor);
	IgnoreActors.Add(DirectTarget);
	UGameplayStatics::ApplyRadialDamage(this, Damage, GetActorLocation(), AreaOfEffectRadius,
		GetDamageTypeClass(), IgnoreActors, this,
		OwnerActor ? OwnerActor->GetInstigatorController() : nullptr, true);

	int32 DamagingHits = 0;
	for (const TPair<AActionRPGEnemyCharacter*, float>& Before : EnemyHealthBefore)
	{
		if (IsValid(Before.Key))
		{
			const float ActualDamage = Before.Value - Before.Key->GetCurrentHealth();
			if (ActualDamage > 0.0f)
			{
				++DamagingHits;
				RecordProjectileHitPractice(Before.Key, ActualDamage);
			}
		}
	}
	return DamagingHits;
}

void ASkillProjectile::RecordProjectileHitPractice(AActor* Target, float ActualDamage)
{
	if (ActualDamage <= 0.0f || !SkillReference || !Cast<AActionRPGEnemyCharacter>(Target)) return;
	if (AActionRPGPlayerCharacter* Player = Cast<AActionRPGPlayerCharacter>(OwnerActor))
	{
		if (!Player->IsDead() && Player->TrainingComponent)
		{
			if (SkillReference->GetSkillType() == ESkillType::Magic)
			{
				Player->TrainingComponent->RecordPractice(EPrimaryAttribute::Intelligence, TEXT("MagicHit"), Target);
			}
			else if (SkillReference->GetSkillType() == ESkillType::Ranged)
			{
				Player->TrainingComponent->RecordPractice(EPrimaryAttribute::Dexterity, TEXT("RangedHit"), Target);
			}
		}
	}
}

void ASkillProjectile::DestroyProjectile()
{
	UE_LOG(LogTemp, Log, TEXT("SkillProjectile::DestroyProjectile - Lifetime expired, destroying projectile"));
	Destroy();
}
