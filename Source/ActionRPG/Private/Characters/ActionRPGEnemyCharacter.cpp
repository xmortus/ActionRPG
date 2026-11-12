// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/ActionRPGEnemyCharacter.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Components/Progression/QuestComponent.h"

#include "Characters/ActionRPGPlayerCharacter.h"
#include "Animation/AnimSequenceBase.h"
#include "Combat/DamageResolution.h"
#include "Combat/ActionRPGPhysicalDamageType.h"
#include "Combat/ActionRPGDamageTypes.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/Progression/TrialComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Math/RotationMatrix.h"

AActionRPGEnemyCharacter::AActionRPGEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AActionRPGEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	CameraFacingLabel = FindComponentByClass<UTextRenderComponent>();
	if (CameraFacingLabel) InitialLabelText = CameraFacingLabel->Text;
	SetActorTickEnabled(IsValid(CameraFacingLabel));
	FaceLabelTowardPlayerCamera();
	if (TrainingIdleAnimation && GetMesh()) GetMesh()->PlayAnimation(TrainingIdleAnimation, true);
	MaxHealth = FMath::IsFinite(MaxHealth) ? FMath::Max(MaxHealth, 1.0f) : 100.0f;
	CurrentHealth = MaxHealth;
	bIsDead = false;
	InitialCollisionEnabled = GetCapsuleComponent()->GetCollisionEnabled();
	InitialMovementMode = GetCharacterMovement()->MovementMode;
	if (bAttacksPlayers && FMath::IsFinite(AttackIntervalSeconds) && AttackIntervalSeconds > 0.0f)
	{
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AActionRPGEnemyCharacter::AttemptMeleeAttack,
			AttackIntervalSeconds, true);
	}
}

void AActionRPGEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FaceLabelTowardPlayerCamera();
}

void AActionRPGEnemyCharacter::FaceLabelTowardPlayerCamera()
{
	if (!IsValid(CameraFacingLabel))
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
	CameraFacingLabel->SetWorldRotation(FRotationMatrix::MakeFromXZ(TowardCamera, CameraUp).Rotator());
}

void AActionRPGEnemyCharacter::AttemptMeleeAttack()
{
	if (bIsDead || bAttackInProgress || !FMath::IsFinite(AttackRange) || AttackRange <= 0.0f ||
		!FMath::IsFinite(AttackDamage) || AttackDamage <= 0.0f) return;

	AActionRPGPlayerCharacter* NearestPlayer = nullptr;
	float NearestDistance = AttackRange;
	for (TActorIterator<AActionRPGPlayerCharacter> It(GetWorld()); It; ++It)
	{
		if (It->IsDead()) continue;
		const float Distance = FVector::Dist2D(GetActorLocation(), It->GetActorLocation());
		if (Distance < NearestDistance)
		{
			NearestPlayer = *It;
			NearestDistance = Distance;
		}
	}
	if (NearestPlayer)
	{
		FVector TowardPlayer = NearestPlayer->GetActorLocation() - GetActorLocation();
		TowardPlayer.Z = 0.0f;
		if (!TowardPlayer.IsNearlyZero())
		{
			SetActorRotation(FRotator(0.0f, TowardPlayer.Rotation().Yaw, 0.0f));
		}
		if (TrainingAttackAnimation && GetMesh() && TrainingAttackAnimation->GetPlayLength() > 0.0f)
		{
			const float AttackDuration = TrainingAttackAnimation->GetPlayLength();
			bAttackInProgress = true;
			PendingAttackTarget = NearestPlayer;
			GetMesh()->PlayAnimation(TrainingAttackAnimation, false);
			GetWorldTimerManager().SetTimer(AttackHitTimer, this, &AActionRPGEnemyCharacter::ApplyPendingMeleeHit,
				AttackDuration * 0.5f, false);
			GetWorldTimerManager().SetTimer(AttackIdleTimer, this, &AActionRPGEnemyCharacter::FinishMeleeAttack,
				AttackDuration, false);
			return;
		}
		UGameplayStatics::ApplyDamage(NearestPlayer, AttackDamage, GetController(), this,
			UActionRPGPhysicalDamageType::StaticClass());
	}
}

void AActionRPGEnemyCharacter::ApplyPendingMeleeHit()
{
	AActionRPGPlayerCharacter* Target = PendingAttackTarget;
	PendingAttackTarget = nullptr;
	if (bIsDead || !IsValid(Target) || Target->IsDead() ||
		FVector::Dist2D(GetActorLocation(), Target->GetActorLocation()) > AttackRange) return;
	UGameplayStatics::ApplyDamage(Target, AttackDamage, GetController(), this,
		UActionRPGPhysicalDamageType::StaticClass());
}

void AActionRPGEnemyCharacter::FinishMeleeAttack()
{
	bAttackInProgress = false;
	if (!bIsDead && TrainingIdleAnimation && GetMesh()) GetMesh()->PlayAnimation(TrainingIdleAnimation, true);
}

void AActionRPGEnemyCharacter::ClearMeleeAttackState()
{
	GetWorldTimerManager().ClearTimer(AttackHitTimer);
	GetWorldTimerManager().ClearTimer(AttackIdleTimer);
	PendingAttackTarget = nullptr;
	bAttackInProgress = false;
}

bool AActionRPGEnemyCharacter::Revive()
{
	if (!bIsDead || !FMath::IsFinite(MaxHealth) || MaxHealth < 1.0f) return false;
	if (UWorld* World = GetWorld()) World->GetTimerManager().ClearTimer(RespawnTimer);
	ClearMeleeAttackState();
	CurrentHealth = MaxHealth;
	bIsDead = false;
	if (TrainingIdleAnimation && GetMesh()) GetMesh()->PlayAnimation(TrainingIdleAnimation, true);
	if (CameraFacingLabel) CameraFacingLabel->SetText(InitialLabelText);
	GetCapsuleComponent()->SetCollisionEnabled(InitialCollisionEnabled);
	GetCharacterMovement()->SetMovementMode(InitialMovementMode);
	UE_LOG(LogTemp, Display, TEXT("Training enemy %s is ready for combat again"), *GetName());
	return true;
}

void AActionRPGEnemyCharacter::RespawnAfterDelay()
{
	Revive();
}

AActionRPGPlayerCharacter* AActionRPGEnemyCharacter::ResolveKiller(
	AController* EventInstigator, AActor* DamageCauser) const
{
	if (EventInstigator)
	{
		if (AActionRPGPlayerCharacter* Player = Cast<AActionRPGPlayerCharacter>(EventInstigator->GetPawn()))
		{
			return Player;
		}
	}
	if (AActionRPGPlayerCharacter* Player = Cast<AActionRPGPlayerCharacter>(DamageCauser))
	{
		return Player;
	}
	if (DamageCauser)
	{
		if (AActionRPGPlayerCharacter* Player = Cast<AActionRPGPlayerCharacter>(DamageCauser->GetOwner()))
		{
			return Player;
		}
		return Cast<AActionRPGPlayerCharacter>(DamageCauser->GetInstigator());
	}
	return nullptr;
}

float AActionRPGEnemyCharacter::GetDamageResistancePercent(EDamageType DamageType) const
{
	const float* Resistance = DamageResistancePercent.Find(DamageType);
	return Resistance && FMath::IsFinite(*Resistance) ? FMath::Clamp(*Resistance, 0.0f, 100.0f) : 0.0f;
}

float AActionRPGEnemyCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	const TOptional<EDamageType> DamageKind = FActionRPGDamageTypes::GetKind(DamageEvent.DamageTypeClass);
	const bool bPhysicalHit = DamageKind.IsSet() && DamageKind.GetValue() == EDamageType::Physical;
	const float ResistedDamage = DamageKind.IsSet()
		? DamageAmount * (1.0f - GetDamageResistancePercent(DamageKind.GetValue()) / 100.0f)
		: DamageAmount;
	const FActionRPGDamageResult Result = bPhysicalHit
		? FActionRPGDamageResolver::ResolvePhysical(ResistedDamage, CurrentHealth, bIsDead, Armor, ArmorMitigationScale)
		: FActionRPGDamageResolver::Resolve(ResistedDamage, CurrentHealth, bIsDead);
	if (Result.AppliedDamage <= 0.0f)
	{
		return 0.0f;
	}

	Super::TakeDamage(Result.AppliedDamage, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth = Result.RemainingHealth;
	if (Result.bKilled)
	{
		bIsDead = true;
		ClearMeleeAttackState();
		if (TrainingDeathAnimation && GetMesh()) GetMesh()->PlayAnimation(TrainingDeathAnimation, false);
		if (CameraFacingLabel && bRespawnAfterDeath)
		{
			CameraFacingLabel->SetText(FText::FromString(TEXT("DEFEATED - RESPAWNING")));
		}
		GetCharacterMovement()->DisableMovement();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AActionRPGPlayerCharacter* Killer = ResolveKiller(EventInstigator, DamageCauser);
		if (Killer && Killer->TrialComponent)
		{
			Killer->TrialComponent->ReportEnemyDefeated(EnemyId, this);
		}
		if (Killer && Killer->EquipmentComponent)
		{
			Killer->EquipmentComponent->ReportCreditedEnemyDefeat();
		}
		if (Killer && Killer->QuestComponent)
		{
			Killer->QuestComponent->ReportEnemyDefeated(EnemyId, this);
		}
		OnEnemyDied.Broadcast(this, Killer);
		if (bRespawnAfterDeath && FMath::IsFinite(RespawnDelaySeconds) && RespawnDelaySeconds > 0.0f)
		{
			GetWorldTimerManager().SetTimer(RespawnTimer, this, &AActionRPGEnemyCharacter::RespawnAfterDelay,
				RespawnDelaySeconds, false);
		}
	}
	return Result.AppliedDamage;
}
