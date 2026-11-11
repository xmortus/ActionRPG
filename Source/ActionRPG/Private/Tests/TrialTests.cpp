// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Characters/ActionRPGEnemyCharacter.h"
#include "Components/Progression/ExperienceComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/ClassComponent.h"
#include "Components/Progression/TrialComponent.h"
#include "Components/Inventory/InventoryComponent.h"
#include "Components/Skills/SkillComponent.h"
#include "Components/Skills/SkillManagerComponent.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Core/ItemBase.h"
#include "Crafting/CraftingRecipeDataAsset.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Skills/Implementations/SkillBlacksmithCrafting.h"
#include "Skills/Implementations/SkillMeleeAttack.h"
#include "Components/Progression/TrainingComponent.h"
#include "Components/CapsuleComponent.h"
#include "Progression/Core/ProfessionDataAsset.h"
#include "Trials/TrialDataAsset.h"
#include "Trials/TrialTriggerActor.h"

#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTrialLifecycleTest, "ActionRPG.Trials.Lifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatTrialDeathTest, "ActionRPG.Trials.CombatDeath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatTrialMeleeHitTest, "ActionRPG.Trials.CombatMeleeHit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTrialRewardsTest, "ActionRPG.Trials.Rewards",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTrialCraftingTest, "ActionRPG.Trials.Crafting",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTrialRewardAssetValuesTest, "ActionRPG.Trials.RewardAssetValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTrialRewardChoiceTest, "ActionRPG.Trials.RewardChoice",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTrialRewardChoiceTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 3000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	UTrialComponent* Trials = Player->TrialComponent;
	UExperienceComponent* Experience = Player->ExperienceComponent;
	UAttributeComponent* Attributes = Player->AttributeComponent;
	const int32 StartingPoints = Attributes->GetUnspentAttributePoints();
	UTrialDataAsset* Trial = NewObject<UTrialDataAsset>(Player);
	Trial->TrialId = TEXT("ChoiceTest");
	Trial->ExperienceReward = std::numeric_limits<float>::infinity(); // Unused fixed reward must not block choices.
	Trial->AttributePointReward = -1;
	FTrialObjectiveDefinition Objective;
	Objective.ObjectiveId = TEXT("ChoiceGoal");
	Trial->Objectives.Add(Objective);
	FTrialRewardChoice First;
	First.ChoiceName = FText::FromString(TEXT("Insight"));
	First.ExperienceReward = 25.0f;
	First.AttributePointReward = 2;
	UItemDataAsset* HeavyReward = NewObject<UItemDataAsset>(Trial);
	HeavyReward->ItemID = TEXT("TooHeavyReward");
	HeavyReward->MaxStackSize = 1;
	HeavyReward->Weight = Player->InventoryComponent->GetMaxWeight() + 1.0f;
	First.ItemReward.Item = HeavyReward;
	FTrialRewardChoice Second;
	Second.ChoiceName = FText::FromString(TEXT("Practice"));
	Second.ExperienceReward = 50.0f;
	Second.AttributePointReward = 1;
	Trial->RewardChoices = {First, Second};
	TestTrue(TEXT("Two-choice trial starts"), Trials->StartTrial(Trial));
	TestTrue(TEXT("Objective completes and awaits a choice"), Trials->AdvanceObjective(TEXT("ChoiceGoal")));
	TestEqual(TEXT("Choice trial remains active"), Trials->GetActiveTrial(), Trial);
	TestEqual(TEXT("No XP pays before selection"), Experience->GetUnallocatedExperience(), 0.0f);
	TestEqual(TEXT("No points pay before selection"), Attributes->GetUnspentAttributePoints(), StartingPoints);
	TestFalse(TEXT("Invalid choice cannot pay"), Trials->ChooseActiveTrialReward(2));
	TestFalse(TEXT("Generic retry cannot bypass selection"), Trials->TryCompleteActiveTrial());
	TestFalse(TEXT("Overweight choice stays pending"), Trials->ChooseActiveTrialReward(0));
	TestEqual(TEXT("Blocked choice pays no XP"), Experience->GetUnallocatedExperience(), 0.0f);
	TestEqual(TEXT("Blocked choice pays no points"), Attributes->GetUnspentAttributePoints(), StartingPoints);
	TestTrue(TEXT("Second choice pays successfully"), Trials->ChooseActiveTrialReward(1));
	TestEqual(TEXT("Only selected XP pays"), Experience->GetUnallocatedExperience(), 50.0f);
	TestEqual(TEXT("Only selected points pay"), Attributes->GetUnspentAttributePoints(), StartingPoints + 1);
	TestEqual(TEXT("Trial completes once"), Trials->GetCompletionCount(Trial), 1);
	TestFalse(TEXT("Cannot claim the other choice afterward"), Trials->ChooseActiveTrialReward(0));
	const FTrialRecord* Record = Trials->GetTrialRecords().FindByPredicate(
		[Trial](const FTrialRecord& Candidate) { return Candidate.Trial == Trial; });
	if (TestNotNull(TEXT("Trial record exists"), Record))
	{
		TestEqual(TEXT("Selected reward is recorded"), Record->LastRewardChoiceIndex, 1);
	}
	Player->Destroy();
	return true;
}

bool FTrialRewardAssetValuesTest::RunTest(const FString& Parameters)
{
	static const TCHAR* TrialPaths[] = {
		TEXT("/Game/Data/Trials/DA_Trial_FirstSteps.DA_Trial_FirstSteps"),
		TEXT("/Game/Data/Trials/DA_Trial_TrainingCombat.DA_Trial_TrainingCombat"),
		TEXT("/Game/Data/Trials/DA_Trial_DashPractice.DA_Trial_DashPractice"),
		TEXT("/Game/Data/Trials/DA_Trial_ForgeIronSword.DA_Trial_ForgeIronSword")
	};
	for (const TCHAR* TrialPath : TrialPaths)
	{
		const UTrialDataAsset* Trial = LoadObject<UTrialDataAsset>(nullptr, TrialPath);
		if (!TestNotNull(FString::Printf(TEXT("Trial asset %s"), TrialPath), Trial))
		{
			continue;
		}
		TestTrue(FString::Printf(TEXT("Trial XP in 100-200 range: %s"), TrialPath),
			Trial->ExperienceReward >= 100.0f && Trial->ExperienceReward <= 200.0f);
		if (Trial->TrialId == FName(TEXT("FirstSteps")))
		{
			TestEqual(TEXT("First Steps offers two rewards"), Trial->RewardChoices.Num(), 2);
			if (Trial->RewardChoices.Num() == 2)
			{
				TestEqual(TEXT("Growth keeps its XP reward"), Trial->RewardChoices[0].ExperienceReward, 100.0f);
				TestEqual(TEXT("Learning keeps its XP reward"), Trial->RewardChoices[1].ExperienceReward, 100.0f);
				TestEqual(TEXT("Growth grants attribute points"), Trial->RewardChoices[0].AttributePointReward, 2);
				TestEqual(TEXT("Learning grants skill points"), Trial->RewardChoices[1].SkillPointReward, 2);
			}
		}
	}
	return true;
}

bool FTrialLifecycleTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World))
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 100000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawns"), Player))
	{
		return false;
	}

	UTrialComponent* Trials = Player->TrialComponent;
	UExperienceComponent* Experience = Player->ExperienceComponent;
	TestNotNull(TEXT("Trial component is attached"), Trials);
	TestNotNull(TEXT("Experience component is attached"), Experience);
	if (!Trials || !Experience)
	{
		Player->Destroy();
		return false;
	}

	UTrialDataAsset* Trial = NewObject<UTrialDataAsset>(GetTransientPackage());
	Trial->TrialId = TEXT("TestCombatTrial");
	Trial->ExperienceReward = 25.0f;
	FTrialObjectiveDefinition First;
	First.ObjectiveId = TEXT("HitTargets");
	First.TargetCount = 2;
	FTrialObjectiveDefinition Second;
	Second.ObjectiveId = TEXT("ReachExit");
	Second.TargetCount = 1;
	Trial->Objectives = {First, Second};

	TestTrue(TEXT("Valid trial can start"), Trials->StartTrial(Trial));
	TestFalse(TEXT("Same trial cannot start twice concurrently"), Trials->StartTrial(Trial));
	TestFalse(TEXT("Unknown objective is rejected"), Trials->AdvanceObjective(TEXT("Unknown")));
	TestTrue(TEXT("First hit is recorded"), Trials->AdvanceObjective(TEXT("HitTargets")));
	TestTrue(TEXT("Large increment is capped at target"), Trials->AdvanceObjective(TEXT("HitTargets"), 100));
	TestFalse(TEXT("Completed objective cannot advance again"), Trials->AdvanceObjective(TEXT("HitTargets")));
	TestTrue(TEXT("Last objective completes the trial"), Trials->AdvanceObjective(TEXT("ReachExit")));
	TestNull(TEXT("No active trial after completion"), Trials->GetActiveTrial());
	TestEqual(TEXT("One completion is recorded"), Trials->GetCompletionCount(Trial), 1);
	TestEqual(TEXT("XP reward enters unallocated pool"), Experience->GetUnallocatedExperience(), 25.0f);
	TestFalse(TEXT("One-time trial cannot restart"), Trials->StartTrial(Trial));

	UTrialDataAsset* Followup = NewObject<UTrialDataAsset>(GetTransientPackage());
	Followup->TrialId = TEXT("TestFollowupTrial");
	Followup->Objectives = {Second};
	Followup->RequiredTrials.Add(Trial);
	Followup->bRepeatable = true;
	TestTrue(TEXT("Prerequisite permits follow-up trial"), Trials->StartTrial(Followup));
	TestTrue(TEXT("Active trial can fail"), Trials->FailActiveTrial());
	TestEqual(TEXT("Failure grants no completion"), Trials->GetCompletionCount(Followup), 0);
	TestTrue(TEXT("Failed trial can restart"), Trials->StartTrial(Followup));
	TestTrue(TEXT("Follow-up completes"), Trials->AdvanceObjective(TEXT("ReachExit")));
	TestTrue(TEXT("Repeatable trial can restart"), Trials->StartTrial(Followup));
	TestTrue(TEXT("Repeated follow-up completes"), Trials->AdvanceObjective(TEXT("ReachExit")));
	TestEqual(TEXT("Repeatable completion count increments"), Trials->GetCompletionCount(Followup), 2);

	UTrialDataAsset* Invalid = NewObject<UTrialDataAsset>(GetTransientPackage());
	Invalid->TrialId = TEXT("InvalidTrial");
	Invalid->Objectives = {First, First};
	TestFalse(TEXT("Duplicate objective IDs are rejected"), Trials->CanStartTrial(Invalid));
	Invalid->Objectives = {First};
	Invalid->ExperienceReward = std::numeric_limits<float>::infinity();
	TestFalse(TEXT("Non-finite reward is rejected"), Trials->CanStartTrial(Invalid));
	Invalid->ExperienceReward = 0.0f;
	Invalid->RequiredPlayerLevel = 10;
	TestFalse(TEXT("Unmet level requirement is rejected"), Trials->CanStartTrial(Invalid));

	ATrialTriggerActor* StartMarker = World->SpawnActor<ATrialTriggerActor>(
		FVector(100300.0f, 100000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	ATrialTriggerActor* GoalMarker = World->SpawnActor<ATrialTriggerActor>(
		FVector(100500.0f, 100000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (TestNotNull(TEXT("Start marker spawns"), StartMarker)
		&& TestNotNull(TEXT("Goal marker spawns"), GoalMarker))
	{
		UTrialDataAsset* TriggerTrial = NewObject<UTrialDataAsset>(GetTransientPackage());
		TriggerTrial->TrialId = TEXT("TriggerTrial");
		TriggerTrial->Objectives = {Second};
		TriggerTrial->ExperienceReward = 10.0f;
		StartMarker->Trial = TriggerTrial;
		GoalMarker->Trial = TriggerTrial;
		GoalMarker->Action = ETrialTriggerAction::AdvanceObjective;
		GoalMarker->ObjectiveId = Second.ObjectiveId;
		TestFalse(TEXT("Goal cannot finish a trial before start"), GoalMarker->TryTrigger(Player));
		TestTrue(TEXT("Start marker begins a trial"), StartMarker->TryTrigger(Player));
		TestFalse(TEXT("Start marker cannot restart active trial"), StartMarker->TryTrigger(Player));
		TestTrue(TEXT("Goal marker completes the active trial"), GoalMarker->TryTrigger(Player));
		TestEqual(TEXT("Goal marker delivered XP once"), Experience->GetUnallocatedExperience(), 35.0f);
		TestFalse(TEXT("Goal cannot pay twice"), GoalMarker->TryTrigger(Player));
		TestEqual(TEXT("XP remains unchanged after repeat overlap"), Experience->GetUnallocatedExperience(), 35.0f);
	}
	if (StartMarker) StartMarker->Destroy();
	if (GoalMarker) GoalMarker->Destroy();

	USkillComponent* Skills = NewObject<USkillComponent>(Player);
	Player->AddInstanceComponent(Skills);
	Skills->RegisterComponent();
	USkillDataAsset* SkillData = NewObject<USkillDataAsset>(GetTransientPackage());
	SkillData->SkillID = TEXT("TrialPracticeSkill");
	USkillBase* Skill = NewObject<USkillBase>(Player);
	Skill->SkillData = SkillData;
	TestTrue(TEXT("Practice skill is granted"), Skills->GrantSkill(Skill));
	UTrialDataAsset* SkillTrial = NewObject<UTrialDataAsset>(GetTransientPackage());
	SkillTrial->TrialId = TEXT("TestSkillTrial");
	SkillTrial->TrialType = ETrialType::Skill;
	FTrialObjectiveDefinition SkillObjective;
	SkillObjective.ObjectiveId = SkillData->SkillID;
	SkillObjective.TargetCount = 2;
	SkillTrial->Objectives = {SkillObjective};
	TestTrue(TEXT("Skill trial starts"), Trials->StartTrial(SkillTrial));
	TestFalse(TEXT("Unrelated skill ID does not advance"), Trials->ReportSkillActivated(TEXT("OtherSkill")));
	TestFalse(TEXT("Empty skill ID does not advance"), Trials->ReportSkillActivated(NAME_None));
	TestTrue(TEXT("Skill can be revoked"), Skills->RemoveSkill(Skill));
	TestFalse(TEXT("Revoked skill cannot activate"), Skills->ActivateSkill(Skill));
	TestEqual(TEXT("Rejected activation does not advance"), Trials->GetTrialProgress(SkillTrial), 0.0f);
	TestTrue(TEXT("Skill can be granted again"), Skills->GrantSkill(Skill));
	TestTrue(TEXT("Successful skill use advances objective"), Skills->ActivateSkill(Skill));
	TestEqual(TEXT("One skill use is half progress"), Trials->GetTrialProgress(SkillTrial), 0.5f);
	TestTrue(TEXT("Second skill use completes objective"), Skills->ActivateSkill(Skill));
	TestEqual(TEXT("Skill trial completes once"), Trials->GetCompletionCount(SkillTrial), 1);
	TestTrue(TEXT("Skill can still activate after trial completion"), Skills->ActivateSkill(Skill));
	TestEqual(TEXT("Extra skill use cannot repeat payout"), Trials->GetCompletionCount(SkillTrial), 1);

	Player->Destroy();
	return true;
}

bool FCombatTrialDeathTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World))
	{
		return false;
	}
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(101000.0f, 101000.0f, 100.0f), FRotator::ZeroRotator, Params);
	AActionRPGEnemyCharacter* EnemyA = World->SpawnActor<AActionRPGEnemyCharacter>(
		FVector(101300.0f, 101000.0f, 100.0f), FRotator::ZeroRotator, Params);
	AActionRPGEnemyCharacter* EnemyB = World->SpawnActor<AActionRPGEnemyCharacter>(
		FVector(101600.0f, 101000.0f, 100.0f), FRotator::ZeroRotator, Params);
	AActionRPGEnemyCharacter* EnemyUncredited = World->SpawnActor<AActionRPGEnemyCharacter>(
		FVector(101900.0f, 101000.0f, 100.0f), FRotator::ZeroRotator, Params);
	AActor* DamageProxy = World->SpawnActor<AActor>(
		FVector(102200.0f, 101000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawns"), Player) || !TestNotNull(TEXT("First enemy spawns"), EnemyA)
		|| !TestNotNull(TEXT("Second enemy spawns"), EnemyB)
		|| !TestNotNull(TEXT("Uncredited enemy spawns"), EnemyUncredited)
		|| !TestNotNull(TEXT("Damage proxy spawns"), DamageProxy))
	{
		if (DamageProxy) DamageProxy->Destroy();
		if (EnemyUncredited) EnemyUncredited->Destroy();
		if (EnemyB) EnemyB->Destroy();
		if (EnemyA) EnemyA->Destroy();
		if (Player) Player->Destroy();
		return false;
	}

	EnemyA->EnemyId = TEXT("TrainingEnemy");
	EnemyB->EnemyId = TEXT("TrainingEnemy");
	EnemyUncredited->EnemyId = TEXT("TrainingEnemy");
	UTrialDataAsset* Trial = NewObject<UTrialDataAsset>(GetTransientPackage());
	Trial->TrialId = TEXT("TrainingCombatTrial");
	Trial->TrialType = ETrialType::Combat;
	Trial->ExperienceReward = 25.0f;
	FTrialObjectiveDefinition Objective;
	Objective.ObjectiveId = TEXT("TrainingEnemy");
	Objective.TargetCount = 2;
	Trial->Objectives = {Objective};
	TestTrue(TEXT("Combat trial starts"), Player->TrialComponent->StartTrial(Trial));

	const FDamageEvent DamageEvent;
	TestEqual(TEXT("Non-finite damage is rejected"), EnemyA->TakeDamage(
		std::numeric_limits<float>::quiet_NaN(), DamageEvent, nullptr, Player), 0.0f);
	TestEqual(TEXT("Nonlethal player damage is applied"), EnemyA->TakeDamage(40.0f, DamageEvent, nullptr, Player), 40.0f);
	TestFalse(TEXT("Nonlethal hit does not count as a kill"), EnemyA->IsDead());
	TestEqual(TEXT("Combat progress remains zero"), Player->TrialComponent->GetTrialProgress(Trial), 0.0f);
	TestEqual(TEXT("Unattributed death takes damage"), EnemyUncredited->TakeDamage(
		100.0f, DamageEvent, nullptr, DamageProxy), 100.0f);
	TestEqual(TEXT("Unattributed death does not credit player"), Player->TrialComponent->GetTrialProgress(Trial), 0.0f);
	TestEqual(TEXT("Lethal player hit applies remaining health"), EnemyA->TakeDamage(
		100.0f, DamageEvent, nullptr, Player), 60.0f);
	TestTrue(TEXT("First enemy is dead"), EnemyA->IsDead());
	TestEqual(TEXT("First kill is half progress"), Player->TrialComponent->GetTrialProgress(Trial), 0.5f);
	TestEqual(TEXT("Repeated damage to dead enemy is rejected"), EnemyA->TakeDamage(
		100.0f, DamageEvent, nullptr, Player), 0.0f);
	TestEqual(TEXT("Repeat hit does not grant progress"), Player->TrialComponent->GetTrialProgress(Trial), 0.5f);
	DamageProxy->SetOwner(Player);
	TestEqual(TEXT("Player-owned projectile can deliver lethal damage"), EnemyB->TakeDamage(
		100.0f, DamageEvent, nullptr, DamageProxy), 100.0f);
	TestEqual(TEXT("Two confirmed deaths complete combat trial"), Player->TrialComponent->GetCompletionCount(Trial), 1);
	TestEqual(TEXT("Combat XP is paid once"), Player->ExperienceComponent->GetUnallocatedExperience(), 25.0f);
	TestFalse(TEXT("Completed combat trial cannot receive duplicate kill"), Player->TrialComponent->ReportEnemyDefeated(
		TEXT("TrainingEnemy"), EnemyB));
	TestEqual(TEXT("Combat XP remains unchanged"), Player->ExperienceComponent->GetUnallocatedExperience(), 25.0f);

	DamageProxy->Destroy();
	EnemyUncredited->Destroy();
	EnemyB->Destroy();
	EnemyA->Destroy();
	Player->Destroy();
	return true;
}

bool FCombatTrialMeleeHitTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World))
	{
		return false;
	}

	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(103000.0f, 101000.0f, 100.0f), FRotator::ZeroRotator, Params);
	AActionRPGEnemyCharacter* Enemy = World->SpawnActor<AActionRPGEnemyCharacter>(
		FVector(103120.0f, 101000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawns"), Player) || !TestNotNull(TEXT("Enemy spawns"), Enemy))
	{
		if (Enemy) Enemy->Destroy();
		if (Player) Player->Destroy();
		return false;
	}
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	if (!Enemy->HasActorBegunPlay()) Enemy->DispatchBeginPlay();
	Enemy->EnemyId = TEXT("TrainingEnemy");
	Enemy->bRespawnAfterDeath = true;
	Enemy->RespawnDelaySeconds = 4.0f;

	UTrialDataAsset* Trial = NewObject<UTrialDataAsset>(GetTransientPackage());
	Trial->TrialId = TEXT("MeleeCombatTrialTest");
	Trial->TrialType = ETrialType::Combat;
	FTrialObjectiveDefinition Objective;
	Objective.ObjectiveId = TEXT("TrainingEnemy");
	Objective.TargetCount = 1;
	Trial->Objectives = {Objective};
	TestTrue(TEXT("Combat trial starts"), Player->TrialComponent->StartTrial(Trial));

	USkillMeleeAttack* Attack = NewObject<USkillMeleeAttack>(Player);
	Attack->OwnerActor = Player;
	Attack->SkillData = NewObject<USkillDataAsset>(Attack);
	Attack->SkillData->Range = 150.0f;
	Attack->SkillData->BaseDamage = 25.0f;
	for (int32 Swing = 0; Swing < 4; ++Swing)
	{
		TestTrue(TEXT("Melee attack activates"), Attack->Activate() == Attack);
		Attack->UpdateCooldown(1.0f);
	}
	TestTrue(TEXT("Nearby enemy is killed by melee attacks"), Enemy->IsDead());
	TestEqual(TEXT("Successful melee damage records one cooldown-limited Strength practice"),
		Player->TrainingComponent->GetPracticeProgress(EPrimaryAttribute::Strength, TEXT("MeleeHit")), 1);
	TestEqual(TEXT("Melee kill completes combat trial"), Player->TrialComponent->GetCompletionCount(Trial), 1);
	TestEqual(TEXT("Defeated enemy disables hit collision"), Enemy->GetCapsuleComponent()->GetCollisionEnabled(),
		ECollisionEnabled::NoCollision);
	TestTrue(TEXT("Training enemy can recover after trial completion"), Enemy->Revive());
	TestFalse(TEXT("Recovered enemy is alive"), Enemy->IsDead());
	TestEqual(TEXT("Recovery restores full health"), Enemy->GetCurrentHealth(), Enemy->MaxHealth);
	TestTrue(TEXT("Recovery restores hit collision"),
		Enemy->GetCapsuleComponent()->GetCollisionEnabled() != ECollisionEnabled::NoCollision);
	TestTrue(TEXT("Melee attack activates after trial"), Attack->Activate() == Attack);
	TestEqual(TEXT("Recovered enemy takes melee damage"), Enemy->GetCurrentHealth(), Enemy->MaxHealth - 25.0f);
	TestEqual(TEXT("Completed trial is not credited again"), Player->TrialComponent->GetCompletionCount(Trial), 1);

	Enemy->Destroy();
	Player->Destroy();
	return true;
}

bool FTrialRewardsTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World))
	{
		return false;
	}
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(103000.0f, 103000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawns"), Player))
	{
		return false;
	}
	USkillComponent* Skills = NewObject<USkillComponent>(Player);
	USkillManagerComponent* Manager = NewObject<USkillManagerComponent>(Player);
	Player->AddInstanceComponent(Skills);
	Player->AddInstanceComponent(Manager);
	Skills->RegisterComponent();
	Manager->RegisterComponent();
	Manager->SkillComponent = Skills;

	UItemDataAsset* Ore = NewObject<UItemDataAsset>(GetTransientPackage());
	Ore->ItemID = TEXT("TrialRewardOre");
	Ore->MaxStackSize = 99;
	Ore->Weight = 1.0f;
	USkillDataAsset* SkillData = NewObject<USkillDataAsset>(GetTransientPackage());
	SkillData->SkillID = TEXT("TrialRewardSkill");
	UTrialDataAsset* RewardTrial = NewObject<UTrialDataAsset>(GetTransientPackage());
	RewardTrial->TrialId = TEXT("RewardTrial");
	FTrialObjectiveDefinition Objective;
	Objective.ObjectiveId = TEXT("Complete");
	RewardTrial->Objectives = {Objective};
	RewardTrial->ExperienceReward = 25.0f;
	RewardTrial->ItemReward.Item = Ore;
	RewardTrial->ItemReward.Quantity = 2;
	RewardTrial->SkillReward.SkillClass = USkillBase::StaticClass();
	RewardTrial->SkillReward.SkillData = SkillData;
	RewardTrial->AttributePointReward = 3;
	RewardTrial->SkillPointReward = 2;
	TestTrue(TEXT("Combined reward trial starts"), Player->TrialComponent->StartTrial(RewardTrial));
	TestTrue(TEXT("Combined reward trial completes"), Player->TrialComponent->AdvanceObjective(TEXT("Complete")));
	TestEqual(TEXT("XP reward granted"), Player->ExperienceComponent->GetUnallocatedExperience(), 25.0f);
	TestEqual(TEXT("Item reward granted"), Player->InventoryComponent->CountItemData(Ore), 2);
	TestEqual(TEXT("Attribute points granted"), Player->AttributeComponent->GetUnspentAttributePoints(), 3);
	TestEqual(TEXT("Skill points have a separate balance"), Manager->GetUnspentSkillPoints(), 2);
	TestEqual(TEXT("Skill reward granted"), Manager->GetUnlockedSkills().Num(), 1);
	TestFalse(TEXT("One-time combined reward cannot repeat"), Player->TrialComponent->StartTrial(RewardTrial));

	UTrialDataAsset* Blocked = NewObject<UTrialDataAsset>(GetTransientPackage());
	Blocked->TrialId = TEXT("BlockedRewardTrial");
	Blocked->Objectives = {Objective};
	Blocked->ExperienceReward = 50.0f;
	Blocked->ItemReward.Item = Ore;
	Blocked->ItemReward.Quantity = 100000;
	Blocked->AttributePointReward = 4;
	Blocked->SkillPointReward = 5;
	TestTrue(TEXT("Blocked reward trial can start"), Player->TrialComponent->StartTrial(Blocked));
	TestFalse(TEXT("Insufficient inventory blocks entire payout"), Player->TrialComponent->AdvanceObjective(TEXT("Complete")));
	TestEqual(TEXT("Blocked payout grants no extra XP"), Player->ExperienceComponent->GetUnallocatedExperience(), 25.0f);
	TestEqual(TEXT("Blocked payout grants no extra items"), Player->InventoryComponent->CountItemData(Ore), 2);
	TestEqual(TEXT("Blocked payout grants no attribute points"), Player->AttributeComponent->GetUnspentAttributePoints(), 3);
	TestEqual(TEXT("Blocked payout grants no skill points"), Manager->GetUnspentSkillPoints(), 2);
	TestTrue(TEXT("Blocked trial can be abandoned"), Player->TrialComponent->FailActiveTrial());
	TestTrue(TEXT("Free skill points can be spent independently"), Manager->SpendSkillPoints(1));
	TestEqual(TEXT("One free skill point remains"), Manager->GetUnspentSkillPoints(), 1);
	TestEqual(TEXT("Spending a skill point leaves attribute points intact"), Player->AttributeComponent->GetUnspentAttributePoints(), 3);
	TestEqual(TEXT("Failure is recorded"), Player->TrialComponent->GetTrialRecords().Last().FailureCount, 1);

	UTrialDataAsset* Repeatable = NewObject<UTrialDataAsset>(GetTransientPackage());
	Repeatable->TrialId = TEXT("RepeatableRewardTrial");
	Repeatable->Objectives = {Objective};
	Repeatable->ExperienceReward = 5.0f;
	Repeatable->bRepeatable = true;
	TestTrue(TEXT("Repeatable trial starts"), Player->TrialComponent->StartTrial(Repeatable));
	TestTrue(TEXT("Repeatable trial first completion"), Player->TrialComponent->AdvanceObjective(TEXT("Complete")));
	TestTrue(TEXT("Repeatable trial restarts"), Player->TrialComponent->StartTrial(Repeatable));
	TestTrue(TEXT("Repeatable trial second completion"), Player->TrialComponent->AdvanceObjective(TEXT("Complete")));
	TestEqual(TEXT("Repeatable completion count is two"), Player->TrialComponent->GetCompletionCount(Repeatable), 2);
	TestEqual(TEXT("Repeatable XP pays per completion"), Player->ExperienceComponent->GetUnallocatedExperience(), 35.0f);

	UTrialDataAsset* Invalid = NewObject<UTrialDataAsset>(GetTransientPackage());
	Invalid->TrialId = TEXT("InvalidRewardTrial");
	Invalid->Objectives = {Objective};
	Invalid->SkillReward.SkillClass = USkillBase::StaticClass();
	TestFalse(TEXT("Incomplete skill reward definition is rejected"), Player->TrialComponent->CanStartTrial(Invalid));
	Player->Destroy();
	return true;
}

bool FTrialCraftingTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World))
	{
		return false;
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(100000.0f, 12000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawns"), Player))
	{
		return false;
	}
	UItemDataAsset* Ore = NewObject<UItemDataAsset>(GetTransientPackage());
	Ore->ItemID = TEXT("TrialIronOre");
	Ore->MaxStackSize = 99;
	Ore->Weight = 1.0f;
	UItemDataAsset* Sword = NewObject<UItemDataAsset>(GetTransientPackage());
	Sword->ItemID = TEXT("TrialIronSword");
	Sword->MaxStackSize = 1;
	Sword->Weight = 3.0f;
	UCraftingRecipeDataAsset* Recipe = NewObject<UCraftingRecipeDataAsset>(GetTransientPackage());
	Recipe->RecipeID = TEXT("TrialIronSwordRecipe");
	FCraftingIngredient Ingredient;
	Ingredient.Item = Ore;
	Ingredient.Quantity = 2;
	Recipe->Ingredients.Add(Ingredient);
	Recipe->OutputItem = Sword;
	Recipe->OutputQuantity = 1;
	USkillComponent* Skills = NewObject<USkillComponent>(Player);
	USkillManagerComponent* Manager = NewObject<USkillManagerComponent>(Player);
	Player->AddInstanceComponent(Skills);
	Player->AddInstanceComponent(Manager);
	Skills->RegisterComponent();
	Manager->RegisterComponent();
	Manager->SkillComponent = Skills;
	UProfessionDataAsset* Blacksmith = NewObject<UProfessionDataAsset>(GetTransientPackage());
	Blacksmith->GrantedSkillClass = USkillBlacksmithCrafting::StaticClass();
	Blacksmith->GrantedSkillData = NewObject<USkillDataAsset>(Blacksmith);
	Blacksmith->Recipes.Add(Recipe);
	Player->ExperienceComponent->AddExperience(100.0f);
	TestTrue(TEXT("XP unlocks Blacksmith profession"), Player->ClassComponent->AllocateExperienceToProfession(Blacksmith, 100.0f));
	const TArray<USkillBase*> Unlocked = Manager->GetUnlockedSkills();
	USkillBlacksmithCrafting* Crafting = Unlocked.Num() == 1 ? Cast<USkillBlacksmithCrafting>(Unlocked[0]) : nullptr;
	if (!TestNotNull(TEXT("Crafting skill granted"), Crafting))
	{
		Player->Destroy();
		return false;
	}
	UTrialDataAsset* Trial = NewObject<UTrialDataAsset>(GetTransientPackage());
	Trial->TrialId = TEXT("CraftingTrial");
	Trial->TrialType = ETrialType::Crafting;
	Trial->ExperienceReward = 12.0f;
	FTrialObjectiveDefinition Objective;
	Objective.ObjectiveId = Recipe->RecipeID;
	Objective.TargetCount = 1;
	Trial->Objectives.Add(Objective);
	TestTrue(TEXT("Crafting trial starts"), Player->TrialComponent->StartTrial(Trial));
	TestFalse(TEXT("Wrong recipe does not progress"), Player->TrialComponent->ReportRecipeCrafted(TEXT("WrongRecipe")));
	TestFalse(TEXT("Cannot craft without ingredients"), Crafting->CraftRecipe(Recipe));
	TestEqual(TEXT("Failed craft grants no trial progress"), Player->TrialComponent->GetTrialProgress(Trial), 0.0f);
	UItemBase* OreItem = NewObject<UItemBase>(Player);
	OreItem->ItemData = Ore;
	TestTrue(TEXT("Ingredients are added"), Player->InventoryComponent->AddItem(OreItem, 2));
	TestTrue(TEXT("Recipe crafts successfully"), Crafting->CraftRecipe(Recipe));
	TestEqual(TEXT("Ingredients consumed"), Player->InventoryComponent->CountItemData(Ore), 0);
	TestEqual(TEXT("Output produced"), Player->InventoryComponent->CountItemData(Sword), 1);
	TestEqual(TEXT("Crafting trial completes"), Player->TrialComponent->GetCompletionCount(Trial), 1);
	TestEqual(TEXT("Crafting reward pays once"), Player->ExperienceComponent->GetUnallocatedExperience(), 12.0f);
	TestFalse(TEXT("Duplicate recipe event does not pay again"), Player->TrialComponent->ReportRecipeCrafted(Recipe->RecipeID));
	TestFalse(TEXT("One-time crafting trial cannot restart"), Player->TrialComponent->StartTrial(Trial));
	Player->Destroy();
	return true;
}

#endif
