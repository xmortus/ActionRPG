// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestMarkerActor.generated.h"

class AActionRPGPlayerCharacter;
class UPrimitiveComponent;
class USphereComponent;
class UTextRenderComponent;
class UQuestDataAsset;

/** Walk into the marker to accept a quest; return to it to claim the completed reward. */
UCLASS(Blueprintable)
class ACTIONRPG_API AQuestMarkerActor : public AActor
{
	GENERATED_BODY()

public:
	AQuestMarkerActor();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Quests")
	bool TryTrigger(AActionRPGPlayerCharacter* Player);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quests")
	TObjectPtr<UQuestDataAsset> Quest;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> MarkerText;

private:
	void FaceMarkerTowardPlayerCamera();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
};
