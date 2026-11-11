// Copyright Epic Games, Inc. All Rights Reserved.

#include "Quests/QuestMarkerActor.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Progression/QuestComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Math/RotationMatrix.h"
#include "Quests/QuestDataAsset.h"

AQuestMarkerActor::AQuestMarkerActor()
{
	PrimaryActorTick.bCanEverTick = true;
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	SetRootComponent(TriggerSphere);
	TriggerSphere->InitSphereRadius(80.0f);
	TriggerSphere->SetCollisionProfileName(TEXT("Trigger"));
	TriggerSphere->SetGenerateOverlapEvents(true);
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AQuestMarkerActor::OnOverlapBegin);
	MarkerText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("MarkerText"));
	MarkerText->SetupAttachment(TriggerSphere);
	MarkerText->SetRelativeLocation(FVector(0.0f, 0.0f, 95.0f));
	MarkerText->SetHorizontalAlignment(EHTA_Center);
	MarkerText->SetWorldSize(32.0f);
	MarkerText->SetText(FText::FromString(TEXT("QUEST: ACCEPT / RETURN")));
}

void AQuestMarkerActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (MarkerText && Quest)
	{
		MarkerText->SetText(FText::FromString(FString::Printf(TEXT("QUEST: %s"), *Quest->QuestName.ToString())));
	}
}

void AQuestMarkerActor::BeginPlay()
{
	Super::BeginPlay();
	FaceMarkerTowardPlayerCamera();
}

void AQuestMarkerActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FaceMarkerTowardPlayerCamera();
}

void AQuestMarkerActor::FaceMarkerTowardPlayerCamera()
{
	if (!MarkerText) return;
	const APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!Camera) return;
	const FRotator Rotation = Camera->GetCameraRotation();
	MarkerText->SetWorldRotation(FRotationMatrix::MakeFromXZ(
		-Rotation.Vector(), Rotation.RotateVector(FVector::UpVector)).Rotator());
}

bool AQuestMarkerActor::TryTrigger(AActionRPGPlayerCharacter* Player)
{
	if (!IsValid(Player) || !Quest || !Player->QuestComponent) return false;
	if (Player->QuestComponent->GetActiveQuest() == Quest) return Player->QuestComponent->TryTurnInQuest();
	return Player->QuestComponent->StartQuest(Quest);
}

void AQuestMarkerActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TryTrigger(Cast<AActionRPGPlayerCharacter>(OtherActor));
}
