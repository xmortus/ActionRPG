#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Blueprint/WidgetTree.h"
#include "Characters/ActionRPGPlayerCharacter.h"
#include "Components/Button.h"
#include "Components/Progression/SecondaryAttributeComponent.h"
#include "Core/ActionRPGPlayerController.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Progression/Core/SecondaryAttributeDataAsset.h"
#include "UI/HUD/PlayerDeathWidget.h"
#include "TimerManager.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerDeathFlowTest, "ActionRPG.Combat.PlayerDeathFlow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerDeathFlowTest::RunTest(const FString& Parameters)
{
	UWorld* World = GWorld;
	if (!TestNotNull(TEXT("Editor world exists"), World)) return false;
	UPlayerDeathWidget* Widget = NewObject<UPlayerDeathWidget>(GetTransientPackage());
	Widget->WidgetTree = NewObject<UWidgetTree>(Widget);
	Widget->NativeOnInitialized();
	if (!TestNotNull(TEXT("Death screen builds a respawn button"), Widget->RespawnButton.Get())) return false;
	TestFalse(TEXT("Respawn starts locked"), Widget->RespawnButton->GetIsEnabled());
	TestTrue(TEXT("Respawn button has a click handler"), Widget->RespawnButton->OnClicked.IsBound());
	Widget->SetRespawnReady(true);
	TestTrue(TEXT("Respawn unlocks"), Widget->RespawnButton->GetIsEnabled());

	FActorSpawnParameters SpawnParams;
	SpawnParams.ObjectFlags |= RF_Transient;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActionRPGPlayerCharacter* PlayerPawn = World->SpawnActor<AActionRPGPlayerCharacter>(
		FVector(105000.0f, 105000.0f, 100.0f), FRotator::ZeroRotator, SpawnParams);
	if (!TestNotNull(TEXT("Player spawns"), PlayerPawn)) return false;
	AActionRPGPlayerController* PlayerController = World->SpawnActor<AActionRPGPlayerController>(
		FVector(105000.0f, 105000.0f, 100.0f), FRotator::ZeroRotator, SpawnParams);
	if (!TestNotNull(TEXT("Controller spawns"), PlayerController))
	{
		PlayerPawn->Destroy();
		return false;
	}
	USecondaryAttributeDataAsset* Rules = NewObject<USecondaryAttributeDataAsset>(PlayerPawn);
	FSecondaryAttributeFormula Health;
	Health.BaseValue = 100.0f;
	Rules->Formulas.Add(ESecondaryAttribute::MaxHealth, Health);
	PlayerPawn->SecondaryAttributeComponent->SecondaryAttributeDataAsset = Rules;
	PlayerPawn->SecondaryAttributeComponent->RecalculateSecondaryAttributes();
	const FVector InitialLocation = PlayerPawn->GetActorLocation();
	PlayerController->LifeEventPlayer = PlayerPawn;
	PlayerController->InitialPlayerTransform = PlayerPawn->GetActorTransform();
	PlayerController->RespawnDelaySeconds = 0.0f;
	PlayerController->DeathWidget = Widget;
	Widget->OnRespawnRequested.AddDynamic(PlayerController, &AActionRPGPlayerController::RequestRespawn);
	TestTrue(TEXT("Death widget has a respawn request listener"), Widget->OnRespawnRequested.IsBound());
	TestEqual(TEXT("Lethal damage applies"), PlayerPawn->TakeDamage(150.0f, FDamageEvent(), nullptr, nullptr), 100.0f);
	TestTrue(TEXT("Player is dead"), PlayerPawn->IsDead());
	TestTrue(TEXT("Dead player cannot use a health potion path"), !PlayerPawn->SecondaryAttributeComponent->RestoreResource(ESecondaryAttribute::MaxHealth, 10.0f));
	TestTrue(TEXT("Controller keeps its world"), PlayerController->GetWorld() == World);
	TestTrue(TEXT("Controller tracks the dead player"), PlayerController->LifeEventPlayer == PlayerPawn);
	TestFalse(TEXT("Respawn unlock timer is inactive"), World->GetTimerManager().IsTimerActive(PlayerController->RespawnUnlockTimer));
	PlayerPawn->SetActorLocation(InitialLocation + FVector(500.0f, 0.0f, 0.0f));
	const AActor* Start = UGameplayStatics::GetActorOfClass(PlayerController, APlayerStart::StaticClass());
	const FVector ExpectedLocation = Start ? Start->GetActorLocation() : InitialLocation;
	PlayerController->RequestRespawn();
	TestFalse(TEXT("Controller request revives the player"), PlayerPawn->IsDead());
	TestEqual(TEXT("Respawn restores full health"), PlayerPawn->SecondaryAttributeComponent->GetCurrentSecondaryAttribute(ESecondaryAttribute::MaxHealth), 100.0f);
	TestTrue(TEXT("Respawn returns to PlayerStart or the initial spawn"), PlayerPawn->GetActorLocation().Equals(ExpectedLocation, 1.0f));
	PlayerController->Destroy();
	PlayerPawn->Destroy();
	return true;
}

#endif
