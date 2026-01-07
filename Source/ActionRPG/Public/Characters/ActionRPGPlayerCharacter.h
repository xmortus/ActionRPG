// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/ActionRPGCharacter.h"
#include "ActionRPGPlayerCharacter.generated.h"

UCLASS(Blueprintable)
class ACTIONRPG_API AActionRPGPlayerCharacter : public AActionRPGCharacter
{
	GENERATED_BODY()

public:
	AActionRPGPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	// Movement
	void Move(const FVector2D& MovementVector);
	void Look(const FVector2D& LookVector);

protected:
	virtual void BeginPlay() override;
};

