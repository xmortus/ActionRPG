// Copyright Epic Games, Inc. All Rights Reserved.

#include "Trials/TrialTriggerActor.h"

#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Progression/TrialComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Math/RotationMatrix.h"
#include "Trials/TrialDataAsset.h"

ATrialTriggerActor::ATrialTriggerActor()
{
	// The world marker needs to track the player's changing camera angle.
	PrimaryActorTick.bCanEverTick = true;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	SetRootComponent(TriggerSphere);
	TriggerSphere->InitSphereRadius(80.0f);
	TriggerSphere->SetCollisionProfileName(TEXT("Trigger"));
	TriggerSphere->SetGenerateOverlapEvents(true);
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ATrialTriggerActor::OnOverlapBegin);

	MarkerText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("MarkerText"));
	MarkerText->SetupAttachment(TriggerSphere);
	MarkerText->SetRelativeLocation(FVector(0.0f, 0.0f, 95.0f));
	MarkerText->SetHorizontalAlignment(EHTA_Center);
	MarkerText->SetWorldSize(32.0f);
	MarkerText->SetText(FText::FromString(TEXT("START TRIAL")));
}

void ATrialTriggerActor::BeginPlay()
{
	Super::BeginPlay();
	RefreshMarkerText();
	FaceMarkerTowardPlayerCamera();
}

void ATrialTriggerActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FaceMarkerTowardPlayerCamera();
}

void ATrialTriggerActor::FaceMarkerTowardPlayerCamera()
{
	if (!MarkerText)
	{
		return;
	}

	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager)
	{
		return;
	}

	const FRotator CameraRotation = CameraManager->GetCameraRotation();
	const FVector TowardCamera = -CameraRotation.Vector();
	const FVector CameraUp = CameraRotation.RotateVector(FVector::UpVector);
	MarkerText->SetWorldRotation(FRotationMatrix::MakeFromXZ(TowardCamera, CameraUp).Rotator());
}

void ATrialTriggerActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RefreshMarkerText();
}

void ATrialTriggerActor::RefreshMarkerText()
{
	if (MarkerText)
	{
		const FString Prefix = Action == ETrialTriggerAction::Start ? TEXT("START") : TEXT("GOAL");
		const FString Label = Trial
			? FString::Printf(TEXT("%s: %s"), *Prefix, *Trial->TrialName.ToString())
			: (Action == ETrialTriggerAction::Start ? TEXT("START TRIAL") : TEXT("TRIAL GOAL"));
		MarkerText->SetText(FText::FromString(Label));
	}
}

bool ATrialTriggerActor::TryTrigger(AActionRPGPlayerCharacter* Player)
{
	if (!IsValid(Player) || !Trial)
	{
		return false;
	}

	UTrialComponent* Trials = Player->TrialComponent;
	if (!Trials)
	{
		return false;
	}

	if (Action == ETrialTriggerAction::Start)
	{
		return Trials->StartTrial(Trial);
	}

	if (ObjectiveId.IsNone() || Trials->GetActiveTrial() != Trial)
	{
		return false;
	}
	return Trials->GetTrialProgress(Trial) >= 1.0f
		? Trials->TryCompleteActiveTrial()
		: Trials->AdvanceObjective(ObjectiveId);
}

void ATrialTriggerActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	TryTrigger(Cast<AActionRPGPlayerCharacter>(OtherActor));
}
