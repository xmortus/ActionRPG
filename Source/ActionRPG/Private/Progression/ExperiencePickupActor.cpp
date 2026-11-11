// Copyright Epic Games, Inc. All Rights Reserved.

#include "Progression/ExperiencePickupActor.h"

#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AExperiencePickupActor::AExperiencePickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetSphereRadius(60.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AExperiencePickupActor::OnOverlapBegin);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(CollisionComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetRelativeScale3D(FVector(0.5f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(SphereMesh.Object);
	}
}

bool AExperiencePickupActor::TryCollect(AActionRPGPlayerCharacter* Player)
{
	if (bCollected || !IsValid(Player) || !FMath::IsFinite(ExperienceAmount) || ExperienceAmount <= 0.0f)
	{
		return false;
	}

	UExperienceComponent* Experience = Player->ExperienceComponent;
	if (!IsValid(Experience) ||
		!FMath::IsFinite(Experience->GetUnallocatedExperience() + ExperienceAmount) ||
		!FMath::IsFinite(Experience->GetTotalExperience() + ExperienceAmount))
	{
		return false;
	}

	Experience->AddExperience(ExperienceAmount);
	bCollected = true;
	Destroy();
	return true;
}

void AExperiencePickupActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	TryCollect(Cast<AActionRPGPlayerCharacter>(OtherActor));
}
