#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Characters/ActionRPGEnemyCharacter.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Progression/TrainingComponent.h"
#include "Progression/Core/SecondaryAttributeDataAsset.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatDodgeTrainingTest, "ActionRPG.Progression.CombatDodgeTraining",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatDodgeTrainingTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(105000.0f, 105000.0f, 100.0f), FRotator::ZeroRotator, Params);
	AActionRPGEnemyCharacter* Enemy = World->SpawnActor<AActionRPGEnemyCharacter>(
		FVector(105200.0f, 105000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawned"), Player) || !TestNotNull(TEXT("Enemy spawned"), Enemy)) return false;

	USecondaryAttributeDataAsset* Resources = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula Stamina;
	Stamina.BaseValue = 100.0f;
	Resources->Formulas.Add(ESecondaryAttribute::MaxStamina, Stamina);
	FSecondaryAttributeFormula Health;
	Health.BaseValue = 100.0f;
	Resources->Formulas.Add(ESecondaryAttribute::MaxHealth, Health);
	Player->SecondaryAttributeComponent->SecondaryAttributeDataAsset = Resources;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	if (!Enemy->HasActorBegunPlay()) Enemy->DispatchBeginPlay();
	Player->SecondaryAttributeComponent->RecalculateSecondaryAttributes();
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	FAttributeTrainingRule* DodgeRule = Player->TrainingComponent->Rules.FindByPredicate([](const FAttributeTrainingRule& Rule)
	{
		return Rule.Attribute == EPrimaryAttribute::Agility && Rule.Activity == TEXT("CombatDodge");
	});
	if (!TestNotNull(TEXT("Agility dodge rule exists"), DodgeRule)) return false;
	DodgeRule->PracticePerGain = 1;
	const float StartingAgility = Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Agility);
	const int32 StartingFreePoints = Player->AttributeComponent->GetUnspentAttributePoints();
	TestTrue(TEXT("Dodge starts when stamina is available"), Player->TryDodge());
	TestFalse(TEXT("Repeated dodge is blocked while active"), Player->TryDodge());
	TestEqual(TEXT("Dodge spends stamina once"),
		Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina), 85.0f);
	Player->SetActorLocation(FVector(104850.0f, 105000.0f, 100.0f), false, nullptr, ETeleportType::TeleportPhysics);
	Player->FinishDodge();
	TestFalse(TEXT("Dodge ends"), Player->IsDodging());
	TestEqual(TEXT("Moving away from a nearby enemy grants Agility"),
		Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Agility), StartingAgility + 1.0f);
	TestEqual(TEXT("Training does not award free points"),
		Player->AttributeComponent->GetUnspentAttributePoints(), StartingFreePoints);
	TestEqual(TEXT("Dodge does not train Dexterity"),
		Player->TrainingComponent->GetPracticeProgress(EPrimaryAttribute::Dexterity, TEXT("RangedHit")), 0);

	AActionRPGPlayerCharacter* Defender = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(105000.0f, 105200.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Second player spawned"), Defender)) return false;
	USecondaryAttributeDataAsset* DefenderResources = NewObject<USecondaryAttributeDataAsset>(Defender);
	DefenderResources->Formulas.Add(ESecondaryAttribute::MaxStamina, Stamina);
	DefenderResources->Formulas.Add(ESecondaryAttribute::MaxHealth, Health);
	Defender->SecondaryAttributeComponent->SecondaryAttributeDataAsset = DefenderResources;
	if (!Defender->HasActorBegunPlay()) Defender->DispatchBeginPlay();
	Defender->SecondaryAttributeComponent->RecalculateSecondaryAttributes();
	Defender->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	const float HealthBefore = Defender->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth);
	TestTrue(TEXT("Defender starts dodge"), Defender->TryDodge());
	TestEqual(TEXT("Enemy strike during dodge is avoided"),
		Defender->TakeDamage(10.0f, FDamageEvent(), nullptr, Enemy), 0.0f);
	TestEqual(TEXT("Avoided strike leaves health intact"),
		Defender->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth), HealthBefore);
	TestEqual(TEXT("Avoided enemy strike records practice"),
		Defender->TrainingComponent->GetPracticeProgress(EPrimaryAttribute::Agility, TEXT("CombatDodge")), 1);
	Defender->FinishDodge();
	TestEqual(TEXT("One dodge records at most one practice event"),
		Defender->TrainingComponent->GetPracticeProgress(EPrimaryAttribute::Agility, TEXT("CombatDodge")), 1);

	Defender->Destroy();
	Enemy->Destroy();
	Player->Destroy();
	return true;
}

#endif
