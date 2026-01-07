// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/ActionRPGCharacter.h"

AActionRPGCharacter::AActionRPGCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set default values
	PrimaryActorTick.bCanEverTick = true;
}

void AActionRPGCharacter::BeginPlay()
{
	Super::BeginPlay();
}

