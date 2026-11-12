// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/ActionRPGPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/AcquiredAttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Combat/StatusEffectComponent.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Components/Progression/ClassComponent.h"
#include "Components/Progression/TrialComponent.h"
#include "Components/Progression/QuestComponent.h"
#include "Components/Progression/TrainingComponent.h"
#include "Characters/ActionRPGEnemyCharacter.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Combat/DamageResolution.h"
#include "Combat/ActionRPGDamageTypes.h"
#include "Progression/Core/SecondaryAttributeDataAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Items/Core/ItemBase.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Core/ItemTypes.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"

AActionRPGPlayerCharacter::AActionRPGPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set default values for player character
	PrimaryActorTick.bCanEverTick = true;
	
	// Don't rotate character to controller rotation (top-down camera)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure Character Movement for top-down gameplay
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		// Don't orient rotation to movement - we handle rotation manually via mouse cursor
		MovementComp->bOrientRotationToMovement = false;
		
		// Don't use controller desired rotation - we set rotation directly
		MovementComp->bUseControllerDesiredRotation = false;
		
		// Set rotation rate for smooth rotation (if we add interpolation later)
		MovementComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // 540 degrees per second on Yaw
	}

	// Create Spring Arm Component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 500.0f; // Distance from character
	SpringArmComponent->bUsePawnControlRotation = false; // Don't rotate with controller
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;
	
	// Set Spring Arm rotation for top-down view
	SpringArmComponent->SetRelativeRotation(FRotator(-65.0f, 0.0f, 0.0f)); // 65 degrees down
	SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f)); // Position above character

	// Create Camera Component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false; // Camera doesn't rotate with controller

	// Create Inventory Component
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// Create Attribute Components
	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	AcquiredAttributeComponent = CreateDefaultSubobject<UAcquiredAttributeComponent>(TEXT("AcquiredAttributeComponent"));
	TrainingComponent = CreateDefaultSubobject<UTrainingComponent>(TEXT("TrainingComponent"));
	SecondaryAttributeComponent = CreateDefaultSubobject<USecondaryAttributeComponent>(TEXT("SecondaryAttributeComponent"));
	StatusEffectComponent = CreateDefaultSubobject<UStatusEffectComponent>(TEXT("StatusEffectComponent"));

	// Create Equipment Component
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));

	MainHandAttachmentName = TEXT("HandGrip_R");
	WeaponGripTransform = FTransform::Identity;
	// The imported sword spans about 7.9 m in source units; display it as an 80 cm weapon.
	// Point the blade forward and place the imported mesh's hilt on the hand socket.
	WeaponGripTransform.SetScale3D(FVector(0.1f));
	WeaponGripTransform.SetRotation(FRotator(0.0f, 0.0f, -90.0f).Quaternion());
	WeaponGripTransform.SetLocation(FVector(0.0f, 0.0f, 30.0f));
	EquippedWeaponVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EquippedWeaponVisual"));
	EquippedWeaponVisual->SetupAttachment(GetMesh(), MainHandAttachmentName);
	EquippedWeaponVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EquippedWeaponVisual->SetGenerateOverlapEvents(false);
	EquippedWeaponVisual->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UAnimSequence> AttackSequence(
		TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Attack/MM_Attack_01_InPlace.MM_Attack_01_InPlace"));
	if (AttackSequence.Succeeded())
	{
		BasicMeleeAttackAnimation = AttackSequence.Object;
	}

	// Create Progression Components
	ExperienceComponent = CreateDefaultSubobject<UExperienceComponent>(TEXT("ExperienceComponent"));
	ClassComponent = CreateDefaultSubobject<UClassComponent>(TEXT("ClassComponent"));
	TrialComponent = CreateDefaultSubobject<UTrialComponent>(TEXT("TrialComponent"));
	QuestComponent = CreateDefaultSubobject<UQuestComponent>(TEXT("QuestComponent"));
}

void AActionRPGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	bIsDead = false;

	if (EquipmentComponent)
	{
		EquipmentComponent->OnEquipmentChangedNative.AddUObject(this, &AActionRPGPlayerCharacter::HandleEquipmentChanged);
		RefreshMainHandVisual();
	}
	
	// Verify character movement component exists
	if (!GetCharacterMovement())
	{
		UE_LOG(LogTemp, Error, TEXT("ActionRPGPlayerCharacter: CharacterMovementComponent is NULL!"));
	}
	else
	{
		// Ensure movement settings are correct (in case Blueprint overrides them)
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}

	// Set camera as view target for player controller
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetViewTarget(this);
	}

	// Bind to inventory item used event
	if (InventoryComponent)
	{
		InventoryComponent->OnItemUsed.AddDynamic(this, &AActionRPGPlayerCharacter::OnItemUsed);
		UE_LOG(LogTemp, Log, TEXT("ActionRPGPlayerCharacter: Bound to InventoryComponent OnItemUsed event"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter: InventoryComponent is NULL, cannot bind to OnItemUsed"));
	}
	if (StarterRegenerationSkillClass)
	{
		if (USkillManagerComponent* Skills = FindComponentByClass<USkillManagerComponent>())
		{
			USkillBase* StarterSkill = NewObject<USkillBase>(this, StarterRegenerationSkillClass);
			if (Skills->UnlockSkill(StarterSkill) && !Skills->IsSlotOccupied(7))
			{
				Skills->AssignSkillToSlot(7, StarterSkill);
			}
		}
	}
	if (StarterBasicAttackSkillClass)
	{
		if (USkillManagerComponent* Skills = FindComponentByClass<USkillManagerComponent>())
		{
			USkillBase* StarterSkill = NewObject<USkillBase>(this, StarterBasicAttackSkillClass);
			if (Skills->UnlockSkill(StarterSkill))
			{
				Skills->AssignMainHandSkill(StarterSkill);
			}
		}
	}
}

float AActionRPGPlayerCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (bDodgeActive && !bIsDead && FMath::IsFinite(DamageAmount) && DamageAmount > 0.0f)
	{
		AActionRPGEnemyCharacter* Enemy = Cast<AActionRPGEnemyCharacter>(DamageCauser);
		if (!Enemy && DamageCauser) Enemy = Cast<AActionRPGEnemyCharacter>(DamageCauser->GetOwner());
		if (!Enemy && EventInstigator) Enemy = Cast<AActionRPGEnemyCharacter>(EventInstigator->GetPawn());
		if (Enemy && !Enemy->IsDead()) RecordSuccessfulDodge(Enemy);
		return 0.0f;
	}
	if (!SecondaryAttributeComponent)
	{
		return 0.0f;
	}

	const float CurrentHealth = SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth);
	const TOptional<EDamageType> DamageKind = FActionRPGDamageTypes::GetKind(DamageEvent.DamageTypeClass);
	const bool bPhysicalHit = DamageKind.IsSet() && DamageKind.GetValue() == EDamageType::Physical;
	const float Armor = bPhysicalHit ? SecondaryAttributeComponent->GetSecondaryAttribute(ESecondaryAttribute::Armor) : 0.0f;
	const USecondaryAttributeDataAsset* Rules = SecondaryAttributeComponent->SecondaryAttributeDataAsset;
	const float ArmorScale = Rules ? Rules->ArmorMitigationScale : 100.0f;
	float ResistedDamage = DamageAmount;
	if (DamageKind.IsSet())
	{
		ResistedDamage *= 1.0f - SecondaryAttributeComponent->GetDamageResistancePercent(DamageKind.GetValue()) / 100.0f;
	}
	const FActionRPGDamageResult Result = bPhysicalHit
		? FActionRPGDamageResolver::ResolvePhysical(ResistedDamage, CurrentHealth, bIsDead, Armor, ArmorScale)
		: FActionRPGDamageResolver::Resolve(ResistedDamage, CurrentHealth, bIsDead);
	if (Result.AppliedDamage <= 0.0f)
	{
		return 0.0f;
	}

	Super::TakeDamage(Result.AppliedDamage, DamageEvent, EventInstigator, DamageCauser);
	SecondaryAttributeComponent->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth, Result.RemainingHealth);
	if (Result.bKilled)
	{
		HandleDeath(DamageCauser);
	}
	else if (Result.AppliedDamage >= 5.0f && TrainingComponent)
	{
		AActionRPGEnemyCharacter* Enemy = Cast<AActionRPGEnemyCharacter>(DamageCauser);
		if (!Enemy && DamageCauser) Enemy = Cast<AActionRPGEnemyCharacter>(DamageCauser->GetOwner());
		if (!Enemy && EventInstigator) Enemy = Cast<AActionRPGEnemyCharacter>(EventInstigator->GetPawn());
		if (Enemy && !Enemy->IsDead())
		{
			TrainingComponent->RecordPractice(EPrimaryAttribute::Constitution, TEXT("SurviveEnemyHit"), Enemy);
		}
	}
	return Result.AppliedDamage;
}

void AActionRPGPlayerCharacter::HandleDeath(AActor* DamageCauser)
{
	if (bIsDead)
	{
		return;
	}
	bIsDead = true;
	GetWorldTimerManager().ClearTimer(DodgeEndTimer);
	bDodgeActive = false;
	DodgePracticeTarget = nullptr;
	if (StatusEffectComponent) StatusEffectComponent->RemoveEffectsOnDeath();
	if (SecondaryAttributeComponent)
	{
		SecondaryAttributeComponent->SetHealthRegenerationSuppressed(true);
	}
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->DisableMovement();
	}
	OnPlayerDied.Broadcast(this, DamageCauser);
}

bool AActionRPGPlayerCharacter::Revive(float HealthFraction)
{
	if (!bIsDead || !SecondaryAttributeComponent || !FMath::IsFinite(HealthFraction)
		|| HealthFraction <= 0.0f || HealthFraction > 1.0f)
	{
		return false;
	}
	const float MaxHealth = SecondaryAttributeComponent->GetSecondaryAttribute(ESecondaryAttribute::MaxHealth);
	if (!FMath::IsFinite(MaxHealth) || MaxHealth <= 0.0f)
	{
		return false;
	}
	bIsDead = false;
	SecondaryAttributeComponent->SetCurrentSecondaryAttribute(
		ESecondaryAttribute::MaxHealth, FMath::Max(1.0f, MaxHealth * HealthFraction));
	SecondaryAttributeComponent->SetHealthRegenerationSuppressed(false);
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->SetMovementMode(MOVE_Walking);
	}
	OnPlayerRevived.Broadcast(this);
	return true;
}

void AActionRPGPlayerCharacter::HandleEquipmentChanged(EEquipmentSlot Slot, UEquipmentItem* Item)
{
	if (Slot == EEquipmentSlot::MainHand)
	{
		RefreshMainHandVisual();
	}
}

void AActionRPGPlayerCharacter::RefreshMainHandVisual()
{
	if (!EquippedWeaponVisual || !EquipmentComponent || !GetMesh())
	{
		return;
	}

	const FName AttachmentName = GetMesh()->DoesSocketExist(MainHandAttachmentName)
		? MainHandAttachmentName : FName(TEXT("hand_r"));
	if (!GetMesh()->DoesSocketExist(AttachmentName))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player weapon visual: no main-hand bone or socket on the character mesh"));
		EquippedWeaponVisual->SetStaticMesh(nullptr);
		EquippedWeaponVisual->SetVisibility(false);
		return;
	}

	EquippedWeaponVisual->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachmentName);
	EquippedWeaponVisual->SetRelativeTransform(WeaponGripTransform);

	const UEquipmentItem* MainHandItem = EquipmentComponent->GetEquippedItem(EEquipmentSlot::MainHand);
	UStaticMesh* WeaponMesh = MainHandItem && MainHandItem->ItemData
		? MainHandItem->ItemData->EquippedWeaponMesh.Get() : nullptr;
	EquippedWeaponVisual->SetStaticMesh(WeaponMesh);
	EquippedWeaponVisual->SetVisibility(WeaponMesh != nullptr);
}

void AActionRPGPlayerCharacter::PlayBasicMeleeAttackAnimation(float AttackIntervalSeconds)
{
	const bool bHasMainHandItem = EquipmentComponent &&
		EquipmentComponent->GetEquippedItem(EEquipmentSlot::MainHand) != nullptr;
	UAnimSequenceBase* Animation = bHasMainHandItem && EquippedMeleeAttackAnimation
		? EquippedMeleeAttackAnimation.Get() : BasicMeleeAttackAnimation.Get();
	if (!Animation || !GetMesh())
	{
		return;
	}

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float ClipLength = Animation->GetPlayLength();
		const float PlayRate = AttackIntervalSeconds > KINDA_SMALL_NUMBER && ClipLength > KINDA_SMALL_NUMBER
			? ClipLength / AttackIntervalSeconds : 1.0f;
		if (!AnimInstance->PlaySlotAnimationAsDynamicMontage(
			Animation, TEXT("DefaultSlot"), 0.08f, 0.15f, FMath::Max(0.1f, PlayRate)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player melee attack animation could not play on DefaultSlot"));
		}
	}
}

UAnimMontage* AActionRPGPlayerCharacter::PlayMagicCastAnimation(float& OutReleaseDelaySeconds)
{
	OutReleaseDelaySeconds = 0.0f;
	if (!MagicCastAnimation || !GetMesh())
	{
		return nullptr;
	}

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float PlayRate = FMath::Max(0.1f, MagicCastPlayRate);
		if (UAnimMontage* Montage = AnimInstance->PlaySlotAnimationAsDynamicMontage(
			MagicCastAnimation, TEXT("DefaultSlot"), 0.08f, 0.15f,
			PlayRate))
		{
			OutReleaseDelaySeconds = MagicCastAnimation->GetPlayLength()
				* FMath::Clamp(MagicProjectileReleaseFraction, 0.05f, 0.95f) / PlayRate;
			return Montage;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Player magic cast animation could not play on DefaultSlot"));
		}
	}
	return nullptr;
}

void AActionRPGPlayerCharacter::Move(const FVector2D& MovementVector)
{
	// Early return if no movement input
	if (MovementVector.IsZero())
	{
		return;
	}

	// Get character's forward and right vectors based on current rotation
	// MovementVector.X = forward/backward (W/S keys)
	// MovementVector.Y = left/right (A/D keys)
	FRotator CharacterRotation = GetActorRotation();
	FVector ForwardVector = FRotationMatrix(CharacterRotation).GetUnitAxis(EAxis::X); // Forward
	FVector RightVector = FRotationMatrix(CharacterRotation).GetUnitAxis(EAxis::Y);  // Right
	
	// Calculate movement direction relative to character's facing direction
	// This allows movement relative to where the character is facing (top-down style)
	FVector Direction = (ForwardVector * MovementVector.X) + (RightVector * MovementVector.Y);
	Direction.Z = 0.0f; // Keep movement on horizontal plane
	Direction.Normalize();
	LastMovementDirection = Direction;

	// Apply movement to Character Movement Component
	if (GetCharacterMovement())
	{
		AddMovementInput(Direction, 1.0f);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Move: CharacterMovementComponent is NULL!"));
	}
}

bool AActionRPGPlayerCharacter::TryDodge()
{
	UWorld* World = GetWorld();
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (!World || bIsDead || bDodgeActive || !Movement || Movement->MovementMode == MOVE_None ||
		!SecondaryAttributeComponent || !FMath::IsFinite(DodgeStaminaCost) || DodgeStaminaCost < 0.0f ||
		!FMath::IsFinite(DodgeCooldownSeconds) || DodgeCooldownSeconds <= 0.0f ||
		!FMath::IsFinite(DodgeDurationSeconds) || DodgeDurationSeconds <= 0.0f ||
		!FMath::IsFinite(DodgeLaunchSpeed) || DodgeLaunchSpeed <= 0.0f ||
		World->GetTimeSeconds() - LastDodgeStartTime < DodgeCooldownSeconds ||
		!SecondaryAttributeComponent->TrySpendResource(ESecondaryAttribute::MaxStamina, DodgeStaminaCost))
	{
		return false;
	}

	DodgePracticeTarget = nullptr;
	float BestDistance = 400.0f;
	for (TActorIterator<AActionRPGEnemyCharacter> It(World); It; ++It)
	{
		if (It->IsDead()) continue;
		const float Distance = FVector::Dist2D(GetActorLocation(), It->GetActorLocation());
		if (Distance < BestDistance)
		{
			BestDistance = Distance;
			DodgePracticeTarget = *It;
		}
	}

	FVector Direction = LastMovementDirection.GetSafeNormal2D();
	if (Direction.IsNearlyZero() && DodgePracticeTarget)
	{
		Direction = (GetActorLocation() - DodgePracticeTarget->GetActorLocation()).GetSafeNormal2D();
	}
	if (Direction.IsNearlyZero()) Direction = -GetActorForwardVector().GetSafeNormal2D();
	if (Direction.IsNearlyZero()) Direction = FVector::BackwardVector;

	DodgeStartLocation = GetActorLocation();
	DodgeStartTargetDistance = DodgePracticeTarget
		? FVector::Dist2D(DodgeStartLocation, DodgePracticeTarget->GetActorLocation()) : 0.0f;
	bDodgePracticeAwarded = false;
	bDodgeSuccessNoted = false;
	bDodgeActive = true;
	LastDodgeStartTime = World->GetTimeSeconds();
	LaunchCharacter(Direction * DodgeLaunchSpeed, true, false);
	World->GetTimerManager().SetTimer(DodgeEndTimer, this, &AActionRPGPlayerCharacter::FinishDodge,
		DodgeDurationSeconds, false);
	return true;
}

void AActionRPGPlayerCharacter::RecordSuccessfulDodge(AActionRPGEnemyCharacter* Enemy)
{
	if (!bDodgeSuccessNoted && IsValid(Enemy))
	{
		bDodgeSuccessNoted = true;
	}
	if (!bDodgePracticeAwarded && TrainingComponent && IsValid(Enemy))
	{
		bDodgePracticeAwarded = TrainingComponent->RecordPractice(
			EPrimaryAttribute::Agility, TEXT("CombatDodge"), Enemy);
	}
}

void AActionRPGPlayerCharacter::FinishDodge()
{
	if (!bDodgeActive) return;
	GetWorldTimerManager().ClearTimer(DodgeEndTimer);
	bDodgeActive = false;
	if (!bDodgePracticeAwarded && !bIsDead && IsValid(DodgePracticeTarget) && !DodgePracticeTarget->IsDead())
	{
		const float Displacement = FVector::Dist2D(DodgeStartLocation, GetActorLocation());
		const float SeparationGain = FVector::Dist2D(GetActorLocation(), DodgePracticeTarget->GetActorLocation())
			- DodgeStartTargetDistance;
		if (Displacement >= 100.0f && SeparationGain >= 80.0f)
		{
			RecordSuccessfulDodge(DodgePracticeTarget);
		}
	}
	DodgePracticeTarget = nullptr;
}

void AActionRPGPlayerCharacter::Look(const FVector2D& LookVector)
{
	// For top-down gameplay, character rotation is handled by RotateToMouseCursor() in Tick
	// This function is called by the input system but doesn't need to do anything
	// The LookVector parameter could be used for camera rotation if needed in the future
}

void AActionRPGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Continuously update character rotation to face mouse cursor
	RotateToMouseCursor();
}

void AActionRPGPlayerCharacter::RotateToMouseCursor()
{
	// Get the player controller (required for mouse projection)
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	// Project mouse cursor position to world space
	FVector WorldLocation;
	FVector WorldDirection;
	
	if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		// Calculate intersection point on the ground plane at character's height
		FVector CharacterLocation = GetActorLocation();
		float PlaneZ = CharacterLocation.Z;
		
		// Calculate intersection with horizontal plane using ray-plane intersection
		if (FMath::Abs(WorldDirection.Z) > 0.001f) // Avoid division by zero
		{
			// Ray-plane intersection formula: T = (PlaneZ - RayOrigin.Z) / RayDirection.Z
			float T = (PlaneZ - WorldLocation.Z) / WorldDirection.Z;
			FVector TargetLocation = WorldLocation + (WorldDirection * T);
			TargetLocation.Z = CharacterLocation.Z; // Ensure target is at character's height
			
			// Calculate direction from character to target location
			FVector Direction = TargetLocation - CharacterLocation;
			Direction.Z = 0.0f; // Keep rotation on horizontal plane only
			
			// Only rotate if direction is significant (avoid jitter from tiny movements)
			if (!Direction.IsNearlyZero(1.0f))
			{
				// Calculate rotation to face the target
				FRotator NewRotation = Direction.Rotation();
				
				// Set character rotation to face mouse cursor (Yaw only, no pitch/roll)
				SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
			}
		}
	}
}

void AActionRPGPlayerCharacter::OnItemUsed(UItemBase* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter::OnItemUsed - EVENT FIRED! Item: %s"), 
		Item ? TEXT("Valid") : TEXT("NULL"));

	if (!Item || !Item->ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter::OnItemUsed - Item or ItemData is NULL"));
		return;
	}

	FName ItemID = Item->ItemData->ItemID;
	EItemType ItemType = Item->ItemData->Type;
	FString ItemIDString = ItemID.ToString();
	FString ItemNameString = Item->ItemData->ItemName.ToString();

	UE_LOG(LogTemp, Warning, TEXT("ActionRPGPlayerCharacter::OnItemUsed - Item used: %s (ID: %s, Type: %d)"),
		*ItemNameString, *ItemIDString, (int32)ItemType);
}
