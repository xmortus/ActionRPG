// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Core/ItemBase.h"
#include "InventoryComponent.generated.h"

/**
 * Structure representing a single inventory slot.
 * Contains an item reference, quantity, and empty status.
 */
USTRUCT(BlueprintType)
struct ACTIONRPG_API FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UItemBase> Item;

	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	int32 Quantity;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bIsEmpty;

	FInventorySlot()
		: Item(nullptr), Quantity(0), bIsEmpty(true)
	{}
};

/**
 * Enum for quick-use slot type.
 * Slots 1-8 are for skills (Phase 3), slots 9-10 are for consumables (Phase 2).
 */
UENUM(BlueprintType)
enum class EQuickUseSlotType : uint8
{
	Skill		UMETA(DisplayName = "Skill"),      // Slots 1-8 (Phase 3)
	Consumable	UMETA(DisplayName = "Consumable")  // Slots 9-10 (Phase 2)
};

/**
 * Structure representing a quick-use slot.
 * Contains item reference, inventory slot index, and slot type.
 */
USTRUCT(BlueprintType)
struct ACTIONRPG_API FQuickUseSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Quick Use")
	TObjectPtr<UItemBase> Item;

	UPROPERTY(BlueprintReadWrite, Category = "Quick Use")
	int32 InventorySlotIndex; // -1 if not assigned

	UPROPERTY(BlueprintReadWrite, Category = "Quick Use")
	EQuickUseSlotType SlotType;

	FQuickUseSlot()
		: Item(nullptr), InventorySlotIndex(-1), SlotType(EQuickUseSlotType::Consumable)
	{}
};

/**
 * Inventory Component for managing player inventory.
 * Handles item storage, stacking, weight/capacity limits, and item operations.
 * Supports drag and drop, item usage, and inventory events.
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	// Component lifecycle
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Inventory Management
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UItemBase* Item, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(int32 SlotIndex, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool MoveItem(int32 FromSlot, int32 ToSlot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SwapItems(int32 SlotA, int32 SlotB);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItem(int32 SlotIndex);

	// Stack Splitting
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SplitStack(int32 SlotIndex, int32 SplitQuantity);

	// World Item Dropping
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool DropItemToWorld(int32 SlotIndex, int32 Quantity, const FVector& WorldLocation);

	// Query Methods
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	UItemBase* GetItemAt(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 FindItemSlot(const FName& ItemID) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	bool HasSpaceFor(const UItemBase* Item, int32 Quantity = 1) const;

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetMaxCapacity() const { return MaxCapacity; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	float GetMaxWeight() const { return MaxWeight; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	float GetCurrentWeight() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetTotalItemCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	const TArray<FInventorySlot>& GetInventorySlots() const { return InventorySlots; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetEmptySlotCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetUsedSlotCount() const;

	// Quick-Use Bar Management
	UFUNCTION(BlueprintCallable, Category = "Inventory|Quick Use")
	bool AssignItemToQuickUseSlot(int32 InventorySlotIndex, int32 QuickUseSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Quick Use")
	bool UseQuickUseSlot(int32 QuickUseSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Quick Use")
	void ClearQuickUseSlot(int32 QuickUseSlotIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Quick Use")
	FQuickUseSlot GetQuickUseSlot(int32 QuickUseSlotIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Quick Use")
	TArray<FQuickUseSlot> GetAllQuickUseSlots() const { return QuickUseSlots; }

	// Debug
	UFUNCTION(BlueprintCallable, Category = "Inventory|Debug")
	void ReportInventoryContents() const;

	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryChanged, int32, SlotIndex, UItemBase*, Item);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, UItemBase*, Item);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, UItemBase*, Item, int32, Quantity);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUsed, UItemBase*, Item);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuickUseSlotChanged, int32, QuickUseSlotIndex, UItemBase*, Item);

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events", meta = (DisplayName = "On Inventory Changed"))
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events", meta = (DisplayName = "On Item Added"))
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events", meta = (DisplayName = "On Item Removed"))
	FOnItemRemoved OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events", meta = (DisplayName = "On Item Used"))
	FOnItemUsed OnItemUsed;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Quick Use|Events", meta = (DisplayName = "On Quick Use Slot Changed"))
	FOnQuickUseSlotChanged OnQuickUseSlotChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FInventorySlot> InventorySlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "1", ClampMax = "1000"))
	int32 MaxCapacity = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = "0.0", ClampMax = "10000.0"))
	float MaxWeight = 100.0f;

	// Quick-Use Bar (10 slots: 1-8 for skills, 9-10 for consumables)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quick Use", meta = (AllowPrivateAccess = "true"))
	TArray<FQuickUseSlot> QuickUseSlots;

private:
	// Helper methods for item stacking
	bool TryStackItem(UItemBase* Item, int32 Quantity, int32& RemainingQuantity);
	int32 FindEmptySlot() const;
	void UpdateSlotEmptyStatus(int32 SlotIndex);
	void BroadcastInventoryChanged(int32 SlotIndex, UItemBase* Item);
};
