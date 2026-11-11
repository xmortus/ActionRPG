#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Characters/ActionRPGEnemyCharacter.h"
#include "Components/Progression/AttributeComponent.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Progression/Core/SecondaryAttributeDataAsset.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Combat/DamageResolution.h"
#include "Combat/ActionRPGPhysicalDamageType.h"
#include "Combat/ActionRPGMagicDamageType.h"
#include "Combat/ActionRPGDamageTypes.h"
#include "Actors/SkillProjectile.h"
#include "Components/Skills/SkillComponent.h"
#include "Skills/Core/SkillBase.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Skills/Implementations/SkillMagic.h"
#include "Skills/Implementations/SkillDash.h"
#include "Skills/Implementations/SkillMeleeAttack.h"
#include "Skills/Implementations/SkillRangedAttack.h"

#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerDamageAndReviveTest, "ActionRPG.Combat.PlayerDamageAndRevive",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerDamageAndReviveTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(103000.0f, 103000.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;

	USecondaryAttributeDataAsset* Rules = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula HealthFormula;
	HealthFormula.BaseValue = 100.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxHealth, HealthFormula);
	Player->SecondaryAttributeComponent->SecondaryAttributeDataAsset = Rules;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	Player->SecondaryAttributeComponent->RecalculateSecondaryAttributes();
	const FDamageEvent DamageEvent;
	TestEqual(TEXT("Non-finite damage is rejected"), Player->TakeDamage(
		std::numeric_limits<float>::quiet_NaN(), DamageEvent, nullptr, nullptr), 0.0f);
	TestEqual(TEXT("Nonlethal damage is applied"), Player->TakeDamage(40.0f, DamageEvent, nullptr, nullptr), 40.0f);
	TestEqual(TEXT("Health resource tracks damage"), Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(
		ESecondaryAttribute::MaxHealth), 60.0f);
	TestFalse(TEXT("Player remains alive after nonlethal hit"), Player->IsDead());
	TestEqual(TEXT("Lethal hit applies only remaining health"), Player->TakeDamage(
		100.0f, DamageEvent, nullptr, nullptr), 60.0f);
	TestTrue(TEXT("Player enters dead state"), Player->IsDead());
	TestEqual(TEXT("Dead player has zero health"), Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(
		ESecondaryAttribute::MaxHealth), 0.0f);
	TestEqual(TEXT("Repeated hit cannot damage dead player"), Player->TakeDamage(
		100.0f, DamageEvent, nullptr, nullptr), 0.0f);
	TestFalse(TEXT("Invalid revive fraction is rejected"), Player->Revive(0.0f));
	TestTrue(TEXT("Explicit revive succeeds"), Player->Revive(0.5f));
	TestFalse(TEXT("Revived player is alive"), Player->IsDead());
	TestEqual(TEXT("Revive restores requested health"), Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(
		ESecondaryAttribute::MaxHealth), 50.0f);
	TestEqual(TEXT("Revived player can take damage again"), Player->TakeDamage(
		10.0f, DamageEvent, nullptr, nullptr), 10.0f);
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPhysicalArmorMitigationTest, "ActionRPG.Combat.PhysicalArmorMitigation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPhysicalArmorMitigationTest::RunTest(const FString& Parameters)
{
	const FActionRPGDamageResult NoArmor = FActionRPGDamageResolver::ResolvePhysical(40.0f, 100.0f, false, 0.0f, 100.0f);
	TestEqual(TEXT("Zero Armor preserves physical damage"), NoArmor.AppliedDamage, 40.0f);
	const FActionRPGDamageResult Armored = FActionRPGDamageResolver::ResolvePhysical(40.0f, 100.0f, false, 100.0f, 100.0f);
	TestEqual(TEXT("100 Armor halves physical damage at the default scale"), Armored.AppliedDamage, 20.0f);
	TestEqual(TEXT("Armor-adjusted health is correct"), Armored.RemainingHealth, 80.0f);
	const FActionRPGDamageResult Untyped = FActionRPGDamageResolver::Resolve(40.0f, 100.0f, false);
	TestEqual(TEXT("Untyped damage retains its prior value"), Untyped.AppliedDamage, 40.0f);

	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(103100.0f, 103100.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	USecondaryAttributeDataAsset* Rules = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula HealthFormula;
	HealthFormula.BaseValue = 100.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxHealth, HealthFormula);
	FSecondaryAttributeFormula ArmorFormula;
	ArmorFormula.BaseValue = 100.0f;
	Rules->Formulas.Add(ESecondaryAttribute::Armor, ArmorFormula);
	Player->SecondaryAttributeComponent->SecondaryAttributeDataAsset = Rules;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	Player->SecondaryAttributeComponent->RecalculateSecondaryAttributes();
	const FDamageEvent PhysicalEvent(UActionRPGPhysicalDamageType::StaticClass());
	TestEqual(TEXT("Player Armor mitigates a tagged physical hit"),
		Player->TakeDamage(40.0f, PhysicalEvent, nullptr, nullptr), 20.0f);
	TestEqual(TEXT("Player health reflects mitigated damage"),
		Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth), 80.0f);
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProjectileDamageClassificationTest, "ActionRPG.Combat.ProjectileDamageClassification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FProjectileDamageClassificationTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASkillProjectile* Projectile = World->SpawnActor<ASkillProjectile>(
		FVector(103200.0f, 103200.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Projectile spawned"), Projectile)) return false;
	TestEqual(TEXT("Projectile without skill keeps untyped damage"),
		Projectile->GetDamageTypeClass().Get(), UDamageType::StaticClass());
	USkillBase* Skill = NewObject<USkillBase>(Projectile);
	USkillDataAsset* SkillData = NewObject<USkillDataAsset>(Skill);
	Skill->SkillData = SkillData;
	Projectile->SetSkillReference(Skill);
	SkillData->DamageType = EDamageType::Physical;
	TestEqual(TEXT("Physical ranged skill uses Armor-aware damage"),
		Projectile->GetDamageTypeClass().Get(), UActionRPGPhysicalDamageType::StaticClass());
	SkillData->DamageType = EDamageType::Fire;
	TestEqual(TEXT("Fire skill keeps its own damage type"),
		Projectile->GetDamageTypeClass().Get(), UActionRPGFireDamageType::StaticClass());
	for (const EDamageType Kind : { EDamageType::Physical, EDamageType::Fire, EDamageType::Cold,
		EDamageType::Lightning, EDamageType::Poison, EDamageType::Magic, EDamageType::Dark, EDamageType::Holy })
	{
		const TOptional<EDamageType> RoundTrip = FActionRPGDamageTypes::GetKind(FActionRPGDamageTypes::GetClass(Kind));
		TestTrue(TEXT("Every skill damage kind survives Unreal damage class mapping"), RoundTrip.IsSet() && RoundTrip.GetValue() == Kind);
	}
	Projectile->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerElementalResistanceTest, "ActionRPG.Combat.PlayerElementalResistance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerElementalResistanceTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(103300.0f, 103300.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	USecondaryAttributeDataAsset* Rules = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula HealthFormula;
	HealthFormula.BaseValue = 100.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxHealth, HealthFormula);
	FSecondaryAttributeFormula ArmorFormula;
	ArmorFormula.BaseValue = 100.0f;
	Rules->Formulas.Add(ESecondaryAttribute::Armor, ArmorFormula);
	Rules->DamageResistancePercent.Add(EDamageType::Fire, 25.0f);
	Player->SecondaryAttributeComponent->SecondaryAttributeDataAsset = Rules;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	Player->SecondaryAttributeComponent->RecalculateSecondaryAttributes();
	const FDamageEvent FireEvent(UActionRPGFireDamageType::StaticClass());
	TestEqual(TEXT("Fire resistance prevents 25 percent, without Armor"),
		Player->TakeDamage(40.0f, FireEvent, nullptr, nullptr), 30.0f);
	UItemDataAsset* RingData = NewObject<UItemDataAsset>(Player);
	RingData->Type = EItemType::Equipment;
	RingData->EquipmentSlot = EEquipmentSlot::Ring;
	RingData->EquipmentDamageResistanceBonuses.Add(EDamageType::Fire, 25.0f);
	UEquipmentItem* Ring = NewObject<UEquipmentItem>(Player);
	Ring->ItemData = RingData;
	TestTrue(TEXT("Resistance ring equips"), Player->EquipmentComponent->EquipItem(Ring));
	TestEqual(TEXT("Equipped resistance stacks with base resistance"),
		Player->SecondaryAttributeComponent->GetDamageResistancePercent(EDamageType::Fire), 50.0f);
	Player->SecondaryAttributeComponent->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth, 100.0f);
	TestEqual(TEXT("Equipped resistance reduces Fire damage"),
		Player->TakeDamage(40.0f, FireEvent, nullptr, nullptr), 20.0f);
	TestTrue(TEXT("Resistance ring unequips"), Player->EquipmentComponent->UnequipSlot(EEquipmentSlot::Ring) == Ring);
	TestEqual(TEXT("Unequip restores base resistance"),
		Player->SecondaryAttributeComponent->GetDamageResistancePercent(EDamageType::Fire), 25.0f);
	Player->SecondaryAttributeComponent->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth, 100.0f);
	const FDamageEvent PhysicalEvent(UActionRPGPhysicalDamageType::StaticClass());
	TestEqual(TEXT("Armor still reduces physical damage separately"),
		Player->TakeDamage(40.0f, PhysicalEvent, nullptr, nullptr), 20.0f);
	Player->SecondaryAttributeComponent->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth, 100.0f);
	TestEqual(TEXT("Untyped hits ignore configured resistance"),
		Player->TakeDamage(40.0f, FDamageEvent(), nullptr, nullptr), 40.0f);
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyPhysicalArmorTest, "ActionRPG.Combat.EnemyPhysicalArmor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEnemyPhysicalArmorTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGEnemyCharacter* Enemy = World->SpawnActor<AActionRPGEnemyCharacter>(
		FVector(103400.0f, 103400.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Enemy spawned"), Enemy)) return false;
	Enemy->Armor = 100.0f;
	Enemy->ArmorMitigationScale = 100.0f;
	Enemy->DamageResistancePercent.Add(EDamageType::Physical, 50.0f);
	Enemy->DamageResistancePercent.Add(EDamageType::Fire, 25.0f);
	if (!Enemy->HasActorBegunPlay()) Enemy->DispatchBeginPlay();
	TestEqual(TEXT("Enemy physical resistance and Armor both apply"),
		Enemy->TakeDamage(40.0f, FDamageEvent(UActionRPGPhysicalDamageType::StaticClass()), nullptr, nullptr), 10.0f);
	TestEqual(TEXT("Enemy health reflects mitigated physical hit"), Enemy->GetCurrentHealth(), 90.0f);
	TestEqual(TEXT("Enemy Fire resistance applies without Armor"),
		Enemy->TakeDamage(40.0f, FDamageEvent(UActionRPGFireDamageType::StaticClass()), nullptr, nullptr), 30.0f);
	TestEqual(TEXT("Enemy health reflects resisted Fire hit"), Enemy->GetCurrentHealth(), 60.0f);
	TestEqual(TEXT("Untyped lethal damage still applies and kills"),
		Enemy->TakeDamage(100.0f, FDamageEvent(), nullptr, nullptr), 60.0f);
	TestTrue(TEXT("Enemy death still occurs"), Enemy->IsDead());
	Enemy->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMeleeWeaponDamageTypeTest, "ActionRPG.Combat.MeleeWeaponDamageType",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMeleeWeaponDamageTypeTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(103500.0f, 103500.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	USkillMeleeAttack* Attack = NewObject<USkillMeleeAttack>(Player);
	Attack->OwnerActor = Player;
	Attack->SkillData = NewObject<USkillDataAsset>(Attack);
	Attack->SkillData->DamageType = EDamageType::Physical;
	Attack->SkillData->BaseDamage = 25.0f;
	Attack->SkillData->MeleeDamagePerStrength = 0.0f;
	Attack->SkillData->DamagePercentPerSkillLevel = 3.0f;
	Attack->SkillData->CooldownDuration = 1.0f;
	Attack->SkillData->Range = 150.0f;
	TestEqual(TEXT("Unarmed melee uses skill's Physical type"),
		Attack->GetMeleeDamageTypeClass().Get(), UActionRPGPhysicalDamageType::StaticClass());
	UItemDataAsset* SwordData = NewObject<UItemDataAsset>(Player);
	SwordData->Type = EItemType::Equipment;
	SwordData->EquipmentSlot = EEquipmentSlot::MainHand;
	SwordData->WeaponDamage = 25.0f;
	SwordData->WeaponDamageType = EDamageType::Fire;
	UEquipmentItem* Sword = NewObject<UEquipmentItem>(Player);
	Sword->ItemData = SwordData;
	TestTrue(TEXT("Fire sword equips"), Player->EquipmentComponent->EquipItem(Sword));
	TestEqual(TEXT("Melee uses equipped Fire sword type"),
		Attack->GetMeleeDamageTypeClass().Get(), UActionRPGFireDamageType::StaticClass());
	AActionRPGEnemyCharacter* Enemy = World->SpawnActor<AActionRPGEnemyCharacter>(
		FVector(103620.0f, 103500.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Enemy spawned"), Enemy)) { Player->Destroy(); return false; }
	if (!Enemy->HasActorBegunPlay()) Enemy->DispatchBeginPlay();
	TestTrue(TEXT("Sword attack activates"), Attack->Activate() == Attack);
	TestEqual(TEXT("Sword damage adds to skill base damage"), Enemy->GetCurrentHealth(), 50.0f);
	TestTrue(TEXT("Fire sword unequips"), Player->EquipmentComponent->UnequipSlot(EEquipmentSlot::MainHand) == Sword);
	TestEqual(TEXT("Melee returns to skill type after unequip"),
		Attack->GetMeleeDamageTypeClass().Get(), UActionRPGPhysicalDamageType::StaticClass());
	Attack->SkillData->MeleeDamagePerStrength = 0.5f;
	const float InitialDamage = Attack->GetAttackDamage();
	const float InitialCooldown = Attack->GetEffectiveCooldownDuration();
	TMap<EPrimaryAttribute, float> TrainingBonus;
	TrainingBonus.Add(EPrimaryAttribute::Strength, 2.0f);
	TrainingBonus.Add(EPrimaryAttribute::Agility, 10.0f);
	TestTrue(TEXT("Training attributes applied"), Player->AttributeComponent->SetModifierSource(TEXT("Test.CombatTraining"), TrainingBonus, true));
	TestTrue(TEXT("Strength raises actual melee damage"), Attack->GetAttackDamage() > InitialDamage);
	TestTrue(TEXT("Agility shortens melee cooldown"), Attack->GetEffectiveCooldownDuration() < InitialCooldown);
	Attack->UpdateCooldown(1.0f);
	TestTrue(TEXT("Unarmed attack activates"), Attack->Activate() == Attack);
	TestTrue(TEXT("Unarmed hit uses the scaled damage"),
		FMath::IsNearlyEqual(Enemy->GetCurrentHealth(), 50.0f - Attack->GetAttackDamage()));
	const float LevelOneMeleeDamage = Attack->GetAttackDamage();
	Attack->SkillLevel = 2;
	TestTrue(TEXT("Melee mastery raises the same damage calculation used by hits"),
		FMath::IsNearlyEqual(Attack->GetAttackDamage(), LevelOneMeleeDamage * 1.03f));
	Enemy->Destroy();
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProjectileActivationFailureTest, "ActionRPG.Combat.ProjectileActivationFailure",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FProjectileActivationFailureTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(103800.0f, 103800.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	USecondaryAttributeDataAsset* Resources = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula Mana;
	Mana.BaseValue = 100.0f;
	Resources->Formulas.Add(ESecondaryAttribute::MaxMana, Mana);
	FSecondaryAttributeFormula Stamina;
	Stamina.BaseValue = 100.0f;
	Resources->Formulas.Add(ESecondaryAttribute::MaxStamina, Stamina);
	Player->SecondaryAttributeComponent->SecondaryAttributeDataAsset = Resources;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	Player->SecondaryAttributeComponent->RecalculateSecondaryAttributes();
	USkillComponent* Skills = NewObject<USkillComponent>(Player);
	Skills->RegisterComponent();
	TestFalse(TEXT("Null skill activation is rejected safely"), Skills->ActivateSkill(nullptr));

	USkillMagic* Magic = NewObject<USkillMagic>(Player);
	Magic->SkillData = NewObject<USkillDataAsset>(Magic);
	Magic->SkillData->ManaCost = 8.0f;
	Magic->SkillData->CooldownDuration = 2.0f;
	TestTrue(TEXT("Broken magic skill can be granted for failure test"), Skills->GrantSkill(Magic));
	AddExpectedError(TEXT("SkillMagic::Activate - ProjectileClass is not set"),
		EAutomationExpectedErrorFlags::Contains, 1, false);
	const float ManaBefore = Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxMana);
	TestFalse(TEXT("Magic cast without projectile class fails"), Skills->ActivateSkill(Magic));
	TestEqual(TEXT("Failed magic cast refunds mana"),
		Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxMana), ManaBefore);
	TestEqual(TEXT("Failed magic cast has no cooldown"), Magic->GetCooldownRemaining(), 0.0f);
	TestEqual(TEXT("Failed magic cast grants no skill XP"), Skills->GetSkillExperience(Magic), 0.0f);

	USkillRangedAttack* Ranged = NewObject<USkillRangedAttack>(Player);
	Ranged->SkillData = NewObject<USkillDataAsset>(Ranged);
	Ranged->SkillData->StaminaCost = 15.0f;
	Ranged->SkillData->CooldownDuration = 1.0f;
	TestTrue(TEXT("Broken ranged skill can be granted for failure test"), Skills->GrantSkill(Ranged));
	AddExpectedError(TEXT("SkillRangedAttack::Activate - ProjectileClass is not set"),
		EAutomationExpectedErrorFlags::Contains, 1, false);
	const float StaminaBefore = Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina);
	TestFalse(TEXT("Ranged attack without projectile class fails"), Skills->ActivateSkill(Ranged));
	TestEqual(TEXT("Failed ranged attack refunds stamina"),
		Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina), StaminaBefore);
	TestEqual(TEXT("Failed ranged attack has no cooldown"), Ranged->GetCooldownRemaining(), 0.0f);
	TestEqual(TEXT("Failed ranged attack grants no skill XP"), Skills->GetSkillExperience(Ranged), 0.0f);
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDashSkillGrowthTest, "ActionRPG.Combat.DashSkillGrowth",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDashSkillGrowthTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(104100.0f, 104100.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	USecondaryAttributeDataAsset* Resources = NewObject<USecondaryAttributeDataAsset>(Player);
	FSecondaryAttributeFormula Stamina;
	Stamina.BaseValue = 100.0f;
	Resources->Formulas.Add(ESecondaryAttribute::MaxStamina, Stamina);
	Player->SecondaryAttributeComponent->SecondaryAttributeDataAsset = Resources;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();
	Player->SecondaryAttributeComponent->RecalculateSecondaryAttributes();
	USkillComponent* Skills = NewObject<USkillComponent>(Player);
	Skills->RegisterComponent();
	USkillDash* Dash = NewObject<USkillDash>(Player);
	Dash->SkillData = NewObject<USkillDataAsset>(Dash);
	Dash->SkillData->AbilityMoveSpeed = 1200.0f;
	Dash->SkillData->StaminaCost = 10.0f;
	TestTrue(TEXT("Dash granted"), Skills->GrantSkill(Dash));
	TestEqual(TEXT("Level one Dash speed"), Dash->GetDashSpeed(), 1200.0f);
	TestEqual(TEXT("Level one Dash stamina cost"), Dash->GetStaminaCost(), 10.0f);
	Skills->AddSkillExperience(Dash, 100.0f);
	TestEqual(TEXT("Use experience raises Dash to level two"), Skills->GetSkillLevel(Dash), 2);
	TestEqual(TEXT("Level two Dash speed grows"), Dash->GetDashSpeed(), 1260.0f);
	Player->SecondaryAttributeComponent->SetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina, 9.75f);
	TestTrue(TEXT("Leveled Dash is available below its level one cost"), Skills->CanActivateSkill(Dash));
	TestTrue(TEXT("Leveled Dash activates"), Skills->ActivateSkill(Dash));
	TestTrue(TEXT("Activation spends the leveled stamina cost"), FMath::IsNearlyEqual(
		Player->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxStamina), 0.25f));
	Player->Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProjectileAttributeDamageTest, "ActionRPG.Combat.ProjectileAttributeDamage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FProjectileAttributeDamageTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world is available"), World)) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(104400.0f, 104400.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Player spawned"), Player)) return false;
	if (!Player->HasActorBegunPlay()) Player->DispatchBeginPlay();

	USkillRangedAttack* Ranged = NewObject<USkillRangedAttack>(Player);
	Ranged->OwnerActor = Player;
	Ranged->SkillData = NewObject<USkillDataAsset>(Ranged);
	Ranged->SkillData->BaseDamage = 20.0f;
	Ranged->SkillData->DamagePercentPerSkillLevel = 3.0f;
	Ranged->SkillData->ProjectileClass = ASkillProjectile::StaticClass();
	USkillMagic* Magic = NewObject<USkillMagic>(Player);
	Magic->OwnerActor = Player;
	Magic->SkillData = NewObject<USkillDataAsset>(Magic);
	Magic->SkillData->BaseDamage = 30.0f;
	Magic->SkillData->DamagePercentPerSkillLevel = 3.0f;
	Magic->SkillData->ProjectileClass = ASkillProjectile::StaticClass();
	const float RangedBefore = Ranged->GetAttackDamage();
	const float MagicBefore = Magic->GetAttackDamage();
	TMap<EPrimaryAttribute, float> TrainingBonus;
	TrainingBonus.Add(EPrimaryAttribute::Dexterity, 4.0f);
	TrainingBonus.Add(EPrimaryAttribute::Intelligence, 4.0f);
	TestTrue(TEXT("Combat attributes applied"), Player->AttributeComponent->SetModifierSource(TEXT("Test.ProjectileAttributes"), TrainingBonus, true));
	TestTrue(TEXT("Dexterity raises ranged damage"), FMath::IsNearlyEqual(Ranged->GetAttackDamage(), RangedBefore + 2.0f));
	TestTrue(TEXT("Intelligence raises magic damage"), FMath::IsNearlyEqual(Magic->GetAttackDamage(), MagicBefore + 3.0f));

	TestTrue(TEXT("Ranged attack spawns"), Ranged->Activate() == Ranged);
	ASkillProjectile* RangedProjectile = nullptr;
	for (TActorIterator<ASkillProjectile> It(World); It; ++It)
	{
		if (It->GetOwner() == Player) { RangedProjectile = *It; break; }
	}
	if (TestNotNull(TEXT("Ranged projectile exists"), RangedProjectile))
	{
		TestTrue(TEXT("Ranged projectile carries scaled damage"), FMath::IsNearlyEqual(RangedProjectile->GetDamage(), Ranged->GetAttackDamage()));
		RangedProjectile->Destroy();
	}
	TestTrue(TEXT("Magic attack spawns"), Magic->Activate() == Magic);
	ASkillProjectile* MagicProjectile = nullptr;
	for (TActorIterator<ASkillProjectile> It(World); It; ++It)
	{
		if (It->GetOwner() == Player) { MagicProjectile = *It; break; }
	}
	if (TestNotNull(TEXT("Magic projectile exists"), MagicProjectile))
	{
		TestTrue(TEXT("Magic projectile carries scaled damage"), FMath::IsNearlyEqual(MagicProjectile->GetDamage(), Magic->GetAttackDamage()));
		MagicProjectile->Destroy();
	}
	const float LevelOneRangedDamage = Ranged->GetAttackDamage();
	const float LevelOneMagicDamage = Magic->GetAttackDamage();
	USkillComponent* Skills = NewObject<USkillComponent>(Player);
	Skills->RegisterComponent();
	TestTrue(TEXT("Ranged skill granted for mastery"), Skills->GrantSkill(Ranged));
	TestTrue(TEXT("Magic skill granted for mastery"), Skills->GrantSkill(Magic));
	Skills->AddSkillExperience(Ranged, 100.0f);
	Skills->AddSkillExperience(Magic, 100.0f);
	TestEqual(TEXT("Ranged use XP reaches level two"), Skills->GetSkillLevel(Ranged), 2);
	TestEqual(TEXT("Magic use XP reaches level two"), Skills->GetSkillLevel(Magic), 2);
	TestTrue(TEXT("Ranged mastery raises damage"), FMath::IsNearlyEqual(Ranged->GetAttackDamage(), LevelOneRangedDamage * 1.03f));
	TestTrue(TEXT("Magic mastery raises damage"), FMath::IsNearlyEqual(Magic->GetAttackDamage(), LevelOneMagicDamage * 1.03f));
	TestTrue(TEXT("Leveled ranged attack spawns"), Ranged->Activate() == Ranged);
	bool bFoundLeveledRangedProjectile = false;
	for (TActorIterator<ASkillProjectile> It(World); It; ++It)
	{
		if (It->GetOwner() == Player)
		{
			bFoundLeveledRangedProjectile = true;
			TestTrue(TEXT("Leveled ranged projectile carries mastery damage"), FMath::IsNearlyEqual(It->GetDamage(), Ranged->GetAttackDamage()));
			It->Destroy();
			break;
		}
	}
	TestTrue(TEXT("Leveled ranged projectile exists"), bFoundLeveledRangedProjectile);
	TestTrue(TEXT("Leveled magic attack spawns"), Magic->Activate() == Magic);
	bool bFoundLeveledMagicProjectile = false;
	for (TActorIterator<ASkillProjectile> It(World); It; ++It)
	{
		if (It->GetOwner() == Player)
		{
			bFoundLeveledMagicProjectile = true;
			TestTrue(TEXT("Leveled magic projectile carries mastery damage"), FMath::IsNearlyEqual(It->GetDamage(), Magic->GetAttackDamage()));
			It->Destroy();
			break;
		}
	}
	TestTrue(TEXT("Leveled magic projectile exists"), bFoundLeveledMagicProjectile);
	Player->Destroy();
	return true;
}

#endif
