#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Characters/ActionRPGEnemyCharacter.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Progression/TrainingComponent.h"
#include "Progression/Core/SecondaryAttributeDataAsset.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FConstitutionTrainingTest, "ActionRPG.Progression.ConstitutionTraining",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FConstitutionTrainingTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(106000.0f, 106000.0f, 100.0f), FRotator::ZeroRotator, Params);
	AActionRPGEnemyCharacter* Enemy = World->SpawnActor<AActionRPGEnemyCharacter>(
		FVector(106100.0f, 106000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawned"), Player) || !TestNotNull(TEXT("Enemy spawned"), Enemy)) return false;

	USecondaryAttributeDataAsset* Resources = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula Health;
	Health.BaseValue = 100.0f;
	Resources->Formulas.Add(ESecondaryAttribute::MaxHealth, Health);
	Player->SecondaryAttributeComponent->SecondaryAttributeDataAsset = Resources;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	if (!Enemy->HasActorBegunPlay()) Enemy->DispatchBeginPlay();
	Player->SecondaryAttributeComponent->RecalculateSecondaryAttributes();

	FAttributeTrainingRule* Rule = Player->TrainingComponent->Rules.FindByPredicate([](const FAttributeTrainingRule& Candidate)
	{
		return Candidate.Attribute == EPrimaryAttribute::Constitution && Candidate.Activity == TEXT("SurviveEnemyHit");
	});
	if (!TestNotNull(TEXT("Constitution survival rule exists"), Rule)) return false;
	Rule->PracticePerGain = 1;
	const float InitialConstitution = Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Constitution);
	const int32 InitialFreePoints = Player->AttributeComponent->GetUnspentAttributePoints();
	const float InitialHealth = Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth);
	Enemy->AttemptMeleeAttack();
	TestTrue(TEXT("Enemy strike damages nearby player"),
		Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth) < InitialHealth);
	TestEqual(TEXT("Surviving enemy strike grants Constitution"),
		Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Constitution), InitialConstitution + 1.0f);
	TestEqual(TEXT("Survival training does not grant free attribute points"),
		Player->AttributeComponent->GetUnspentAttributePoints(), InitialFreePoints);

	Player->TakeDamage(6.0f, FDamageEvent(), nullptr, Player);
	TestEqual(TEXT("Self damage grants no further Constitution"),
		Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Constitution), InitialConstitution + 1.0f);
	Enemy->AttemptMeleeAttack();
	TestEqual(TEXT("Same target cooldown prevents immediate farming"),
		Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Constitution), InitialConstitution + 1.0f);

	Enemy->Destroy();
	Player->Destroy();
	return true;
}

#endif
