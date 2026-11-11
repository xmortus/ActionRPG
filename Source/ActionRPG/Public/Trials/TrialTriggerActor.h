// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrialTriggerActor.generated.h"

class AActionRPGPlayerCharacter;
class UPrimitiveComponent;
class USphereComponent;
class UTextRenderComponent;
class UTrialDataAsset;

UENUM(BlueprintType)
enum class ETrialTriggerAction : uint8
{
	Start,
	AdvanceObjective
};

/** A C++ world marker that starts a trial or reports one objective on player overlap. */
UCLASS(Blueprintable)
class ACTIONRPG_API ATrialTriggerActor : public AActor
{
	GENERATED_BODY()

public:
	ATrialTriggerActor();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Processes a qualifying player; exposed for deterministic automation tests. */
	UFUNCTION(BlueprintCallable, Category = "Trials")
	bool TryTrigger(AActionRPGPlayerCharacter* Player);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trials")
	TObjectPtr<UTrialDataAsset> Trial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trials")
	ETrialTriggerAction Action = ETrialTriggerAction::Start;

	/** Used only when Action is AdvanceObjective. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trials", meta = (EditCondition = "Action == ETrialTriggerAction::AdvanceObjective"))
	FName ObjectiveId;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTextRenderComponent> MarkerText;

private:
	void RefreshMarkerText();
	void FaceMarkerTowardPlayerCamera();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
};
