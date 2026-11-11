// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Inventory/EquipmentComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Items/Core/ItemDataAsset.h"
#include "Items/Equipment/EquipmentItem.h"
#include "Skills/Core/SkillDataAsset.h"
#include "Skills/Implementations/SkillMeleeAttack.h"
#include "Engine/World.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMainHandWeaponVisualTest, "ActionRPG.Equipment.MainHandWeaponVisual",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMainHandWeaponVisualTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	UClass* PlayerClass = LoadClass<AActionRPGPlayerCharacter>(nullptr,
		TEXT("/Game/Blueprints/Characters/BP_ActionRPGPlayerCharacter.BP_ActionRPGPlayerCharacter_C"));
	UItemDataAsset* SwordData = LoadObject<UItemDataAsset>(nullptr,
		TEXT("/Game/Data/Items/Data/Equipment/DA_Item_Weapon_Sword.DA_Item_Weapon_Sword"));
	if (!TestNotNull(TEXT("World is available"), World) ||
		!TestNotNull(TEXT("Configured player class loads"), PlayerClass) ||
		!TestNotNull(TEXT("Iron Sword data loads"), SwordData))
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* Player = World->SpawnActor<AActionRPGPlayerCharacter>(
		PlayerClass, FVector(100000.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParameters);
	if (!TestNotNull(TEXT("Configured player spawns"), Player))
	{
		return false;
	}
	if (!Player->HasActorBegunPlay())
	{
		Player->DispatchBeginPlay();
	}

	UStaticMeshComponent* Visual = Player->EquippedWeaponVisual;
	if (!TestNotNull(TEXT("Weapon visual exists"), Visual) ||
		!TestNotNull(TEXT("Equipment component exists"), Player->EquipmentComponent.Get()))
	{
		Player->Destroy();
		return false;
	}
	TestTrue(TEXT("Unarmed visual starts hidden"), !Visual->IsVisible());
	TestNotNull(TEXT("Basic melee animation is configured"), Player->BasicMeleeAttackAnimation.Get());
	const UAnimSequence* AttackSequence = Cast<UAnimSequence>(Player->BasicMeleeAttackAnimation.Get());
	if (TestNotNull(TEXT("Basic melee animation is an animation sequence"), AttackSequence))
	{
		TestFalse(TEXT("Basic melee animation cannot move the character with root motion"), AttackSequence->bEnableRootMotion);
		TestTrue(TEXT("Basic melee animation locks the root in place"), AttackSequence->bForceRootLock);
	}
	TestTrue(TEXT("Hand grip socket exists"), Player->GetMesh()->DoesSocketExist(Player->MainHandAttachmentName));
	TestEqual(TEXT("Weapon grip uses the hilt-facing roll"), Player->WeaponGripTransform.GetRotation().Rotator().Roll, -90.0);
	TestEqual(TEXT("Weapon grip offsets the hilt onto the hand"), Player->WeaponGripTransform.GetLocation().Z, 30.0);
	TestNotNull(TEXT("Iron Sword data has an equipped mesh"), SwordData->EquippedWeaponMesh.Get());
	TestTrue(TEXT("Native equipment change handler is bound"), Player->EquipmentComponent->OnEquipmentChangedNative.IsBound());

	UEquipmentItem* Sword = NewObject<UEquipmentItem>(Player);
	Sword->ItemData = SwordData;
	TestTrue(TEXT("Iron Sword can be equipped"), Player->EquipmentComponent->EquipItem(Sword));
	TestTrue(TEXT("Iron Sword occupies the main hand"), Player->EquipmentComponent->GetEquippedItem(EEquipmentSlot::MainHand) == Sword);
	TestTrue(TEXT("Equipped visual uses Iron Sword mesh"), Visual->GetStaticMesh() == SwordData->EquippedWeaponMesh);
	TestTrue(TEXT("Equipped visual is shown"), Visual->IsVisible());
	TestEqual(TEXT("Visual attaches to right-hand socket"), Visual->GetAttachSocketName(), Player->MainHandAttachmentName);
	USkillMeleeAttack* MeleeAttack = NewObject<USkillMeleeAttack>(Player);
	MeleeAttack->SkillData = NewObject<USkillDataAsset>(MeleeAttack);
	MeleeAttack->OwnerActor = Player;
	const FVector PositionBeforeAttack = Player->GetActorLocation();
	TestTrue(TEXT("Basic melee skill activates"), MeleeAttack->Activate() == MeleeAttack);
	TestTrue(TEXT("Basic melee skill does not translate the character"),
		Player->GetActorLocation().Equals(PositionBeforeAttack));

	TestTrue(TEXT("Iron Sword can be unequipped"), Player->EquipmentComponent->UnequipSlot(EEquipmentSlot::MainHand) == Sword);
	TestTrue(TEXT("Unequipped visual clears its mesh"), Visual->GetStaticMesh() == nullptr);
	TestTrue(TEXT("Unequipped visual is hidden"), !Visual->IsVisible());
	Player->Destroy();
	return true;
}

#endif
