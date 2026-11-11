#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Actors/SkillProjectile.h"
#include "Characters/ActionRPGEnemyCharacter.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Combat/StatusEffectDataAsset.h"
#include "Components/Combat/StatusEffectComponent.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Components/Progression/TrainingComponent.h"
#include "Progression/Core/SecondaryAttributeDataAsset.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRemainingAttributeTrainingTest, "ActionRPG.Progression.MagicAndHealingTraining",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRemainingAttributeTrainingTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world exists"), World)) return false;
	FActorSpawnParameters Params;
	Params.ObjectFlags |= RF_Transient;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(107000.0f, 107000.0f, 100.0f), FRotator::ZeroRotator, Params);
	AActionRPGEnemyCharacter* Enemy = World->SpawnActor<AActionRPGEnemyCharacter>(
		FVector(107200.0f, 107000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Player spawned"), Player) || !TestNotNull(TEXT("Enemy spawned"), Enemy)) return false;
	USecondaryAttributeDataAsset* Resources = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula Health;
	Health.BaseValue = 100.0f;
	Resources->Formulas.Add(ESecondaryAttribute::MaxHealth, Health);
	Player->SecondaryAttributeComponent->SecondaryAttributeDataAsset = Resources;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	if (!Enemy->HasActorBegunPlay()) Enemy->DispatchBeginPlay();
	Player->SecondaryAttributeComponent->RecalculateSecondaryAttributes();

	FAttributeTrainingRule* MagicRule = Player->TrainingComponent->Rules.FindByPredicate([](const FAttributeTrainingRule& Rule)
	{
		return Rule.Attribute == EPrimaryAttribute::Intelligence && Rule.Activity == TEXT("MagicHit");
	});
	FAttributeTrainingRule* HealingRule = Player->TrainingComponent->Rules.FindByPredicate([](const FAttributeTrainingRule& Rule)
	{
		return Rule.Attribute == EPrimaryAttribute::Wisdom && Rule.Activity == TEXT("EffectiveSkillHeal");
	});
	FAttributeTrainingRule* RangedRule = Player->TrainingComponent->Rules.FindByPredicate([](const FAttributeTrainingRule& Rule)
	{
		return Rule.Attribute == EPrimaryAttribute::Dexterity && Rule.Activity == TEXT("RangedHit");
	});
	if (!TestNotNull(TEXT("Intelligence rule exists"), MagicRule) ||
		!TestNotNull(TEXT("Wisdom rule exists"), HealingRule) ||
		!TestNotNull(TEXT("Dexterity ranged rule exists"), RangedRule)) return false;
	MagicRule->PracticePerGain = 1;
	HealingRule->PracticePerGain = 1;
	RangedRule->PracticePerGain = 1;
	HealingRule->ActivityCooldownSeconds = 0.0f;
	const float StartingIntelligence = Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Intelligence);
	const float StartingDexterity = Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Dexterity);
	const float StartingWisdom = Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Wisdom);
	const int32 StartingFreePoints = Player->AttributeComponent->GetUnspentAttributePoints();

	USkillDataAsset* FireballData = LoadObject<USkillDataAsset>(nullptr,
		TEXT("/Game/Data/Skills/Data/Magic/DA_Skill_Fireball.DA_Skill_Fireball"));
	USkillDataAsset* RangedData = LoadObject<USkillDataAsset>(nullptr,
		TEXT("/Game/Data/Skills/Data/Range/DA_Skill_RangedAttack.DA_Skill_RangedAttack"));
	if (!TestNotNull(TEXT("Fireball skill data exists"), FireballData) ||
		!TestNotNull(TEXT("Ranged skill data exists"), RangedData)) return false;
	if (!TestNotNull(TEXT("Fireball projectile class exists"), FireballData->ProjectileClass.Get()) ||
		!TestNotNull(TEXT("Ranged projectile class exists"), RangedData->ProjectileClass.Get())) return false;
	if (!TestTrue(TEXT("Fireball uses the native damage and training projectile"),
		FireballData->ProjectileClass->IsChildOf(ASkillProjectile::StaticClass())) ||
		!TestTrue(TEXT("Ranged attack uses the native damage and training projectile"),
		RangedData->ProjectileClass->IsChildOf(ASkillProjectile::StaticClass()))) return false;
	ASkillProjectile* Projectile = Cast<ASkillProjectile>(World->SpawnActor<AActor>(
		FireballData->ProjectileClass, FVector(107100.0f, 107000.0f, 100.0f), FRotator::ZeroRotator, Params));
	if (!TestNotNull(TEXT("Fireball projectile spawned"), Projectile)) return false;
	USkillBase* MagicSkill = NewObject<USkillBase>(Projectile);
	MagicSkill->SkillData = FireballData;
	Projectile->SetOwnerActor(Player);
	Projectile->SetSkillReference(MagicSkill);
	Projectile->SetDamage(20.0f);
	const float EnemyHealthBefore = Enemy->GetCurrentHealth();
	Projectile->OnBeginOverlap(nullptr, Enemy, nullptr, 0, false, FHitResult());
	TestTrue(TEXT("Magic projectile damages enemy"), Enemy->GetCurrentHealth() < EnemyHealthBefore);
	TestEqual(TEXT("Damaging magic hit grants Intelligence"),
		Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Intelligence), StartingIntelligence + 1.0f);
	TestEqual(TEXT("Magic hit does not train Dexterity"),
		Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Dexterity), StartingDexterity);

	ASkillProjectile* RangedProjectile = Cast<ASkillProjectile>(World->SpawnActor<AActor>(
		RangedData->ProjectileClass, FVector(107100.0f, 107000.0f, 100.0f), FRotator::ZeroRotator, Params));
	if (!TestNotNull(TEXT("Ranged projectile spawned"), RangedProjectile)) return false;
	USkillBase* RangedSkill = NewObject<USkillBase>(RangedProjectile);
	RangedSkill->SkillData = RangedData;
	RangedProjectile->SetOwnerActor(Player);
	RangedProjectile->SetSkillReference(RangedSkill);
	RangedProjectile->SetDamage(10.0f);
	RangedProjectile->OnBeginOverlap(nullptr, Enemy, nullptr, 0, false, FHitResult());
	TestEqual(TEXT("Damaging ranged hit grants Dexterity"),
		Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Dexterity), StartingDexterity + 1.0f);
	TestEqual(TEXT("Ranged hit does not train Intelligence"),
		Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Intelligence), StartingIntelligence + 1.0f);

	MagicRule->ActivityCooldownSeconds = 0.0f;
	MagicRule->TargetCooldownSeconds = 0.0f;
	FActorSpawnParameters OwnedParams = Params;
	OwnedParams.Owner = Player;
	ASkillProjectile* SplashProjectile = Cast<ASkillProjectile>(World->SpawnActor<AActor>(
		FireballData->ProjectileClass, FVector(107100.0f, 107000.0f, 100.0f), FRotator::ZeroRotator, OwnedParams));
	AActor* ImpactSurface = World->SpawnActor<AActor>(
		FVector(107100.0f, 107000.0f, 100.0f), FRotator::ZeroRotator, Params);
	if (!TestNotNull(TEXT("Splash projectile spawned"), SplashProjectile) ||
		!TestNotNull(TEXT("Impact surface spawned"), ImpactSurface)) return false;
	if (!SplashProjectile->HasActorBegunPlay()) SplashProjectile->DispatchBeginPlay();
	USkillBase* SplashSkill = NewObject<USkillBase>(SplashProjectile);
	SplashSkill->SkillData = FireballData;
	SplashProjectile->SetSkillReference(SplashSkill);
	SplashProjectile->SetDamage(10.0f);
	SplashProjectile->SetAreaOfEffectRadius(150.0f);
	const float PlayerHealthBeforeSplash = Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth);
	SplashProjectile->OnBeginOverlap(nullptr, Player, nullptr, 0, false, FHitResult());
	TestEqual(TEXT("Projectile never damages its owner"),
		Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth), PlayerHealthBeforeSplash);
	const float EnemyHealthBeforeSplash = Enemy->GetCurrentHealth();
	SplashProjectile->OnHit(nullptr, ImpactSurface, nullptr, FVector::ZeroVector, FHitResult());
	TestTrue(TEXT("Fireball splash damages a nearby enemy"), Enemy->GetCurrentHealth() < EnemyHealthBeforeSplash);
	TestEqual(TEXT("Damaging splash hit grants Intelligence"),
		Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Intelligence), StartingIntelligence + 2.0f);
	ImpactSurface->Destroy();

	Player->TakeDamage(20.0f, FDamageEvent(), nullptr, Player);
	UStatusEffectDataAsset* HealingEffect = NewObject<UStatusEffectDataAsset>(Player);
	HealingEffect->EffectID = TEXT("TrainingHeal");
	HealingEffect->DurationSeconds = 1.0f;
	HealingEffect->TickIntervalSeconds = 0.1f;
	HealingEffect->HealthPerTick = 3.0f;
	TestTrue(TEXT("Healing effect applies"), Player->StatusEffectComponent->ApplyEffect(HealingEffect, Player));
	const float HealthBeforeHeal = Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth);
	Player->StatusEffectComponent->AdvanceEffects();
	TestTrue(TEXT("Healing effect restores missing health"),
		Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth) > HealthBeforeHeal);
	TestEqual(TEXT("Effective healing grants Wisdom"),
		Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Wisdom), StartingWisdom + 1.0f);
	Player->SecondaryAttributeComponent->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth,
		Player->SecondaryAttributeComponent->GetSecondaryAttribute(ESecondaryAttribute::MaxHealth));
	Player->StatusEffectComponent->AdvanceEffects();
	TestEqual(TEXT("Overhealing grants no additional Wisdom"),
		Player->AttributeComponent->GetPersistentAttribute(EPrimaryAttribute::Wisdom), StartingWisdom + 1.0f);
	TestEqual(TEXT("Magic and healing training leave free attribute points unchanged"),
		Player->AttributeComponent->GetUnspentAttributePoints(), StartingFreePoints);

	Enemy->Destroy();
	Player->Destroy();
	return true;
}

#endif
