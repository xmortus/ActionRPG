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

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UCharacterStatusPanelWidget> CharacterStatusPanelWidgetClass;

	UPROPERTY()
	TObjectPtr<UCharacterStatusPanelWidget> CharacterStatusPanelWidget;

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

