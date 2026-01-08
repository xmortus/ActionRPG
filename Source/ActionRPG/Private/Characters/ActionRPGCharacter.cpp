// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/ActionRPGCharacter.h"

AActionRPGCharacter::AActionRPGCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Base character class - can be used for NPCs and enemies
	// Player-specific functionality is in ActionRPGPlayerCharacter
	PrimaryActorTick.bCanEverTick = true;
}

void AActionRPGCharacter::BeginPlay()
{
	Super::BeginPlay();
}

