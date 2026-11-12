// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ActionRPGCharacter.h"
#include "Items/Core/ItemTypes.h"
#include "Progression/Core/ProgressionTypes.h"
#include "TimerManager.h"
#include "ActionRPGPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAttributeComponent;
class UAcquiredAttributeComponent;
class USecondaryAttributeComponent;
class UStatusEffectComponent;
class USkillBase;
class UEquipmentComponent;
class UExperienceComponent;
class UClassComponent;
class UTrialComponent;
class UQuestComponent;
class UTrainingComponent;
class AActionRPGEnemyCharacter;
class UStaticMeshComponent;
class UAnimSequenceBase;
class UAnimMontage;
class UEquipmentItem;
class AActionRPGPlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerDied, AActionRPGPlayerCharacter*, Player, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerRevived, AActionRPGPlayerCharacter*, Player);

/**
 * Player-specific Character class for ActionRPG.
 * Handles player movement, camera setup, and input.
 * Configured for top-down action RPG gameplay with mouse-driven rotation.
 */
UCLASS(Blueprintable)
class ACTIONRPG_API AActionRPGPlayerCharacter : public AActionRPGCharacter
{
	GENERATED_BODY()

public:
	AActionRPGPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "Combat|Life")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "Combat|Life", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	bool Revive(float HealthFraction = 1.0f);

	UPROPERTY(BlueprintAssignable, Category = "Combat|Life")
	FOnPlayerDied OnPlayerDied;

	UPROPERTY(BlueprintAssignable, Category = "Combat|Life")
	FOnPlayerRevived OnPlayerRevived;

	// Movement
	void Move(const FVector2D& MovementVector);
	void Look(const FVector2D& LookVector);

	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<class UInventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAcquiredAttributeComponent> AcquiredAttributeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<USecondaryAttributeComponent> SecondaryAttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStatusEffectComponent> StatusEffectComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skills")
	TSubclassOf<USkillBase> StarterRegenerationSkillClass;

	/** Basic attack known at character creation and assigned to the main hand. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skills")
	TSubclassOf<USkillBase> StarterBasicAttackSkillClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<UExperienceComponent> ExperienceComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (InstanceEditable = "true"))
	TObjectPtr<UClassComponent> ClassComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTrialComponent> TrialComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UQuestComponent> QuestComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTrainingComponent> TrainingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Visual")
	TObjectPtr<UStaticMeshComponent> EquippedWeaponVisual;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Visual")
	FName MainHandAttachmentName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Visual")
	FTransform WeaponGripTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation")
	TObjectPtr<UAnimSequenceBase> BasicMeleeAttackAnimation;

	/** Optional swing used for an equipped main-hand weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation")
	TObjectPtr<UAnimSequenceBase> EquippedMeleeAttackAnimation;

	/** Optional casting motion used by projectile magic skills. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation")
	TObjectPtr<UAnimSequenceBase> MagicCastAnimation;

	/** Speeds up long showcase clips to fit the prototype's combat pace. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation", meta = (ClampMin = "0.1"))
	float MagicCastPlayRate = 2.5f;

	/** Point in the cast clip where the raised-hand motion releases the projectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation", meta = (ClampMin = "0.05", ClampMax = "0.95"))
	float MagicProjectileReleaseFraction = 0.6f;

	void PlayBasicMeleeAttackAnimation(float AttackIntervalSeconds);
	UAnimMontage* PlayMagicCastAnimation(float& OutReleaseDelaySeconds);

	/** Starts a short stamina-costing dodge in the last movement direction. */
	UFUNCTION(BlueprintCallable, Category = "Combat|Dodge")
	bool TryDodge();

	UFUNCTION(BlueprintPure, Category = "Combat|Dodge")
	bool IsDodging() const { return bDodgeActive; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Dodge", meta = (ClampMin = "0.0"))
	float DodgeStaminaCost = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Dodge", meta = (ClampMin = "0.1"))
	float DodgeCooldownSeconds = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Dodge", meta = (ClampMin = "0.1"))
	float DodgeDurationSeconds = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Dodge", meta = (ClampMin = "1.0"))
	float DodgeLaunchSpeed = 900.0f;

protected:
#if WITH_DEV_AUTOMATION_TESTS
	friend class FCombatDodgeTrainingTest;
#endif
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Rotate character to face mouse cursor
	void RotateToMouseCursor();

	// Item usage handler
	UFUNCTION()
	void OnItemUsed(class UItemBase* Item);

	UFUNCTION()
	void HandleEquipmentChanged(EEquipmentSlot Slot, UEquipmentItem* Item);

	void RefreshMainHandVisual();

	void HandleDeath(AActor* DamageCauser);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat|Life")
	bool bIsDead = false;

private:
	void FinishDodge();
	void RecordSuccessfulDodge(AActionRPGEnemyCharacter* Enemy);

	UPROPERTY()
	TObjectPtr<AActionRPGEnemyCharacter> DodgePracticeTarget;

	FTimerHandle DodgeEndTimer;
	FVector LastMovementDirection = FVector::ZeroVector;
	FVector DodgeStartLocation = FVector::ZeroVector;
	float DodgeStartTargetDistance = 0.0f;
	double LastDodgeStartTime = -1000000.0;
	bool bDodgeActive = false;
	bool bDodgePracticeAwarded = false;
	bool bDodgeSuccessNoted = false;

};

