#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillCatalogEntryWidget.generated.h"

class USkillManagerComponent;
class UTextBlock;
class UButton;

/** One learnable skill with its cost, current eligibility, and purchase action. */
UCLASS()
class ACTIONRPG_API USkillCatalogEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCatalogEntry(USkillManagerComponent* InManager, int32 InCatalogIndex);

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY() TObjectPtr<USkillManagerComponent> SkillManager;
	UPROPERTY() TObjectPtr<UTextBlock> NameText;
	UPROPERTY() TObjectPtr<UTextBlock> StatusText;
	UPROPERTY() TObjectPtr<UTextBlock> LearnText;
	UPROPERTY() TObjectPtr<UButton> LearnButton;
	int32 CatalogIndex = INDEX_NONE;

	UFUNCTION() void OnLearnClicked();
	void RefreshDisplay();
};
