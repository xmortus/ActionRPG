// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "ActionRPGPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;
class UInventoryWidget;
class UEquipmentWidget;
class UUnlockedSkillsPanelWidget;
class UCharacterStatusPanelWidget;
class UClassSelectionPanelWidget;
class USkillManagerComponent;
class UTrialTrackerWidget;
class UQuestTrackerWidget;
class UCraftingPanelWidget;
class UVitalBarsWidget;
class UPlayerDeathWidget;
class UPrototypeMenuWidget;
class AActionRPGPlayerCharacter;

/**
 * PlayerController for ActionRPG.
 * Handles Enhanced Input System integration and routes input to the player character.
 * Supports movement, look, and action inputs (interact, attack, dodge, inventory, skill panel, skill slots 1-8).
 */
UCLASS()
class ACTIONRPG_API AActionRPGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AActionRPGPlayerController();

	/**
	 * Get the InputAction for a quick-use slot index.
	 * @param SlotIndex The slot index (0-9)
	 * @return The InputAction for this slot, or nullptr if invalid
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Input")
	UInputAction* GetInputActionForQuickUseSlot(int32 SlotIndex) const;

	/**
	 * Get the default Input Mapping Context.
	 * @return The DefaultMappingContext, or nullptr if not set
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Input")
	UInputMappingContext* GetDefaultMappingContext() const { return DefaultMappingContext; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Input")
	UInputAction* GetAttackInputAction() const { return AttackAction; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Input")
	UInputAction* GetOffhandInputAction() const { return OffhandAction; }

	/** Apply damage to the possessed player for PIE combat and respawn testing. */
	UFUNCTION(Exec)
	void RPGTestDamageSelf(float DamageAmount);

	/** Print current Strength practice and permanent training gain in PIE. */
	UFUNCTION(Exec)
	void RPGTrainingStatus();

	/** Grant free skill points for testing upgrades in PIE only. */
	UFUNCTION(Exec)
	void RPGTestGrantSkillPoints(int32 Points);

	/** Grant unallocated XP to the possessed player for PIE progression checks. */
	UFUNCTION(Exec)
	void RPGTestGrantXP(float Amount);

protected:
#if WITH_DEV_AUTOMATION_TESTS
	friend class FPlayerDeathFlowTest;
#endif
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	void EnsureTrialTracker();
	void EnsureQuestTracker();
	void EnsureCraftingPanel();
	void EnsureVitalBars();
	void ShowPrototypeMenu(bool bInitial);
	void TogglePauseMenu();
	UFUNCTION()
	void HandleMenuPlay();
	UFUNCTION()
	void HandleMenuExit();
	void BindPlayerLifeEvents();
	UFUNCTION()
	void HandlePlayerDied(AActionRPGPlayerCharacter* DeadCharacter, AActor* DamageCauser);
	UFUNCTION()
	void HandlePlayerRevived(AActionRPGPlayerCharacter* RevivedCharacter);
	UFUNCTION()
	void RequestRespawn();
	void UnlockRespawn();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Life", meta = (ClampMin = "0.0"))
	float RespawnDelaySeconds = 3.0f;
	UPROPERTY()
	TObjectPtr<AActionRPGPlayerCharacter> LifeEventPlayer;
	UPROPERTY()
	TObjectPtr<UPlayerDeathWidget> DeathWidget;
	UPROPERTY()
	TObjectPtr<UPrototypeMenuWidget> PrototypeMenuWidget;
	bool bHasStartedPrototype = false;
	FTimerHandle RespawnUnlockTimer;
	FTransform InitialPlayerTransform;

	UPROPERTY()
	TObjectPtr<UTrialTrackerWidget> TrialTrackerWidget;

	UPROPERTY()
	TObjectPtr<UQuestTrackerWidget> QuestTrackerWidget;

	UPROPERTY()
	TObjectPtr<UCraftingPanelWidget> CraftingPanelWidget;

	UPROPERTY()
	TObjectPtr<UVitalBarsWidget> VitalBarsWidget;

	// Enhanced Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> OffhandAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> OpenInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> OpenEquipmentAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> OpenSkillPanelAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> OpenCharacterStatusPanelAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> OpenClassPanelAction;

	// UI Widgets
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UEquipmentWidget> EquipmentWidgetClass;

	UPROPERTY()
	TObjectPtr<UEquipmentWidget> EquipmentWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUnlockedSkillsPanelWidget> SkillPanelWidgetClass;

	UPROPERTY()
	TObjectPtr<UUnlockedSkillsPanelWidget> SkillPanelWidget;

	// Retained for existing Blueprint serialization; the character-status panel is built natively.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UCharacterStatusPanelWidget> CharacterStatusPanelWidgetClass;

	UPROPERTY()
	TObjectPtr<UCharacterStatusPanelWidget> CharacterStatusPanelWidget;

	// Retained for existing Blueprint serialization; the class/profession panel is built natively.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UClassSelectionPanelWidget> ClassSelectionPanelWidgetClass;

	UPROPERTY()
	TObjectPtr<UClassSelectionPanelWidget> ClassSelectionPanelWidget;

	// Skill Slot Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Skills")
	TObjectPtr<UInputAction> SkillSlot1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Skills")
	TObjectPtr<UInputAction> SkillSlot2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Skills")
	TObjectPtr<UInputAction> SkillSlot3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Skills")
	TObjectPtr<UInputAction> SkillSlot4Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Skills")
	TObjectPtr<UInputAction> SkillSlot5Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Skills")
	TObjectPtr<UInputAction> SkillSlot6Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Skills")
	TObjectPtr<UInputAction> SkillSlot7Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Skills")
	TObjectPtr<UInputAction> SkillSlot8Action;

	// Quick-Use Bar Input Actions (for consumables - Phase 2)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Quick Use")
	TObjectPtr<UInputAction> QuickUseSlot9Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Quick Use")
	TObjectPtr<UInputAction> QuickUseSlot10Action;

	// Input Handlers
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnInteract();
	void OnAttack();
	void OnDodge();
	void OnOffhand();
	void OnOpenInventory();
	void OnOpenEquipment();
	void OnOpenSkillPanel();
	void OnOpenCharacterStatusPanel();
	void OnOpenClassPanel();
	void OnOpenCrafting();

	// Skill Slot Handlers
	void OnSkillSlot1();
	void OnSkillSlot2();
	void OnSkillSlot3();
	void OnSkillSlot4();
	void OnSkillSlot5();
	void OnSkillSlot6();
	void OnSkillSlot7();
	void OnSkillSlot8();

	// Helper to activate skill slot (0-7)
	void ActivateSkillSlot(int32 SlotIndex);

	// Quick-Use Bar Handlers (for consumables - Phase 2)
	UFUNCTION()
	void OnQuickUseSlot9(const FInputActionValue& Value);

	UFUNCTION()
	void OnQuickUseSlot10(const FInputActionValue& Value);
};

