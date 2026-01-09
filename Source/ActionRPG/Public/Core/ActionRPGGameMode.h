// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ActionRPGGameMode.generated.h"

/**
 * Base GameMode class for ActionRPG.
 * Sets default classes for PlayerController, Pawn, HUD, GameState, and PlayerState.
 */
UCLASS()
class ACTIONRPG_API AActionRPGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AActionRPGGameMode();
};

