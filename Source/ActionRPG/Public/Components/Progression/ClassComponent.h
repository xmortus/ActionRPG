// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Progression/Core/ProgressionTypes.h"
#include "ClassComponent.generated.h"

class UClassDataAsset;
class UProfessionDataAsset;
class UAttributeComponent;
class USecondaryAttributeComponent;
class UExperienceComponent;
class USkillBase;
class USkillComponent;

USTRUCT(BlueprintType)
struct FClassProgress
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	float Experience = 0.0f;
};

USTRUCT()
struct FClassGrantedSkillState
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<USkillBase>> Instances;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClassChanged, UClassDataAsset*, NewClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProfessionChanged, UProfessionDataAsset*, NewProfession);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClassListChanged, UClassDataAsset*, ChangedClass, bool, bAdded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProfessionListChanged, UProfessionDataAsset*, ChangedProfession, bool, bAdded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClassExperienceChanged, UClassDataAsset*, ClassAsset, float, NewExperience);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClassLevelChanged, UClassDataAsset*, ClassAsset, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProfessionExperienceChanged, UProfessionDataAsset*, ProfessionAsset, float, NewExperience);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProfessionLevelChanged, UProfessionDataAsset*, ProfessionAsset, int32, NewLevel);

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONRPG_API UClassComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UClassComponent();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class", meta = (ClampMin = "0"))
	int32 MaxClasses = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class", meta = (ClampMin = "0"))
	int32 MaxProfessions = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class", meta = (ClampMin = "1.0"))
	float BaseExperienceToLevel = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class", meta = (ClampMin = "0.0"))
	float ExperiencePerLevel = 25.0f;

	UFUNCTION(BlueprintCallable, Category = "Class")
	bool SelectClass(UClassDataAsset* NewClass);

	UFUNCTION(BlueprintCallable, Category = "Class")
	bool SelectProfession(UProfessionDataAsset* NewProfession);

	UFUNCTION(BlueprintCallable, Category = "Class")
	bool AddClass(UClassDataAsset* NewClass);

	UFUNCTION(BlueprintCallable, Category = "Class")
	bool AddProfession(UProfessionDataAsset* NewProfession);

	UFUNCTION(BlueprintCallable, Category = "Class")
	bool RemoveClass(UClassDataAsset* ClassToRemove);

	UFUNCTION(BlueprintCallable, Category = "Class")
	bool RemoveProfession(UProfessionDataAsset* ProfessionToRemove);

	UFUNCTION(BlueprintCallable, Category = "Class")
	void ClearClass();

	UFUNCTION(BlueprintCallable, Category = "Class")
	void ClearProfession();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class")
	UClassDataAsset* GetCurrentClass() const { return CurrentClass; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class")
	UProfessionDataAsset* GetCurrentProfession() const { return CurrentProfession; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class")
	TArray<UClassDataAsset*> GetSelectedClasses() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class")
	TArray<UProfessionDataAsset*> GetSelectedProfessions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class")
	FText GetSelectedProfessionsText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class")
	FText GetSelectedClassesText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class")
	int32 GetPlayerLevel() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class")
	int32 GetClassLevel(UClassDataAsset* ClassAsset) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class")
	float GetClassExperience(UClassDataAsset* ClassAsset) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Class")
	float GetClassExperienceToNextLevel(UClassDataAsset* ClassAsset) const;

	/** Returns the first unmet skill milestone for the next class level, if any. */
	bool GetUnmetClassSkillRequirement(UClassDataAsset* ClassAsset, FName& OutSkillID, int32& OutRequiredLevel) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Profession")
	int32 GetProfessionLevel(UProfessionDataAsset* ProfessionAsset) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Profession")
	float GetProfessionExperience(UProfessionDataAsset* ProfessionAsset) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Profession")
	float GetProfessionExperienceToNextLevel(UProfessionDataAsset* ProfessionAsset) const;

	UFUNCTION(BlueprintCallable, Category = "Profession")
	bool AllocateExperienceToProfession(UProfessionDataAsset* ProfessionAsset, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Class")
	bool AllocateExperienceToClass(UClassDataAsset* ClassAsset, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Class")
	bool AllocateExperienceToClassAllowUnselected(UClassDataAsset* ClassAsset, float Amount);

	UPROPERTY(BlueprintAssignable, Category = "Class|Events")
	FOnClassChanged OnClassChanged;

	UPROPERTY(BlueprintAssignable, Category = "Class|Events")
	FOnProfessionChanged OnProfessionChanged;

	UPROPERTY(BlueprintAssignable, Category = "Class|Events")
	FOnClassListChanged OnClassListChanged;

	UPROPERTY(BlueprintAssignable, Category = "Class|Events")
	FOnProfessionListChanged OnProfessionListChanged;

	UPROPERTY(BlueprintAssignable, Category = "Class|Events")
	FOnClassExperienceChanged OnClassExperienceChanged;

	UPROPERTY(BlueprintAssignable, Category = "Class|Events")
	FOnClassLevelChanged OnClassLevelChanged;

	UPROPERTY(BlueprintAssignable, Category = "Profession|Events")
	FOnProfessionExperienceChanged OnProfessionExperienceChanged;

	UPROPERTY(BlueprintAssignable, Category = "Profession|Events")
	FOnProfessionLevelChanged OnProfessionLevelChanged;

private:
	UPROPERTY()
	TObjectPtr<UClassDataAsset> CurrentClass;

	UPROPERTY()
	TObjectPtr<UProfessionDataAsset> CurrentProfession;

	UPROPERTY()
	TArray<TObjectPtr<UClassDataAsset>> SelectedClasses;

	UPROPERTY()
	TArray<TObjectPtr<UProfessionDataAsset>> SelectedProfessions;

	UPROPERTY()
	TMap<TObjectPtr<UProfessionDataAsset>, TObjectPtr<USkillBase>> GrantedProfessionSkills;

	UPROPERTY()
	TMap<TObjectPtr<UClassDataAsset>, FClassGrantedSkillState> GrantedClassSkills;

	UPROPERTY()
	TMap<TObjectPtr<UClassDataAsset>, FClassProgress> ClassProgress;

	/** Highest class level whose tier-based free attribute points were awarded. */
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClassDataAsset>, int32> AwardedAttributePointLevels;

	UPROPERTY()
	TMap<TObjectPtr<UProfessionDataAsset>, FClassProgress> ProfessionProgress;

	UPROPERTY()
	TObjectPtr<UAttributeComponent> AttributeComponent;

	UPROPERTY()
	TObjectPtr<USecondaryAttributeComponent> SecondaryAttributeComponent;

	UPROPERTY()
	TObjectPtr<UExperienceComponent> ExperienceComponent;

	UPROPERTY()
	TObjectPtr<USkillComponent> SkillComponent;

	bool MeetsRequirements(int32 RequiredLevel, const TMap<EPrimaryAttribute, float>& RequiredAttributes) const;
	bool GrantProfessionSkill(UProfessionDataAsset* Profession);
	void RevokeProfessionSkill(UProfessionDataAsset* Profession);
	bool HasValidClassSkillGrants(const UClassDataAsset* ClassAsset) const;
	bool GrantClassSkills(UClassDataAsset* ClassAsset);
	void RevokeClassSkills(UClassDataAsset* ClassAsset);
	void UpdateAppliedBonuses();
	void GrantAttributePointsForClassLevels(UClassDataAsset* ClassAsset, int32 NewLevel);

	bool CanAddClass() const;
	bool CanAddProfession() const;
	float GetExperienceRequiredForLevel(UClassDataAsset* ClassAsset, int32 Level) const;

	bool AllocateExperienceInternal(UClassDataAsset* ClassAsset, float Amount, bool bAllowUnselected);
	bool MeetsClassSkillRequirements(UClassDataAsset* ClassAsset, int32 TargetLevel) const;
	bool AdvanceClassIfReady(UClassDataAsset* ClassAsset, FClassProgress& Progress);

	UFUNCTION()
	void HandleSkillLevelUp(USkillBase* Skill, int32 NewLevel);

	UFUNCTION()
	void HandleSkillRevoked(USkillBase* Skill);
};
