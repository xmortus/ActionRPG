#pragma once

#include "CoreMinimal.h"
#include "Skills/Core/SkillBase.h"
#include "SkillRegeneration.generated.h"

class UStatusEffectDataAsset;

UCLASS(BlueprintType, Blueprintable)
class ACTIONRPG_API USkillRegeneration : public USkillBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Regeneration")
	TObjectPtr<UStatusEffectDataAsset> EffectDefinition;
	virtual USkillBase* Activate(AActor* Target = nullptr) override;
	virtual bool CanActivate(AActor* Target = nullptr) const override;
};
