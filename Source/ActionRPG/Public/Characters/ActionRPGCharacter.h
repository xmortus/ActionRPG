// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ActionRPGCharacter.generated.h"

UCLASS(Blueprintable)
class ACTIONRPG_API AActionRPGCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AActionRPGCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
};

