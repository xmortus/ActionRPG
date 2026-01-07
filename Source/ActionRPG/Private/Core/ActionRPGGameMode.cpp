// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/ActionRPGGameMode.h"
#include "Core/ActionRPGPlayerController.h"
#include "Core/ActionRPGGameState.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "UI/HUD/ActionRPGHUD.h"

AActionRPGGameMode::AActionRPGGameMode()
{
	// Set default pawn class to our player character
	DefaultPawnClass = AActionRPGPlayerCharacter::StaticClass();
	
	// Set default player controller class
	PlayerControllerClass = AActionRPGPlayerController::StaticClass();
	
	// Set default game state class
	GameStateClass = AActionRPGGameState::StaticClass();
	
	// Set default HUD class
	HUDClass = AActionRPGHUD::StaticClass();
}

