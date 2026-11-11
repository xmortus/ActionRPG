// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "SkillDragDropOperation.generated.h"

class USkillBase;

/** Drag payload for assigning an unlocked skill to a hotbar slot. */
UCLASS()
class ACTIONRPG_API USkillDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	TObjectPtr<USkillBase> Skill;

	UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
	bool bWasHandled = false;
};
