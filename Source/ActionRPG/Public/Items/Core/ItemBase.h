// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemDataAsset.h"
#include "ItemBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API UItemBase : public UObject
{
	GENERATED_BODY()

public:
	UItemBase();

	// Item Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UItemDataAsset> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity;

	// Item Usage
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void Use();

	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual bool CanUse() const;

	// Item Information
	UFUNCTION(BlueprintCallable, Category = "Item")
	FName GetItemID() const;

	UFUNCTION(BlueprintCallable, Category = "Item")
	FText GetItemName() const;

	UFUNCTION(BlueprintCallable, Category = "Item")
	EItemType GetItemType() const;

	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUsed, UItemBase*, Item);
	
	UPROPERTY(BlueprintAssignable, Category = "Item")
	FOnItemUsed OnItemUsed;
};

