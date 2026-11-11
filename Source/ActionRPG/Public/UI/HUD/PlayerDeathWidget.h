#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDeathWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRespawnRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathExitRequested);

UCLASS()
class ACTIONRPG_API UPlayerDeathWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Death")
	void SetRespawnReady(bool bReady);
	UPROPERTY(BlueprintAssignable, Category = "Death")
	FOnRespawnRequested OnRespawnRequested;
	UPROPERTY(BlueprintAssignable, Category = "Death")
	FOnDeathExitRequested OnExitRequested;
protected:
	virtual void NativeOnInitialized() override;
private:
#if WITH_DEV_AUTOMATION_TESTS
	friend class FPlayerDeathFlowTest;
#endif
	UFUNCTION()
	void HandleRespawnClicked();
	UFUNCTION()
	void HandleExitClicked();
	UPROPERTY()
	TObjectPtr<UButton> RespawnButton;
	UPROPERTY()
	TObjectPtr<UTextBlock> InstructionText;
};
