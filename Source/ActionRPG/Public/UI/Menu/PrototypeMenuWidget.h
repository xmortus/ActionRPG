// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PrototypeMenuWidget.generated.h"

class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPrototypeMenuAction);

/** Native start and pause menu shared by PIE and the packaged prototype. */
UCLASS()
class ACTIONRPG_API UPrototypeMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetInitialMenu(bool bInitial);

	UPROPERTY(BlueprintAssignable, Category = "Menu")
	FOnPrototypeMenuAction OnPlayRequested;

	UPROPERTY(BlueprintAssignable, Category = "Menu")
	FOnPrototypeMenuAction OnExitRequested;

protected:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void HandlePlayClicked();

	UFUNCTION()
	void HandleExitClicked();

	UPROPERTY()
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY()
	TObjectPtr<UTextBlock> DescriptionText;

	UPROPERTY()
	TObjectPtr<UTextBlock> PlayButtonText;
};
