// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExperienceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExperienceChanged, float, NewUnallocatedExperience, float, Delta);

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UExperienceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UExperienceComponent();

	UFUNCTION(BlueprintCallable, Category = "Experience")
	void AddExperience(float Amount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Experience")
	float GetUnallocatedExperience() const { return UnallocatedExperience; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Experience")
	float GetTotalExperience() const { return TotalExperience; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Experience")
	bool CanSpendExperience(float Amount) const;

	UFUNCTION(BlueprintCallable, Category = "Experience")
	bool SpendExperience(float Amount);

	UPROPERTY(BlueprintAssignable, Category = "Experience|Events")
	FOnExperienceChanged OnExperienceChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Experience", meta = (ClampMin = "0.0"))
	float UnallocatedExperience = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Experience", meta = (ClampMin = "1.0"))
	float TotalExperience = 0.0f;
};
