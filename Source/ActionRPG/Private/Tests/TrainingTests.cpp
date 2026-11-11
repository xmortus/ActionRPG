#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Progression/TrainingComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Engine/World.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAttributeTrainingTest, "ActionRPG.Progression.AttributeTraining",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAttributeSourceBreakdownTest, "ActionRPG.Progression.AttributeSourceBreakdown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FAttributeTrainingTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(104000.0f, 104000.0f, 100.0f), FRotator::ZeroRotator, Params);
	AActor* Target = World->SpawnActor<AActor>(FVector(104100.0f, 104000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawned"), Player) || !TestNotNull(TEXT("Target spawned"), Target)) return false;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	UTrainingComponent* Training = Player->TrainingComponent;
	UAttributeComponent* Attributes = Player->AttributeComponent;
	const float StartingStrength = Attributes->GetPersistentAttribute(EPrimaryAttribute::Strength);
	const int32 StartingFreePoints = Attributes->GetUnspentAttributePoints();

	TestFalse(TEXT("Unknown activities do not advance training"),
		Training->RecordPractice(EPrimaryAttribute::Strength, TEXT("Idle"), Target));
	TestFalse(TEXT("Melee practice requires a target"),
		Training->RecordPractice(EPrimaryAttribute::Strength, TEXT("MeleeHit"), nullptr));
	TestTrue(TEXT("First valid hit counts"),
		Training->RecordPractice(EPrimaryAttribute::Strength, TEXT("MeleeHit"), Target));
	TestFalse(TEXT("Immediate repeat hit is throttled"),
		Training->RecordPractice(EPrimaryAttribute::Strength, TEXT("MeleeHit"), Target));
	TestEqual(TEXT("Only one hit counted"),
		Training->GetPracticeProgress(EPrimaryAttribute::Strength, TEXT("MeleeHit")), 1);

	FAttributeTrainingRule& Rule = Training->Rules[0];
	Rule.PracticePerGain = 2;
	Rule.ActivityCooldownSeconds = 0.0f;
	Rule.TargetCooldownSeconds = 0.0f;
	TestTrue(TEXT("Second accepted hit completes milestone"),
		Training->RecordPractice(EPrimaryAttribute::Strength, TEXT("MeleeHit"), Target));
	TestEqual(TEXT("Milestone resets practice progress"),
		Training->GetPracticeProgress(EPrimaryAttribute::Strength, TEXT("MeleeHit")), 0);
	TestEqual(TEXT("Training earns one permanent Strength"),
		Attributes->GetPersistentAttribute(EPrimaryAttribute::Strength), StartingStrength + 1.0f);
	TestEqual(TEXT("Training source has one Strength"),
		Attributes->GetModifierSourceBonus(TEXT("Training.Strength"), EPrimaryAttribute::Strength), 1.0f);
	TestEqual(TEXT("Training does not change free attribute points"),
		Attributes->GetUnspentAttributePoints(), StartingFreePoints);
	TestEqual(TEXT("Training tracks earned gain"), Training->GetEarnedTrainingGain(EPrimaryAttribute::Strength), 1);

	Target->Destroy();
	Player->Destroy();
	return true;
}

bool FAttributeSourceBreakdownTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(108000.0f, 108000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	UAttributeComponent* Attributes = Player->AttributeComponent;
	Attributes->SetAttribute(EPrimaryAttribute::Strength, 10.0f);
	Attributes->AddAttributePoints(2);
	TestTrue(TEXT("Free points can be allocated"), Attributes->AllocateAttributePoints(EPrimaryAttribute::Strength, 2));
	TestTrue(TEXT("Selected class bonus is applied"), Attributes->SetModifierSource(
		TEXT("Class.Selected"), {{EPrimaryAttribute::Strength, 3.0f}}, false));
	TestTrue(TEXT("Earned class gain is applied"), Attributes->SetModifierSource(
		TEXT("Class.Earned"), {{EPrimaryAttribute::Strength, 2.0f}}, true));
	TestTrue(TEXT("Training gain is applied"), Attributes->SetModifierSource(
		TEXT("Training.Strength"), {{EPrimaryAttribute::Strength, 1.0f}}, true));
	TestTrue(TEXT("Temporary effect is applied"), Attributes->SetModifierSource(
		TEXT("Status.Blessing"), {{EPrimaryAttribute::Strength, 4.0f}}, false));

	const FAttributeBreakdown Breakdown = Attributes->GetAttributeBreakdown(EPrimaryAttribute::Strength);
	TestEqual(TEXT("Base is separate"), Breakdown.Base, 10.0f);
	TestEqual(TEXT("Allocated points are separate"), Breakdown.Allocated, 2.0f);
	TestEqual(TEXT("Four source contributions are visible"), Breakdown.Sources.Num(), 4);
	TestEqual(TEXT("Breakdown reconciles with displayed total"), Breakdown.FinalTotal,
		Attributes->GetAttribute(EPrimaryAttribute::Strength));
	TestEqual(TEXT("Persistent total excludes temporary sources"),
		Attributes->GetPersistentAttribute(EPrimaryAttribute::Strength), 15.0f);
	TestEqual(TEXT("No attribute limit adjustment below cap"), Breakdown.LimitAdjustment, 0.0f);
	if (Breakdown.Sources.Num() == 4)
	{
		TestEqual(TEXT("Sources have stable sorted order"), Breakdown.Sources[0].SourceId, FName(TEXT("Class.Earned")));
		TestTrue(TEXT("Earned class gain is persistent"), Breakdown.Sources[0].bPersistent);
		TestFalse(TEXT("Selected class bonus is removable"), Breakdown.Sources[1].bPersistent);
	}
	Attributes->SetAttribute(EPrimaryAttribute::Strength, 997.0f);
	const FAttributeBreakdown Capped = Attributes->GetAttributeBreakdown(EPrimaryAttribute::Strength);
	TestEqual(TEXT("Attribute cap is reflected in final total"), Capped.FinalTotal,
		Attributes->GetAttribute(EPrimaryAttribute::Strength));
	TestTrue(TEXT("Breakdown identifies the cap adjustment"), Capped.LimitAdjustment < 0.0f);
	Player->Destroy();
	return true;
}

#endif
